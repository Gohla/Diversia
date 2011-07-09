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

#ifndef DIVERSIA_OBJECT_OBJECTMANAGER_H
#define DIVERSIA_OBJECT_OBJECTMANAGER_H

#include "Object/Platform/Prerequisites.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

typedef std::map<String, Object*> Objects;

/**
Manages objects and component factories. Objects may only be created and destroyed through this
manager. Can be inherited to create custom object types and regulate construction/destruction of
objects.
**/
class DIVERSIA_OBJECT_API ObjectManager: public boost::noncopyable, public sigc::trackable
{
public:
    /**
    Constructor.

    @param  mode                            The mode the object manager must run in.
    @param  ownGUID                         Own unique identifier.
    @param  serverGUID                      Server unique identifier.
    @param [in,out] rUpdateSignal           The frame/tick update signal.
    @param [in,out] rLateUpdateSignal       The late frame/tick update signal.
    @param [in,out] rObjectTemplateManager  The object template manager.
    @param [in,out] rReplicaManager         Replica manager.
    @param [in,out] rNetworkIDManager       Network ID manager.
    @param [in,out] rRPC3                   RPC3 object.
    @param  offlineMode                     True to set offline mode. Defaults to false.
    **/
    ObjectManager( Mode mode, RakNet::RakNetGUID ownGUID, RakNet::RakNetGUID serverGUID,
        sigc::signal<void>& rUpdateSignal, sigc::signal<void>& rLateUpdateSignal, 
        ObjectTemplateManager& rObjectTemplateManager, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager, RakNet::RPC3& rRPC3, bool offlineMode = false );
    /**
    Destructor.
    **/
    virtual ~ObjectManager();

    /**
    Creates an object.

    @param  rName           The name of the object.
    @param  type            The (local/remote) type of the object, defaults to local.
    @param  rDisplayName    The display name of the object, defaults to the name of the object.
    @param  source          Source of the construction request. Leave blank to use this system's
                            GUID.

    @throw  Exception   When an object with that name already exists, or a client defined exception.

    @return The created object.
    **/
    Object& createObject( const String& rName, NetworkingType type = LOCAL,
        const String& rDisplayName = "", RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Creates a runtime object that is not serialized or shown in editors. Use this for objects that
    are created by game modes or scripts so that they are not destroyed by users.

    @param  rName           The name of the object.
    @param  type            The (local/remote) type of the object, defaults to local.
    @param  rDisplayName    The display name of the object, defaults to the name of the object.
    @param  source          Source of the construction request. Leave blank to use this system's
                            GUID.

    @throw  Exception   When an object with that name already exists, or a client defined exception.

    @return The created object.
    **/
    Object& createRuntimeObject( const String& rName, NetworkingType type = LOCAL,
        const String& rDisplayName = "", RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Gets an object by name.

    @param  rName   The name of the object.

    @throw  Exception   When object doesn't exist.
    **/
    Object& getObject( const String& rName ) const;
    /**
    Gets the map of objects.
    **/
    inline const Objects& getObjects() const { return mObjects; }
    /**
    Query if an object exists.

    @param  rName   The name of the object.

    @return true if object exists, false if not.
    **/
    bool hasObject( const String& rName ) const;
    /**
    Destroys an object by reference. The object will be destroyed in the next update tick.

    @param  rObject Reference to the object.
    @param  source  Source of the destruction request. Leave blank to use this system's GUID.

    @throw  Exception   Can throw a client defined exception.
    **/
    void destroyObject( Object& rObject, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Destroys an object by name. The object will be destroyed in the next update tick.

    @param  rName   The name of the object.
    @param  source  Source of the destruction request. Leave blank to use this system's GUID.

    @throw  Exception   When an object with that name doesn't exist, or a client defined exception.
    **/
    inline void destroyObject( const String& rName,
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) )
    {
        ObjectManager::destroyObject( ObjectManager::getObject( rName ), source );
    }
    /**
    Destroys a tree of objects by reference. The objects will be destroyed in the next update tick.

    @param  rObject             Reference to the object.
    @param  source              Source of the destruction request. Leave blank to use this
                                system's GUID.
    @param  redirectToChilds    True to redirect call to all childs. Do not set this manually!

    @throw  Exception   Can throw a client defined exception.
    **/
    void destroyObjectTree( Object& rObject, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ),
        bool redirectToChilds = false );
    /**
    Destroys a tree of objects by name. The objects will be destroyed in the next update tick.

    @param  rName               The name of the object.
    @param  source              Source of the destruction request. Leave blank to use this
                                system's GUID.
    @param  redirectToChilds    True to redirect call to all childs. Do not set this manually!

    @throw  Exception   Can throw a client defined exception.
    **/
    inline void destroyObjectTree( const String& rName,
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ), bool redirectToChilds = false )
    {
        ObjectManager::destroyObjectTree( ObjectManager::getObject( rName ), source,
            redirectToChilds );
    }
    /**
    Destroys all objects in the object tree that the referenced object is part of. The objects will
    be destroyed in the next update tick.

    @param  rObject             Reference to the object.
    @param  source              Source of the destruction request. Leave blank to use this
                                system's GUID.
    @param  redirectToChilds    True to redirect call to all childs. Do not set this manually!

    @throw  Exception   Can throw a client defined exception.
    **/
    void destroyWholeObjectTree( Object& rObject, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ),
        bool redirectToChilds = false );
    /**
    Destroys all objects in the object tree that the named object is part of. The objects will be
    destroyed in the next update tick.

    @param  rName               The name of the object.
    @param  source              Source of the destruction request. Leave blank to use this
                                system's GUID.
    @param  redirectToChilds    True to redirect call to all childs. Do not set this manually!

    @throw  Exception   Can throw a client defined exception.
    **/
    inline void destroyWholeObjectTree( const String& rName,
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ), bool redirectToChilds = false )
    {
        ObjectManager::destroyWholeObjectTree( ObjectManager::getObject( rName ), source,
            redirectToChilds );
    }
    /**
    Immediately removes all all objects.
    **/
    void reset();
    /**
    Gets the mode.
    **/
    inline Mode getMode() const { return mMode; }
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
    Gets the object template manager.
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
    Gets the RPC3 object.
    **/
    inline RakNet::RPC3& getRPC3() const { return mRPC3; }

    /**
    Connects a slot to the object created/destroyed signal.

    @param [in,out] rSlot   The slot (signature: void func(Object&, bool [true when object is
                            created, false when destroyed])) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connect( const sigc::slot<void, Object&, bool>& rSlot )
    {
        return mObjectSignal.connect( rSlot );
    }

protected:
    friend class Object;	///< For delayed destruction.

    /**
    Implementation for creating an object.

    @param  rName           The name of the object.
    @param  type            The (local/remote) type of the object, defaults to local.
    @param  rDisplayName    The display name of the object, defaults to the name of the object.
    @param  source          Source of the construction request. Leave blank to use this system's
                            GUID.

    @note   Override this in a parent class to create your own types of objects. Throw an exception
            if the object may not be created, the exception will be thrown back to the caller of
            createObject() and the object will not be created.
    **/
    virtual Object& createObjectImpl( const String& rName, NetworkingType type,
        const String& rDisplayName, RakNet::RakNetGUID source ) = 0;
    /**
    Query if an object may be destroyed.

    @param [in,out] pObject The object to destroy.
    @param  source  Source of the destruction request.

    @note   Override this in a parent class to stop destruction of objects. Throw an exception if
            the object may not be destroyed, the exception will be thrown back to the caller of
            destroyObject() and the object will not be destroyed.
    **/
    virtual void queryDestroyObject( Object& rObject, RakNet::RakNetGUID source ) = 0;
    /**
    Called if the offline mode is changed.

    @param  offlineMode True if set to offline mode, false if set to online mode.
    **/
    void offlineModeChanged( bool offlineMode );

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    /**
    Tells the object manager that an object is ready for destruction when a delayed destruction is
    taking place.

    @param  rObject The object.
    **/
    void readyForDestruction( Object& rObject );
    /**
    Destroys objects that are ready for destruction.
    **/
    void update();
    /**
    Updates nodes.
    **/
    void lateUpdate();

    Mode                                mMode;
    bool                                mOfflineMode;
    RakNet::RakNetGUID                  mOwnGUID;
    RakNet::RakNetGUID                  mServerGUID;
    Objects                             mObjects;
    std::set<Object*>                   mDestroyedObjects;
    sigc::signal<void, Object&, bool>   mObjectSignal;
    sigc::signal<void>&                 mUpdateSignal;
    sigc::connection                    mUpdateConnection;

    ObjectTemplateManager&              mObjectTemplateManager;
    RakNet::ReplicaManager3&            mReplicaManager;
    RakNet::NetworkIDManager&           mNetworkIDManager;
    RakNet::RPC3&                       mRPC3;

    CAMP_RTTI()
};

//------------------------------------------------------------------------------
} // Namespace Object
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::ObjectSystem::ObjectManager,
    &Diversia::ObjectSystem::Bindings::CampBindings::bindObjectManager );

#endif