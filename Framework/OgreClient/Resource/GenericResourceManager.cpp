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

#include "OgreClient/Resource/GenericResourceManager.h"
#include "OgreClient/Resource/GenericResource.h"

template<> Diversia::OgreClient::GenericResourceManager* 
    Ogre::Singleton<Diversia::OgreClient::GenericResourceManager>::ms_Singleton = 0;

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

GenericResourceManager::GenericResourceManager()
{
    mResourceType = "Generic";
    mLoadOrder = 50.0f;
    Ogre::ResourceGroupManager::getSingleton()._registerResourceManager( mResourceType, this );
}

GenericResourceManager::~GenericResourceManager()
{
    Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager( mResourceType );
}

GenericResourceManager& GenericResourceManager::getSingleton()
{
    if ( ms_Singleton == 0 )
    {
        ms_Singleton = new GenericResourceManager();
    }

    return *ms_Singleton;
}

GenericResourceManager* GenericResourceManager::getSingletonPtr()
{
    if ( ms_Singleton == 0 )
    {
        ms_Singleton = new GenericResourceManager();
    }

    return ms_Singleton;
}

Ogre::Resource* GenericResourceManager::createImpl( const Ogre::String& rName, 
    Ogre::ResourceHandle handle, const Ogre::String& rGroup, bool isManual, 
    Ogre::ManualResourceLoader* pLoader, const Ogre::NameValuePairList* pCreateParams )
{
    return new GenericResource( this, rName, handle, rGroup, isManual, pLoader );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia