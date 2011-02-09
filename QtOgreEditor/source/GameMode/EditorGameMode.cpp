/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Client/ClientServerPlugin/ServerPluginManager.h"
#include "Client/Object/ClientObjectManager.h"
#include "GameMode/EditorGameMode.h"
#include "OgreClient/Object/Audio.h"
#include "OgreClient/Object/Camera.h"
#include "OgreClient/Object/CollisionShape.h"
#include "OgreClient/Object/Entity.h"
#include "OgreClient/Object/LuaObjectScript.h"
#include "OgreClient/Object/Mesh.h"
#include "OgreClient/Object/Particle.h"
#include "OgreClient/Object/RigidBody.h"
#include "Shared/Physics/Physics.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

EditorGameMode::EditorGameMode( GameModePlugin& rGameModePlugin ):
    GameModeBase( rGameModePlugin ),
    mObjectManager( mGameModePlugin.getServer().getPluginManager().getPlugin<ClientObjectManager>() ),
    mDestroyNextUpdate( 0 )
{
    EditorGameMode::reset();
}

EditorGameMode::~EditorGameMode()
{
    EditorGameMode::destroy();
}

GameModeBase* EditorGameMode::createGameMode( GameModePlugin* pPlugin )
{
    return new EditorGameMode( *pPlugin );
}

void EditorGameMode::create()
{
    try
    {
        mCameraPitch = &mObjectManager.createObject( String( "EditorCameraPitch" ) + 
            mGameModePlugin.getGUIDString(), LOCAL );
        mCameraPitch->setThisClientControlled();
        Camera& camera = mCameraPitch->createComponent<Camera>( "Camera" );
        camera.setLocalOverride( true );
        camera.setActive( true );
        mCameraYaw = &mObjectManager.createObject( String( "EditorCameraYaw" ) + 
            mGameModePlugin.getGUIDString(), LOCAL );
        mCameraYaw->setThisClientControlled();
        mCameraPitch->parent( mCameraYaw );
        mCameraYaw->setPosition( 0, 150, 0 );

        mCameraPitchDestroyedConnection = mCameraPitch->connectDestruction( sigc::mem_fun( this, 
            &EditorGameMode::cameraDestroyed ) );
        mCameraYawDestroyedConnection = mCameraYaw->connectDestruction( sigc::mem_fun( this, 
            &EditorGameMode::cameraDestroyed ) );
        mComponentDestroyedConnection = mCameraPitch->connectComponentChange( sigc::mem_fun( this, 
            &EditorGameMode::componentChange ) );
        mUpdateConnection = EditorGlobals::mFrameSignal->connect( sigc::mem_fun( this, 
            &EditorGameMode::update ) );

        GlobalsBase::mInput->subscribeKeyboard( *this );
        GlobalsBase::mInput->subscribeMouse( *this );
    }
    catch( Exception e )
    {
        LOGE << "Could not create camera: " << e.what();
        EditorGameMode::destroy();
    }
    catch( camp::Error e )
    {
        LOGE << "Could not create camera: " << e.what();
        EditorGameMode::destroy();
    }
}

void EditorGameMode::destroy()
{
    GlobalsBase::mInput->unsubscribeKeyboard( *this );
    GlobalsBase::mInput->unsubscribeMouse( *this );

    if( mCameraYaw ) mObjectManager.destroyWholeObjectTree( *mCameraYaw );
    else if( mCameraPitch ) mObjectManager.destroyObject( *mCameraPitch );

    EditorGameMode::reset();
}

void EditorGameMode::reset()
{
    mDestroyNextUpdate = 0;
    mCameraPitch = 0;
    mCameraYaw = 0;
    mRMB = false;
    mMouseState.clear();
    mMove = 0;
    mRotation = 0;
    mSpeedMultiplier = 1.0;
    mSpeedModifier = 50.0;
    mRotationModifier = 50.0;

    mCameraPitchDestroyedConnection.disconnect();
    mCameraYawDestroyedConnection.disconnect();
    mComponentDestroyedConnection.disconnect();
    mUpdateConnection.disconnect();
}

void EditorGameMode::setServerState( ServerState serverState )
{
    if( serverState == CONNECTEDACTIVE )
    {
        EditorGameMode::create();
    }
    else
    {
        EditorGameMode::destroy();
    }
}

bool EditorGameMode::mouseMoved( const MouseState& rState )
{
    mMouseState = rState;
    return mRMB; // Allow other mouse listeners to process mouse movement if RMB is not pressed.
}

bool EditorGameMode::mousePressed( const MouseButton button )
{
    switch( button )
    {
        case MB_Right: mRMB = true; return true;
    }

    return false;
}

void EditorGameMode::mouseReleased( const MouseButton button )
{
    switch( button )
    {
        case MB_Right: mRMB = false; return;
    }
}

bool EditorGameMode::keyPressed( const KeyboardButton button, unsigned int text )
{
    static int random = (int)Math::RangeRandom( -1000000, 1000000 );
    static int objectCounter = 1;

    switch( button )
    {
        case KC_W: mMove += 1; return true;
        case KC_S: mMove -= 1; return true;
        case KC_A: mRotation += 1; return true;
        case KC_D: mRotation -= 1; return true;
        case KC_LSHIFT: mSpeedMultiplier = 5.0; return true;
        /*case KC_C:
        {
            Object* object = 0;
            try
            {
                StringStream objectName; objectName << "EditorTestBarrel" << random << 
                    objectCounter++;
                object = &mObjectManager.createObject( objectName.str(), LOCAL );
                object->setPosition( mCameraPitch->_getDerivedPosition() );
                object->setOrientation( mCameraPitch->_getDerivedOrientation() );
                object->yaw( Degree( 180 ) );
                object->translate( 0.0, 0.0, 2.0, Node::TS_LOCAL );
                object->setScale( Vector3( 0.01 ) );

                Mesh& mesh = object->createComponent<Mesh>( "Mesh" );
                mesh.set( "MeshFile", "meshes/barrel.mesh" );
                mesh.insert( "ResourceList", ResourceInfo( "images/barrel1.jpg" ) );

                Entity& entity = object->createComponent<Entity>( "Barrel" );

                CollisionShape& collisionShape = object->createComponent<CollisionShape>( "Collision" );
                collisionShape.set( "ShapeType", PHYSICSSHAPE_CONVEXHULL );
                collisionShape.set( "CollisionFile", "physics/barrel.bullet" );

                RigidBody& rigidBody = object->createComponent<RigidBody>( "Physics" );
                rigidBody.set( "PhysicsType", PHYSICSTYPE_DYNAMIC );
                rigidBody.set( "Mass", 10.0 );
                rigidBody.call
                ( 
                    "ApplyImpulse", 
                    camp::Args
                    ( 
                        object->getOrientation() * Vector3( 0, 0, Math::RangeRandom( 200, 300 ) ),
                        Vector3::ZERO 
                    ) 
                );

                if( !mObjectManager.isOfflineMode() ) object->setNetworkingType( REMOTE );
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
                StringStream objectName; objectName << "EditorTestBomb" << random << objectCounter++;
                object = &mObjectManager.createObject( objectName.str(), LOCAL );
                object->setPosition( mCameraPitch->_getDerivedPosition() );
                object->setOrientation( mCameraPitch->_getDerivedOrientation() );
                object->yaw( Degree( 180 ) );
                object->translate( 0.0, 0.0, 2.0, Node::TS_LOCAL );
                object->setScale( Vector3( 0.01 ) );

                Mesh& mesh = object->createComponent<Mesh>( "Mesh" );
                mesh.set( "MeshFile", "meshes/bomb.mesh" );

                Entity& entity = object->createComponent<Entity>( "Bomb" );

                CollisionShape& collisionShape = object->createComponent<CollisionShape>( "Collision" );
                collisionShape.set( "ShapeType", PHYSICSSHAPE_SPHERE );
                collisionShape.set( "ShapeParameters", Vector3( 30, 30, 30 ) );

                RigidBody& rigidBody = object->createComponent<RigidBody>( "Physics" );
                rigidBody.set( "PhysicsType", PHYSICSTYPE_DYNAMIC );
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

                if( !mObjectManager.isOfflineMode() ) object->setNetworkingType( REMOTE );
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
        }*/
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
                            -( ( ( i->second->getPosition() - mCameraYaw->getPosition() ) * 100.0 ) / 
                                ( i->second->getPosition() - mCameraYaw->getPosition() ).length() ),
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

            break;
        }
    }

    return false;
}

void EditorGameMode::keyReleased( const KeyboardButton button, unsigned int text )
{
    switch( button )
    {
        case KC_W: mMove -= 1; return;
        case KC_S: mMove += 1; return;
        case KC_A: mRotation -= 1; return;
        case KC_D: mRotation += 1; return;
        case KC_LSHIFT: mSpeedMultiplier = 1.0; return;
    }
}

void EditorGameMode::update( Real timeElapsed )
{
    // Cap values
    if( mMove < -1 ) mMove = -1;
    if( mMove > 1 ) mMove = 1;
    if( mRotation < -1 ) mRotation = -1;
    if( mRotation > 1 ) mRotation = 1;

    if( mDestroyNextUpdate )
    {
        ++mDestroyNextUpdate;

        if( mDestroyNextUpdate == 3 ) 
        {
            // Wait one extra tick for objects to be destroyed.
            EditorGameMode::destroy();
            mDestroyNextUpdate = 3;
            mUpdateConnection = EditorGlobals::mFrameSignal->connect( sigc::mem_fun( this, 
                &EditorGameMode::update ) );
        }
        else if( mDestroyNextUpdate == 4 )
        {
            // Wait another extra tick for objects to be created.
            mDestroyNextUpdate = 0;
            mUpdateConnection.disconnect();
            EditorGameMode::create();
        }

        return;
    }

    // Movement
    if( mMove != 0 )
        mCameraYaw->translate( mCameraPitch->getOrientation() * 
            Vector3( 0, 0, -mMove * mSpeedModifier * mSpeedMultiplier * timeElapsed ),
            Node::TS_LOCAL );

    // Rotation & strafing
    if( mRotation != 0 )
    {
        if( mRMB )  // Strafe
            mCameraYaw->translate( 
                Vector3( -mRotation * mSpeedModifier * mSpeedMultiplier * timeElapsed, 0, 0 ), 
                Node::TS_LOCAL );
        else        // Turn
            mCameraYaw->yaw( Degree( mRotation * mRotationModifier * mSpeedMultiplier * timeElapsed ) );
    }

    // Yaw
    if( mRMB )
        mCameraYaw->yaw( Degree( -mMouseState.x.rel * 0.13 ) );

    // Pitch
    if( mRMB )
    {
        Radian pitch = mCameraPitch->getOrientation().getPitch();

        // Limit pitch of camera to 90 and -85 degrees so the camera does not go upside down.
        if ( ( pitch + Degree( -mMouseState.y.rel * 0.13 ) ) < Angle( 90 ) && 
            ( pitch + Degree( -mMouseState.y.rel * 0.13 ) ) > Angle( -85 ) )
        {
            mCameraPitch->pitch( Degree( -mMouseState.y.rel * 0.13 ) );
        }
    }

    mMouseState.clear();
}

void EditorGameMode::cameraDestroyed( Object& rObject )
{
    if( mCameraPitch && mCameraPitch->getName() == rObject.getName() )
    {
        mCameraPitch = 0;
        mDestroyNextUpdate = 1;
    }
    else if( mCameraYaw && mCameraYaw->getName() == rObject.getName() )
    {
        mCameraYaw = 0;
        mDestroyNextUpdate = 1;
    }
}

void EditorGameMode::componentChange( Component& rComponent, bool created )
{
    if( !created )
    {
        mDestroyNextUpdate = 1;
    }
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia