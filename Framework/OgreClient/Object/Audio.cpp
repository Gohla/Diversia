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

#include "OgreClient/Object/Audio.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "OgreClient/Resource/GenericResource.h"
#include "OgreClient/Audio/AudioManager.h"
#include "Util/Helper/Transition.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Audio::Audio( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, Audio::getTypeStatic(), source, localOverride, 
        rObject ),
    mResourceManager( rObject.getClientObjectManager().getPluginManager().getPlugin<ResourceManager>() ),
    mVolumeSmoothTimescale( 1.0 ),
    mAutoPlay( false ),
    mAudioSource( 0 ),
    mCreated( false )
{
    PropertySynchronization::storeUserObject();

    static unsigned int instanceNumber = 0;
    mInstanceNumber = instanceNumber++;

    PropertySynchronization::queue( initializer< std::set<String> >( "File" ) );
    if( Component::isCreatedByServer() ) PropertySynchronization::queueConstruction( true );
}

Audio::~Audio()
{
    if( mAudioSource )
    {
        TransitionBase<cAudio::IAudioSource, Audio>::stop( mInstanceNumber );
        mAudioSource->stop();
        GlobalsBase::mAudio->getAudioManager()->release( mAudioSource );
    }
}

void Audio::setAudioFile( const Path& rAudioFile )
{
    if( rAudioFile.empty() || mFile == rAudioFile ) return;
    mFile = rAudioFile;

    if( mCreated )
    {
        // TODO: Store and load properties when reloading the audio file so all the properties from
        // previous audio component also get set onto the new audio component.

        try
        {
            mResourceManager.loadResource( ResourceInfo( mFile, RESOURCETYPE_AUDIO ), sigc::mem_fun( this, 
                &Audio::resourceLoaded ) );
        }
        catch( FileNotFoundException e )
        {
            CLOGE << "Could not load resource for audio component: " << e.what();
        }
    }
}

void Audio::create()
{
    mCreated = true;
    if( mFile.empty() ) return;

    try
    {
        mResourceManager.loadResource( ResourceInfo( mFile, RESOURCETYPE_AUDIO ), sigc::mem_fun( this, 
            &Audio::resourceLoaded ) );
    }
    catch( FileNotFoundException e )
    {
        CLOGE << "Could not load resource for audio component: " << e.what();
        Component::destroyComponentLocally();
    }
}

void Audio::resourceLoaded( Ogre::ResourcePtr pResource )
{
    if( mAudioSource )
    {
        TransitionBase<cAudio::IAudioSource, Audio>::stop( mInstanceNumber );
        mAudioSource->stop();
        GlobalsBase::mAudio->getAudioManager()->release( mAudioSource );
    }

    mAudioSource = GlobalsBase::mAudio->getAudioManager()->create( 
        Component::getGUIDString().c_str(), pResource->getName().c_str() );

    if( !mAudioSource )
    {
        CLOGE << "Could not load audio file.";
        mTransformSignal.disconnect();
    }
    else
    {
        TransitionBase<cAudio::IAudioSource, Audio>::setUpdateSignal( *GlobalsBase::mFrameSignal );
        TransitionBase<cAudio::IAudioSource, Audio>::setInstanceNumberFunction( 
            &Audio::getInstanceNumber );

        PropertySynchronization::processQueuedConstruction();
        PropertySynchronization::processQueue();

        // TODO: Store and load properties when reloading the audio file so all the properties from
        // previous audio component also get set onto the new audio component.

        mAudioSource->setPosition( toVector3<cAudio::cVector3>( 
            Component::getObject()._getDerivedPosition() ) );

        if( mAutoPlay )
            mAudioSource->play();

        if( mTransformSignal.empty() )
            mTransformSignal = Component::getObject().connectTransformChange( sigc::mem_fun( this, 
                &Audio::transformChange ) );
    }
}

void Audio::transformChange( const Node& rNode )
{
    if( mAudioSource ) 
    {
        mAudioSource->setPosition( toVector3<cAudio::cVector3>( rNode._getDerivedPosition() ) );
    }
}

void Audio::pluginStateChanged( PluginState state, PluginState prevState )
{
    if( !mAudioSource ) return;

    switch( state )
    {
        case STOP: mAudioSource->stop(); break;
        case PAUSE: mAudioSource->pause(); break;
        case PLAY: if( prevState == PAUSE && mAudioSource->isPaused() ) mAudioSource->play(); break;
    }
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia