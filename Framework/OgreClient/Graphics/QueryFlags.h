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

#ifndef DIVERSIA_OGRECLIENT_QUERYFLAGS_H
#define DIVERSIA_OGRECLIENT_QUERYFLAGS_H

#include "OgreClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

/**
Values that represent OGRE query flags.
**/
enum QueryFlags
{
    QueryFlags_NonSelectable = 0,
    QueryFlags_Entity = 1,
    QueryFlags_Particle = 2,
    QueryFlags_Camera = 4,
    QueryFlags_Terrain = 8,
    QueryFlags_Gizmo = 16
};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_QUERYFLAGS_H