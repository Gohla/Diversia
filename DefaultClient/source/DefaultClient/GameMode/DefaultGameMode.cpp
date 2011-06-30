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

#include "DefaultClient/Platform/StableHeaders.h"

#include "Client/Plugin/ClientPluginManager.h"
#include "Client/Communication/Server.h"
#include "Client/Object/ClientObjectManager.h"
#include "DefaultClient/GameMode/DefaultGameMode.h"
#include "OgreClient/Graphics/Terrain.h"
#include "OgreClient/Input/InputManager.h"
#include "OgreClient/Object/Animation.h"
#include "OgreClient/Object/Audio.h"
#include "OgreClient/Object/Camera.h"
#include "OgreClient/Object/Entity.h"
#include "OgreClient/Object/LuaObjectScript.h"
#include "OgreClient/Object/Mesh.h"
#include "OgreClient/Object/Particle.h"
#include "OgreClient/Object/RigidBody.h"
#include "OgreClient/Object/SceneNode.h"
#include "OgreClient/Object/Text.h"
#include "Shared/Communication/UserInfo.h"

namespace Diversia
{
namespace DefaultClient 
{
//------------------------------------------------------------------------------

DefaultGameMode::DefaultGameMode( GameModePlugin& rGameModePlugin ):
    GameModeBase( rGameModePlugin ),
    mObjectManager( mGameModePlugin.getServer().getPluginManager().getPlugin<ClientObjectManager>() )
{
    DefaultGameMode::reset();
}

DefaultGameMode::~DefaultGameMode()
{
    DefaultGameMode::destroy();
}

void DefaultGameMode::create()
{
    try
    {
        // Create avatar
        // Objects
        mAvatar = &mObjectManager.createObject( String( "Avatar" ) + 
            mGameModePlugin.getGUIDString(), LOCAL );
        mAvatarDestroyedConnection = mAvatar->connectDestruction( sigc::mem_fun( this, 
            &DefaultGameMode::avatarDestroyed ) );
        mAvatar->setThisClientControlled();
        mAvatar->setScale( Vector3( 0.2, 0.2, 0.2 ) );
        mAvatarOffset = &mObjectManager.createObject( String( "AvatarOffset" ) + 
            mGameModePlugin.getGUIDString(), LOCAL );
        mComponentDestroyedConnection = mAvatarOffset->connectComponentChange( sigc::mem_fun( this, 
            &DefaultGameMode::componentChange ) );
        mAvatarOffset->setThisClientControlled();
        mAvatarOffset->setPosition( Vector3( 0, 2.5, 0 ) );
        mAvatarOffset->parent( mAvatar );

        // Mesh
        Mesh& mesh = mAvatarOffset->createComponent<Mesh>( "Mesh" );
        mesh.set( "MeshFile", "meshes/Ogre.mesh" );
        mesh.insert( "ResourceList", ResourceInfo( "meshes/Ogre.skeleton" ) );
        mesh.insert( "ResourceList", ResourceInfo( "images/ogre.green.tex.jpg" ) );
        mesh.insert( "ResourceList", ResourceInfo( "images/ogreahead.green.tex.jpg" ) );

        // Entity
        Entity& entity = mAvatarOffset->createComponent<Entity>( "Avatar" );

        // Kinematic RigidBody
        RigidBody& rigidBody = mAvatarOffset->createComponent<RigidBody>( "Physics" );
        rigidBody.set( "PhysicsType", PHYSICSTYPE_KINEMATIC );
        rigidBody.set( "PhysicsShape", PHYSICSSHAPE_FILE );
        rigidBody.set( "CollisionShapeFile", "physics/Ogre.bullet" );

        // Text
        mNickname = &mAvatarOffset->createComponent<Text>( "Player name" );
        mNickname->set( "Caption", mGameModePlugin.getServer().getUserInfo().mNickname );

        // Animation
        mIdleAnimation = &mAvatarOffset->createComponent<Animation>( "Idle" );
        mIdleAnimation->set( "AnimationName", "Idle" );
        mIdleAnimation->set( "Enabled", true );
        mWalkAnimation = &mAvatarOffset->createComponent<Animation>( "Walk" );
        mWalkAnimation->set( "AnimationName", "Walk" );
        mRunAnimation = &mAvatarOffset->createComponent<Animation>( "Run" );
        mRunAnimation->set( "AnimationName", "Run" );

        // Create camera
        mCamera = &mObjectManager.createObject( String( "CameraZoom" ) + 
            mGameModePlugin.getGUIDString(), LOCAL );
        mCamera->createComponent<Camera>( "Camera", true );
        mCameraPitch = &mObjectManager.createObject( String( "CameraPitch" ) + 
            mGameModePlugin.getGUIDString(), LOCAL );
        mCamera->parent( mCameraPitch );
        mCameraYaw = &mObjectManager.createObject( String( "CameraYaw" ) + 
            mGameModePlugin.getGUIDString(), LOCAL );
        mCameraYaw->setPosition( Vector3( 0, 5, 0 ) );
        mCameraPitch->parent( mCameraYaw );

        // Attach the camera yaw node to the avatar node, but don't inherit orientation and scale.
        // This is done outside of the object library because the camera objects shouldn't be
        // broadcasted to the server, this would happen if mCameraYaw had mAvatar as parent.
        Ogre::SceneNode* cameraYawNode = mCameraYaw->getComponent<SceneNode>().getNode();
        Ogre::SceneNode* avatarNode = mAvatar->getComponent<SceneNode>().getNode();
        cameraYawNode->getParentSceneNode()->removeChild( cameraYawNode );
        avatarNode->addChild( cameraYawNode );
        cameraYawNode->setInheritOrientation( false );
        cameraYawNode->setInheritScale( false );

        mCameraPitch->pitch( Angle( -10 ) ); 
        mCameraYaw->yaw( Angle( 180 ) );
        mCamera->setPosition( Vector3( 0, 0, 5 ) );

        // Broadcast objects and start using them.
        mCamera->getComponent<Camera>().setActive( true );
        mAvatar->setNetworkingType( REMOTE );

        GlobalsBase::mInput->subscribeMouse( *this );
        GlobalsBase::mInput->subscribeKeyboard( *this );
        mUpdateConnection = GlobalsBase::mFrameSignal->connect( sigc::mem_fun( this, 
            &DefaultGameMode::update ) );
    }
    catch( Exception e )
    {
        LOGE << "Could not create avatar: " << e.what();
        DefaultGameMode::destroy();
    }
    catch( camp::Error e )
    {
        LOGE << "Could not create avatar: " << e.what();
        DefaultGameMode::destroy();
    }
}

void DefaultGameMode::destroy()
{
    GlobalsBase::mInput->unsubscribeKeyboard( *this );
    GlobalsBase::mInput->unsubscribeMouse( *this );
    GlobalsBase::mInput->claimMouse( false, *this );

    if( mAvatar ) mObjectManager.destroyObject( *mAvatar );
    if( mCamera ) mObjectManager.destroyWholeObjectTree( *mCamera );

    DefaultGameMode::reset();
}

void DefaultGameMode::reset()
{
    mAvatarOffset = 0;
    mAvatar = 0;
    mCameraYaw = 0;
    mCameraPitch = 0;
    mCamera = 0;
    mIdleAnimation = 0;
    mWalkAnimation = 0;
    mRunAnimation = 0;
    mNickname = 0;
    mLMB = false;
    mRMB = false;
    mMouseState.clear();
    mMove = 0;
    mRotation = 0;
    mSpeedMultiplier = 1.0;
    mSpeedModifier = 5.0;
    mRotationModifier = 100.0;

    mUpdateConnection.disconnect();
    mAvatarDestroyedConnection.disconnect();
    mComponentDestroyedConnection.disconnect();
}

bool DefaultGameMode::mouseMoved( const MouseState& rState )
{
    mMouseState = rState;
    return true;
}

bool DefaultGameMode::mousePressed( const MouseButton button )
{
    switch( button )
    {
        case MB_Left:
            mLMB = true;
            GlobalsBase::mInput->claimMouse( true, *this );
            return true;
        case MB_Right:
            mRMB = true;
            GlobalsBase::mInput->claimMouse( true, *this );
            return true;
    }

    return false;
}

void DefaultGameMode::mouseReleased( const MouseButton button )
{
    switch( button )
    {
        case MB_Left:
            mLMB = false;
            if( !mRMB ) GlobalsBase::mInput->claimMouse( false, *this );
            return;
        case MB_Right:
            mRMB = false;
            if( !mLMB ) GlobalsBase::mInput->claimMouse( false, *this );
            return;
    }
}

bool DefaultGameMode::keyPressed( const KeyboardButton button, unsigned int text )
{
    static int random = (int)Math::RangeRandom( -1000000, 1000000 );
    static int objectCounter = 1;

    switch( button )
    {
        case KC_W: mMove += 1; return true;
        case KC_S: mMove -= 1; return true;
        case KC_A: mRotation += 1; return true;
        case KC_D: mRotation -= 1; return true;
        case KC_LSHIFT:
        case KC_RSHIFT: mSpeedMultiplier = 50.0; return true;
        case KC_EQUALS:
        case KC_ADD: mCamera->translate( 0, 0, 1, Node::TS_LOCAL ); return true;
        case KC_SUBTRACT:
        case KC_MINUS: mCamera->translate( 0, 0, -1, Node::TS_LOCAL ); return true;
        case KC_C:
        {
            Object* object = 0;
            try
            {
                StringStream objectName; objectName << "ClientTestBarrel" << random << 
                    objectCounter++;
                object = &mObjectManager.createObject( objectName.str(), LOCAL );
                Vector3 avatarPos = mAvatar->getPosition(); avatarPos.y += 5;
                object->setPosition( avatarPos );
                object->setScale( Vector3( 0.01 ) );

                Mesh& mesh = object->createComponent<Mesh>( "Mesh" );
                mesh.set( "MeshFile", "meshes/barrel.mesh" );
                mesh.insert( "ResourceList", ResourceInfo( "images/barrel1.jpg" ) );

                Entity& entity = object->createComponent<Entity>( "Barrel" );

                RigidBody& rigidBody = object->createComponent<RigidBody>( "Physics" );
                rigidBody.set( "PhysicsType", PHYSICSTYPE_DYNAMIC );
                rigidBody.set( "PhysicsShape", PHYSICSSHAPE_FILE );
                rigidBody.set( "CollisionShapeFile", "physics/barrel.bullet" );
                rigidBody.set( "Mass", 10.0 );

                object->setNetworkingType( REMOTE );
            }
            catch( Exception e )
            {
                LOGW << "Could not create barrel: " << e.what();
                if( object ) object->destroyObject();
            }
            catch( camp::Error e )
            {
                LOGW << "Could not create barrel: " << e.what();
                if( object ) object->destroyObject();
            }

            break;
        }
        case KC_B:
        {
            Object* object = 0;
            try
            {
                StringStream objectName; objectName << "ClientTestBomb" << random << objectCounter++;
                object = &mObjectManager.createObject( objectName.str(), LOCAL );
                object->setPosition( mAvatar->getPosition() );
                object->setOrientation( mAvatar->getOrientation() );
                object->translate( 0.0, 0.5, 1.0, Node::TS_LOCAL );
                object->setScale( Vector3( 0.01 ) );

                Mesh& mesh = object->createComponent<Mesh>( "Mesh" );
                mesh.set( "MeshFile", "meshes/bomb.mesh" );

                Entity& entity = object->createComponent<Entity>( "Bomb" );

                RigidBody& rigidBody = object->createComponent<RigidBody>( "Physics" );
                rigidBody.set( "PhysicsType", PHYSICSTYPE_DYNAMIC );
                rigidBody.set( "PhysicsShape", PHYSICSSHAPE_SPHERE );
                rigidBody.set( "CollisionShapeParameters", Vector3( 30, 30, 30 ) );
                rigidBody.set( "Mass", 20.0 );
                rigidBody.call
                ( 
                    "ApplyImpulse", 
                    camp::Args
                    ( 
                        object->getOrientation() * Vector3( 0, 0, Math::RangeRandom( 500, 800 ) ),
                        Vector3::ZERO 
                    ) 
                );

                Audio& audio = object->createComponent<Audio>( "Explosion" );
                audio.set( "File", "sounds/explosion.ogg" );

                Particle& smoke = object->createComponent<Particle>( "Smoke" );
                smoke.set( "Name", "CTF/Smoke" );
                smoke.insert( "ResourceList", ResourceInfo( "images/smoke.png" ) );

                Particle& fire = object->createComponent<Particle>( "Fire" );
                fire.set( "Name", "CTF/Explosion" );
                fire.insert( "ResourceList", ResourceInfo( "images/explosion.png" ) );

                LuaObjectScript& script = object->createComponent<LuaObjectScript>( "Script" );
                script.set( "ServerScriptFile", "object-lua/Bomb.lua" );
                script.set( "ServerSecurityLevel", LUASEC_LOW );

                object->setNetworkingType( REMOTE );
            }
            catch( Exception e )
            {
                LOGW << "Could not create bomb: " << e.what();
                if( object ) object->destroyObject();
            }
            catch( camp::Error e )
            {
                LOGW << "Could not create bomb: " << e.what();
                if( object ) object->destroyObject();
            }

            break;
        }
        case KC_E:
        {
            const Objects& objects = mObjectManager.getObjects();
            for( Objects::const_iterator i = objects.begin(); i != objects.end(); ++i )
            {
                try
                {
                    i->second->getComponent<RigidBody>().call( "Activate", camp::Args( true ) );
                    i->second->getComponent<RigidBody>().call
                    ( 
                        "ApplyImpulse", 
                        camp::Args
                        ( 
                            -( ( ( i->second->getPosition() - mAvatar->getPosition() ) * 100.0 ) / 
                                ( i->second->getPosition() - mAvatar->getPosition() ).length() ),
                            Vector3::ZERO
                        ) 
                    );
                }
                catch( Exception e )
                {
                    // Ignore exception	
                }
            }

            break;
        }
        case KC_R:
        {
            const Objects& objects = mObjectManager.getObjects();
            for( Objects::const_iterator i = objects.begin(); i != objects.end(); ++i )
            {
                try
                {
                    i->second->getComponent<RigidBody>().call( "Activate", camp::Args( true ) );
                    i->second->getComponent<RigidBody>().call
                    ( 
                        "ApplyImpulse", 
                        camp::Args
                        ( 
                            Vector3( Math::RangeRandom( -100, 100 ), 
                                Math::RangeRandom( -100, 100 ), Math::RangeRandom( -100, 100 ) ), 
                            Vector3( Math::RangeRandom( -.1, .1 ), 
                                Math::RangeRandom( -.1, .1 ), Math::RangeRandom( -.1, .1 ) ) 
                        ) 
                    );
                }
                catch( Exception e )
                {
                    // Ignore exception	
                }
            }
        }
        
        break;
    }
    return false;
}

void DefaultGameMode::keyReleased( const KeyboardButton button, unsigned int text )
{
    switch( button )
    {
        case KC_W: mMove -= 1; return;
        case KC_S: mMove += 1; return;
        case KC_A: mRotation -= 1; return;
        case KC_D: mRotation += 1; return;
        case KC_LSHIFT:
        case KC_RSHIFT: mSpeedMultiplier = 1.0; return;
    }
}

void DefaultGameMode::update( Real timeElapsed )
{
    // Cap values
    if( mMove < -1 ) mMove = -1;
    if( mMove > 1 ) mMove = 1;
    if( mRotation < -1 ) mRotation = -1;
    if( mRotation > 1 ) mRotation = 1;

    // Avatar
    // Yaw
    if( mRMB )
        mAvatar->yaw( Degree( -mMouseState.x.rel * 0.13 ) );

    // Movement
    if( mMove != 0 )
        mAvatar->translate( Vector3( 0, 0, mMove * mSpeedModifier * mSpeedMultiplier * timeElapsed ),
            Node::TS_LOCAL );

    // Rotation & strafing
    if( mRotation != 0 )
    {
        if( mRMB )  // Strafe
            mAvatar->translate( 
                Vector3( mRotation * mSpeedModifier * mSpeedMultiplier * timeElapsed, 0, 0  ), 
                Node::TS_LOCAL );
        else        // Turn
            mAvatar->yaw( Degree( mRotation * mRotationModifier * mSpeedMultiplier * timeElapsed ) );
    }

    // Height
    Vector3 avatarPos = mAvatar->getPosition();
    avatarPos.y = Terrain::getHeightAt( mAvatar->getPosition() );
    mAvatar->setPosition( avatarPos );

    // Animation
    if( mMove || ( mRotation && mRMB ) )
    {
        mIdleAnimation->set( "Weight", 0.01 );

        if( mSpeedMultiplier > 1.0 )
        {
            mWalkAnimation->set( "Enabled", false );
            mRunAnimation->set( "Enabled", true );
        }
        else
        {
            mWalkAnimation->set( "Enabled", true );
            mRunAnimation->set( "Enabled", false );
        }

        if( mMove < 0 )
        {
            mWalkAnimation->set( "Speed", -1.0 );
            mRunAnimation->set( "Speed", -1.0 );
        }
        else
        {
            mWalkAnimation->set( "Speed", 1.0 );
            mRunAnimation->set( "Speed", 1.0 );
        }
    }
    else
    {
        mIdleAnimation->set( "Weight", 1.0 );
        mWalkAnimation->set( "Enabled", false );
        mRunAnimation->set( "Enabled", false );
    }

    // Y-axis rotation offsets.
    if( mMove != 0 && mRotation != 0 && mRMB )
    {
        mAvatarOffset->setOrientation( Quaternion( Degree( 45 * mRotation * mMove ), 
            Vector3::UNIT_Y ) );
    }
    else if( mMove == 0 && mRotation != 0 && mRMB )
    {
        mAvatarOffset->setOrientation( Quaternion( Degree( 90 * mRotation ), Vector3::UNIT_Y ) );
    }
    else
    {
        mAvatarOffset->resetOrientation();
    }

    // Camera
    // Yaw
    if( mLMB && !mRMB )
    {
        // Just yaw the camera.
        mCameraYaw->yaw( Degree( -mMouseState.x.rel * 0.13 ) );
    }
    else if( mRMB )
    {
        // Copy the avatar yaw to the camera's yaw.
        mCameraYaw->resetOrientation();
        mCameraYaw->yaw( mAvatar->getOrientation().getYaw() + Degree( 180 ) );
    }

    // Pitch
    if( mLMB || mRMB )
    {
        Radian pitch = mCameraPitch->getOrientation().getPitch();

        // Limit pitch of camera to 90 and -85 degrees so the camera does not go upside down.
        if ( ( pitch + Degree( -mMouseState.y.rel * 0.13 ) ) < Angle( 90 ) && 
            ( pitch + Degree( -mMouseState.y.rel * 0.13 ) ) > Angle( -85 ) )
        {
            mCameraPitch->pitch( Degree( -mMouseState.y.rel * 0.13 ) );
        }
    }

    // Distance
    Ogre::Real distance = mCamera->getPosition().z;
    if( mMouseState.z.rel > 0 )
    {
        distance -= distance / 12;
        if( distance < 1 ) distance = 1; // Minimum distance
    }
    else if( mMouseState.z.rel < 0 )
    {
        distance += distance / 12;
        if( distance > 2500 ) distance = 2500; // Maximum distance
    }
    mCamera->setPosition( Vector3( 0, 0, distance ) );

    // Name disco effect
    mNickname->set( "Colour", Colour( Math::RangeRandom( 0, 255 ), Math::RangeRandom( 0, 255 ), 
        Math::RangeRandom( 0, 255 ), Math::RangeRandom( 0, 255 ) ) );

    mMouseState.clear();
}

GameModeBase* DefaultGameMode::createGameMode( GameModePlugin* pPlugin )
{
    return new DefaultGameMode( *pPlugin );
}

void DefaultGameMode::setServerState( ServerState serverState )
{
    if( serverState == CONNECTEDACTIVE )
    {
        DefaultGameMode::create();
    }
    else
    {
        DefaultGameMode::destroy();
    }
}

void DefaultGameMode::avatarDestroyed( Object& rObject )
{
    if( mAvatar ) mAvatar = 0;
    DefaultGameMode::destroy();
}

void DefaultGameMode::componentChange( Component& rComponent, bool created )
{
    // Destroy everything if a component is destroyed.
    if( !created )
        DefaultGameMode::destroy();
}

//------------------------------------------------------------------------------
} // Namespace DefaultClient
} // Namespace Diversia