/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_OBJECT_OBJECT_H
#define DIVERSIA_OBJECT_OBJECT_H

#include "Object/Platform/Prerequisites.h"

#include "Object/ObjectManager.h"
#include "Util/Math/Node.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

typedef DiversiaHashMap<String, Object*> ObjectChilds;
typedef std::multimap<ComponentType, Component*> ComponentsByType;
typedef std::map<String, Component*> ComponentsByName;

/**
Component handle consisting of the component's type and name. Unique within an object.
**/
struct DIVERSIA_OBJECT_API ComponentHandle
{
    /**
    Default constructor.
    **/
    ComponentHandle(): mType( 0 ), mName() {}
    /**
    Constructor.

    @param  type    The component type.
    @param  rName   The name of the component.
    **/
    ComponentHandle( ComponentType type, const String& rName ): mType( type ),
        mName( rName ) {}

    bool operator<( const ComponentHandle& lhs ) const
    {
        return ( mType < lhs.mType || !( lhs.mType < mType ) && mName < lhs.mName );
    }

    ComponentType   mType;
    String          mName;
};

typedef std::map<ComponentHandle, Component*> ComponentsByHandle;

/**
An object that manages components.
**/
class DIVERSIA_OBJECT_API Object : public RakNet::Replica3, public Node, public boost::noncopyable,
    public sigc::trackable
{
public:
    /**
    Sets this object to local or remote mode.

    @param  type                Local or remote.
    @param  redirectToChilds    True to redirect call to all childs. Do not set this manually!

    @throws Exception   When networking type cannot be set.
    **/
    void setNetworkingType( NetworkingType type, bool redirectToChilds = false );
    /**
    Query if this object can be set to local or remote mode.

    @param  type                Local or remote.
    @param  redirectToChilds    True to redirect call to all childs. Do not set this manually!

    @throws Exception   When networking type cannot be set.
    **/
    void querySetNetworkingType( NetworkingType type, bool redirectToChilds = false );
    /**
    Gets the networking (local/remote) type.
    **/
    inline NetworkingType getNetworkingType() const { return mType; }

    /**
    Gets the name for this object.
    **/
    inline const String& getName() const { return mName; }
    /**
    Gets the display name for this object.
    **/
    inline const String& getDisplayName() const { return mDisplayName; }
    /**
    Sets the display name for this object.
    **/
    void setDisplayName( const String& rDisplayName );
    /**
    Gets the object manager this object belongs to.
    **/
    inline ObjectManager& getObjectManager() const { return mObjectManager; }
    /**
    Gets the replica manager.
    **/
    inline RakNet::ReplicaManager3& getReplicaManager() const { return mReplicaManager; }
    /**
    Gets the network ID manager.
    **/
    inline RakNet::NetworkIDManager& getNetworkIDManager() const { return mNetworkIDManager; }
    /**
    Queries if a delayed destruction is taking place.

    @return True if a delayed destruction is taking place, false if not.
    **/
    inline bool queryDelayedDestruction() const { return !mDelayedDestruction.empty(); }
    /**
    Gets the mode.
    **/
    inline Mode getMode() const { return mMode; }
    /**
    Gets the source of this object.
    **/
    inline Source getSource() const { return mSource; }
    /**
    Gets our own unique identifier.
    **/
    inline RakNet::RakNetGUID getOwnGUID() const { return mOwnGUID; }
    /**
    Gets the server unique identifier.
    **/
    inline RakNet::RakNetGUID getServerGUID() const { return mServerGUID; }
    /**
    Gets the frame/tick update signal.
    **/
    inline sigc::signal<void>& getUpdateSignal() { return mUpdateSignal; }
    /**
    Query if this object is created by our own GUID. Uses source GUID, not CreatingSystemGUID.

    @return True if created by our own GUID, false if not.
    **/
    inline bool isCreatedByOwnGUID() const { return mSourceGUID == mOwnGUID; }
    /**
    Query if this object is created by the server. Uses source GUID, not CreatingSystemGUID.

    @return True if created by the server, false if not.
    **/
    inline bool isCreatedByServer() const { return mSourceGUID == mServerGUID; }
    /**
    Query if this object is created by a given GUID.

    @return True if created by the given GUID, false if not.
    **/
    inline bool isCreatedBy( RakNet::RakNetGUID guid ) const { return Replica3::GetCreatingSystemGUID() == guid; }
    /**
    Query if this object is created by a given GUID. Uses source GUID, not CreatingSystemGUID.

    @return True if created by the given GUID, false if not.
    **/
    inline bool isCreatedBySource( RakNet::RakNetGUID guid ) const { return mSourceGUID == guid; }
    /**
    Gets the source GUID of this object. On the client the source GUID will only be the client's
    own GUID or the server GUID. Objects created by other clients have the server GUID as source.
    This should only be used in situations where Replica3::CreatingSystemGUID has not been set yet.
    **/
    inline RakNet::RakNetGUID getSourceGUID() const { return mSourceGUID; }
    /**
    Sets this object to be a runtime object which will not be serialized and cannot be deleted
    by editors.
    
    @param  runtime True to set to a runtime object, false to set to a normal object.
    **/
    inline void setRuntimeObject( bool runtime = true ) { mRuntime = runtime; }
    /**
    Query if this object is runtime object. 
    
    @return True if runtime object, false if not. 
    **/
    inline bool isRuntimeObject() const { return mRuntime; }
    /**
    Creates a child object. 
    
    @param  rName   The name of the child object. 
    **/
    Object& createChildObject( const String& rName );
    /**
    Duplicates this object and all its components.
    
    @param  rName           The name of the new object. Empty to generate name.
    
    @return A duplicate of this object. 
    **/
    Object& duplicate( const String& rName = "" );
    /**
    Convenience function for destroying this object.
    **/
    inline void destroyObject() { mObjectManager.destroyObject( mName ); }

    /**
    Creates a component. The component specific part is initialized in the next update tick.

    @param  type            The type of the component to create.
    @param  rName           The name of the component, must be unique within the object.
    @param  localOverride   True to override component to local mode even if the object is a
                            remote object.
    @param  source          Source of the construction request. Defaults to this system.

    @throw  Exception   When this object already has that type of component, or a client defined
                        exception.
    **/
    Component& createComponent( ComponentType type, const String& rName, bool localOverride = false,
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Creates a component.

    @param  rName           The name of the component, must be unique within the object.
    @param  localOverride   True to override component to local mode even if the object is a
                            remote object.
    @param  source          Source of the construction request. Defaults to this system.

    @throw  Exception   When this object already has that type of component, or a client defined
                        exception.
    **/
    template <class T> T& createComponent( const String& rName, bool localOverride = false,
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) )
    {
        return static_cast<T&>( Object::createComponent( T::getTypeStatic(), rName, localOverride,
            source ) );
    }
    /**
    Creates a component using a handle. The component specific part is initialized in the next
    update tick.

    @param  rHandle     Component handle.

    @throw  Exception   When this object already has that type of component, or a client defined
                        exception.
    **/
    inline Component& createComponent( const ComponentHandle& rHandle ) {
        return Object::createComponent( rHandle.mType, rHandle.mName );
    }
    /**
    Gets a component using the component's type. This will only return the first found component if
    multiple components are allowed for the given type, use Object::getComponents to get all
    components.

    @param  type    The type of the component.

    @throw  Exception   When the component cannot be found in this object.
    **/
    Component& getComponent( ComponentType type ) const;
    /**
    Gets a component using the component's C++ type as template parameter. This will only return
    the first found component if multiple components are allowed for the given type, use
    Object::getComponents to get all components.

    @throw  Exception   When the component cannot be found in this object.
    **/
    template<class T> inline T& getComponent() const
    {
        return static_cast<T&>( Object::getComponent( T::getTypeStatic() ) );
    }
    /**
    Gets a component by name.

    @param  rName   The name of the component.

    @throw  Exception   When the component cannot be found in this object.
    **/
    Component& getComponent( const String& rName ) const;
    /**
    Gets a component by name and cast it to the correct type.

    @param  rName   The name of the component.

    @throw  Exception   When the component cannot be found in this object.
    **/
    template<class T> inline T& getComponentCast( const String& rName ) const
    {
        return static_cast<T&>( Object::getComponent( rName ) );
    }
    /**
    Gets multiple components using the component's C++ type as template parameter.

    @return A beginning and end iterator to go through the found components.
    **/
    template<class T> inline std::pair<ComponentsByType::iterator, ComponentsByType::iterator>
        getComponents()
    {
        return mComponentsByType.equal_range( T::getTypeStatic() );
    }
    /**
    Gets multiple components using the component's type.

    @param  type    The type of the component.

    @return A beginning and end iterator to go through the found components.
    **/
    inline std::pair<ComponentsByType::iterator, ComponentsByType::iterator> getComponents(
        ComponentType type ) { return mComponentsByType.equal_range( type ); }
    /**
    Gets the components by type map.
    **/
    inline const ComponentsByType& getComponentsByType() const { return mComponentsByType; }
    /**
    Gets the components by name map.
    **/
    inline const ComponentsByName& getComponentsByName() const { return mComponentsByName; }
    /**
    Query if this object has a component using the component's C++ type as template parameter.

    @return true if object has the component, false if not.
    **/
    template<class T> inline bool hasComponent() const
    {
        if ( mComponentsByType.find( T::getTypeStatic() ) != mComponentsByType.end() )
        {
            return true;
        }
        return false;
    }
    /**
    Query if this object has a component using the component's type.

    @param  type    The type of the component.

    @return True if object has the component, false if not.
    **/
    bool hasComponent( ComponentType type ) const;
    /**
    Query if this object has a component using the component's name.

    @param  rName   The name of the component.

    @return True if object has the component, false if not.
    **/
    bool hasComponent( const String& rName ) const;
    /**
    Returns how many components of given type are in the object.

    @param  type    The type of the components.
    **/
    std::size_t componentCount( ComponentType type ) const;
    /**
    Destroys a component by type. The component will be destroyed in the next update frame/tick
    signal.

    @param  type    The type of the component.
    @param  source  Source of the destruction request.

    @throw  Exception   When this object does not have the component.
    @throw  Exception   When this object has multiple components of given type.
    @throw  Exception   A client defined exception.
    **/
    void destroyComponent( ComponentType type, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Destroys a component by name. The component will be destroyed in the next update frame/tick
    signal.

    @param  rName   The name of the component.
    @param  source  Source of the destruction request.

    @throw  Exception   When this object does not have the component, or a client defined exception.
    **/
    void destroyComponent( const String& rName, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Destroys all components of given type. The component will be destroyed in the next update
    frame/tick signal.

    @param  type    The type of the components.
    @param  source  Source of the destruction request.
    **/
    void destroyComponents( ComponentType type, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Adds a component to the auto create component list. All components in that list will be
    automatically created locally for every object that is constructed.

    @param  rName   The name the component will be given.
    **/
    template<class T> static void addAutoCreateComponent( const String& rName )
    {
        Object::addAutoCreateComponent( T::getTypeStatic(), rName );
    }
    /**
    Adds a component to the auto create component list. All components in that list will be
    automatically created locally for every object that is constructed.

    @param  type    The type of the component.
    @param  rName   The name the component will be given.
    **/
    static void addAutoCreateComponent( ComponentType type, const String& rName );
    /**
    Query if given component type will be automatically created on object creation.
    
    @param  type    The component type. 
    
    @return True if given component type will be automatically created on object creation, false
            otherwise.       
    **/
    static bool hasAutoCreateComponent( ComponentType type );

    /**
    Sets the transform to be (client) controlled. The client can start serializing this object's
    transformation.

    @param  controller  The controller's GUID or UNASSIGNED_RAKNET_GUID to disable.
    **/
    void setClientControlled( RakNet::RakNetGUID controller = RakNet::UNASSIGNED_RAKNET_GUID );
    /**
    Sets the transform to be controlled by this client. This client can start serializing this
    object's transformation.
    **/
    inline void setThisClientControlled() { setClientControlled( mOwnGUID ); }
    /**
    Gets the controller of this object.

    @return The client controller or UNASSIGNED_RAKNET_GUID if the server
    **/
    RakNet::RakNetGUID getClientController() const { return mController; }
    /**
    Query if this object is client controlled.
    **/
    inline bool isClientControlled() const { return mController != RakNet::UNASSIGNED_RAKNET_GUID; }
    /**
    Query if this object is server controlled.
    **/
    inline bool isServerControlled() const { return mController == RakNet::UNASSIGNED_RAKNET_GUID; }
    /**
    Query if this object is controlled by the current application.
    **/
    inline bool isThisControlled() const { return mController == mOwnGUID; }

    /**
    Gets the parent object.
    **/
    inline Object* getParentObject() const { return static_cast<Object*>( Node::getParent() ); }
    /**
    Gets the root parent object.
    **/
    inline Object* getRootParentObject() { return static_cast<Object*>( Node::getRootParent() ); }
    /**
    Gets a copy of the childs map.
    **/
    inline ObjectChilds getChildObjects() const
    {
        ChildNodeMap childs = Node::getChildren();
        ObjectChilds objectChilds;
        for( ChildNodeMap::iterator i = childs.begin(); i != childs.end(); ++i )
        {
            objectChilds.insert( std::make_pair( i->first, static_cast<Object*>( i->second ) ) );
        }
        return objectChilds;
    }
    /**
    Sets the parent object for this object using a pointer to the parent object.

    @param  pObject The new parent object, or 0 to unparent.
    @param  source  Source of the parent change request. Defaults to this system.
    **/
    void parent( Object* pObject, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Sets the parent object for this object using the name of the parent object. If the object with
    given name does not exist, this object will wait until that object is created and then set that
    object as parent.

    @param  rName   The name of the parent, or empty string to unparent. The non changeable name of
                    the parent is used, not the display name.
    **/
    void parent( const String& rName );
    /**
    Gets the parent name of the parent object or an empty string if this object has no parent.
    **/
    String getParentName() const;
    /**
    Sets the object template.
    
    @param [in,out] pTemplate   The object template.
    **/
    void setTemplate( ObjectTemplate* pTemplate );
    /**
    Sets the object template for this object using the name of the object template.
    
    @param  rName   The name of the template, or empty string to remove template. The non 
                    changeable name of the template is used, not the display name.
    **/
    void setTemplate( const String& rName );
    /**
    Gets the object template for this object.
        
    @return The object template or 0 if this object has no template.
    **/
    inline ObjectTemplate* getTemplate() const { return mTemplate; }
    /**
    Gets the name of the template for this object or an empty string if this object has no template.
    **/
    String getTemplateName() const;

    /**
    Connects a slot to the destruction signal.

    @param [in,out] rSlot   The slot (signature: void func(Object& [this object])) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectDestruction( const sigc::slot<void, Object&>& rSlot )
    {
        return mDestructionSignal.connect( rSlot );
    }
    /**
    Connects a slot to the component created/destroyed signal.

    @param [in,out] rSlot   The slot (signature: void func(Component&, bool [true when component is
                            created, false when destroyed])) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectComponentChange( const sigc::slot<void, Component&, bool>& rSlot )
    {
        return mComponentSignal.connect( rSlot );
    }
    /**
    Connects a slot to the controller changed signal

    @param [in,out] rSlot   The slot (signature: void func(RakNet::RakNetGUID)) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectControllerChange(
        const sigc::slot<void, RakNet::RakNetGUID>& rSlot )
    {
        return mControllerSignal.connect( rSlot );
    }
    /**
    Connects a slot to the parent/unparent signal.

    @param [in,out] rSlot   The slot (signature: void func(Object* [pointer to parent object or
                            null when unparented])) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectParentChange( const sigc::slot<void, Object*>& rSlot )
    {
        return mParentSignal.connect( rSlot );
    }
    /**
    Connects a slot to the display name changed signal.

    @param [in,out] rSlot   The slot (signature: void func(const String& [name])) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectDisplayNameChange( const sigc::slot<void, const String&>& rSlot )
    {
        return mDisplayNameSignal.connect( rSlot );
    }

protected:
    friend class ObjectManager;	///< Only the ObjectManager class may construct objects.
    friend class Component;	///< For delayed destruction calls.

    /**
    Constructor.

    @param  rName                       The name of the object.
    @param  mode                        The mode (Client/Server) the object will run in.
    @param  type                        If the object should be a remote or local object. (Local
                                        means the object will not be propagated to the server)
    @param  rDisplayName                The display name of the object.
    @param  source                      The source of this object.
    @param  ownGUID                     Own unique identifier.
    @param  serverGUID                  Server unique identifier.
    @param [in,out] rUpdateSignal       The frame/tick update signal.
    @param [in,out] rObjectManager      The object manager that created this object.
    @param [in,out] rReplicaManager     The replica manager.
    @param [in,out] rNetworkIDManager   The network ID manager.
    **/
    Object( const String& rName, Mode mode, NetworkingType type, const String& rDisplayName,
        RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, RakNet::RakNetGUID serverGUID,
        sigc::signal<void>& rUpdateSignal, ObjectManager& rObjectManager,
        RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager,
        RakNet::RPC3& rRPC3 );
    /**
    Destructor.
    **/
    virtual ~Object();
    /**
    Query if a component may be created.

    @param  type            The type of the component.
    @param  source          Source of the object, if it was made on the client or remotely created
                            by the server.
    @param  localOverride   True to override component to local mode even if the object is a
                            remote object.

    @note   Override this in a parent class to stop creation of components. Throw an exception if
            the component may not be created, the exception will be thrown back to the caller of
            createComponent().
    **/
    virtual void queryCreateComponent( ComponentType type, RakNet::RakNetGUID source,
        bool localOverride ) = 0;
    /**
    Query if a component may be destroyed.

    @param [in,out] rComponent  The component to destroy.
    @param  source          Source of the object, if it was made on the client or remotely created
                            by the server.
    @note   Override this in a parent class to stop destruction of components. Throw an exception if
            the component may not be destroyed, the exception will be thrown back to the caller of
            destroyComponent().
    **/
    virtual void queryDestroyComponent( Component& rComponent, RakNet::RakNetGUID source ) = 0;
    /**
    Sets this object to local or remote mode.

    @param  type    Local or remote.

    @throws Exception   When networking type cannot be set.
    **/
    virtual void setNetworkingTypeImpl( NetworkingType type ) = 0;
    /**
    Query if this object can be set to local or remote mode.

    @param  type    Local or remote.

    @throws Exception   When networking type cannot be set.
    **/
    virtual void querySetNetworkingTypeImpl( NetworkingType type ) = 0;
    /**
    Cleanup the changes made by querySetNetworkingTypeImpl.

    @param  type    Local or remote.
    **/
    virtual void cleanupQuerySetNetworkingType( NetworkingType type ) = 0;
    /**
    Query if the parent object may be changed to a new parent object, or no parent object.

    @param  pNewParent  The new parent object, or 0 to unparent.
    @param  source      Source of the set parent request, if it was made on the client or remotely
                        created by the server.
    @note   Override this in a parent class to stop changing the parent object. Throw an exception
            if the parent may not be changed, the exception will be thrown back to the caller of
            parent().
    **/
    virtual void querySetParent( Object* pNewParent, RakNet::RakNetGUID source ) = 0;
    /**
    Destroys components that are ready for destruction and initialize components that were
    created in the previous tick.
    **/
    void update();

    /**
    Provide implementation for these functions based on permissions in the client/server.
    **/
    virtual RakNet::RM3ConstructionState QueryConstruction(
        RakNet::Connection_RM3* pDestinationConnection,
        RakNet::ReplicaManager3* pReplicaManager3 ) = 0;
    virtual bool QueryRemoteConstruction ( RakNet::Connection_RM3* pSourceConnection ) = 0;

    /**
    Not used in Object.
    **/
    virtual void SerializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pDestinationConnection );
    virtual bool DeserializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pSourceConnection );
    inline virtual void SerializeDestruction( RakNet::BitStream* pDestructionBitstream,
        RakNet::Connection_RM3* pDestinationConnection ) {}
    virtual bool DeserializeDestruction( RakNet::BitStream* pDestructionBitstream,
        RakNet::Connection_RM3* pSourceConnection );
    inline virtual RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(
        RakNet::Connection_RM3* pDroppedConnection ) const { return RakNet::RM3AOPC_DO_NOTHING; }
    virtual RakNet::RM3QuerySerializationResult QuerySerialization(
        RakNet::Connection_RM3* pDestinationConnection );
    virtual RakNet::RM3SerializationResult Serialize(
        RakNet::SerializeParameters* pSerializeParameters );
    virtual void Deserialize( RakNet::DeserializeParameters* pDeserializeParameters );

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.
    friend void camp::detail::destroy<Object>( const UserObject& object );  ///< Allow private access for camp.

    /**
    Starts delayed destruction. Will ask all components if a delayed destruction is needed, if any
    of the components return true then a delayed destruction will take place. Once a component is
    done it will call readyForDestruction(). If all components are ready for destruction,
    readyForDestruction() on the object manager will be called.

    @return True if delayed destruction is necessary, false if not.
    **/
    bool delayedDestruction();
    /**
    Tells the object that a component is ready for destruction when a delayed destruction is
    taking place.

    @param  rComponent  The component.
    **/
    void readyForDestruction( Component& rComponent );
    /**
    Broadcasts this object to the network.
    **/
    void broadcastConstruction();
    /**
    Query if the object should be destroyed on the network before being destroyed locally.
    **/
    bool queryBroadcastDestruction();
    /**
    Destroys this object from the network.
    **/
    void broadcastDestruction();
    /**
    Removes a component by type.
    **/
    void removeComponent( ComponentType type );
    /**
    Removes a component by name.
    **/
    void removeComponent( const String& rName );
    /**
    Called after the object is created.
    **/
    void create();
    /**
    Notification for parent change.
    **/
    void setParent( Node* pParent );
    /**
    Object change notification.
    **/
    void objectChange( Object& rObject, bool created );

    /**
    Gets the parent object by reference.
    **/
    inline Object& getParentObjectRef() const { return *static_cast<Object*>( Node::getParent() ); }

    /**
    Implemented by Object.
    **/
    void WriteAllocationID( RakNet::Connection_RM3* pConnection,
        RakNet::BitStream* pAllocationIdBitstream ) const;
    void DeallocReplica( RakNet::Connection_RM3* pSourceConnection );

    const String								mName;
    String                                      mDisplayName;
    bool                                        mDisplayNameChanged;
    sigc::signal<void, const String&>		    mDisplayNameSignal;
    Mode							            mMode;
    RakNet::RakNetGUID                          mOwnGUID;
    RakNet::RakNetGUID                          mServerGUID;
    Source						                mSource;
    RakNet::RakNetGUID                          mSourceGUID;
    NetworkingType 				                mType;
    sigc::signal<void, Object&>                 mDestructionSignal;
    bool                                        mBroadcastingDestruction;
    bool                                        mRuntime;

    ComponentsByType							mComponentsByType;
    ComponentsByName							mComponentsByName;
    ComponentsByHandle                          mComponentsByHandle;
    std::set<Component*>						mDelayedDestruction;
    ComponentsByType                            mDestroyedComponents;
    ComponentsByType                            mCreatedComponents;
    ObjectManager&								mObjectManager;
    ObjectTemplateManager&                      mObjectTemplateManager;
    sigc::signal<void, Component&, bool>		mComponentSignal;
    static std::map<ComponentType, String>      mAutoCreateComponents;

    bool                                        mParentChanged;
    sigc::signal<void, Object*>                 mParentSignal;
    String                                      mQueuedParentName;
    sigc::connection                            mQueuedParentConnection;
    
    ObjectTemplate*                             mTemplate;

    RakNet::RakNetGUID                          mController;
    sigc::signal<void, RakNet::RakNetGUID>      mControllerSignal;

    RakNet::ReplicaManager3&					mReplicaManager;
    RakNet::NetworkIDManager&					mNetworkIDManager;
    RakNet::RPC3&                               mRPC3;
    sigc::signal<void>&                         mUpdateSignal;
    sigc::connection                            mUpdateConnection;

    CAMP_RTTI()
};

//------------------------------------------------------------------------------
} // Namespace Object
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::ObjectSystem::Object,
    &Diversia::ObjectSystem::Bindings::CampBindings::bindObject );

CAMP_AUTO_TYPE( Diversia::ObjectSystem::ComponentHandle,
    &Diversia::ObjectSystem::Bindings::CampBindings::bindComponentHandle );

#endif