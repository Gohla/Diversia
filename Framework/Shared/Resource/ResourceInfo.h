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

#ifndef DIVERSIA_SHARED_RESOURCEINFO_H
#define DIVERSIA_SHARED_RESOURCEINFO_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

/**
Values that represent a resource location type.
**/
enum ResourceLocationType 
{
    RESOURCELOCATIONTYPE_FILESYSTEM,
    RESOURCELOCATIONTYPE_URL
};

/**
Values that represent resource types. 
**/
enum ResourceType
{
    RESOURCETYPE_GENERIC,
    RESOURCETYPE_OGREMESH,
    RESOURCETYPE_BULLETMESH,
    RESOURCETYPE_SKELETON,
    RESOURCETYPE_TEXTURE,
    RESOURCETYPE_MATERIALSCRIPT,
    RESOURCETYPE_PARTICLESCRIPT,
    RESOURCETYPE_LUASCRIPT,
    RESOURCETYPE_AUDIO,
    RESOURCETYPE_LEVEL
};

/**
Information about a resource.
**/
class DIVERSIA_SHARED_API ResourceInfo
{
public:
    /**
    Default constructor. 
    **/
    ResourceInfo();
    /**
    Construct from a path and filename, type is automatically deduced from extension. If the type
    cannot be deduced from the extension it defaults to 'Generic'.
    
    @param  rPath   Path and filename of the resource.
    **/
    ResourceInfo( const Path& rFile );
    /**
    Constructor. 
    
    @param  rFile       Path and filename of the resource.
    @param  type        The type of the resource.
    @param  priority    The priority this resource should get when loading multiple resources.
                        Lower number has more priority. Defaults to 0, the base priority.
    **/
    ResourceInfo( const Path& rFile, ResourceType type, int priority = 0 );

    /**
    Read from a RakNet bitstream.
    **/
    friend RakNet::BitStream& operator>>( RakNet::BitStream& in, ResourceInfo& out )
    {
        // Do our own string deserialization, cannot include Bitstream.h because of cyclic
        // dependencies.

        // Read file
        RakNet::RakString rakString;
        bool success = in.Read( rakString );
        DivAssert( success, "Reading String from bitstream failed" );
        out.mFile = Path( rakString.C_String() );

        // Read type
        success = in.Read( out.mType );
        DivAssert( success, "Reading type enum from bitstream failed" );

        // Read priority
        success = in.Read( out.mPriority );
        DivAssert( success, "Reading int from bitstream failed" );

        return in;
    }
    /**
    Write to a RakNet bitstream.
    **/
    friend RakNet::BitStream& operator<<( RakNet::BitStream& out, ResourceInfo& in )
    {
        // Do our own string serialization, cannot include Bitstream.h because of cyclic
        // dependencies.

        // Write file
        RakNet::RakString rakString = in.mFile.string().c_str();
        out.Write( rakString );

        // Write type
        out.Write( in.mType );

        // Write priority
        out.Write( in.mPriority );

        return out;
    }
    /**
    Less than comparison of resource info.
    **/
    bool operator<( const ResourceInfo& rhs ) const;
    /**
    Equality comparison of resource info.
    **/
    bool operator==( const ResourceInfo& rhs ) const;
    /**
    Print resource info. 
    **/
    DIVERSIA_SHARED_API friend std::ostream& operator<<( std::ostream& os, const ResourceInfo& rhs );

    /**
    Deduces the resource type of a given file.
    
    @param  rFile   The file to deduce the type for.
    **/
    DIVERSIA_SHARED_API static ResourceType deduceResourceType( const Path& rFile );
    /**
    Converts a resource list to a resource set. 
    
    @param  rResourceList   List of resources. 
    **/
    DIVERSIA_SHARED_API static std::set<ResourceInfo> toResourceSet( 
        const std::vector<ResourceInfo>& rResourceList );
    /**
    Converts a resource set to a resource list. 
    
    @param  rResourceList   Set of resources. 
    **/
    DIVERSIA_SHARED_API static std::vector<ResourceInfo> toResourceList( 
        const std::set<ResourceInfo>& rResourceList );

    Path            mFile;
    ResourceType    mType;
    int             mPriority;

};

typedef std::vector<ResourceInfo>   ResourceList;
typedef std::set<ResourceInfo>      ResourceSet;

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::ResourceLocationType, 
    &Diversia::Shared::Bindings::CampBindings::bindResourceLocationType );

CAMP_AUTO_TYPE( Diversia::ResourceType, 
    &Diversia::Shared::Bindings::CampBindings::bindResourceType );

CAMP_AUTO_TYPE( Diversia::ResourceInfo, 
    &Diversia::Shared::Bindings::CampBindings::bindResourceInfo );

#endif // DIVERSIA_SHARED_RESOURCEINFO_H