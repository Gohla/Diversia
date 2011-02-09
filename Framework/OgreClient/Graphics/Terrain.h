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

#ifndef DIVERSIA_OGRECLIENT_TERRAIN_H
#define DIVERSIA_OGRECLIENT_TERRAIN_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/ClientServerPlugin/ServerPlugin.h"
#include "Shared/Terrain/Terrain.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API Terrain : public ServerPlugin
{
public:
    Terrain( Mode mode, ServerPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    ~Terrain();

    /**
    Gets the terrain. 
    **/
    inline Ogre::Terrain* getTerrain() const { return mTerrain; }
    /**
    Gets the height at given terrain position;
    **/
    static Real getHeightAt( const Vector3& rPosition );
    /**
    Gets the plugin type.
    **/
    inline ClientServerPluginTypeEnum getType() const { return CLIENTSERVERPLUGINTYPE_TERRAIN; }
    static inline ClientServerPluginTypeEnum getTypeStatic() { return CLIENTSERVERPLUGINTYPE_TERRAIN; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return CLIENTSERVERPLUGINNAME_TERRAIN; }
    static inline String getTypeNameStatic() { return CLIENTSERVERPLUGINNAME_TERRAIN; }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void resourcesLoaded();

    /**
    Creates the specific part of this plugin. This is called in the tick/frame update after the
    plugin is created.
    **/
    void create();
    /**
    Notifies the plugin about a server state change.
    **/
    inline void setServerState( ServerState serverState ) {}

    Ogre::Terrain*                      mTerrain;
    static Ogre::TerrainGlobalOptions*  mTerrainGlobals;
    ResourceManager&                    mResourceManager;

    TerrainTypeEnum     mTerrainType;
    HeightmapTypeEnum   mHeightmapType;
    Path                mHeightmapFile;
    Real                mInputScale;
    Real                mHeightOffset;
    LayerInstances      mLayerInstances;

    // For TERRAINTYPE_AUTOBLENDMAP
    Real mMinHeight0;
    Real mFadeDistance0;
    Real mMinHeight1;
    Real mFadeDistance1;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::Terrain, 
    &Diversia::OgreClient::Bindings::CampBindings::bindTerrain );

#endif // DIVERSIA_OGRECLIENT_TERRAIN_H