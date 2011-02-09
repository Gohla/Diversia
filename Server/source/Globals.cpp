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

#include "Globals.h"

namespace Diversia
{
namespace Server 
{
//------------------------------------------------------------------------------

Application*                Globals::mApp = 0;
ConfigManager*              Globals::mConfig = 0;
LocalResourceManager*            Globals::mResource = 0;
ClientConnection*           Globals::mClient = 0;
PhysicsManager*             Globals::mPhysics = 0;
btDiscreteDynamicsWorld*    Globals::mWorld = 0;
btAxisSweep3*               Globals::mBroadphase = 0;
LuaManager*                 Globals::mLua = 0;

sigc::signal<void>*         Globals::mUpdateSignal = 0;
sigc::signal<void, Real>*   Globals::mFrameSignal = 0;

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia