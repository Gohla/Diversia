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
    mType( ResourceInfo::deduceResourceType( rFile ) ),
    mPriority( 0 )
{
    if( mType == RESOURCETYPE_SKELETON )
    {
        // Higher priority needed to prevent Ogre from loading .skeleton files when a .mesh file is 
        // loaded. Otherwise Ogre loads the .skeleton and the resource manager cannot detect when
        // the resource has finished loading and callback.
        mPriority = -1; 
    }
}

ResourceInfo::ResourceInfo( const Path& rFile, ResourceType type, int priority /*= 0*/ ):
    mFile( rFile ),
    mType( type ),
    mPriority( priority )
{

}

bool ResourceInfo::operator<( const ResourceInfo& rhs ) const
{
    if( mPriority != rhs.mPriority ) return mPriority < rhs.mPriority;
    if( mType != rhs.mType ) return mType < rhs.mType;
    if( mFile != rhs.mFile ) return mFile < rhs.mFile;
    return false;    
}

bool ResourceInfo::operator==( const ResourceInfo& rhs ) const
{
    return ( mPriority == rhs.mPriority ) && ( mType == rhs.mType ) && ( mFile == rhs.mFile );
}

std::ostream& operator<<( std::ostream& os, const ResourceInfo& rResourceInfo )
{
    return os << rResourceInfo.mFile << " of type " << rResourceInfo.mType;
}

ResourceType ResourceInfo::deduceResourceType( const Path& rFile )
{
    String ext = rFile.extension();

    if( ext == ".mesh" ) return RESOURCETYPE_OGREMESH;
    if( ext == ".bullet" ) return RESOURCETYPE_BULLETMESH;
    else if( ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".tga" || ext == ".dds" ) 
        return RESOURCETYPE_TEXTURE;
    else if( ext == ".skeleton" ) return RESOURCETYPE_SKELETON;
    else if( ext == ".material" ) return RESOURCETYPE_MATERIALSCRIPT;
    else if( ext == ".particle" ) return RESOURCETYPE_PARTICLESCRIPT;
    else if( ext == ".lua" ) return RESOURCETYPE_LUASCRIPT;
    else if( ext == ".wav" || ext == ".ogg" || ext == ".mp3" ) return RESOURCETYPE_AUDIO;
    else if( ext == ".lvl" ) return RESOURCETYPE_LEVEL;

    return RESOURCETYPE_GENERIC;
}

ResourceSet ResourceInfo::toResourceSet( const ResourceList& rResourceList )
{
    ResourceSet set;
    std::insert_iterator<ResourceSet> ii( set, set.begin() );
    std::copy( rResourceList.begin(), rResourceList.end(), ii );
    return set;
}

ResourceList ResourceInfo::toResourceList( const std::set<ResourceInfo>& rResourceList )
{
    ResourceList list;
    std::insert_iterator<ResourceList> ii( list, list.begin() );
    std::copy( rResourceList.begin(), rResourceList.end(), ii );
    return list;
}

//------------------------------------------------------------------------------
} // Namespace Diversia
