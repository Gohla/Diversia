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

#ifndef DIVERSIA_SERVER_TERRAIN_H
#define DIVERSIA_SERVER_TERRAIN_H

#include "Platform/Prerequisites.h"

#include "ClientServerPlugin/ClientPlugin.h"
#include "Shared/Terrain/Terrain.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

class Terrain : public ClientPlugin
{
public:
    Terrain( Mode mode, ClientPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    ~Terrain();

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

    /**
    Creates the specific part of this plugin. This is called in the tick/frame update after the
    plugin is created.
    **/
    void create();

    TerrainTypeEnum     mTerrainType;
    HeightmapTypeEnum   mHeightmapType;
    Path                mClientHeightmapFile;
    Path                mServerHeightmapFile;
    LayerInstances      mLayerInstances;
    Real                mInputScale;
    Real                mHeightOffset;

    Object* mTerrain;

    // For TERRAINTYPE_AUTOBLENDMAP
    Real mMinHeight0;
    Real mFadeDistance0;
    Real mMinHeight1;
    Real mFadeDistance1;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::Terrain, 
    &Diversia::Server::Bindings::CampBindings::bindTerrain );

#endif // DIVERSIA_SERVER_TERRAIN_H