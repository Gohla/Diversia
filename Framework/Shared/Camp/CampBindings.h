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

#ifndef DIVERSIA_SHARED_CAMPBINDINGS_H
#define DIVERSIA_SHARED_CAMPBINDINGS_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
namespace Shared
{
namespace Bindings
{
//------------------------------------------------------------------------------

class DIVERSIA_SHARED_API CampBindings
{
public:
    static void bindClientServerPluginManager();
    static void bindClientServerPlugin();
    static void bindServerInfo();
    static void bindServerNeighbors();
    static void bindDirection();
    static void bindClientServerPluginTypeEnum();
    static void bindComponentTypeEnum();
    static void bindResourceInfo();
    static void bindTerrainTypeEnum();
    static void bindHeightmapTypeEnum();
    static void bindLayerInstance();
    static void bindUserInfo();
    static void bindPhysicsType();
    static void bindPhysicsShape();
    static void bindLuaManager();
    static void bindLuaSecurityLevel();
    static void bindCrashReporter();
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
    static void bindWindowsCrashReporter();
#endif
    static void bindPermission();
    static void bindPropertySynchronization();
    static void bindGraphicsShape();
    static void bindResourceLocationType();
    static void bindSkyType();
    static void bindLightType();

};

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace Shared
} // Namespace Diversia

#endif // DIVERSIA_SHARED_CAMPBINDINGS_H