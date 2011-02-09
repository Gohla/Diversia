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

#include "OgreClient/Audio/AudioResourceLoader.h"

#include <IDataSourceFactory.h>
#include <cMemorySource.h>

#include "OgreClient/Resource/GenericResourceManager.h"
#include "OgreClient/Resource/GenericResource.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

AudioResourceLoader::AudioResourceLoader( const String& rResourceGroup ): 
    mResourceGroup( rResourceGroup )
{

}

AudioResourceLoader::~AudioResourceLoader()
{

}

cAudio::IDataSource* AudioResourceLoader::CreateDataSource( const char* filename, bool streamingRequested )
{
    cAudio::cAudioMutexBasicLock lock( Mutex );

    GenericResourcePtr resource = GenericResourceManager::getSingletonPtr()->getByName( filename, 
        mResourceGroup );
    if( !resource.isNull() )
    {
        char* data = resource->getCacheCopy();
        cAudio::IDataSource* source = CAUDIO_NEW cAudio::cMemorySource( data, 
            resource->getCache()->size(), true );
        delete[] data;
        return source;
    }

    return 0;
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia