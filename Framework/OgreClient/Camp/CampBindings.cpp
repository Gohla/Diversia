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

#include "OgreClient/Platform/StableHeaders.h"

#include "OgreClient/Audio/AudioManager.h"
#include "OgreClient/Camp/CampBindings.h"
#include "OgreClient/GameMode/GameModePlugin.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include "OgreClient/Graphics/SceneManagerPlugin.h"
#include "OgreClient/Graphics/SkyPlugin.h"
#include "OgreClient/Graphics/Terrain.h"
#include "OgreClient/Graphics/TextCanvasBatch.h"
#include "OgreClient/Input/InputManager.h"
#include "OgreClient/Object/Animation.h"
#include "OgreClient/Object/AreaTrigger.h"
#include "OgreClient/Object/Audio.h"
#include "OgreClient/Object/Camera.h"
#include "OgreClient/Object/CollisionShape.h"
#include "OgreClient/Object/Entity.h"
#include "OgreClient/Object/ForceField.h"
#include "OgreClient/Object/Light.h"
#include "OgreClient/Object/LuaObjectScript.h"
#include "OgreClient/Object/Mesh.h"
#include "OgreClient/Object/Particle.h"
#include "OgreClient/Object/RigidBody.h"
#include "OgreClient/Object/SceneNode.h"
#include "OgreClient/Object/Text.h"
#include "OgreClient/Physics/PhysicsManager.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "OgreClient/Resource/URLArchive.h"
#include "Util/Camp/ValueMapper.h"
#include "Util/Helper/Transition.h"

namespace Diversia
{
namespace OgreClient 
{
namespace Bindings
{
//------------------------------------------------------------------------------

void CampBindings::bindTextureFilterOptions()
{
    camp::Enum::declare<Ogre::TextureFilterOptions>( "TextureFilterOptions" )
        .value( "None", Ogre::TFO_NONE )
        .value( "Bilinear", Ogre::TFO_BILINEAR )
        .value( "Trilinear", Ogre::TFO_TRILINEAR )
        .value( "Anisotropic", Ogre::TFO_ANISOTROPIC );
}

void CampBindings::bindOgreResourceParams()
{
    camp::Class::declare<OgreResourceParams>( "OgreResourceParams" )
        // Constructors
        .constructor0()
        .constructor4<const Path&, const String&, const String&, bool>()
        // Properties (read-only)
        // Properties (read/write)
        .property( "Location", &OgreResourceParams::mLocation )
            .tag( "Configurable" )
        .property( "Group", &OgreResourceParams::mGroup )
            .tag( "Configurable" )
        .property( "Archive", &OgreResourceParams::mArchive )
            .tag( "Configurable" )
        .property( "Recursive", &OgreResourceParams::mRecursive )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindGraphicsManager()
{
    camp::Class::declare<GraphicsManager>( "GraphicsManager" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "AtlasFont", &GraphicsManager::mAtlasFont )
            .tag( "Configurable" )
        .property( "Plugins", &GraphicsManager::mPlugins )
            .tag( "Configurable" )
        .property( "RootResourceLocation", &GraphicsManager::mRootResourceLocation )
            .tag( "Configurable" )
        .property( "ResourceLocations", &GraphicsManager::mResourceLocations )
            .tag( "Configurable" )
        .property( "RendererName", &GraphicsManager::mRendererName )
            .tag( "Configurable" )
        .property( "WindowWidth", &GraphicsManager::mWindowWidth )
            .tag( "Configurable" )
        .property( "WindowHeight", &GraphicsManager::mWindowHeight )
            .tag( "Configurable" )
        .property( "WindowName", &GraphicsManager::mWindowName )
            .tag( "Configurable" )
        .property( "Fullscreen", &GraphicsManager::mFullscreen )
            .tag( "Configurable" )
        .property( "VSync", &GraphicsManager::mVSync )
            .tag( "Configurable" )
        .property( "Aliasing", &GraphicsManager::mAliasing )
            .tag( "Configurable" )
        .property( "NumMipmaps", &GraphicsManager::mNumMipmaps )
            .tag( "Configurable" )
        .property( "TextureFilteringType", &GraphicsManager::mTextureFilteringType )
            .tag( "Configurable" )
        .property( "AnisotropyLevel", &GraphicsManager::mAnisotropyLevel )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindResourceManager()
{
    camp::Class::declare<ResourceManager>( "ResourceManager" )
        .tag( "QtIcon", ":/Icons/Icons/status/folder-drag-accept.png" )
        .base<ServerPlugin>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Type", &ResourceManager::mType )
        .property( "ResourceLocation", &ResourceManager::mResourceLocation, &ResourceManager::setResourceLocation )
        .property( "Group", &ResourceManager::mGroup )
            .tag( "NoBitStream" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindURLArchiveParams()
{
    camp::Class::declare<URLArchiveParams>( "URLArchiveParams" )
        // Constructors
        .constructor0()
        .constructor4<const String&, const String&, unsigned int, unsigned int>()
        // Properties (read-only)
        // Properties (read/write)
        .property( "Filename", &URLArchiveParams::mFilename )
        .property( "Directory", &URLArchiveParams::mDirectory )
        .property( "Size", &URLArchiveParams::mSize )
        .property( "Date", &URLArchiveParams::mDate );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindURLArchiveParamsHolder()
{
    camp::Class::declare<URLArchiveParamsHolder>( "URLArchiveParamsHolder" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)  
        .property( "Files", &URLArchiveParamsHolder::mVector );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindPrecipitationType()
{
    camp::Enum::declare<Caelum::PrecipitationType>( "PrecipitationType" )
        .value( "Drizzle", Caelum::PRECTYPE_DRIZZLE )
        .value( "Rain", Caelum::PRECTYPE_RAIN )
        .value( "Snow", Caelum::PRECTYPE_SNOW )
        .value( "Snowgrains", Caelum::PRECTYPE_SNOWGRAINS )
        .value( "Icecrystals", Caelum::PRECTYPE_ICECRYSTALS )
        .value( "Icepellets", Caelum::PRECTYPE_ICEPELLETS )
        .value( "Hail", Caelum::PRECTYPE_HAIL )
        .value( "SmallHail", Caelum::PRECTYPE_SMALLHAIL )
        .value( "Custom", Caelum::PRECTYPE_CUSTOM );
}

void CampBindings::bindSkyPlugin()
{
    camp::Class::declare<SkyPlugin>( "Sky" )
        .tag( "QtIcon", ":/Icons/Icons/status/skype_skypeme.png" )
        .base<ServerPlugin>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Timescale", 
            &Caelum::UniversalClock::getTimeScale, 
            &Caelum::UniversalClock::setTimeScale, &SkyPlugin::getCaelumClock )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
        .property( "FogDensity", 
            &Caelum::CaelumSystem::getSceneFogDensityMultiplier, 
            boost::bind( &Transition<Real, Caelum::CaelumSystem, SkyPlugin, &Caelum::CaelumSystem::setSceneFogDensityMultiplier>::setTransition, boost::bind( &Caelum::CaelumSystem::getSceneFogDensityMultiplier, _1 ), _2, 0.05, _1, (SkyPlugin*)0 ),
            &SkyPlugin::getCaelumSystem ) 
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
                //.tag( "QtContinuousUpdate" )
        .property( "FogColour", 
            boost::bind( &toColour<Colour, Ogre::ColourValue>, boost::bind( &Caelum::CaelumSystem::getSceneFogColourMultiplier, _1 ) ), 
            boost::bind( &Transition<const Ogre::ColourValue&, Caelum::CaelumSystem, SkyPlugin, &Caelum::CaelumSystem::setSceneFogColourMultiplier>::setTransition, boost::bind( &Caelum::CaelumSystem::getSceneFogColourMultiplier, _1 ), boost::bind( &toColour<Ogre::ColourValue, Colour>, _2 ), 0.5, _1, (SkyPlugin*)0 ),
            //boost::bind( &Caelum::CaelumSystem::setSceneFogColourMultiplier, _1, boost::bind( &toColour<Ogre::ColourValue, Colour>, _2 ) ), 
            &SkyPlugin::getCaelumSystem )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
                //.tag( "QtContinuousUpdate" )
        .property( "ObserverLongitude", 
            boost::bind( &toDegree<Degree, Ogre::Degree>, boost::bind( &Caelum::CaelumSystem::getObserverLongitude, _1 ) ), 
            boost::bind( &Caelum::CaelumSystem::setObserverLongitude, _1, boost::bind( &toDegree<Ogre::Degree, Degree>, _2 ) ), 
            &SkyPlugin::getCaelumSystem )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
        .property( "ObserverLatitude", 
            boost::bind( &toDegree<Degree, Ogre::Degree>, boost::bind( &Caelum::CaelumSystem::getObserverLatitude, _1 ) ), 
            boost::bind( &Caelum::CaelumSystem::setObserverLatitude, _1, boost::bind( &toDegree<Ogre::Degree, Degree>, _2 ) ), 
            &SkyPlugin::getCaelumSystem )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
        .property( "CloudSpeed", 
            boost::bind( &toVector2<Vector2, Ogre::Vector2>, boost::bind( &Caelum::FlatCloudLayer::getCloudSpeed, _1 ) ),
            boost::bind( &Transition<const Ogre::Vector2&, Caelum::FlatCloudLayer, SkyPlugin, &Caelum::FlatCloudLayer::setCloudSpeed>::setTransition, boost::bind( &Caelum::FlatCloudLayer::getCloudSpeed, _1 ), boost::bind( &toVector2<Ogre::Vector2, Vector2>, _2 ), 0.2, _1, (SkyPlugin*)0 ),
            &SkyPlugin::getCaelumCloudLayer )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
                //.tag( "QtContinuousUpdate" )
        .property( "CloudBlendTime", 
            &Caelum::FlatCloudLayer::getCloudBlendTime, 
            &Caelum::FlatCloudLayer::setCloudBlendTime, 
            &SkyPlugin::getCaelumCloudLayer )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
        .property( "CloudCoverage", 
            &Caelum::FlatCloudLayer::getCloudCover, 
            boost::bind( &Transition<Real, Caelum::FlatCloudLayer, SkyPlugin, &Caelum::FlatCloudLayer::setCloudCover>::setTransition, boost::bind( &Caelum::FlatCloudLayer::getCloudCover, _1 ), _2, 0.05, _1, (SkyPlugin*)0 ), 
            &SkyPlugin::getCaelumCloudLayer )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
                //.tag( "QtContinuousUpdate" )
        .property( "CloudHeight", 
            &Caelum::FlatCloudLayer::getHeight, 
            &Caelum::FlatCloudLayer::setHeight, 
            &SkyPlugin::getCaelumCloudLayer )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
        .property( "PrecipitationType", 
            &SkyPlugin::getPrecipitationType, 
            &SkyPlugin::setPrecipitationType )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
        .property( "PrecipitationIntensity", 
            &Caelum::PrecipitationController::getIntensity, 
            boost::bind( &Transition<Real, Caelum::PrecipitationController, SkyPlugin, &Caelum::PrecipitationController::setIntensity>::setTransition, boost::bind( &Caelum::PrecipitationController::getIntensity, _1 ), _2, 0.05, _1, (SkyPlugin*)0 ), 
            &SkyPlugin::getCaelumPrecipitation )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
                //.tag( "QtContinuousUpdate" )
        .property( "PrecipitationSpeed", 
            &Caelum::PrecipitationController::getSpeed, 
            &Caelum::PrecipitationController::setSpeed, 
            //boost::bind( &Transition<Real, Caelum::PrecipitationController, &Caelum::PrecipitationController::setSpeed>::setTransition, boost::bind( &Caelum::PrecipitationController::getSpeed, _1 ), _2, 0.2, _1, (SkyPlugin*)0 ),  // Speed transition looks odd.
            &SkyPlugin::getCaelumPrecipitation )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
        .property( "PrecipitationCameraSpeedScale", 
            boost::bind( &Ogre::Vector3::x, boost::bind( &Caelum::PrecipitationController::getCameraSpeedScale, _1 ) ), 
            (void(Caelum::PrecipitationController::*)(Ogre::Real))(&Caelum::PrecipitationController::setCameraSpeedScale), 
            &SkyPlugin::getCaelumPrecipitation )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
        .property( "SunAmbientMultiplier", 
            boost::bind( &toColour<Colour, Ogre::ColourValue>, boost::bind( &Caelum::BaseSkyLight::getAmbientMultiplier, _1 ) ), 
            //boost::bind( &Caelum::BaseSkyLight::setAmbientMultiplier, _1, boost::bind( &toColour<Ogre::ColourValue, Colour>, _2 ) ), 
            boost::bind( &Transition<const Ogre::ColourValue&, Caelum::BaseSkyLight, SkyPlugin, &Caelum::BaseSkyLight::setAmbientMultiplier>::setTransition, boost::bind( &Caelum::BaseSkyLight::getAmbientMultiplier, _1 ), boost::bind( &toColour<Ogre::ColourValue, Colour>, _2 ), 0.05, _1, (SkyPlugin*)0 ),
            &SkyPlugin::getCaelumSun )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
                //.tag( "QtContinuousUpdate" )
        .property( "SunSpecularMultiplier", 
            boost::bind( &toColour<Colour, Ogre::ColourValue>, boost::bind( &Caelum::BaseSkyLight::getSpecularMultiplier, _1 ) ), 
            //boost::bind( &Caelum::BaseSkyLight::setSpecularMultiplier, _1, boost::bind( &toColour<Ogre::ColourValue, Colour>, _2 ) ), 
            boost::bind( &Transition<const Ogre::ColourValue&, Caelum::BaseSkyLight, SkyPlugin, &Caelum::BaseSkyLight::setSpecularMultiplier>::setTransition, boost::bind( &Caelum::BaseSkyLight::getSpecularMultiplier, _1 ), boost::bind( &toColour<Ogre::ColourValue, Colour>, _2 ), 0.05, _1, (SkyPlugin*)0 ),
            &SkyPlugin::getCaelumSun )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded )
                .tag( "QtContinuousUpdate" )
        .property( "SunDiffuseMultiplier", 
            boost::bind( &toColour<Colour, Ogre::ColourValue>, boost::bind( &Caelum::BaseSkyLight::getDiffuseMultiplier, _1 ) ), 
            //boost::bind( &Caelum::BaseSkyLight::setDiffuseMultiplier, _1, boost::bind( &toColour<Ogre::ColourValue, Colour>, _2 ) ), 
            boost::bind( &Transition<const Ogre::ColourValue&, Caelum::BaseSkyLight, SkyPlugin, &Caelum::BaseSkyLight::setDiffuseMultiplier>::setTransition, boost::bind( &Caelum::BaseSkyLight::getDiffuseMultiplier, _1 ), boost::bind( &toColour<Ogre::ColourValue, Colour>, _2 ), 0.05, _1, (SkyPlugin*)0 ),
            &SkyPlugin::getCaelumSun )
                .readable( &SkyPlugin::isLoaded )
                .writable( &SkyPlugin::isLoaded );
                //.tag( "QtContinuousUpdate" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindSceneNode()
{
    camp::Class::declare<SceneNode>( "SceneNode" )
        .tag( "ComponentType", COMPONENTTYPE_SCENENODE )
        .tag( "QtIcon", ":/Icons/Icons/apps/gcolor2.png" )
        .base<ClientComponent>();
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindEntity()
{
    camp::Class::declare<Entity>( "Entity" )
        .tag( "ComponentType", COMPONENTTYPE_ENTITY )
        .tag( "QtIcon", ":/Icons/Icons/apps/access.png" )
        .base<ClientComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Material", 
            &Entity::mMaterial, 
            &Entity::setMaterial )
                .readable( &Entity::isLoaded )
                .writable( &Entity::isLoaded )
        .property( "CastShadows", 
            &Ogre::Entity::getCastShadows, 
            &Ogre::Entity::setCastShadows, 
            &Entity::getEntity )
                .readable( &Entity::isLoaded )
                .writable( &Entity::isLoaded )
        .property( "Visible", 
            &Ogre::Entity::getVisible, 
            &Ogre::Entity::setVisible, 
            &Entity::getEntity )
                .readable( &Entity::isLoaded )
                .writable( &Entity::isLoaded );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindCamera()
{
    camp::Class::declare<Camera>( "Camera" )
        .tag( "ComponentType", COMPONENTTYPE_CAMERA )
        .tag( "QtIcon", ":/Icons/Icons/devices/camera-photo.png" )
        .base<ClientComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Active", &Camera::mActive, &Camera::setActive );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindGameModePlugin()
{
    camp::Class::declare<GameModePlugin>( "GameMode" )
        .tag( "QtIcon", ":/Icons/Icons/categories/applications-games.png" )
        .base<ServerPlugin>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "ClientLoadDefaultGamemode", &GameModePlugin::mClientLoadDefaultGamemode )
        .property( "ClientScriptFiles", &GameModePlugin::mClientScriptFiles )
        .property( "ClientSecurityLevel", &GameModePlugin::mClientSecurityLevel )
        .property( "ServerScriptFiles", &GameModePlugin::mServerScriptFiles )
        .property( "ServerSecurityLevel", &GameModePlugin::mServerSecurityLevel )
        .property( "MousePriority", &GameModePlugin::mMousePriority )
            .tag( "NoBitStream" )
        .property( "KeyboardPriority", &GameModePlugin::mKeyboardPriority )
            .tag( "NoBitStream" )
        // Functions
        .function( "Reload script", &GameModePlugin::reload )
            .tag( "NoBitStream" )
            .tag( "QtPopupMenu" )
            .tag( "QtIcon", ":/Icons/Icons/actions/reload.png" )
        .function( "Destroy script", &GameModePlugin::destroy )
            .tag( "NoBitStream" )
            .tag( "QtPopupMenu" )
            .tag( "QtIcon", "" )
        .function( "Create script", &GameModePlugin::create )
            .tag( "NoBitStream" )
            .tag( "QtPopupMenu" )
            .tag( "QtIcon", "" )
        .function( "Disconnect", &GameModePlugin::disconnect )
            .tag( "NoBitStream" )
        .function( "IsDisconnect", &GameModePlugin::isDisconnected )
            .tag( "NoBitStream" )
        .function( "Connect", &GameModePlugin::connect )
            .tag( "NoBitStream" )
        .function( "IsConnected", &GameModePlugin::isConnected )
            .tag( "NoBitStream" )
        .function( "Block", &GameModePlugin::block )
            .tag( "NoBitStream" )
        .function( "IsBlocked", &GameModePlugin::isBlocked )
            .tag( "NoBitStream" )
        .function( "DisconnectAll", &GameModePlugin::disconnectAll )
            .tag( "NoBitStream" )
        .function( "ConnectAll", &GameModePlugin::connectAll )
            .tag( "NoBitStream" )
        .function( "BlockAll", &GameModePlugin::blockAll )
            .tag( "NoBitStream" );
        // Static functions
        // Operators
}

void CampBindings::bindLuaGameModeScriptEvent()
{
    camp::Enum::declare<LuaGameModeScriptEvent>( "GameModeScriptEvent" )
        .value( "EarlyUpdate", LUAGAMEMODESCRIPTEVENT_EARLYUPDATE )
        .value( "Update", LUAGAMEMODESCRIPTEVENT_UPDATE )
        .value( "LateUpdate", LUAGAMEMODESCRIPTEVENT_LATEUPDATE )
        .value( "EarlyFrame", LUAGAMEMODESCRIPTEVENT_EARLYFRAME )
        .value( "Frame", LUAGAMEMODESCRIPTEVENT_FRAME )
        .value( "LateFrame", LUAGAMEMODESCRIPTEVENT_LATEFRAME )
        .value( "ObjectChange", LUAGAMEMODESCRIPTEVENT_OBJECTCHANGE )
        .value( "KeyPressed", LUAGAMEMODESCRIPTEVENT_KEYPRESSED )
        .value( "KeyReleased", LUAGAMEMODESCRIPTEVENT_KEYRELEASED )
        .value( "MousePressed", LUAGAMEMODESCRIPTEVENT_MOUSEPRESSED )
        .value( "MouseReleased", LUAGAMEMODESCRIPTEVENT_MOUSERELEASED )
        .value( "MouseMoved", LUAGAMEMODESCRIPTEVENT_MOUSEMOVED );
}

void CampBindings::bindTerrain()
{
    camp::Class::declare<Terrain>( "Terrain" )
        .tag( "QtIcon", ":/Icons/Icons/categories/gnome-globe.png" )
        .base<ServerPlugin>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "TerrainType", &Terrain::mTerrainType )
        .property( "HeightmapType", &Terrain::mHeightmapType )
        .property( "ClientHeightmapFile", &Terrain::mHeightmapFile )
        .property( "InputScale", &Terrain::mInputScale )
        .property( "HeightOffset", &Terrain::mHeightOffset )
        .property( "LayerInstances", &Terrain::mLayerInstances )
        .property( "MinHeight0", &Terrain::mMinHeight0 )
        .property( "FadeDistance0", &Terrain::mFadeDistance0 )
        .property( "MinHeight1", &Terrain::mMinHeight1 )
        .property( "FadeDistance1", &Terrain::mFadeDistance1 );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindAnimation()
{
    camp::Class::declare<Animation>( "Animation" )
        .tag( "ComponentType", COMPONENTTYPE_ANIMATION )
        .tag( "QtIcon", ":/Icons/Icons/apps/preferences-desktop-assistive-technology.png" )
        .base<ClientComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "AnimationName", 
            &Animation::mAnimationName, 
            &Animation::setAnimationName )
        .property( "Speed", 
            &Animation::mSpeed )
        .property( "Enabled", &Ogre::AnimationState::getEnabled, &Ogre::AnimationState::setEnabled, 
            &Animation::getAnimationState )
            .readable( &Animation::isLoaded )
            .writable( &Animation::isLoaded )
        .property( "Loop", &Ogre::AnimationState::getLoop, &Ogre::AnimationState::setLoop, 
            &Animation::getAnimationState )
            .readable( &Animation::isLoaded )
            .writable( &Animation::isLoaded )
        .property( "Position", &Ogre::AnimationState::getTimePosition, 
            &Ogre::AnimationState::setTimePosition, &Animation::getAnimationState )
            .readable( &Animation::isLoaded )
            .writable( &Animation::isLoaded )
        .property( "Length", &Animation::getLength, &Animation::setLength )
            .readable( &Animation::isLoaded )
            .writable( &Animation::isLoaded )
        .property( "AutoLength", &Animation::mAutoLength )
        .property( "Weight", &Ogre::AnimationState::getWeight, 
            &Ogre::AnimationState::setWeight, &Animation::getAnimationState )
            .readable( &Animation::isLoaded )
            .writable( &Animation::isLoaded )
        // Functions
        .function( "HasEnded", &Ogre::AnimationState::hasEnded, &Animation::getAnimationState )
            .callable( &Animation::isLoaded )
        .function( "AddTime", &Ogre::AnimationState::addTime, &Animation::getAnimationState )
            .callable( &Animation::isLoaded );
        // Static functions
        // Operators
}

void CampBindings::bindTextCanvas()
{
    camp::Class::declare<TextCanvas>( "TextCanvas" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Caption", &TextCanvas::mCaption )
        .property( "Colour", &TextCanvas::mColour );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindText()
{
    camp::Class::declare<Text>( "Text" )
        .tag( "ComponentType", COMPONENTTYPE_TEXT )
        .tag( "QtIcon", ":/Icons/Icons/mimetypes/application-text.png" )
        .base<ClientComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Caption", &TextCanvas::mCaption, &TextCanvas::setCaption, &Text::getTextCanvas )
            .readable( &Text::isLoaded )
            .writable( &Text::isLoaded )
        .property( "Colour", &TextCanvas::mColour, &TextCanvas::setColour, &Text::getTextCanvas )
            .readable( &Text::isLoaded )
            .writable( &Text::isLoaded );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindPhysicsManager()
{
    camp::Class::declare<PhysicsManager>( "PhysicsManager" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "DebugDraw", &PhysicsManager::mDebugDraw )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindRigidBody()
{
    camp::Class::declare<RigidBody>( "RigidBody" )
        .tag( "ComponentType", COMPONENTTYPE_RIGIDBODY )
        .tag( "QtIcon", ":/Icons/Icons/apps/preferences-desktop-launch-feedback.png" )
        .base<ClientComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "PhysicsType", &RigidBody::mPhysicsType )
        .property( "Mass", &RigidBody::mMass )
        .property( "Friction", &btRigidBody::getFriction, &btRigidBody::setFriction, &RigidBody::getRigidBody )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        .property( "Restitution", &btRigidBody::getRestitution, &btRigidBody::setRestitution, &RigidBody::getRigidBody )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        .property( "LinearDampening", &RigidBody::getLinearDampening, &RigidBody::setLinearDampening )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        .property( "AngularDampening", &RigidBody::getAngularDampening, &RigidBody::setAngularDampening )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        .property( "LinearSleepingThreshold", &RigidBody::getLinearSleepingThreshold, &RigidBody::setLinearSleepingThreshold )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        .property( "AngularSleepingThreshold", &RigidBody::getAngularSleepingThreshold, &RigidBody::setAngularSleepingThreshold )
            .readable( &RigidBody::isLoaded )
            .writable( &RigidBody::isLoaded )
        // Functions
        .function( "Activate", &btRigidBody::activate, &RigidBody::getRigidBody )
            .callable( &RigidBody::isLoaded )
        .function( "ApplyForce", boost::function<void(RigidBody&, const Vector3&, const Vector3&)>( boost::bind( &btRigidBody::applyForce, boost::bind( &RigidBody::getRigidBody, _1 ), boost::bind( &toVector3<btVector3, Vector3>, _2 ), boost::bind( &toVector3<btVector3, Vector3>, _3 ) ) ) )
            .callable( &RigidBody::isLoaded )
        .function( "ApplyCentralForce", boost::function<void(RigidBody&, const Vector3&)>( boost::bind( &btRigidBody::applyCentralForce, boost::bind( &RigidBody::getRigidBody, _1 ), boost::bind( &toVector3<btVector3, Vector3>, _2 ) ) ) )
            .callable( &RigidBody::isLoaded )
        .function( "ApplyImpulse", boost::function<void(RigidBody&, const Vector3&, const Vector3&)>( boost::bind( &btRigidBody::applyImpulse, boost::bind( &RigidBody::getRigidBody, _1 ), boost::bind( &toVector3<btVector3, Vector3>, _2 ), boost::bind( &toVector3<btVector3, Vector3>, _3 ) ) ) )
            .callable( &RigidBody::isLoaded )
        .function( "ApplyCentralImpulse", boost::function<void(RigidBody&, const Vector3&)>( boost::bind( &btRigidBody::applyCentralImpulse, boost::bind( &RigidBody::getRigidBody, _1 ), boost::bind( &toVector3<btVector3, Vector3>, _2 ) ) ) )
            .callable( &RigidBody::isLoaded )
        .function( "ApplyTorque", boost::function<void(RigidBody&, const Vector3&)>( boost::bind( &btRigidBody::applyTorque, boost::bind( &RigidBody::getRigidBody, _1 ), boost::bind( &toVector3<btVector3, Vector3>, _2 ) ) ) )
            .callable( &RigidBody::isLoaded )
        /*.function( "Activate", boost::function<void(RigidBody&, bool)>( boost::bind( &RigidBody::serializeCall<bool>, _1, String( "Activate" ), _2 ) ) )
            .tag( "NoCall" )
        .function( "ApplyForce", boost::function<void(RigidBody&, const Vector3&, const Vector3&)>( boost::bind( (void(RigidBody::*)(const String&, const Vector3&, const Vector3&))(&RigidBody::serializeCall<const Vector3&, const Vector3&>), _1, String( "ApplyForce" ), _2, _3 ) ) )
            .tag( "NoCall" )
        .function( "ApplyImpulse", boost::function<void(RigidBody&, const Vector3&, const Vector3&)>( boost::bind( (void(RigidBody::*)(const String&, const Vector3&, const Vector3&))(&RigidBody::serializeCall<const Vector3&, const Vector3&>), _1, String( "ApplyImpulse" ), _2, _3 ) ) )
            .tag( "NoCall" );*/
        ;
        // Static functions
        // Operators
}

void CampBindings::bindAudio()
{
    camp::Class::declare<Audio>( "Audio" )
        .tag( "ComponentType", COMPONENTTYPE_AUDIO )
        .tag( "QtIcon", ":/Icons/Icons/mimetypes/audio-x-generic.png" )
        .base<ClientComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "File", &Audio::mFile, &Audio::setAudioFile )
        .property( "Volume", 
            &cAudio::IAudioSource::getVolume,
            &cAudio::IAudioSource::setVolume,
            &Audio::getAudioSource )
                .readable( &Audio::isLoaded )
                .writable( &Audio::isLoaded )
        .property( "VolumeSmooth", 
            boost::bind( &cAudio::IAudioSource::getVolume, boost::bind( &Audio::getAudioSource, _1 ) ),
            boost::bind( &Transition<const float&, cAudio::IAudioSource, Audio, &cAudio::IAudioSource::setVolume>::setTransition, boost::bind( &cAudio::IAudioSource::getVolume, boost::bind( &Audio::getAudioSource, _1 ) ), _2, boost::bind( &Audio::mVolumeSmoothTimescale, _1 ), boost::bind( &Audio::getAudioSource, _1 ), _1 ),
            &Audio::getThis )
                .readable( &Audio::isLoaded )
                .writable( &Audio::isLoaded )
                .tag( "QtContinuousUpdate" )
        .property( "VolumeSmoothTimescale", &Audio::mVolumeSmoothTimescale )
        .property( "Loop", 
            &cAudio::IAudioSource::isLooping, 
            &cAudio::IAudioSource::loop, 
            &Audio::getAudioSource )
                .readable( &Audio::isLoaded )
                .writable( &Audio::isLoaded )
        .property( "AutoPlay", &Audio::mAutoPlay )
        .property( "Pitch", 
            &cAudio::IAudioSource::getPitch, 
            &cAudio::IAudioSource::setPitch, 
            &Audio::getAudioSource )
                .readable( &Audio::isLoaded )
                .writable( &Audio::isLoaded )
        .property( "MinDistance", 
            &cAudio::IAudioSource::getMinDistance, 
            &cAudio::IAudioSource::setMinDistance, 
            &Audio::getAudioSource )
                .readable( &Audio::isLoaded )
                .writable( &Audio::isLoaded )
        .property( "MaxDistance", 
            &cAudio::IAudioSource::getMaxDistance, 
            &cAudio::IAudioSource::setMaxDistance, 
            &Audio::getAudioSource )
                .readable( &Audio::isLoaded )
                .writable( &Audio::isLoaded )
        .property( "MinVolume", 
            &cAudio::IAudioSource::getMinVolume, 
            &cAudio::IAudioSource::setMinVolume, 
            &Audio::getAudioSource )
                .readable( &Audio::isLoaded )
                .writable( &Audio::isLoaded )
        .property( "MaxVolume", 
            &cAudio::IAudioSource::getMaxVolume, 
            &cAudio::IAudioSource::setMaxVolume, 
            &Audio::getAudioSource )
                .readable( &Audio::isLoaded )
                .writable( &Audio::isLoaded )
        .property( "Strength", 
            &cAudio::IAudioSource::getStrength, 
            &cAudio::IAudioSource::setStrength, 
            &Audio::getAudioSource )
                .readable( &Audio::isLoaded )
                .writable( &Audio::isLoaded )
        .property( "DopplerStrength", 
            &cAudio::IAudioSource::getDopplerStrength, 
            &cAudio::IAudioSource::setDopplerStrength, 
            &Audio::getAudioSource )
                .readable( &Audio::isLoaded )
                .writable( &Audio::isLoaded )
        .property( "RolloffFactor", 
            &cAudio::IAudioSource::getRolloffFactor, 
            &cAudio::IAudioSource::setRolloffFactor, 
            &Audio::getAudioSource )
                .readable( &Audio::isLoaded )
                .writable( &Audio::isLoaded )
        // Functions
        .function( "Play", &cAudio::IAudioSource::play, &Audio::getAudioSource )
            .callable( &Audio::isLoaded )
            .tag( "NoBitStream" )
        .function( "Stop", &cAudio::IAudioSource::stop, &Audio::getAudioSource )
            .callable( &Audio::isLoaded )
            .tag( "NoBitStream" )
        .function( "Pause", &cAudio::IAudioSource::pause, &Audio::getAudioSource )
            .callable( &Audio::isLoaded )
            .tag( "NoBitStream" );
        // Static functions
        // Operators
}

void CampBindings::bindAudioManager()
{
    camp::Class::declare<AudioManager>( "AudioManager" )
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Volume", &cAudio::IListener::getMasterVolume, &cAudio::IListener::setMasterVolume, 
            &AudioManager::getListener )
            .readable( &AudioManager::isLoaded )
            .writable( &AudioManager::isLoaded )
            .tag( "Configurable" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindLuaObjectScript()
{
    camp::Class::declare<LuaObjectScript>( "LuaObjectScript" )
        .tag( "ComponentType", COMPONENTTYPE_LUAOBJECTSCRIPT )
        .tag( "QtIcon", ":/Icons/Icons/mimetypes/stock_script.png" )
        .base<ClientComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "ClientScriptFile", 
            &LuaObjectScript::mClientScriptFile, 
            &LuaObjectScript::setClientScriptFile )
        .property( "ClientEnvironmentName", &LuaObjectScript::mClientEnvironmentName )
        .property( "ClientSecurityLevel", &LuaObjectScript::mClientSecurityLevel )
        .property( "ServerScriptFile", &LuaObjectScript::mServerScriptFile )
        .property( "ServerEnvironmentName", &LuaObjectScript::mServerEnvironmentName )
        .property( "ServerSecurityLevel", &LuaObjectScript::mServerSecurityLevel )
        // Functions
         .function( "Reload script", &LuaObjectScript::reload )
            .tag( "NoBitStream" )
            .tag( "QtPopupMenu" )
            .tag( "QtIcon", ":/Icons/Icons/actions/reload.png" )
        .function( "Destroy script", &LuaObjectScript::destroy )
            .tag( "NoBitStream" )
            .tag( "QtPopupMenu" )
            .tag( "QtIcon", "" )
        .function( "Create script", &LuaObjectScript::create )
            .tag( "NoBitStream" )
            .tag( "QtPopupMenu" )
            .tag( "QtIcon", "" )
        .function( "Disconnect", &LuaObjectScript::disconnect )
            .tag( "NoBitStream" )
        .function( "IsDisconnect", &LuaObjectScript::isDisconnected )
            .tag( "NoBitStream" )
        .function( "Connect", &LuaObjectScript::connect )
            .tag( "NoBitStream" )
        .function( "IsConnected", &LuaObjectScript::isConnected )
            .tag( "NoBitStream" )
        .function( "Block", &LuaObjectScript::block )
            .tag( "NoBitStream" )
        .function( "IsBlocked", &LuaObjectScript::isBlocked )
            .tag( "NoBitStream" )
        .function( "DisconnectAll", &LuaObjectScript::disconnectAll )
            .tag( "NoBitStream" )
        .function( "ConnectAll", &LuaObjectScript::connectAll )
            .tag( "NoBitStream" )
        .function( "BlockAll", &LuaObjectScript::blockAll )
            .tag( "NoBitStream" );
        // Static functions
        // Operators
}

void CampBindings::bindLuaObjectScriptEvent()
{
    camp::Enum::declare<LuaObjectScriptEvent>( "ObjectScriptEvent" )
    	.value( "Update", LUAOBJECTSCRIPTEVENT_UPDATE )
        .value( "Frame", LUAOBJECTSCRIPTEVENT_FRAME )
        .value( "ComponentChange", LUAOBJECTSCRIPTEVENT_COMPONENTCHANGE )
        .value( "TransformChange", LUAOBJECTSCRIPTEVENT_TRANSFORMCHANGE )
        .value( "AreaChange", LUAOBJECTSCRIPTEVENT_AREACHANGE )
        .value( "CollisionWith", LUAOBJECTSCRIPTEVENT_COLLISIONWITH );
}

void CampBindings::bindParticle()
{
    camp::Class::declare<Particle>( "Particle" )
        .tag( "ComponentType", COMPONENTTYPE_PARTICLE )
        .tag( "QtIcon", ":/Icons/Icons/apps/preferences-desktop-default-applications.png" )
        .base<ClientComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "Name", &Particle::mParticleSystemName, &Particle::setParticleSystemName )
        .property( "Quota", 
            &Ogre::ParticleSystem::getParticleQuota,   
            &Ogre::ParticleSystem::setParticleQuota,
            &Particle::getParticleSystem )
                .readable( &Particle::isLoaded )
                .writable( &Particle::isLoaded )
        .property( "Enabled", 
            &Ogre::ParticleSystem::getEmitting,
            &Ogre::ParticleSystem::setEmitting,
            &Particle::getParticleSystem )
                .readable( &Particle::isLoaded )
                .writable( &Particle::isLoaded )
        .property( "Speed", 
            &Ogre::ParticleSystem::getSpeedFactor,
            &Ogre::ParticleSystem::setSpeedFactor,
            &Particle::getParticleSystem )
                .readable( &Particle::isLoaded )
                .writable( &Particle::isLoaded )
        // Functions
        .function( "Clear", &Ogre::ParticleSystem::clear, &Particle::getParticleSystem )
            .callable( &Particle::isLoaded );
        // Static functions
        // Operators
}

void CampBindings::bindMouseButton()
{
    camp::Enum::declare<MouseButton>( "MouseButton" )
        .value( "Left", MB_Left )
        .value( "Right", MB_Right )
        .value( "Middle", MB_Middle )
        .value( "Button3", MB_Button3 )
        .value( "Button4", MB_Button4 )
        .value( "Button5", MB_Button5 )
        .value( "Button6", MB_Button6 )
        .value( "Button7", MB_Button7 );
}

void CampBindings::bindKeyboardButton()
{
    camp::Enum::declare<KeyboardButton>( "KeyboardButton" )
        .value( "UNASSIGNED", KC_UNASSIGNED )
        .value( "ESCAPE", KC_ESCAPE )
        .value( "1", KC_1 )
        .value( "2", KC_2 )
        .value( "3", KC_3 )
        .value( "4", KC_4 )
        .value( "5", KC_5 )
        .value( "6", KC_6 )
        .value( "7", KC_7 )
        .value( "8", KC_8 )
        .value( "9", KC_9 )
        .value( "0", KC_0 )
        .value( "MINUS", KC_MINUS )
        .value( "EQUALS", KC_EQUALS )
        .value( "BACK", KC_BACK )
        .value( "TAB", KC_TAB )
        .value( "Q", KC_Q )
        .value( "W", KC_W )
        .value( "E", KC_E )
        .value( "R", KC_R )
        .value( "T", KC_T )
        .value( "Y", KC_Y )
        .value( "U", KC_U )
        .value( "I", KC_I )
        .value( "O", KC_O )
        .value( "P", KC_P )
        .value( "LBRACKET", KC_LBRACKET )
        .value( "RBRACKET", KC_RBRACKET )
        .value( "RETURN", KC_RETURN )
        .value( "LCONTROL", KC_LCONTROL )
        .value( "A", KC_A )
        .value( "S", KC_S )
        .value( "D", KC_D )
        .value( "F", KC_F )
        .value( "G", KC_G )
        .value( "H", KC_H )
        .value( "J", KC_J )
        .value( "K", KC_K )
        .value( "L", KC_L )
        .value( "SEMICOLON", KC_SEMICOLON )
        .value( "APOSTROPHE", KC_APOSTROPHE )
        .value( "GRAVE", KC_GRAVE )
        .value( "LSHIFT", KC_LSHIFT )
        .value( "BACKSLASH", KC_BACKSLASH )
        .value( "Z", KC_Z )
        .value( "X", KC_X )
        .value( "C", KC_C )
        .value( "V", KC_V )
        .value( "B", KC_B )
        .value( "N", KC_N )
        .value( "M", KC_M )
        .value( "COMMA", KC_COMMA )
        .value( "PERIOD", KC_PERIOD )
        .value( "SLASH", KC_SLASH )
        .value( "RSHIFT", KC_RSHIFT )
        .value( "MULTIPLY", KC_MULTIPLY )
        .value( "LMENU", KC_LMENU )
        .value( "SPACE", KC_SPACE )
        .value( "CAPITAL", KC_CAPITAL )
        .value( "F1", KC_F1 )
        .value( "F2", KC_F2 )
        .value( "F3", KC_F3 )
        .value( "F4", KC_F4 )
        .value( "F5", KC_F5 )
        .value( "F6", KC_F6 )
        .value( "F7", KC_F7 )
        .value( "F8", KC_F8 )
        .value( "F9", KC_F9 )
        .value( "F10", KC_F10 )
        .value( "NUMLOCK", KC_NUMLOCK )
        .value( "SCROLL", KC_SCROLL )
        .value( "NUMPAD7", KC_NUMPAD7 )
        .value( "NUMPAD8", KC_NUMPAD8 )
        .value( "NUMPAD9", KC_NUMPAD9 )
        .value( "SUBTRACT", KC_SUBTRACT )
        .value( "NUMPAD4", KC_NUMPAD4 )
        .value( "NUMPAD5", KC_NUMPAD5 )
        .value( "NUMPAD6", KC_NUMPAD6 )
        .value( "ADD", KC_ADD )
        .value( "NUMPAD1", KC_NUMPAD1 )
        .value( "NUMPAD2", KC_NUMPAD2 )
        .value( "NUMPAD3", KC_NUMPAD3 )
        .value( "NUMPAD0", KC_NUMPAD0 )
        .value( "DECIMAL", KC_DECIMAL )
        .value( "OEM_102", KC_OEM_102 )
        .value( "F11", KC_F11 )
        .value( "F12", KC_F12 )
        .value( "F13", KC_F13 )
        .value( "F14", KC_F14 )
        .value( "F15", KC_F15 )
        .value( "KANA", KC_KANA )
        .value( "ABNT_C1", KC_ABNT_C1 )
        .value( "CONVERT", KC_CONVERT )
        .value( "NOCONVERT", KC_NOCONVERT )
        .value( "YEN", KC_YEN )
        .value( "ABNT_C2", KC_ABNT_C2 )
        .value( "NUMPADEQUALS", KC_NUMPADEQUALS )
        .value( "PREVTRACK", KC_PREVTRACK )
        .value( "AT", KC_AT )
        .value( "COLON", KC_COLON )
        .value( "UNDERLINE", KC_UNDERLINE )
        .value( "KANJI", KC_KANJI )
        .value( "STOP", KC_STOP )
        .value( "AX", KC_AX )
        .value( "UNLABELED", KC_UNLABELED )
        .value( "NEXTTRACK", KC_NEXTTRACK )
        .value( "NUMPADENTER", KC_NUMPADENTER )
        .value( "RCONTROL", KC_RCONTROL )
        .value( "MUTE", KC_MUTE )
        .value( "CALCULATOR", KC_CALCULATOR )
        .value( "PLAYPAUSE", KC_PLAYPAUSE )
        .value( "MEDIASTOP", KC_MEDIASTOP )
        .value( "VOLUMEDOWN", KC_VOLUMEDOWN )
        .value( "VOLUMEUP", KC_VOLUMEUP )
        .value( "WEBHOME", KC_WEBHOME )
        .value( "NUMPADCOMMA", KC_NUMPADCOMMA )
        .value( "DIVIDE", KC_DIVIDE )
        .value( "SYSRQ", KC_SYSRQ )
        .value( "RMENU", KC_RMENU )
        .value( "PAUSE", KC_PAUSE )
        .value( "HOME", KC_HOME )
        .value( "UP", KC_UP )
        .value( "PGUP", KC_PGUP )
        .value( "LEFT", KC_LEFT )
        .value( "RIGHT", KC_RIGHT )
        .value( "END", KC_END )
        .value( "DOWN", KC_DOWN )
        .value( "PGDOWN", KC_PGDOWN )
        .value( "INSERT", KC_INSERT )
        .value( "DELETE", KC_DELETE )
        .value( "LWIN", KC_LWIN )
        .value( "RWIN", KC_RWIN )
        .value( "APPS", KC_APPS )
        .value( "POWER", KC_POWER )
        .value( "SLEEP", KC_SLEEP )
        .value( "WAKE", KC_WAKE )
        .value( "WEBSEARCH", KC_WEBSEARCH )
        .value( "WEBFAVORITES", KC_WEBFAVORITES )
        .value( "WEBREFRESH", KC_WEBREFRESH )
        .value( "WEBSTOP", KC_WEBSTOP )
        .value( "WEBFORWARD", KC_WEBFORWARD )
        .value( "WEBBACK", KC_WEBBACK )
        .value( "MYCOMPUTER", KC_MYCOMPUTER )
        .value( "MAIL", KC_MAIL )
        .value( "MEDIASELECT", KC_MEDIASELECT );
}

void CampBindings::bindAxis()
{
    camp::Class::declare<Axis>( "Axis" )
        // Constructors
        .constructor2<int, int>()
        // Properties (read-only)
        // Properties (read/write)
        .property( "Absolute", &Axis::abs )
        .property( "Relative", &Axis::rel )
        // Functions
        .function( "Clear", &Axis::clear );
        // Static functions
        // Operators
}

void CampBindings::bindMouseState()
{
    camp::Class::declare<MouseState>( "MouseState" )
        // Constructors
        .constructor0()
        .constructor1<const MouseState&>()
        // Properties (read-only)
        // Properties (read/write)
        .property( "X", &MouseState::x )
        .property( "Y", &MouseState::y )
        .property( "Z", &MouseState::z )
        // Functions
        .function( "IsButtonDown", &MouseState::isButtonDown )
        .function( "Clear", &MouseState::clear );
        // Static functions
        // Operators
}

void CampBindings::bindKeyboardState()
{
    camp::Class::declare<KeyboardState>( "KeyboardState" )
        // Constructors
        .constructor0()
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        .function( "IsButtonDown", &KeyboardState::isButtonDown )
        .function( "Clear", &KeyboardState::clear );
        // Static functions
        // Operators
}

void CampBindings::bindMouseListener()
{
    camp::Class::declare<MouseListener>( "MouseListener" );
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindKeyboardListener()
{
    camp::Class::declare<KeyboardListener>( "KeyboardListener" );
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindInputManager()
{
    camp::Class::declare<InputManager>( "InputManager" )
        // Constructors
        // Properties (read-only)
        .property( "MouseState", &InputManager::getMouseState )
        .property( "KeyboardState", &InputManager::getKeyboardState )
        .property( "WindowWidth", &InputManager::getWindowWidth )
        .property( "WindowHeight", &InputManager::getWindowHeight );
        // Properties (read/write)
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindMesh()
{
    camp::Class::declare<Mesh>( "Mesh" )
        .tag( "ComponentType", COMPONENTTYPE_MESH )
        .tag( "QtIcon", ":/Icons/Icons/apps/blender.png" )
        .base<ClientComponent>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "MeshFile", &Mesh::mMeshFile, &Mesh::setMeshFile )
        .property( "ShapeType", 
            &Mesh::mShapeType, 
            &Mesh::setShapeParameter<GraphicsShape, &Mesh::mShapeType> )
        .property( "ShapeParameters", 
            &Mesh::mShapeParameters, 
            &Mesh::setShapeParameter<Vector3, &Mesh::mShapeParameters> )
                .tag( "DeserializeReSet" )
        .property( "ShapeUV", 
            &Mesh::mShapeUV, 
            &Mesh::setShapeParameter<Vector2, &Mesh::mShapeUV> )
                .tag( "DeserializeReSet" )
        .property( "ShapeXSegments", 
            &Mesh::mShapeXSegments, 
            &Mesh::setShapeParameter<int, &Mesh::mShapeXSegments> )
        .property( "ShapeYSegments", 
            &Mesh::mShapeYSegments, 
            &Mesh::setShapeParameter<int, &Mesh::mShapeYSegments> )
        .property( "ShapeZSegments", 
            &Mesh::mShapeZSegments, 
            &Mesh::setShapeParameter<int, &Mesh::mShapeZSegments> )
        .property( "ShapeRadius", 
            &Mesh::mShapeRadius, 
            &Mesh::setShapeParameter<Real, &Mesh::mShapeRadius> )
        .property( "ShapeRadius2", 
            &Mesh::mShapeRadius2, 
            &Mesh::setShapeParameter<Real, &Mesh::mShapeRadius2> );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindCollisionShape()
{
    camp::Class::declare<CollisionShape>( "CollisionShape" )
        .tag( "ComponentType", COMPONENTTYPE_COLLISIONSHAPE )
        .tag( "QtIcon", ":/Icons/Icons/apps/blender.png" )
        .base<ClientComponent>()
        // Constructors
        // Properties (read-only)
        .property( "IsReceivingCollisionCallbacks", &CollisionShape::isReceivingCollisionCallbacks )
            .tag( "NoBitStream" )
        // Properties (read/write)
        .property( "CollisionFile", &CollisionShape::mCollisionFile, &CollisionShape::setCollisionFile )
        .property( "ShapeType", &CollisionShape::mShapeType, &CollisionShape::setShapeType )
        .property( "ShapeParameters", &CollisionShape::mShapeParameters, &CollisionShape::setShapeParameters )
            .tag( "DeserializeReSet" )
        .property( "CollisionPriority", &CollisionShape::mCollisionPriority );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindSceneManagerPlugin()
{
    camp::Class::declare<SceneManagerPlugin>( "SceneManager" )
        .tag( "QtIcon", ":/Icons/Icons/categories/gnome-settings.png" )
        .base<ServerPlugin>()
        // Constructors
        // Properties (read-only)
        // Properties (read/write)
        .property( "SkyType", 
            &SceneManagerPlugin::mSkyType, 
            &SceneManagerPlugin::setSky<SkyType, &SceneManagerPlugin::mSkyType> )
        .property( "SkyEnabled", 
            &SceneManagerPlugin::mSkyEnabled, 
            &SceneManagerPlugin::setSky<bool, &SceneManagerPlugin::mSkyEnabled> )
        .property( "SkyMaterial", 
            &SceneManagerPlugin::mSkyMaterial, 
            &SceneManagerPlugin::setSkyMaterial )
        .property( "SkyDistance", 
            &SceneManagerPlugin::mSkyDistance, 
            &SceneManagerPlugin::setSky<Real, &SceneManagerPlugin::mSkyDistance> )
        .property( "SkyDrawFirst", 
            &SceneManagerPlugin::mSkyDrawFirst, 
            &SceneManagerPlugin::setSky<bool, &SceneManagerPlugin::mSkyDrawFirst> )
        .property( "SkyOrientation", 
            &SceneManagerPlugin::mSkyOrientation, 
            &SceneManagerPlugin::setSky<const Quaternion&, &SceneManagerPlugin::mSkyOrientation> )
        .property( "SkyDomeCurvature", 
            &SceneManagerPlugin::mSkyDomeCurvature, 
            &SceneManagerPlugin::setSky<Real, &SceneManagerPlugin::mSkyDomeCurvature> )
        .property( "SkyDomeTiling", 
            &SceneManagerPlugin::mSkyDomeTiling, 
            &SceneManagerPlugin::setSky<Real, &SceneManagerPlugin::mSkyDomeTiling> )
        .property( "SkyDomeXSegments", 
            &SceneManagerPlugin::mSkyDomeXSegments, 
            &SceneManagerPlugin::setSky<int, &SceneManagerPlugin::mSkyDomeXSegments> )
        .property( "SkyDomeYSegments", 
            &SceneManagerPlugin::mSkyDomeYSegments, 
            &SceneManagerPlugin::setSky<int, &SceneManagerPlugin::mSkyDomeYSegments> )
        .property( "SkyDomeKeepYSegments", 
            &SceneManagerPlugin::mSkyDomeKeepYSegments, 
            &SceneManagerPlugin::setSky<int, &SceneManagerPlugin::mSkyDomeKeepYSegments> )
        .property( "AmbientLight", 
            boost::bind( &toColour<Colour, Ogre::ColourValue>, boost::bind( &Ogre::SceneManager::getAmbientLight, _1 ) ),
            boost::bind( &Ogre::SceneManager::setAmbientLight, _1, boost::bind( &toColour<Ogre::ColourValue, Colour>, _2 ) ),
            &SceneManagerPlugin::getSceneManager )
                .tag( "DeserializeCopySet" );
        // Functions
        // Static functions
        // Operators
}

void CampBindings::bindAreaTrigger()
{
    camp::Class::declare<AreaTrigger>( "AreaTrigger" )
        .tag( "ComponentType", COMPONENTTYPE_COLLISIONSHAPE )
        .tag( "QtIcon", ":/Icons/Icons/categories/package_network.png" )
        .base<ClientComponent>();
    	// Constructors
    	// Properties (read-only)
    	// Properties (read/write)
    	// Functions
    	// Static functions
    	// Operators
}

void CampBindings::bindForceField()
{
    camp::Class::declare<ForceField>( "ForceField" )
        .tag( "ComponentType", COMPONENTTYPE_FORCEFIELD )
        .tag( "QtIcon", ":/Icons/Icons/apps/gtk-recordmydesktop.png" )
        .base<ClientComponent>()
	    // Constructors
	    // Properties (read-only)
	    // Properties (read/write)
        .property( "Enabled", &ForceField::mEnabled, &ForceField::setEnabled )
        .property( "Force", &ForceField::getForce, &ForceField::setForce )
            .tag( "DeserializeCopySet" );
	    // Functions
	    // Static functions
	    // Operators
}

void CampBindings::bindLight()
{
    camp::Class::declare<Light>( "Light" )
        .tag( "ComponentType", COMPONENTTYPE_FORCEFIELD )
        .tag( "QtIcon", ":/Icons/Icons/categories/package_graphics.png" )
        .base<ClientComponent>()
	    // Constructors
	    // Properties (read-only)
	    // Properties (read/write)
        .property( "Type", &Light::mType, &Light::setType )
        .property( "DiffuseColour", 
            boost::bind( &toColour<Colour, Ogre::ColourValue>, boost::bind( &Ogre::Light::getDiffuseColour, _1 ) ), 
            boost::bind( &Ogre::Light::setDiffuseColour, _1, boost::bind( &toColour<Ogre::ColourValue, Colour>, _2 ) ), 
            &Light::getLight )
                .readable( &Light::isLoaded )
                .writable( &Light::isLoaded )
        .property( "SpecularColour", 
            boost::bind( &toColour<Colour, Ogre::ColourValue>, boost::bind( &Ogre::Light::getSpecularColour, _1 ) ), 
            boost::bind( &Ogre::Light::setSpecularColour, _1, boost::bind( &toColour<Ogre::ColourValue, Colour>, _2 ) ), 
            &Light::getLight )
                .readable( &Light::isLoaded )
                .writable( &Light::isLoaded )
        .property( "PowerScale", 
            &Ogre::Light::getPowerScale, 
            &Ogre::Light::setPowerScale, 
            &Light::getLight )
                .readable( &Light::isLoaded )
                .writable( &Light::isLoaded )
        .property( "SpotlightFalloff", 
            &Ogre::Light::getSpotlightFalloff, 
            &Ogre::Light::setSpotlightFalloff, 
                &Light::getLight )
                .readable( &Light::isLoaded )
                .writable( &Light::isLoaded )
        .property( "SpotlightInnerAngle", 
            boost::bind( &toRadian<Radian, Ogre::Radian>, boost::bind( &Ogre::Light::getSpotlightInnerAngle, _1 ) ), 
            boost::bind( &Ogre::Light::setSpotlightInnerAngle, _1, boost::bind( &toRadian<Ogre::Radian, Radian>, _2 ) ), 
                &Light::getLight )
                .readable( &Light::isLoaded )
                .writable( &Light::isLoaded )
        .property( "SpotlightOuterAngle", 
            boost::bind( &toRadian<Radian, Ogre::Radian>, boost::bind( &Ogre::Light::getSpotlightOuterAngle, _1 ) ), 
            boost::bind( &Ogre::Light::setSpotlightOuterAngle, _1, boost::bind( &toRadian<Ogre::Radian, Radian>, _2 ) ), 
                &Light::getLight )
                .readable( &Light::isLoaded )
                .writable( &Light::isLoaded );
	    // Functions
	    // Static functions
	    // Operators
}

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace OgreClient
} // Namespace Diversia