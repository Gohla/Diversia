/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

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

#ifndef DIVERSIA_OBJECT_OBJECTTEMPLATE_H
#define DIVERSIA_OBJECT_OBJECTTEMPLATE_H

#include "Object/Platform/Prerequisites.h"

#include "Object/Object.h"
#include "Object/ObjectTemplateManager.h"
#include "Util/Math/Node.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

typedef DiversiaHashMap<String, ObjectTemplate*> ObjectTemplateChilds;
typedef std::multimap<ComponentType, ComponentTemplate*> ComponentTemplatesByType;
typedef std::map<String, ComponentTemplate*> ComponentTemplatesByName;
typedef std::map<ComponentHandle, ComponentTemplate*> ComponentTemplatesByHandle;

class DIVERSIA_OBJECT_API ObjectTemplate : public RakNet::Replica3, public Node
{
public:
    /**
    Sets this object template to local or remote mode.

    @param  type                Local or remote.
    @param  redirectToChilds    True to redirect call to all childs. Do not set this manually!

    @throws Exception   When networking type cannot be set.
    **/
    void setNetworkingType( NetworkingType type, bool redirectToChilds = false );
    /**
    Query if this object template can be set to local or remote mode.

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
    inline ObjectTemplateManager& getObjectTemplateManager() const { return mObjectTemplateManager; }
    /**
    Gets the replica manager.
    **/
    inline RakNet::ReplicaManager3& getReplicaManager() const { return mReplicaManager; }
    /**
    Gets the network ID manager.
    **/
    inline RakNet::NetworkIDManager& getNetworkIDManager() const { return mNetworkIDManager; }
    /**
    Gets the mode.
    **/
    inline Mode getMode() const { return mMode; }
    /**
    Gets the source of this object template.
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
    Query if this object template is created by our own GUID. Uses source GUID, not 
    CreatingSystemGUID.

    @return True if created by our own GUID, false if not.
    **/
    inline bool isCreatedByOwnGUID() const { return mSourceGUID == mOwnGUID; }
    /**
    Query if this object template is created by the server. Uses source GUID, not 
    CreatingSystemGUID.

    @return True if created by the server, false if not.
    **/
    inline bool isCreatedByServer() const { return mSourceGUID == mServerGUID; }
    /**
    Query if this object template is created by a given GUID.

    @return True if created by the given GUID, false if not.
    **/
    inline bool isCreatedBy( RakNet::RakNetGUID guid ) const { return Replica3::GetCreatingSystemGUID() == guid; }
    /**
    Query if this object template is created by a given GUID. Uses source GUID, not 
    CreatingSystemGUID.

    @return True if created by the given GUID, false if not.
    **/
    inline bool isCreatedBySource( RakNet::RakNetGUID guid ) const { return mSourceGUID == guid; }
    /**
    Gets the source GUID of this object template. On the client the source GUID will only be the 
    client's own GUID or the server GUID. Objects created by other clients have the server GUID as 
    source. This should only be used in situations where Replica3::CreatingSystemGUID has not been 
    set yet.
    **/
    inline RakNet::RakNetGUID getSourceGUID() const { return mSourceGUID; }
    /**
    Creates a child object template. 
    
    @param  rName   The name of the child object template. 
    **/
    ObjectTemplate& createChildObjectTemplate( const String& rName );
    /**
    Convenience function for destroying this object template.
    **/
    inline void destroyObjectTemplate() { mObjectTemplateManager.destroyObjectTemplate( mName ); }

    /**
    Queries if a delayed destruction is taking place.

    @return True if a delayed destruction is taking place, false if not.
    **/

    /**
    Creates a component template.

    @param  type            The type of the component template to create.
    @param  rName           The name of the component template, must be unique within the object 
                            template.
    @param  localOverride   True to override component template to local mode even if the 
                            object template is set to remote.
    @param  source          Source of the construction request. Defaults to this system.

    @throw  Exception   When this object template already has a component template with the same 
                        name or type, or a client defined exception.
    **/
	ComponentTemplate& createComponentTemplate( ComponentType type, const String& rName,
        bool localOverride = false, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Creates a component template.

    @param  rName           The name of the component template, must be unique within the object 
                            template.
    @param  localOverride   True to override component template to local mode even if the 
                            object template is set to remote.
    @param  source          Source of the construction request. Defaults to this system.

    @throw  Exception   When this object template already has a component template with the same 
                        name or type, or a client defined exception.
    **/
    template <class T> ComponentTemplate& createComponentTemplate( const String& rName,
        bool localOverride = false, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) ) {
        return static_cast<T&>( ObjectTemplate::createComponentTemplate( T::getTypeStatic(), 
            rName, localOverride, source ) );
    }
    /**
    Creates a component template using a handle.

    @param  rHandle     Component handle.

    @throw  Exception   When this object template already has a component template with the same 
                        name or type, or a client defined exception.
    **/
    inline ComponentTemplate& createComponentTemplate( const ComponentHandle& rHandle ) {
        return ObjectTemplate::createComponentTemplate( rHandle.mType, rHandle.mName );
    }
    /**
    Query if this object template has a component template using the component's type.

    @param  type    The type of the component.

    @return True if object template has the component template, false if not.
    **/
    bool hasComponentTemplate( ComponentType type ) const;
    /**
    Query if this object template has a component template using the component's name.

    @param  rName   The name of the component.

    @return True if object template has the component template, false if not.
    **/
    bool hasComponentTemplate( const String& rName ) const;
    /**
    Destroys a component template by type.

    @param  type    The type of the component template.
    @param  source  Source of the destruction request.

    @throw  Exception   When this object template does not have the component.
    @throw  Exception   When this object has multiple component templates of given type.
    @throw  Exception   A client defined exception.
    **/
    void destroyComponentTemplate( ComponentType type, 
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Destroys a component template by name.

    @param  rName   The name of the component template.
    @param  source  Source of the destruction request.

    @throw  Exception   When this object template does not have the component.
    @throw  Exception   When this object has multiple component templates of given type.
    @throw  Exception   A client defined exception.
    **/
    void destroyComponentTemplate( const String& rName,
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );

    /**
    Gets the parent object template.
    **/
    inline ObjectTemplate* getParentObjectTemplate() const { return static_cast<ObjectTemplate*>( Node::getParent() ); }
    /**
    Gets the root parent object template.
    **/
    inline ObjectTemplate* getRootParentObjectTemplate() { return static_cast<ObjectTemplate*>( Node::getRootParent() ); }
    /**
    Gets a copy of the childs map.
    **/
    inline ObjectTemplateChilds getChildObjectTemplates() const
    {
        ChildNodeMap childs = Node::getChildren();
        ObjectTemplateChilds objectTemplateChildren;
        for( ChildNodeMap::iterator i = childs.begin(); i != childs.end(); ++i )
        {
            objectTemplateChildren.insert( std::make_pair( i->first, static_cast<ObjectTemplate*>( 
                i->second ) ) );
        }
        return objectTemplateChildren;
    }
    /**
    Sets the parent for this object template using a pointer to the parent.

    @param  pObjectTemplate The new parent object template, or 0 to unparent.
    @param  source          Source of the parent change request. Defaults to this system.
    **/
    void parent( ObjectTemplate* pObjectTemplate, 
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Sets the parent for this object template using the name of the parent. If the template with
    given name does not exist, this object template will wait until that object is created and then 
    set that object as parent.

    @param  rName   The name of the parent, or empty string to unparent. The non changeable name of
                    the parent is used, not the display name.
    **/
    void parent( const String& rName );
    /**
    Gets the parent name of the parent object template or an empty string if this object has no 
    parent.
    **/
    String getParentName() const;

    /**
    Connects a slot to the destruction signal.

    @param [in,out] rSlot   The slot (signature: void func(ObjectTemplate&)) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectDestruction( const sigc::slot<void, ObjectTemplate&>& rSlot )
    {
        return mDestructionSignal.connect( rSlot );
    }
    /**
    Connects a slot to the component template created/destroyed signal.

    @param [in,out] rSlot   The slot (signature: void func(ComponentTemplate&, bool [true when 
                            component is created, false when destroyed])) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectComponentChange( const sigc::slot<void, ComponentTemplate&, bool>& rSlot )
    {
        return mComponentTemplateSignal.connect( rSlot );
    }
    /**
    Connects a slot to the parent/unparent signal.

    @param [in,out] rSlot   The slot (signature: void func(ObjectTemplate* [pointer to parent 
                            object or null when unparented])) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectParentChange( const sigc::slot<void, ObjectTemplate*>& rSlot )
    {
        return mParentSignal.connect( rSlot );
    }
    
protected:
    friend class ObjectTemplateManager;	///< Only the ObjectTemplateManager class may construct object templates.

    /**
    Constructor. 
    
    @param  rName                           The name of the object template. 
    @param  mode                            The mode (Client/Server) the object template will run
                                            in. 
    @param  type                            If the object template should be a remote or local
                                            object. 
    @param  rDisplayName                    The display name of the object template. 
    @param  source                          The source of this object template. 
    @param  ownGUID                         Own unique identifier. 
    @param  serverGUID                      Server unique identifier. 
    @param [in,out] rObjectTemplateManager  The object template manager that created this object. 
    @param [in,out] rReplicaManager         The replica manager. 
    @param [in,out] rNetworkIDManager       The network ID manager. 
    **/
    ObjectTemplate( const String& rName, Mode mode, NetworkingType type, const String& rDisplayName,
        RakNet::RakNetGUID source, RakNet::RakNetGUID ownGUID, RakNet::RakNetGUID serverGUID,
        ObjectTemplateManager& rObjectTemplateManager, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    /**
    Destructor. 
    **/
    virtual ~ObjectTemplate();

    /**
    Provide implementation for these functions based on permissions in the client/server.
    **/
    virtual RakNet::RM3ConstructionState QueryConstruction(
        RakNet::Connection_RM3* pDestinationConnection,
        RakNet::ReplicaManager3* pReplicaManager3 ) = 0;
    virtual bool QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection ) = 0;
    virtual RakNet::RM3QuerySerializationResult QuerySerialization(
        RakNet::Connection_RM3* pDestinationConnection ) = 0;
    virtual void SerializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pDestinationConnection ) = 0;
    virtual bool DeserializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pSourceConnection ) = 0;
    inline virtual void SerializeDestruction( RakNet::BitStream* pDestructionBitstream,
        RakNet::Connection_RM3* pDestinationConnection ) {}
    inline virtual bool DeserializeDestruction( RakNet::BitStream* pDestructionBitstream,
        RakNet::Connection_RM3* pSourceConnection ) { return true; }
    virtual RakNet::RM3SerializationResult Serialize(
        RakNet::SerializeParameters* pSerializeParameters ) = 0;
    virtual void Deserialize( RakNet::DeserializeParameters* pDeserializeParameters ) = 0;

    /**
    Not used in ObjectTemplate.
    **/
    inline virtual RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(
        RakNet::Connection_RM3* pDroppedConnection ) const { return RakNet::RM3AOPC_DO_NOTHING; }

    /**
    Query if a component template may be created.

    @param  type            The type of the component template.
    @param  localOverride   True to override component template to local mode even if the 
                            object template is set to remote.
    @param  source          Source of the creation request, if it was made on the client or remotely 
                            created by the server.

    @note   Override this in a parent class to stop creation of component templates. Throw an 
            exception if the template may not be created, the exception will be thrown back to the 
            caller of createComponentTemplate().
    **/
    virtual void queryCreateComponentTemplate( ComponentType type, bool localOverride, 
        RakNet::RakNetGUID source ) = 0;
    /**
    Implementation for creating a component template.

    @param  type            The type of the component template.
    @param  rName           The name of the component template.
    @param  localOverride   True to override component template to local mode even if the 
                            object template is set to remote.
    @param  source          Source of the creation request, if it was made on the client or remotely 
                            created by the server.

    @note   Override this in a parent class to create your own types of component templates.
    **/
    virtual ComponentTemplate* createComponentTemplateImpl( ComponentType type, const String& rName, 
        bool localOverride, RakNet::RakNetGUID source ) = 0;
    /**
    Query if a component template may be destroyed.

    @param [in,out] rComponent  The component to destroy.
    @param  source              Source of the object, if it was made on the client or remotely created
                                by the server.

    @note   Override this in a parent class to stop destruction of components. Throw an exception if
            the component may not be destroyed, the exception will be thrown back to the caller of
            destroyComponent().
    **/
    virtual void queryDestroyComponentTemplate( ComponentTemplate& rTemplate, 
        RakNet::RakNetGUID source ) = 0;
    /**
    Sets this object template to local or remote mode.

    @param  type    Local or remote.

    @throws Exception   When networking type cannot be set.
    **/
    virtual void setNetworkingTypeImpl( NetworkingType type ) = 0;
    /**
    Query if this object template can be set to local or remote mode.

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
    Query if the parent may be changed to a new parent, or no parent.

    @param  pNewParent  The new parent template, or 0 to unparent.
    @param  source      Source of the set parent request, if it was made on the client or remotely
                        created by the server.
    @note   Override this in a parent class to stop changing the parent object. Throw an exception
            if the parent may not be changed, the exception will be thrown back to the caller of
            parent().
    **/
    virtual void querySetParent( ObjectTemplate* pNewParent, RakNet::RakNetGUID source ) = 0;

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.
    friend void camp::detail::destroy<ObjectTemplate>( const UserObject& object );  ///< Allow private access for camp.

    /**
    Broadcasts this object template to the network.
    **/
    void broadcastConstruction();
    /**
    Query if the object template should be destroyed on the network before being destroyed locally.
    **/
    bool queryBroadcastDestruction();
    /**
    Destroys this object template from the network.
    **/
    void broadcastDestruction();
    /**
    Removes a component template by type.
    **/
    void removeComponentTemplate( ComponentType type );
    /**
    Removes a component template by name.
    **/
    void removeComponentTemplate( const String& rName );
    /**
    Notification for parent change.
    **/
    void setParent( Node* pParent );
    /**
    Object change notification.
    **/
    void objectTemplateChange( ObjectTemplate& rObjectTemplate, bool created );

    /**
    Implemented by ObjectTemplate.
    **/
    void WriteAllocationID( RakNet::Connection_RM3* pConnection,
        RakNet::BitStream* pAllocationIdBitstream ) const;
    void DeallocReplica( RakNet::Connection_RM3* pSourceConnection );

    const String		                mName;
    String                              mDisplayName;
    Mode				                mMode;
    RakNet::RakNetGUID                  mOwnGUID;
    RakNet::RakNetGUID                  mServerGUID;
    Source				                mSource;
    RakNet::RakNetGUID                  mSourceGUID;
    NetworkingType 		                mType;
    sigc::signal<void, ObjectTemplate&> mDestructionSignal;
    bool                                mBroadcastingDestruction;

    ObjectTemplateManager&		                    mObjectTemplateManager;
    ComponentTemplatesByType                        mComponentTemplatesByType;
    ComponentTemplatesByName                        mComponentTemplatesByName;
    ComponentTemplatesByHandle                      mComponentTemplatesByHandle;
    sigc::signal<void, ComponentTemplate&, bool>    mComponentTemplateSignal;

    bool                                mParentChanged;
    sigc::signal<void, ObjectTemplate*> mParentSignal;
    String                              mQueuedParentName;
    sigc::connection                    mQueuedParentConnection;

    RakNet::ReplicaManager3&	mReplicaManager;
    RakNet::NetworkIDManager&	mNetworkIDManager;

    CAMP_RTTI()
};

//------------------------------------------------------------------------------
} // Namespace Object
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::ObjectSystem::ObjectTemplate,
    &Diversia::ObjectSystem::Bindings::CampBindings::bindObjectTemplate );

#endif // DIVERSIA_OBJECT_OBJECTTEMPLATE_H