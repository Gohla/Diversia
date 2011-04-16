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

#ifndef DIVERSIA_OBJECT_OBJECTTEMPLATEMANAGER_H
#define DIVERSIA_OBJECT_OBJECTTEMPLATEMANAGER_H

#include "Object/Platform/Prerequisites.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

typedef std::map<String, ObjectTemplate*> ObjectTemplates;

/**
Manages object templates and component factories. Object templates may only be created and destroyed
through this manager. Can be inherited to create custom object template types and regulate
construction/destruction of object templates. 
**/
class DIVERSIA_OBJECT_API ObjectTemplateManager: public boost::noncopyable
{
public:
    /**
    Constructor.

    @param  mode                        The mode the object template manager must run in.
    @param  ownGUID                     Own unique identifier.
    @param  serverGUID                  Server unique identifier.
    @param [in,out] rReplicaManager     Replica manager.
    @param [in,out] rNetworkIDManager   Network ID manager.
    @param  offlineMode                 True to set offline mode. Defaults to false.
    **/
    ObjectTemplateManager( Mode mode, RakNet::RakNetGUID ownGUID, RakNet::RakNetGUID serverGUID,
        RakNet::ReplicaManager3& rReplicaManager, RakNet::NetworkIDManager& rNetworkIDManager, 
        bool offlineMode = false );
    /**
    Destructor.
    **/
    virtual ~ObjectTemplateManager();

    /**
    Creates an object template.

    @param  rName           The name of the object template.
    @param  type            The (local/remote) type of the object template, defaults to local.
    @param  rDisplayName    The display name of the object template, defaults to the name.
    @param  source          Source of the construction request. Leave blank to use this system's
                            GUID.

    @throw  Exception   When an object template with that name already exists, or a client defined 
                        exception.

    @return The created object template.
    **/
    ObjectTemplate& createObjectTemplate( const String& rName, NetworkingType type = LOCAL,
        const String& rDisplayName = "", RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Creates an object template from an existing object. This will also create component templates
    inside the created object template.

    @param  rObject     The object to create a template from.
    @param  rName       The name of the object template.
    @param  type        The (local/remote) type of the object template, defaults to local.

    @throw  Exception   When an object template with that name already exists, or a client defined 
                        exception.

    @return The created object template.
    **/
    ObjectTemplate& createObjectTemplate( const Object& rObject, const String& rName, 
        NetworkingType type = LOCAL );
    /**
    Gets an object template by name.

    @param  rName   The name of the object template.

    @throw  Exception   When object template doesn't exist.
    **/
    ObjectTemplate& getObjectTemplate( const String& rName ) const;
    /**
    Gets the map of object templates.
    **/
    inline const ObjectTemplates& getObjectTemplates() const { return mObjectTemplates; }
    /**
    Query if an object template exists.

    @param  rName   The name of the object template.

    @return true if object template exists, false if not.
    **/
    bool hasObjectTemplate( const String& rName ) const;
    /**
    Destroys an object template. 
    
    @param [in,out] rObjectTemplate Reference to the object template. 
    @param  source                  Source of the destruction request. Leave blank to use this
                                    system's GUID.
                                    
    @throw  Exception   A client defined exception. 
    **/
    void destroyObjectTemplate( ObjectTemplate& rObjectTemplate, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) );
    /**
    Destroys an object template. 
    
    @param  rName   The name of the object template. 
    @param  source  Source of the destruction request. Leave blank to use this system's GUID.
                    
    @throw  Exception   When an object template with that name doesn't exist, or a client defined 
                        exception. 
    **/
    inline void destroyObjectTemplate( const String& rName,
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ) )
    {
        ObjectTemplateManager::destroyObjectTemplate( ObjectTemplateManager::getObjectTemplate( rName ), source );
    }
    /**
    Destroys a tree of object templates by reference.

    @param  rObjectTemplate     Reference to the object template.
    @param  source              Source of the destruction request. Leave blank to use this
                                system's GUID.
    @param  redirectToChilds    True to redirect call to all childs. Do not set this manually!

    @throw  Exception   A client defined exception.
    **/
    void destroyObjectTemplateTree( ObjectTemplate& rObjectTemplate, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ),
        bool redirectToChilds = false );
    /**
    Destroys a tree of object templates by name.

    @param  rName               The name of the object template.
    @param  source              Source of the destruction request. Leave blank to use this
                                system's GUID.
    @param  redirectToChilds    True to redirect call to all childs. Do not set this manually!

    @throw  Exception   A client defined exception.
    **/
    inline void destroyObjectTemplateTree( const String& rName,
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ), bool redirectToChilds = false )
    {
        ObjectTemplateManager::destroyObjectTemplateTree( ObjectTemplateManager::getObjectTemplate( rName ), source,
            redirectToChilds );
    }
    /**
    Destroys all object templates in the object template tree that the referenced object template 
    is part of.

    @param  rObjectTemplate     Reference to the object template.
    @param  source              Source of the destruction request. Leave blank to use this
                                system's GUID.
    @param  redirectToChilds    True to redirect call to all childs. Do not set this manually!

    @throw  Exception   A client defined exception.
    **/
    void destroyWholeObjectTemplateTree( ObjectTemplate& rObjectTemplate, RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ),
        bool redirectToChilds = false );
    /**
    Destroys all object templates in the object template tree that the named object template is 
    part of.

    @param  rName               The name of the object template.
    @param  source              Source of the destruction request. Leave blank to use this
                                system's GUID.
    @param  redirectToChilds    True to redirect call to all childs. Do not set this manually!

    @throw  Exception   A client defined exception.
    **/
    inline void destroyWholeObjectTemplateTree( const String& rName,
        RakNet::RakNetGUID source = RakNet::RakNetGUID( 0 ), bool redirectToChilds = false )
    {
        ObjectTemplateManager::destroyWholeObjectTemplateTree( ObjectTemplateManager::getObjectTemplate( rName ), source,
            redirectToChilds );
    }
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
    Gets the replica manager.
    **/
    inline RakNet::ReplicaManager3& getReplicaManager() const { return mReplicaManager; }
    /**
    Gets the network ID manager.
    **/
    inline RakNet::NetworkIDManager& getNetworkIDManager() const { return mNetworkIDManager; }

    /**
    Connects a slot to the object template created/destroyed signal. 
    
    @param  rSlot   The slot (signature: void func(ObjectTemplate&, bool [true when object
                    template is created, false when destroyed])) to connect. 
    
    @return Connection object to block or disconnect the connection. 
    **/
    inline sigc::connection connect( const sigc::slot<void, ObjectTemplate&, bool>& rSlot )
    {
        return mObjectTemplateSignal.connect( rSlot );
    }

    /**
    Generates a unique name for a new object template.
    **/
    String generateName();

protected:
    friend class ObjectTemplate;	///< For delayed destruction.

    /**
    Implementation for creating an object template.

    @param  rName           The name of the object template.
    @param  type            The (local/remote) type of the object template, defaults to local.
    @param  rDisplayName    The display name of the object template, defaults to the name of the 
                            object template.
    @param  source          Source of the construction request. Leave blank to use this system's
                            GUID.

    @note   Override this in a parent class to create your own types of object templates. Throw an 
            exception if the object template may not be created, the exception will be thrown back 
            to the caller of createObjectTemplate() and the object template will not be created.
    **/
    virtual ObjectTemplate& createObjectTemplateImpl( const String& rName, NetworkingType type,
        const String& rDisplayName, RakNet::RakNetGUID source ) = 0;
    /**
    Query if an object template may be destroyed.

    @param [in,out] pObjectTemplate The object template to destroy.
    @param  source  Source of the destruction request.

    @note   Override this in a parent class to stop destruction of object templates. Throw an 
            exception if the object template may not be destroyed, the exception will be thrown 
            back to the caller of destroyObjectTemplate() and the object template will not be 
            destroyed.
    **/
    virtual void queryDestroyObjectTemplate( ObjectTemplate& rObjectTemplate, RakNet::RakNetGUID source ) = 0;
    /**
    Called if the offline mode is changed.

    @param  offlineMode True if set to offline mode, false if set to online mode.
    **/
    void offlineModeChanged( bool offlineMode );

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    Mode                                        mMode;
    bool                                        mOfflineMode;
    RakNet::RakNetGUID                          mOwnGUID;
    RakNet::RakNetGUID                          mServerGUID;
    ObjectTemplates                             mObjectTemplates;
    sigc::signal<void, ObjectTemplate&, bool>   mObjectTemplateSignal;

    RakNet::ReplicaManager3&    mReplicaManager;
    RakNet::NetworkIDManager&   mNetworkIDManager;

    CAMP_RTTI()
};

//------------------------------------------------------------------------------
} // Namespace ObjectSystem
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::ObjectSystem::ObjectTemplateManager,
    &Diversia::ObjectSystem::Bindings::CampBindings::bindObjectTemplateManager );

#endif