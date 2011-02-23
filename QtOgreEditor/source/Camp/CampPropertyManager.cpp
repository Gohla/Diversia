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
    const camp::Property& rCampProperty, const camp::UserObject& rObject )
{
    DataMap::iterator i = mPropertyData.find( pProperty );
    if( i == mPropertyData.end() )
    {
        // Add new property data.
        CampPropertyData* data = new CampPropertyData( rCampProperty, rObject );
        mPropertyData.insert( std::make_pair( pProperty, data ) );
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
        DataMap::iterator j = mSubPropertyData.find( i->second );
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

    DataMap::iterator i = mPropertyData.find( pParent );
    if( i != mPropertyData.end() )
    {
        mSubPropertyData.insert( std::make_pair( pProperty, i->second ) );
    }
}

void CampCompoundPropertyManager::clearMe()
{
    QtAbstractPropertyManager::clear();
    for( DataMap::iterator i = mPropertyData.begin(); i != mPropertyData.end(); ++i )
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
    DataMap::const_iterator i = mPropertyData.find( pProperty );

    if( i != mPropertyData.end() )
    {
        QString text;
        const camp::UserProperty& userProp = static_cast<const camp::UserProperty&>( 
            i->second->mProperty );
        camp::UserObject object = userProp.get( i->second->mObject ).to<camp::UserObject>();
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

QtProperty* CampPropertyManager::addProperty( const camp::Property& rCampProperty, 
    const camp::UserObject& rObject, QtProperty* pProperty /*= 0*/, 
    const camp::Property* pParentProperty /*= 0*/, 
    const camp::UserObject& rParentObject /*= camp::UserObject::nothing*/ )
{
    // Ignore properties with the NoPropertyBrowser tag.
    if( rCampProperty.hasTag( "NoPropertyBrowser" ) || !rCampProperty.readable( rObject ) ) 
        return 0;

    String name = rCampProperty.name();
    QtProperty* prop = 0;

    switch( rCampProperty.type() )
    {
        case camp::boolType:
            prop = mBoolManager->addProperty( QString( name.c_str() ) );
            mBoolManager->setValue( prop, rCampProperty.get( rObject ).to<bool>() );
            mData.insert( std::make_pair( prop, new CampPropertyData( rCampProperty, rObject, 
                pParentProperty, rParentObject ) ) );
            break;
        case camp::intType:  
            prop = mIntManager->addProperty( QString( name.c_str() ) );
            mIntManager->setValue( prop, rCampProperty.get( rObject ).to<int>() );
            mData.insert( std::make_pair( prop, new CampPropertyData( rCampProperty, rObject, 
                pParentProperty, rParentObject ) ) );
            break;
        case camp::realType: 
            prop = mDoubleManager->addProperty( QString( name.c_str() ) );
            mDoubleManager->setValue( prop, rCampProperty.get( rObject ).to<double>() );
            mData.insert( std::make_pair( prop, new CampPropertyData( rCampProperty, rObject, 
                pParentProperty, rParentObject ) ) );

            if( rCampProperty.hasTag( "QtDoublePrecicion" ) )
                mDoubleManager->setDecimals( prop, rCampProperty.tag( "QtDoublePrecicion" ).to<unsigned int>() );
            else
                mDoubleManager->setDecimals( prop, 4 );

            if( rCampProperty.hasTag( "QtSingleStep" ) )
                mDoubleManager->setSingleStep( prop, rCampProperty.tag( "QtSingleStep" ).to<float>() );
            else
                mDoubleManager->setSingleStep( prop, 0.1 );

            break;
        case camp::stringType: 
            prop = mStringManager->addProperty( QString( name.c_str() ) );
            mStringManager->setValue( prop, QString( rCampProperty.get( 
                rObject ).to<String>().c_str() ) );
            mData.insert( std::make_pair( prop, new CampPropertyData( rCampProperty, rObject, 
                pParentProperty, rParentObject ) ) );
            break;
        case camp::enumType:
        {
            prop = mEnumManager->addProperty( QString( name.c_str() ) );

            // Add enum names
            QStringList names;
            const camp::Enum& enumProp = static_cast<const camp::EnumProperty&>( 
                rCampProperty ).getEnum();
            for( std::size_t i = 0; i != enumProp.size(); ++i )
            {
                // TODO: Support enums that do not start with value 0.
                names << enumProp.pair( i ).name.c_str();
            };
            mEnumManager->setEnumNames( prop, names );

            mEnumManager->setValue( prop, rCampProperty.get( rObject ).to<int>() );
            mData.insert( std::make_pair( prop, new CampPropertyData( rCampProperty, rObject, 
                pParentProperty, rParentObject ) ) );
            break;
        }
        case camp::userType:
        {
            const camp::UserProperty& userProp = static_cast<const camp::UserProperty&>( 
                rCampProperty );
            const camp::Class& subClass = userProp.getClass();
            camp::UserObject object = rCampProperty.get( rObject ).to<camp::UserObject>();

            // Create compound property that manages its sub properties.
            QtProperty* compoundProperty = mCompoundManager->addProperty( QString( 
                name.c_str() ) );
            mCompoundManager->setValue( compoundProperty, userProp, rObject );

            // Do recursion by calling addClassInternal on user type.
            std::set<String> ignore;
            prop = CampPropertyManager::addClassInternal( subClass, object, name, ignore, 
                compoundProperty, &userProp, rObject );
            break;
        }
        case camp::arrayType:
        {
            // TODO: Support arrays
            break;
        }
        case camp::noType: default: LOGE << "Could not create property";
    }

    if( prop )
    {
        mProperties.insert( std::make_pair( std::make_pair( rObject, &rCampProperty ), prop ) );
        //LOGD << "Adding " << (unsigned int)rObject.pointer() << ", " << rCampProperty.name();

        if( rCampProperty.hasTag( "QtContinuousUpdate" ) )
        {
            mContinuousUpdateMap.insert( std::make_pair( &rCampProperty, rObject ) );
        }

        if( pProperty )
            pProperty->addSubProperty( prop );

        if( !rCampProperty.writable( rObject ) )
            prop->setEnabled( false );
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
    QtProperty* pCompoundProperty /*= 0*/, const camp::Property* pParentProperty /*= 0*/, 
    const camp::UserObject& rParentObject /*= camp::UserObject::nothing*/ )
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
            QtProperty* subProp = CampPropertyManager::addProperty( rClass.property( i ), rObject, 
                prop, pParentProperty, rParentObject );
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

    // Propagate property changes to camp.
    CampPropertyManager::setBlockSlots( false );

    // Connect to property changes.
    mPropertyChangeSignal = UserObjectChange::connectChange( rObject, sigc::mem_fun( this, 
        &CampPropertyManager::externalPropertyChanged ) );

    return rootProperties;
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
    CampPropertyManager::propertyChanged( pProperty, mEnumManager, val );
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

    for( DataMap::iterator i = mData.begin(); i != mData.end(); ++i )
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

void CampPropertyManager::externalPropertyChanged( const camp::UserObject& rObject, 
    const camp::Property& rProperty, const camp::Value& rValue )
{
    PropertyMap::iterator i = mProperties.find( std::make_pair( rObject, &rProperty ) );
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
            CampPropertyManager::externalPropertyChanged( i->second, *i->first, 
                i->first->get( i->second ) );
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