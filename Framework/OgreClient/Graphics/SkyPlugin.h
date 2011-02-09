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

#ifndef DIVERSIA_OGRECLIENT_SKYPLUGIN_H
#define DIVERSIA_OGRECLIENT_SKYPLUGIN_H

#include "OgreClient/Platform/Prerequisites.h"

#include <Caelum/Caelum.h>
#include "Client/ClientServerPlugin/ServerPlugin.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API SkyPlugin : public ServerPlugin
{
public:
    /**
    Constructor. 
    
    @param  mode                        The mode (Client/Server) the plugin will run in. 
    @param [in,out] rPluginManager      The plugin manager. 
    @param [in,out] rRakPeer            The peer interface. 
    @param [in,out] rReplicaManager     The replica manager. 
    @param [in,out] rNetworkIDManager   The network ID manager. 
    **/
    SkyPlugin( Mode mode, ServerPluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager );
    ~SkyPlugin();

    /**
    Gets the caelum system. 
    **/
    inline Caelum::CaelumSystem* getCaelumSystem() const { return mCaelumSystem; }
    /**
    Gets the caelum universal clock. 
    **/
    inline Caelum::UniversalClock* getCaelumClock() const { return mCaelumSystem->getUniversalClock(); }
    /**
    Gets the first caelum cloud layer. 
    **/
    inline Caelum::FlatCloudLayer* getCaelumCloudLayer() const { return mCaelumSystem->getCloudSystem()->getLayer( 0 ); }
    /**
    Gets the caelum precipitation controller.
    **/
    inline Caelum::PrecipitationController* getCaelumPrecipitation() { return mCaelumSystem->getPrecipitationController(); }
    /**
    Gets the caelum sun. 
    **/
    inline Caelum::BaseSkyLight* getCaelumSun() const { return mCaelumSystem->getSun(); }
    /**
    Gets the caelum moon. 
    **/
    inline Caelum::Moon* getCaelumMoon() const { return mCaelumSystem->getMoon(); }
    /**
    Query if caelum has been loaded. 
    **/
    inline bool isLoaded() const { return mCaelumSystem != 0; }
    /**
    Gets the plugin type.
    **/
    inline ClientServerPluginTypeEnum getType() const { return CLIENTSERVERPLUGINTYPE_SKY; }
    static inline ClientServerPluginTypeEnum getTypeStatic() { return CLIENTSERVERPLUGINTYPE_SKY; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return CLIENTSERVERPLUGINNAME_SKY; }
    static inline String getTypeNameStatic() { return CLIENTSERVERPLUGINNAME_SKY; }
    /**
    Gets the precipitation type. 
    **/
    Caelum::PrecipitationType getPrecipitationType() const;
    /**
    Sets the precipitation type. 
    **/
    void setPrecipitationType( Caelum::PrecipitationType type );

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    /**
    Creates the specific part of this plugin. This is called in the tick/frame update after the
    plugin is created.
    **/
    void create();
    /**
    Notifies the plugin about a server state change.
    **/
    void setServerState( ServerState serverState );

    /**
    Updates caelum.
    **/
    static void update( Real elapsed );
    /**
    Notifies caelum that the camera has changed.
    **/
    static void cameraChanged( Ogre::Camera* pCamera );
    /**
    Initializes caelum system.
    **/
    static void initialize();
    /**
    Destroys caelum system. 
    **/
    static void destroy();

    static Caelum::CaelumSystem*    mCaelumSystem;
    static Ogre::Camera*            mCamera;
    static sigc::connection         mUpdateConnection;
    static sigc::connection         mCameraConnection;
    static unsigned int             mInstanceCounter;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::SkyPlugin, 
    &Diversia::OgreClient::Bindings::CampBindings::bindSkyPlugin );

CAMP_AUTO_TYPE( Caelum::PrecipitationType, 
    &Diversia::OgreClient::Bindings::CampBindings::bindPrecipitationType );

#endif // DIVERSIA_OGRECLIENT_SKYPLUGIN_H