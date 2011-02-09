/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "Shared/Platform/StableHeaders.h"

#include "Shared/Resource/ResourceInfo.h"
#include "Shared/Communication/BitStream.h"

namespace Diversia
{
//------------------------------------------------------------------------------

ResourceInfo::ResourceInfo()
{

}

ResourceInfo::ResourceInfo( const Path& rFile ):
    mFile( rFile ),
    mType( "Generic" ),
    mPriority( 0 )
{
    // Try to deduce type.
    String ext = rFile.extension();

    if( ext == ".mesh" )
        mType = "Mesh";
    else if( ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".tga" || ext == ".dds" ) 
        mType = "Texture";
    else if( ext == ".skeleton" )
    {
        mType = "Skeleton";
        // Higher priority needed to prevent Ogre from loading .skeleton files when a .mesh file is 
        // loaded. Otherwise Ogre loads the .skeleton and the resource manager cannot detect when
        // the resource has finished loading and callback.
        mPriority = -1; 
    }
    // TODO: Add more type detection.
}

ResourceInfo::ResourceInfo( const Path& rFile, const String& rType, int priority /*= 0*/ ):
    mFile( rFile ),
    mType( rType ),
    mPriority( priority )
{

}

bool ResourceInfo::operator<( const ResourceInfo& rResourceInfo ) const
{
    return mPriority < rResourceInfo.mPriority || !( rResourceInfo.mPriority < mPriority ) && 
        ( mFile.string() + mType ) < ( rResourceInfo.mFile.string() + rResourceInfo.mType );
}

bool ResourceInfo::operator==( const ResourceInfo& rResourceInfo ) const
{
    return ( mFile == rResourceInfo.mFile ) && ( mType == rResourceInfo.mType ) && 
        ( mPriority == rResourceInfo.mPriority );
}

std::ostream& operator<<( std::ostream& os, const ResourceInfo& rResourceInfo )
{
    return os << rResourceInfo.mFile << " of type " << rResourceInfo.mType;
}

ResourceSet ResourceInfo::toResourceSet( const ResourceList& rResourceList )
{
    ResourceSet set;
    std::insert_iterator<ResourceSet> ii( set, set.begin() );
    std::copy( rResourceList.begin(), rResourceList.end(), ii );
    return set;
}

//------------------------------------------------------------------------------
} // Namespace Diversia
