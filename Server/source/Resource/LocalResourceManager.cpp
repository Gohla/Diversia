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

#include "Platform/StableHeaders.h"

#include "Resource/LocalResourceManager.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

LocalResourceManager::LocalResourceManager():
    mRootResourceLocation( "media" )
{
    Globals::mResource = this;

    // Root resource location
    if( !boost::filesystem::exists( mRootResourceLocation ) )
    {
        if( boost::filesystem::exists( "media" ) )
        {
            mRootResourceLocation = "media";
        }
        else if( boost::filesystem::exists( "../../media" ) )
        {
            mRootResourceLocation = "../../media";
        }
        else
        {
            DIVERSIA_EXCEPT( Exception::ERR_FILE_NOT_FOUND, 
                "Media directory cannot be found at 'media' or '../../media', please get the media \
                directory or set the correct directory in config.xml under 'RootResourceLocation'.", 
                "GraphicsManager::init" );
        }
    }
}

LocalResourceManager::~LocalResourceManager()
{
    Globals::mResource = 0;
}

void LocalResourceManager::init()
{
    // Search for resources and signal loaders.
    LocalResourceManager::load( mRootResourceLocation );
}

void LocalResourceManager::load( const Path& rPath )
{
    using namespace boost::filesystem;

    directory_iterator end_itr;
    for ( directory_iterator itr( rPath ); itr != end_itr; ++itr )
    {
        if ( is_directory( itr->status() ) )
        {
            Path dir = itr->path();
            if( dir.leaf() == ".svn" ) continue;
            LocalResourceManager::load( dir ); continue;
        }
        else 
        {
            Path file = itr->path();

            ExtensionSignals::const_iterator i = mExtensionSignals.find( file.extension() );
            if( i != mExtensionSignals.end() )
            {
                i->second( file );
            }
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia