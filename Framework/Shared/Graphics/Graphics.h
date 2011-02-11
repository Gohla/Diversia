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

#ifndef DIVERSIA_SHARED_GRAPHICS_H
#define DIVERSIA_SHARED_GRAPHICS_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

enum GraphicsShape
{
    GRAPHICSSHAPE_BOX,
    GRAPHICSSHAPE_ROUNDEDBOX,
    GRAPHICSSHAPE_PLANE,
    GRAPHICSSHAPE_SPHERE,
    GRAPHICSSHAPE_ICOSPHERE,
    GRAPHICSSHAPE_CYLINDER,
    GRAPHICSSHAPE_TUBE,
    GRAPHICSSHAPE_CAPSULE,
    GRAPHICSSHAPE_CONE,
    GRAPHICSSHAPE_TORUS,
    GRAPHICSSHAPE_MESH
};

enum LightType
{
    LIGHTTYPE_POINT,
    LIGHTTYPE_SPOT,
    LIGHTTYPE_DIRECTIONAL
};

enum SkyType
{
    SKYTYPE_BOX,
    SKYTYPE_DOME,
    SKYTYPE_PLANE
};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::GraphicsShape, 
    &Diversia::Shared::Bindings::CampBindings::bindGraphicsShape );

CAMP_AUTO_TYPE( Diversia::SkyType, &Diversia::Shared::Bindings::CampBindings::bindSkyType );

CAMP_AUTO_TYPE( Diversia::LightType, &Diversia::Shared::Bindings::CampBindings::bindLightType );

#endif // DIVERSIA_SHARED_GRAPHICS_H