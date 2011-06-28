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

#ifndef DIVERSIA_SHARED_TERRAIN_H
#define DIVERSIA_SHARED_TERRAIN_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

typedef unsigned char TerrainType;
enum TerrainTypeEnum
{
    TERRAINTYPE_AUTOBLENDMAP = 0x00,
    TERRAINTYPE_OGREBINARY = 0x01
};

typedef unsigned char HeightmapType;
enum HeightmapTypeEnum
{
    HEIGHTMAPTYPE_IMAGE = 0x00,
    HEIGHTMAPTYPE_RAW = 0x01,
    HEIGHTMAPTYPE_OGREBINARY = 0x02
};

struct DIVERSIA_SHARED_API LayerInstance
{
    /**
    Default constructor. 
    **/
    LayerInstance( Real worldSize = 10 ): mWorldSize( worldSize ) {}

    Real                mWorldSize;
    std::vector<Path>   mTextureFiles;
};
typedef std::vector<LayerInstance> LayerInstances;

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::TerrainTypeEnum, 
    &Diversia::Shared::Bindings::CampBindings::bindTerrainTypeEnum );

CAMP_AUTO_TYPE( Diversia::HeightmapTypeEnum, 
    &Diversia::Shared::Bindings::CampBindings::bindHeightmapTypeEnum );

CAMP_AUTO_TYPE( Diversia::LayerInstance, 
    &Diversia::Shared::Bindings::CampBindings::bindLayerInstance );

#endif // DIVERSIA_SHARED_TERRAIN_H