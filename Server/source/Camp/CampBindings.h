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

#ifndef DIVERSIA_SERVER_CAMPBINDINGS_H
#define DIVERSIA_SERVER_CAMPBINDINGS_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace Server
{
namespace Bindings
{
//------------------------------------------------------------------------------

class CampBindings
{
public:
    static void bindServerObjectManager();
    static void bindServerObject();
    static void bindServerComponent();
    static void bindClientPluginManager();
    static void bindClientPlugin();
    static void bindPermissionManager();
    static void bindServerNeighborsPlugin();
    static void bindResourceManagerPlugin();
    static void bindPrecipitationType();
    static void bindSkyPlugin();
    static void bindClientConnectionSettings();
    static void bindApplication();
    static void bindEntity();
    static void bindGameModePlugin();
    static void bindTerrain();
    static void bindAnimation();
    static void bindText();
    static void bindPhysicsManager();
    static void bindRigidBody();
    static void bindLocalResourceManager();
    static void bindClientConnection();
    static void bindAudio();
    static void bindLuaObjectScript();
    static void bindParticle();
    static void bindUserManager();
    static void bindUser();
    static void bindGroup();
    static void bindMesh();
    static void bindCollisionShape();

};

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace Server
} // Namespace Diversia

#endif // DIVERSIA_SERVER_CAMPBINDINGS_H