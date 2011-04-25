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
Interface for keeping track of camp properties.
**/
struct CampPropertyDataInterface
{
    virtual ~CampPropertyDataInterface() {}

    virtual void set( const camp::Value& rValue ) = 0;
    virtual void setWithUndo( const camp::Value& rValue ) = 0;
    virtual camp::Value get() const = 0;
    virtual camp::Type type() const = 0;
    virtual String name() const = 0;
    virtual bool ignore() const = 0;
    virtual QString valueText() const = 0;
    virtual bool hasTag( const camp::Value& rTag ) const = 0;
    virtual camp::Value tag( const camp::Value& rTag ) const = 0;
    virtual bool writable() const = 0;
    virtual CampPropertyDataInterface* clone() const = 0;
    virtual CampPropertyDataInterface* parent() const = 0;

    virtual bool lessThan(const CampPropertyDataInterface* other) const = 0;

    struct Compare 
    {
        bool operator()(const CampPropertyDataInterface* lhs, const CampPropertyDataInterface* rhs)
        {
            return lhs->lessThan( rhs );
        }
    };
};

typedef std::map<const QtProperty*, CampPropertyDataInterface*> QtPropertyDataMap;

/**
Data class to keep track of normal camp properties.
**/
struct CampPropertyData : public CampPropertyDataInterface
{
    CampPropertyData( const camp::Property& rProperty, const camp::UserObject& rObject,
        CampPropertyDataInterface* pParentPropertyData = 0 );
    ~CampPropertyData();

    inline void set( const camp::Value& rValue ) { mProperty.set( mObject, rValue ); }
    void setWithUndo( const camp::Value& rValue );
    inline camp::Value get() const { return mProperty.get( mObject ); }
    inline camp::Type type() const { return mProperty.type(); }
    inline String name() const { return mProperty.name(); }
    bool ignore() const;
    QString valueText() const;
    inline bool hasTag( const camp::Value& rTag ) const { return mProperty.hasTag( rTag ); }
    inline camp::Value tag( const camp::Value& rTag ) const { return mProperty.tag( rTag ); }
    inline bool writable() const { return mProperty.writable( mObject ); }
    CampPropertyData* clone() const;
    inline CampPropertyDataInterface* parent() const { return mParentPropertyData; }

    bool lessThan(const CampPropertyDataInterface* other) const;

    const camp::Property&       mProperty;
    camp::UserObject            mObject;
    CampPropertyDataInterface*  mParentPropertyData;
};

struct CampValueMapPropertyData : public CampPropertyDataInterface
{
    CampValueMapPropertyData( const camp::DictionaryProperty& rProperty, 
        const camp::UserObject& rObject, const String& rKey );

    inline void set( const camp::Value& rValue ) { mProperty.set( mObject, mKey, rValue ); }
    void setWithUndo( const camp::Value& rValue );
    inline camp::Value get() const { return mProperty.get( mObject, mKey ); }
    inline camp::Type type() const { return get().type(); /* TODO: Use class? */ }
    inline String name() const { return mKey; }
    inline bool ignore() const { return false; }
    inline QString valueText() const { return ""; }
    inline bool hasTag( const camp::Value& rTag ) const { return false; }
    inline camp::Value tag( const camp::Value& rTag ) const { return camp::Value::nothing; }
    inline bool writable() const { return true; }
    CampValueMapPropertyData* clone() const;
    inline CampPropertyDataInterface* parent() const { return 0; }

    bool lessThan(const CampPropertyDataInterface* other) const;

    const camp::DictionaryProperty& mProperty;
    camp::UserObject                mObject;
    String                          mKey;
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

    void setValue( QtProperty* pProperty, CampPropertyDataInterface* pData );
    template <typename T> void propertyChanged( QtProperty* pProperty, T val )
    {
        if( mBlockChangeSignal ) return;

        QtPropertyDataMap::iterator i = mSubPropertyData.find( pProperty );
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

    QtPropertyDataMap mPropertyData;
    QtPropertyDataMap mSubPropertyData;

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
    

    @return Created property or 0 if the property could not be created.
    **/
    QtProperty* addProperty( CampPropertyDataInterface* pData, QtProperty* pProperty = 0 );
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
    Adds a group property. 
    
    @param  rName   The name of the group property.
    
    @return The created group property.
    **/
    QtProperty* addGroupProperty( const String& rName );
    /**
    Sets up the change signals so that edits in the property grid get propagated to camp. This
    is called automatically in addClassTree, so only call this when using addProperty directly.

    @param  rObject The object to setup changes for.
    **/
    void setup( const camp::UserObject& rObject );
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
        QtProperty* pCompoundProperty = 0, CampPropertyDataInterface* pParentData = 0 );
    void addClassTreeInternal( const camp::Class& rClass, const camp::UserObject& rObject,
        std::set<String>& rAddedProperties, std::list<QtProperty*>& rRootProperties );
    void setBlockSlots( bool block );
    void externalPropertyChanged( CampPropertyDataInterface* pData, const camp::Value& rValue );
    void externalPropertyChanged( const camp::UserObject& rObject, const camp::Property& rProperty, 
        const camp::Value& rValue );
    template <typename T, typename U> void propertyChanged( QtProperty* pProperty, U pManager, 
        T val )
    {
        if( mBlockChangeSignal ) return;

        QtPropertyDataMap::iterator i = mData.find( pProperty );
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
            }
        }
    }
    void update();

    typedef std::map<CampPropertyDataInterface*, QtProperty*, CampPropertyDataInterface::Compare> PropertyMap;
    typedef std::set<CampPropertyDataInterface*> ContinuousUpdateMap;

    QtPropertyDataMap               mData;
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