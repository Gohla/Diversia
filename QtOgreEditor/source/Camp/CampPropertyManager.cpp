/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Camp/CampPropertyManager.h"
#include "Util/Signal/UserObjectChange.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

CampPropertyData::CampPropertyData( const camp::Property& rProperty, 
    const camp::UserObject& rObject, CampPropertyDataInterface* pParentPropertyData /*= 0 */ ) :
    mProperty( rProperty ), 
    mObject( rObject ), 
    mParentPropertyData( pParentPropertyData )
{

}

CampPropertyData::~CampPropertyData()
{
    if( mParentPropertyData ) delete mParentPropertyData;
}

void CampPropertyData::setWithUndo( const camp::Value& rValue )
{
    /*if( !mParentPropertyData )
        GlobalsBase::mUndoStack->push( 
            new PropertyChangeCommand( mProperty, mObject, rValue ) );
    else
        GlobalsBase::mUndoStack->push( 
            new PropertyChangeCommand( mProperty, mObject, rValue, 
            &mParentPropertyData->mProperty, mParentPropertyData->mObject ) );*/

    // TODO: Fix undo
    CampPropertyData::set( rValue );
}

bool CampPropertyData::ignore() const
{
    return mProperty.hasTag( "NoPropertyBrowser" ) || !mProperty.readable( mObject );
}

QString CampPropertyData::valueText() const
{
    QString text;
    const camp::UserProperty& userProp = static_cast<const camp::UserProperty&>( mProperty );
    camp::UserObject object = userProp.get( mObject ).to<camp::UserObject>();
    const camp::Class& metaClass = userProp.getClass();
    bool first = true;

    for( std::size_t j = 0; j != metaClass.propertyCount(); ++j )
    {
        const camp::Property& prop = metaClass.property( j );
        if( prop.hasTag( "NoPropertyBrowser" ) || !prop.readable( object ) ) continue;

        if( !first )
            text += ", ";
        else
        {
            text += "(";
            first = false;
        }

        switch( prop.type() )
        {
        case camp::boolType:
            text += boost::lexical_cast<char*>( prop.get( object ).to<bool>() );
            break;
        case camp::intType:
            text += boost::lexical_cast<char*>( prop.get( object ).to<int>() );
            break;
        case camp::realType: 
            text += QString::number( prop.get( object ).to<double>(), 'f', 2 );
            break;
        }
    }

    return text + ")";
}

CampPropertyData* CampPropertyData::clone() const
{
    return new CampPropertyData( mProperty, mObject, mParentPropertyData ? mParentPropertyData->clone() : 0 );
}

bool CampPropertyData::lessThan( const CampPropertyDataInterface* other ) const
{
    // This is ok because CampPropertyDataInterface::operator< checks if other is of this type.
    const CampPropertyData* data = static_cast<const CampPropertyData*>( other );

    if( &mProperty < &data->mProperty ) return true;
    if( &data->mProperty < &mProperty ) return false;

    if( mObject < data->mObject ) return true;

    return false;
}

CampValueMapPropertyData::CampValueMapPropertyData( const camp::DictionaryProperty& rProperty, 
    const camp::UserObject& rObject, const String& rKey ):
    mProperty( rProperty ),
    mObject( rObject ),
    mKey( rKey )
{

}

void CampValueMapPropertyData::setWithUndo( const camp::Value& rValue )
{
    // TODO: Fix undo
    CampValueMapPropertyData::set( rValue );
}

CampValueMapPropertyData* CampValueMapPropertyData::clone() const
{
    return new CampValueMapPropertyData( mProperty, mObject, mKey );
}

bool CampValueMapPropertyData::lessThan( const CampPropertyDataInterface* other ) const
{
    // This is ok because CampPropertyDataInterface::operator< checks if other is of this type.
    const CampValueMapPropertyData* data = static_cast<const CampValueMapPropertyData*>( other );

    if( &mProperty < &data->mProperty ) return true;
    if( &data->mProperty < &mProperty ) return false;

    if( mObject < data->mObject ) return true;
    if( data->mObject < mObject ) return false;

    if( mKey < data->mKey ) return true;

    return false;
}

//------------------------------------------------------------------------------

CampCompoundPropertyManager::CampCompoundPropertyManager( CampPropertyManager* pParent /*= 0*/ ):
    QtAbstractPropertyManager( pParent ),
    mCampPropertyManager( pParent ),
    mBlockChangeSignal( false )
{
    mQtFix = new QtFix( this );
}

CampCompoundPropertyManager::~CampCompoundPropertyManager()
{
    CampCompoundPropertyManager::clearMe();
    delete mQtFix;
}

void CampCompoundPropertyManager::setValue( QtProperty* pProperty, 
    CampPropertyDataInterface* pData )
{
    QtPropertyDataMap::iterator i = mPropertyData.find( pProperty );
    if( i == mPropertyData.end() )
    {
        // Add new property data.
        mPropertyData.insert( std::make_pair( pProperty, pData ) );
    }
    else
    {
        // TODO: Update property data
    }
}

void CampCompoundPropertyManager::externalPropertyChanged( QtProperty* pProperty )
{
    // Update sub properties.
    std::pair<PropertyToSubMap::iterator, PropertyToSubMap::iterator> range = 
        mPropertyToSubMap.equal_range( pProperty );

    for( PropertyToSubMap::iterator i = range.first; i != range.second; ++i )
    {
        QtPropertyDataMap::iterator j = mSubPropertyData.find( i->second );
        if( j != mSubPropertyData.end() )
        {
            camp::UserObject object = j->second->get().to<camp::UserObject>();
            const camp::Property& subProperty = 
                object.getClass().property( i->second->propertyName().toStdString() );
            //LOGD << "Updating " << (unsigned int)object.pointer() << ", " << subProperty.name();
            mCampPropertyManager->externalPropertyChanged( object, subProperty, 
                subProperty.get( object ) );
            mCampPropertyManager->setBlockSlots( true );
        }
    }

    // Update parent property.
    QtAbstractPropertyManager::propertyChanged( pProperty );
}

void CampCompoundPropertyManager::propInserted( QtProperty* pProperty, QtProperty* pParent, 
    QtProperty* pAfter )
{
    mSubToPropertyMap.insert( std::make_pair( pProperty, pParent ) );
    mPropertyToSubMap.insert( std::make_pair( pParent, pProperty ) );

    QtPropertyDataMap::iterator i = mPropertyData.find( pParent );
    if( i != mPropertyData.end() )
    {
        mSubPropertyData.insert( std::make_pair( pProperty, i->second ) );
    }
}

void CampCompoundPropertyManager::clearMe()
{
    QtAbstractPropertyManager::clear();
    for( QtPropertyDataMap::iterator i = mPropertyData.begin(); i != mPropertyData.end(); ++i )
    {
        delete i->second;
    }
    mPropertyData.clear();
    mSubPropertyData.clear();
    mPropertyToSubMap.clear();
}

void CampCompoundPropertyManager::setBlockSlots( bool block )
{
    mBlockChangeSignal = block;
}

bool CampCompoundPropertyManager::hasValue( const QtProperty* pProperty ) const
{
    return true;
}

QString CampCompoundPropertyManager::valueText( const QtProperty* pProperty ) const
{
    QtPropertyDataMap::const_iterator i = mPropertyData.find( pProperty );
    if( i != mPropertyData.end() ) return i->second->valueText();
    return "";
}

//------------------------------------------------------------------------------

QtFix::QtFix( CampCompoundPropertyManager* pInstance ) : mInstance( pInstance )
{
    QObject::connect( mInstance, 
        SIGNAL( propertyInserted(QtProperty*, QtProperty*, QtProperty*) ), this, 
        SLOT( propertyInserted(QtProperty*, QtProperty*, QtProperty*) ) );
}

void QtFix::propertyInserted( QtProperty* pProperty, QtProperty* pParent, QtProperty* pAfter )
{
    mInstance->propInserted( pProperty, pParent, pAfter );
}

void QtFix::boolValueChanged( QtProperty* pProperty, bool val )
{
    mInstance->propertyChanged( pProperty, val );
}

void QtFix::intValueChanged( QtProperty* pProperty, int val )
{
    mInstance->propertyChanged( pProperty, val );
}

void QtFix::doubleValueChanged( QtProperty* pProperty, double val )
{
    mInstance->propertyChanged( pProperty, val );
}

void QtFix::stringValueChanged( QtProperty* pProperty, const QString& rVal )
{
    mInstance->propertyChanged( pProperty, rVal );
}

void QtFix::enumValueChanged( QtProperty* pProperty, int val )
{
    mInstance->propertyChanged( pProperty, val );
}

//------------------------------------------------------------------------------

CampPropertyManager::CampPropertyManager( QObject* pParent /*= 0*/ ):
    QObject( pParent ),
    mGroupManager( new QtGroupPropertyManager( this ) ),
    mBoolManager( new QtBoolPropertyManager( this ) ),
    mIntManager( new QtIntPropertyManager( this ) ),
    mDoubleManager( new QtDoublePropertyManager( this ) ),
    mStringManager( new QtStringPropertyManager( this ) ),
    mEnumManager( new QtEnumPropertyManager( this ) ),
    mCompoundManager( new CampCompoundPropertyManager( this ) ),
    mCheckBoxFactory( new QtCheckBoxFactory( this ) ),
    mSpinBoxFactory( new QtSpinBoxFactory( this ) ),
    mDoubleSpinBoxFactory( new QtDoubleSpinBoxFactory( this ) ),
    mLineEditFactory( new QtLineEditFactory( this ) ),
    mEnumFactory( new QtEnumEditorFactory( this ) )
{
    QObject::connect( mBoolManager, SIGNAL( valueChanged(QtProperty*, bool) ), this, 
        SLOT( boolValueChanged(QtProperty*, bool) ) );
    QObject::connect( mIntManager, SIGNAL( valueChanged(QtProperty*, int) ), this, 
        SLOT( intValueChanged(QtProperty*, int) ) );
    QObject::connect( mDoubleManager, SIGNAL( valueChanged(QtProperty*, double) ), this, 
        SLOT( doubleValueChanged(QtProperty*, double) ) );
    QObject::connect( mStringManager, SIGNAL( valueChanged(QtProperty*, const QString&) ), this, 
        SLOT( stringValueChanged(QtProperty*, const QString&) ) );
    QObject::connect( mEnumManager, SIGNAL( valueChanged(QtProperty*, int) ), this, 
        SLOT( enumValueChanged(QtProperty*, int) ) );

    // Add slots for compound manager
    QObject::connect( mBoolManager, SIGNAL( valueChanged(QtProperty*, bool) ), 
        mCompoundManager->mQtFix, SLOT( boolValueChanged(QtProperty*, bool) ) );
    QObject::connect( mIntManager, SIGNAL( valueChanged(QtProperty*, int) ), 
        mCompoundManager->mQtFix, SLOT( intValueChanged(QtProperty*, int) ) );
    QObject::connect( mDoubleManager, SIGNAL( valueChanged(QtProperty*, double) ), 
        mCompoundManager->mQtFix, SLOT( doubleValueChanged(QtProperty*, double) ) );
    QObject::connect( mStringManager, SIGNAL( valueChanged(QtProperty*, const QString&) ), 
        mCompoundManager->mQtFix, SLOT( stringValueChanged(QtProperty*, const QString&) ) );
    QObject::connect( mEnumManager, SIGNAL( valueChanged(QtProperty*, int) ), 
        mCompoundManager->mQtFix, SLOT( enumValueChanged(QtProperty*, int) ) );

    GlobalsBase::mUpdateSignal->connect( sigc::mem_fun( this, &CampPropertyManager::update ) );

    CampPropertyManager::setBlockSlots( true );
}

CampPropertyManager::~CampPropertyManager()
{
    CampPropertyManager::clear();
}

void CampPropertyManager::setPropertyBrowser( QtTreePropertyBrowser* pBrowser )
{
    pBrowser->setFactoryForManager( mBoolManager, mCheckBoxFactory );
    pBrowser->setFactoryForManager( mIntManager, mSpinBoxFactory );
    pBrowser->setFactoryForManager( mDoubleManager, mDoubleSpinBoxFactory );
    pBrowser->setFactoryForManager( mStringManager, mLineEditFactory );
    pBrowser->setFactoryForManager( mEnumManager, mEnumFactory );
}

QtProperty* CampPropertyManager::addProperty( CampPropertyDataInterface* pData, 
    QtProperty* pProperty /*= 0 */ )
{
    if( pData->ignore() ) return 0;

    String name = pData->name();
    QtProperty* prop = 0;

    switch( pData->type() )
    {
        case camp::boolType:
            prop = mBoolManager->addProperty( QString( name.c_str() ) );
            mBoolManager->setValue( prop, pData->get().to<bool>() );
            mData.insert( std::make_pair( prop, pData ) );
            break;
        case camp::intType:  
            prop = mIntManager->addProperty( QString( name.c_str() ) );
            mIntManager->setValue( prop, pData->get().to<int>() );
            mData.insert( std::make_pair( prop, pData ) );
            break;
        case camp::realType: 
            prop = mDoubleManager->addProperty( QString( name.c_str() ) );
            mDoubleManager->setValue( prop, pData->get().to<double>() );
            mData.insert( std::make_pair( prop, pData ) );

            if( pData->hasTag( "QtDoublePrecicion" ) )
                mDoubleManager->setDecimals( prop, pData->tag( "QtDoublePrecicion" ).to<unsigned int>() );
            else
                mDoubleManager->setDecimals( prop, 4 );

            if( pData->hasTag( "QtSingleStep" ) )
                mDoubleManager->setSingleStep( prop, pData->tag( "QtSingleStep" ).to<float>() );
            else
                mDoubleManager->setSingleStep( prop, 0.1 );

            break;
        case camp::stringType: 
            prop = mStringManager->addProperty( QString( name.c_str() ) );
            mStringManager->setValue( prop, QString( pData->get().to<String>().c_str() ) );
            mData.insert( std::make_pair( prop, pData ) );
            break;
        case camp::enumType:
        {
            camp::EnumObject enumObject = pData->get().to<camp::EnumObject>();
            const camp::Enum& metaenum = enumObject.getEnum();

            prop = mEnumManager->addProperty( QString( name.c_str() ) );

            // Add enum names
            QStringList names;
            for( std::size_t i = 0; i != metaenum.size(); ++i )
            {
                // TODO: Support enums that do not start with value 0.
                names << metaenum.pair( i ).name.c_str();
            };
            mEnumManager->setEnumNames( prop, names );

            mEnumManager->setValue( prop, enumObject.value() );
            mData.insert( std::make_pair( prop, pData ) );
            break;
        }
        case camp::userType:
        {
            camp::UserObject object = pData->get().to<camp::UserObject>();
            const camp::Class& subClass = object.getClass();

            // Create compound property that manages its sub properties.
            QtProperty* compoundProperty = mCompoundManager->addProperty( QString( name.c_str() ) );
            mCompoundManager->setValue( compoundProperty, pData );

            // Do recursion by calling addClassInternal on user type.
            std::set<String> ignore;
            prop = CampPropertyManager::addClassInternal( subClass, object, name, ignore, 
                compoundProperty, pData );
            break;
        }
        case camp::arrayType:
            // TODO: Support arrays
            break;
        case camp::dictionaryType:
            // TODO: Support dictionaries
            break;
        case camp::valueType:
            // TODO: Support type erased values
            break;
        case camp::noType: default: LOGE << "Could not create property";
    }

    if( prop )
    {
        mProperties.insert( std::make_pair( pData, prop ) );
        //LOGD << "Adding " << (unsigned int)rObject.pointer() << ", " << rCampProperty.name();

        if( pData->hasTag( "QtContinuousUpdate" ) )
        {
            mContinuousUpdateMap.insert( pData );
        }

        if( pProperty ) pProperty->addSubProperty( prop );
        if( !pData->writable() ) prop->setEnabled( false );
    }

    return prop;
}

QtProperty* CampPropertyManager::addClass( const camp::Class& rClass, 
    const camp::UserObject& rObject, const String& rName /*= ""*/ )
{
    // Clear before adding properties.
    CampPropertyManager::clear();

    std::set<String> ignore;
    QtProperty* prop = CampPropertyManager::addClassInternal( rClass, rObject, 
        rName.empty() ? rClass.name() : rName, ignore );

    // Propagate property changes to camp.
    CampPropertyManager::setBlockSlots( false );

    return prop;
}

QtProperty* CampPropertyManager::addClassInternal( const camp::Class& rClass, 
    const camp::UserObject& rObject, const String& rName, std::set<String>& rAddedProperties, 
    QtProperty* pCompoundProperty /*= 0*/, CampPropertyDataInterface* pParentData /*= 0*/ )
{
    QtProperty* prop;

    if( pCompoundProperty )
        prop = pCompoundProperty;
    else
        prop = mGroupManager->addProperty( QString( rName.c_str() ) );

    for( std::size_t i = 0; i != rClass.propertyCount(); ++i )
    {
        const camp::Property& property = rClass.property( i );

        if( !rAddedProperties.count( property.name() ) )
        {
            QtProperty* subProp = CampPropertyManager::addProperty( new CampPropertyData( 
                rClass.property( i ), rObject, pParentData ? pParentData->clone() : 0 ), prop );
            if( subProp ) prop->addSubProperty( subProp );
            rAddedProperties.insert( property.name() );
        }
    }

    // Return 0 if this class has no sub properties.
    if( prop->subProperties().size() )
        return prop;
    else
        return 0;
}

std::list<QtProperty*> CampPropertyManager::addClassTree( const camp::Class& rClass, 
    const camp::UserObject& rObject )
{
    // Clear before adding properties.
    CampPropertyManager::clear();

    // Call internal function
    std::set<String> addedProperties;
    std::list<QtProperty*> rootProperties;
    CampPropertyManager::addClassTreeInternal( rClass, rObject, addedProperties, rootProperties );

    CampPropertyManager::setup( rObject );

    return rootProperties;
}

void CampPropertyManager::setup( const camp::UserObject& rObject )
{
    // Propagate property changes to camp.
    CampPropertyManager::setBlockSlots( false );

    // Connect to property changes.
    mPropertyChangeSignal = UserObjectChange::connectChange( rObject, sigc::mem_fun( this, 
        (void(CampPropertyManager::*)(const camp::UserObject&, const camp::Property&, 
        const camp::Value&))&CampPropertyManager::externalPropertyChanged ) );
}

void CampPropertyManager::addClassTreeInternal( const camp::Class& rClass, 
    const camp::UserObject& rObject, std::set<String>& rAddedProperties, 
    std::list<QtProperty*>& rRootProperties )
{
    // Recursively go trough all base classes.
    for( std::size_t i = 0; i != rClass.baseCount(); ++i )
    {
        CampPropertyManager::addClassTreeInternal( rClass.base( i ), rObject, rAddedProperties, 
            rRootProperties );
    }

    // Add current class properties.
    QtProperty* prop = CampPropertyManager::addClassInternal( rClass, rObject, rClass.name(), 
        rAddedProperties );
    if( prop ) rRootProperties.push_back( prop );
}

void CampPropertyManager::boolValueChanged( QtProperty* pProperty, bool val )
{
    CampPropertyManager::propertyChanged( pProperty, mBoolManager, val );
}

void CampPropertyManager::intValueChanged( QtProperty* pProperty, int val )
{
    CampPropertyManager::propertyChanged( pProperty, mIntManager, val );
}

void CampPropertyManager::doubleValueChanged( QtProperty* pProperty, double val )
{
    CampPropertyManager::propertyChanged( pProperty, mDoubleManager, val );
}

void CampPropertyManager::stringValueChanged( QtProperty* pProperty, const QString& rVal )
{
    CampPropertyManager::propertyChanged( pProperty, mStringManager, rVal );
}

void CampPropertyManager::enumValueChanged( QtProperty* pProperty, int val )
{
    QtPropertyDataMap::iterator i = mData.find( pProperty );
    if( i != mData.end() )
    {
        const camp::Enum& metaenum = i->second->get().to<camp::EnumObject>().getEnum();
        CampPropertyManager::propertyChanged( pProperty, mEnumManager, camp::Value( 
            camp::EnumObject( val, metaenum ) ) );
    }
}

void CampPropertyManager::clear()
{
    CampPropertyManager::setBlockSlots( true );
    mPropertyChangeSignal.disconnect();

    mGroupManager->clear();
    mBoolManager->clear();
    mIntManager->clear();
    mDoubleManager->clear();
    mStringManager->clear();
    mEnumManager->clear();
    mCompoundManager->clearMe();

    for( QtPropertyDataMap::iterator i = mData.begin(); i != mData.end(); ++i )
    {
        delete i->second;
    }
    mData.clear();
    mProperties.clear();
    mContinuousUpdateMap.clear();
}

void CampPropertyManager::setBlockSlots( bool block )
{
    mBlockChangeSignal = block;
    mCompoundManager->setBlockSlots( block );
}

void CampPropertyManager::externalPropertyChanged( CampPropertyDataInterface* pData, 
    const camp::Value& rValue )
{
    PropertyMap::iterator i = mProperties.find( pData );
    if( i != mProperties.end() )
    {
        CampPropertyManager::setBlockSlots( true );

        switch( rValue.type() )
        {
            case camp::boolType:
                mBoolManager->setValue( i->second, rValue.to<bool>() );
                break;
            case camp::intType:  
                mIntManager->setValue( i->second, rValue.to<int>() );
                break;
            case camp::realType: 
                mDoubleManager->setValue( i->second, rValue.to<double>() );
                break;
            case camp::stringType: 
                mStringManager->setValue( i->second, rValue.to<QString>() );
                break;
            case camp::enumType:
                mEnumManager->setValue( i->second, rValue.to<int>() );
                break;
            case camp::userType:
                mCompoundManager->externalPropertyChanged( i->second );
                break;
            case camp::arrayType:
                // TODO: Support arrays
                break;
            case camp::noType: default: LOGE << "Could not update property value";
        }

        CampPropertyManager::setBlockSlots( false );
    }
}

void CampPropertyManager::externalPropertyChanged( const camp::UserObject& rObject, 
    const camp::Property& rProperty, const camp::Value& rValue )
{
    CampPropertyData* data = new CampPropertyData( rProperty, rObject );
    CampPropertyManager::externalPropertyChanged( data, rValue );
    delete data;
}

void CampPropertyManager::update()
{
    // Only update at 1/4th of the framerate.
    static unsigned int count = 0;
    if( ++count != 4 ) return;

    for( ContinuousUpdateMap::iterator i = mContinuousUpdateMap.begin(); 
        i != mContinuousUpdateMap.end(); ++i )
    {
        try
        {
            CampPropertyManager::externalPropertyChanged( *i, (*i)->get() );
        }
        catch( camp::Error e )
        {
        	// Ignore error.
        }
    }

    count = 0;
}

//------------------------------------------------------------------------------

} // Namespace QtOgreEditor
} // Namespace Diversia