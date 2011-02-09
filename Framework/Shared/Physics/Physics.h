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

#ifndef DIVERSIA_SHARED_PHYSICS_H
#define DIVERSIA_SHARED_PHYSICS_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

enum PhysicsType
{
    PHYSICSTYPE_STATIC,
    PHYSICSTYPE_KINEMATIC,
    PHYSICSTYPE_DYNAMIC
};

enum PhysicsShape
{
    PHYSICSSHAPE_SPHERE,
    PHYSICSSHAPE_BOX,
    PHYSICSSHAPE_CYLINDER,
    PHYSICSSHAPE_CAPSULE,
    PHYSICSSHAPE_CONE,
    /*PHYSICSSHAPE_MULTISPHERE,
    PHYSICSSHAPE_BOXFROMAABB,
    PHYSICSSHAPE_CONVEXHULL,
    PHYSICSSHAPE_CONVEXTRIANGLEMESH,
    PHYSICSSHAPE_BVHTRIANGLEMESH,
    PHYSICSSHAPE_COMPOUND,*/
    PHYSICSSHAPE_HEIGHTMAPFILE,
    PHYSICSSHAPE_FILE
};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::PhysicsType, 
    &Diversia::Shared::Bindings::CampBindings::bindPhysicsType );

CAMP_AUTO_TYPE( Diversia::PhysicsShape, 
    &Diversia::Shared::Bindings::CampBindings::bindPhysicsShape );

#endif // DIVERSIA_SHARED_PHYSICS_H