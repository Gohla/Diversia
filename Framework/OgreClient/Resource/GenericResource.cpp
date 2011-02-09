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

#include "OgreClient/Resource/GenericResource.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

GenericResource::GenericResource( Ogre::ResourceManager* pCreator, const std::string& rName, 
    Ogre::ResourceHandle handle, const Ogre::String& rGroup, bool isManual /*= false*/, 
    Ogre::ManualResourceLoader* pLoader /*= 0*/ ):
    Ogre::Resource( pCreator, rName, handle, rGroup, isManual, pLoader ), mSize( 0 )
{

}

GenericResource::~GenericResource()
{
    unprepareImpl();
    unloadImpl();
}

char* GenericResource::getCacheCopy() const
{
    char* data = new char[ mCache->size() ];
    mCache->seek( 0 );
    mCache->read( data, mCache->size() );
    return data;
}

void GenericResource::prepareImpl()
{
    mCache = Ogre::ResourceGroupManager::getSingleton().openResource( mName, mGroup, true, this );

    // Buffer into RAM
    mCache = Ogre::DataStreamPtr( OGRE_NEW Ogre::MemoryDataStream( mName, mCache ) );
    mSize = mCache->size();
}

void GenericResource::unprepareImpl()
{
    mCache.setNull();
    mSize = 0;
}

void GenericResource::loadImpl()
{

}

void GenericResource::unloadImpl()
{
    mHolderData = boost::any();
    DivAssert( mHolderData.empty(), "Holder data was not emptied." );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia