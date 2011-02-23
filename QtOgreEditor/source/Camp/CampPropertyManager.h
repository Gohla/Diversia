/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_CAMPPROPERTYMANAGER_H
#define DIVERSIA_QTOGREEDITOR_CAMPPROPERTYMANAGER_H

#include "Platform/Prerequisites.h"

#include "Client/Undo/UndoStack.h"
#include "Client/Undo/PropertyChangeCommand.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

/**
Data class to keep track of camp properties.
**/
struct CampPropertyData
{
    CampPropertyData( const camp::Property& rProperty, const camp::UserObject& rObject,
        const camp::Property* pParentProperty = 0, 
        const camp::UserObject& rParentObject = camp::UserObject::nothing ):
        mProperty( rProperty ), mObject( rObject ), mParentProperty( pParentProperty ),
        mParentObject( rParentObject ) {}

    inline void set( camp::Value value ) { mProperty.set( mObject, value ); }
    inline void setWithUndo( camp::Value value ) { GlobalsBase::mUndoStack->push( 
        new PropertyChangeCommand( mProperty, mObject, value, mParentProperty, mParentObject ) ); }
    inline camp::Value get() const { return mProperty.get( mObject ); }

    const camp::Property&   mProperty;
    camp::UserObject        mObject;
    const camp::Property*   mParentProperty;
    camp::UserObject        mParentObject;
};

//------------------------------------------------------------------------------

// Forward declaration
class QtFix;

/**
Camp compound property manager for managing nested properties.
**/
class CampCompoundPropertyManager : public QtAbstractPropertyManager
{
public:
    CampCompoundPropertyManager( CampPropertyManager* pParent = 0 );
    ~CampCompoundPropertyManager();

    void setValue( QtProperty* pProperty, const camp::Property& rProperty, 
        const camp::UserObject& rObject );
    template <typename T> void propertyChanged( QtProperty* pProperty, T val )
    {
        if( mBlockChangeSignal ) return;

        DataMap::iterator i = mSubPropertyData.find( pProperty );
        if( i != mSubPropertyData.end() )
        {
            // This is called after the sub values have changed, get the value and set it again
            // to trigger the parent setter function in camp.
            try
            {
                camp::UserObject object = i->second->get().to<camp::UserObject>();
                const camp::Property& subProperty = 
                    object.getClass().property( pProperty->propertyName().toStdString() );
                subProperty.set( object, val );

                // TODO: No need to set if 'object' is not a copy but a reference?
                i->second->set( object );

                // TODO: Revert value back if set fails?
            }
            catch( camp::Error e )
            {
                LOGE << "Could not set property: " << e.what();
            }
            catch( Exception e )
            {
                LOGE << "Could not set property: " << e.what();
            }
            catch( Ogre::Exception e )
            {
                LOGE << "Could not set property: " << e.what();
            }
            catch( std::exception e )
            {
                LOGE << "Could not set property: " << e.what();
            }
            catch( ... )
            {
                LOGE << "Could not set property: Other exception";
            }

            SubToPropertyMap::iterator j = mSubToPropertyMap.find( pProperty );
            if( j != mSubToPropertyMap.end() )
                QtAbstractPropertyManager::propertyChanged( j->second );
        }
    }
    void externalPropertyChanged( QtProperty* pProperty );
    void propInserted( QtProperty* pProperty, QtProperty* pParent, QtProperty* pAfter );
    void clearMe();
    void setBlockSlots( bool block );

private:
    friend class CampPropertyManager;	///< Needs access to mQtFix.

    bool hasValue( const QtProperty* pProperty ) const;
    QString valueText( const QtProperty* pProperty ) const;
    inline void initializeProperty( QtProperty* pProperty ) {}

    QtFix*                  mQtFix;
    CampPropertyManager*    mCampPropertyManager;
    bool                    mBlockChangeSignal;

    typedef std::map<const QtProperty*, CampPropertyData*> DataMap;
    DataMap mPropertyData;
    DataMap mSubPropertyData;

    typedef std::map<QtProperty*, QtProperty*> SubToPropertyMap;
    typedef std::multimap<QtProperty*, QtProperty*> PropertyToSubMap;
    SubToPropertyMap mSubToPropertyMap;
    PropertyToSubMap mPropertyToSubMap;
};

//------------------------------------------------------------------------------

/**
Class to fix something stupid in Qt slots&signals...
**/
class QtFix : public QObject
{
    Q_OBJECT

public:
    QtFix( CampCompoundPropertyManager* pInstance );

private slots:
    void propertyInserted( QtProperty* pProperty, QtProperty* pParent, QtProperty* pAfter );
    void boolValueChanged( QtProperty* pProperty, bool val );
    void intValueChanged( QtProperty* pProperty, int val );
    void doubleValueChanged( QtProperty* pProperty, double val );
    void stringValueChanged( QtProperty* pProperty, const QString& rVal );
    void enumValueChanged( QtProperty* pProperty, int val );

private:
    CampCompoundPropertyManager* mInstance;

};

//------------------------------------------------------------------------------

/**
Manages camp properties for a property browser.
**/
class CampPropertyManager : public QObject, public sigc::trackable
{
    Q_OBJECT

public:
    /**
    Constructor. 
    
    @param [in,out] pParent If non-null, the parent QObject.
    **/
    CampPropertyManager( QObject* pParent = 0 );
    /**
    Destructor. 
    **/
    ~CampPropertyManager();

    /**
    Sets up the property factories with the property browser. Call this before using the property
    browser!.
    **/
    void setPropertyBrowser( QtTreePropertyBrowser* pBrowser );
    /**
    Adds a property to the manager and returns the property.
    
    @param  rProperty   The property to add.
    @param  rObject     The object that has rProperty.
    @param  pProperty   Parent property, set this to the root item of the property browser.

    @return Created property or 0 if the property could not be created.
    **/
    QtProperty* addProperty( const camp::Property& rProperty, const camp::UserObject& rObject, 
        QtProperty* pProperty = 0, const camp::Property* pParentProperty = 0, 
        const camp::UserObject& rParentObject = camp::UserObject::nothing );
    /**
    Adds a whole class to the manager as properties and returns the root property.
    
    @param  rClass  The class to add.
    @param  rObject The instance of the class.
    @param  rName   The name the class should get as a property. If left empty it defaults to the
                    name of the class.

    @return Root property of the class or 0 if the class had no sub properties.
    **/
    QtProperty* addClass( const camp::Class& rClass, const camp::UserObject& rObject, 
        const String& rName = "" );
    /**
    Adds a whole class and all base classes separately to the manager as properties and returns all
    root properties.
    
    @param  rClass  The class to add.
    @param  rObject The instance of the class.

    @return List of all root properties.
    **/
    std::list<QtProperty*> addClassTree( const camp::Class& rClass, 
        const camp::UserObject& rObject );
    /**
    Destroys all properties.
    **/
    void clear();

private slots:
    void boolValueChanged( QtProperty* pProperty, bool val );
    void intValueChanged( QtProperty* pProperty, int val );
    void doubleValueChanged( QtProperty* pProperty, double val );
    void stringValueChanged( QtProperty* pProperty, const QString& rCal );
    void enumValueChanged( QtProperty* pProperty, int val );

private:
    friend class CampCompoundPropertyManager;

    QtProperty* addClassInternal( const camp::Class& rClass, const camp::UserObject& rObject, 
        const String& rName, std::set<String>& rAddedProperties, 
        QtProperty* pCompoundProperty = 0, const camp::Property* pParentProperty = 0, 
        const camp::UserObject& rParentObject = camp::UserObject::nothing );
    void addClassTreeInternal( const camp::Class& rClass, const camp::UserObject& rObject,
        std::set<String>& rAddedProperties, std::list<QtProperty*>& rRootProperties );
    void setBlockSlots( bool block );
    void externalPropertyChanged( const camp::UserObject& rObject, const camp::Property& rProperty, 
        const camp::Value& rValue );
    template <typename T, typename U> void propertyChanged( QtProperty* pProperty, U pManager, 
        T val )
    {
        if( mBlockChangeSignal ) return;

        DataMap::iterator i = mData.find( pProperty );
        if( i != mData.end() )
        {
            camp::Value currentVal;
            try
            {
                currentVal = i->second->get();
                i->second->setWithUndo( val );
            }
            catch( camp::Error e )
            {
                LOGE << "Could not set property: " << e.what();
                pManager->setValue( pProperty, currentVal.to<T>() );
            }
            catch( Exception e )
            {
                LOGE << "Could not set property: " << e.what();
                pManager->setValue( pProperty, currentVal.to<T>() );
            }
            catch( Ogre::Exception e )
            {
                LOGE << "Could not set property: " << e.what();
            }
            catch( std::exception e )
            {
                LOGE << "Could not set property: " << e.what();
            }
            catch( ... )
            {
                LOGE << "Could not set property: Unhandled exception";
                //pManager->setValue( pProperty, currentVal.to<T>() );
            }
        }
    }
    void update();

    typedef std::map<QtProperty*, CampPropertyData*> DataMap;
    typedef std::map<std::pair<camp::UserObject, const camp::Property*>, QtProperty*> PropertyMap;
    typedef std::map<const camp::Property*, camp::UserObject> ContinuousUpdateMap;

    DataMap                         mData;
    PropertyMap                     mProperties;
    ContinuousUpdateMap             mContinuousUpdateMap;
    sigc::connection                mPropertyChangeSignal;
    bool                            mBlockChangeSignal;

    QtGroupPropertyManager*         mGroupManager;
    QtBoolPropertyManager*          mBoolManager;
    QtIntPropertyManager*           mIntManager;
    QtDoublePropertyManager*        mDoubleManager;
    QtStringPropertyManager*        mStringManager;
    QtEnumPropertyManager*          mEnumManager;
    CampCompoundPropertyManager*    mCompoundManager;

    QtCheckBoxFactory*          mCheckBoxFactory;
    QtSpinBoxFactory*           mSpinBoxFactory;
    QtDoubleSpinBoxFactory*     mDoubleSpinBoxFactory;
    QtLineEditFactory*          mLineEditFactory;
    QtEnumEditorFactory*        mEnumFactory;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_CAMPPROPERTYMANAGER_H