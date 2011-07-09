/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Diversia is free software: you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

Diversia is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
Diversia. If not, see <http://www.gnu.org/licenses/>.

You may contact the author of Diversia by e-mail at: equabyte@sonologic.nl
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_OGRECLIENT_SCENEMANAGERPLUGIN_H
#define DIVERSIA_OGRECLIENT_SCENEMANAGERPLUGIN_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Plugin/ClientPlugin.h"
#include "Shared/Graphics/Graphics.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API SceneManagerPlugin : public ClientPlugin
{
public:
    SceneManagerPlugin( Mode mode, PluginState state, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    ~SceneManagerPlugin();

    /**
    Gets the scene manager. 
    **/
    Ogre::SceneManager* getSceneManager() const { return GlobalsBase::mScene; }
    /**
    Gets the plugin type.
    **/
    inline PluginTypeEnum getType() const { return PLUGINTYPE_SCENEMANAGER; }
    static inline PluginTypeEnum getTypeStatic() { return PLUGINTYPE_SCENEMANAGER; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return PLUGINNAME_SCENEMANAGER; }
    static inline String getTypeNameStatic() { return PLUGINNAME_SCENEMANAGER; }

    /**
    Sets the sky material.
    **/
    void setSkyMaterial( const String& rMaterial );
    /**
    Sets a sky parameter. 
    **/
    template<typename DataType, typename boost::remove_const<typename boost::remove_reference<DataType>::type>::type SceneManagerPlugin::*MemberVar>
    void setSky( DataType t )
    {
        this->*MemberVar = t;
        SceneManagerPlugin::updateSky();
    }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void create();
    void reset();
    void setServerState( ServerState serverState );
    void resourcesInitialized( ResourceManager& rResourceManager );
    void resourcesLoaded();
    void updateSky();
    void insertTexturesIntoResourceList();

    ResourceManager&    mResourceManager;
    ResourceList        mResourceList;

    bool        mCreated;
    bool        mResourcesInitialized;
    bool        mLoaded;
    SkyType     mSkyType;
    bool        mSkyEnabled;
    String      mSkyMaterial;
    Real        mSkyDistance;
    bool        mSkyDrawFirst;
    Quaternion  mSkyOrientation;
    Real        mSkyDomeCurvature;
    Real        mSkyDomeTiling;
    int         mSkyDomeXSegments;
    int         mSkyDomeYSegments;
    int         mSkyDomeKeepYSegments;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::SceneManagerPlugin, 
    &Diversia::OgreClient::Bindings::CampBindings::bindSceneManagerPlugin );

#endif // DIVERSIA_OGRECLIENT_SCENEMANAGERPLUGIN_H