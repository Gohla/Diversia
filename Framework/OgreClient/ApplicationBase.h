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

#ifndef DIVERSIA_OGRECLIENT_APPLICATIONBASE_H
#define DIVERSIA_OGRECLIENT_APPLICATIONBASE_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class ApplicationBase
{
public:
    ApplicationBase() {}
    virtual ~ApplicationBase() {}

    virtual void init() = 0;
    virtual void run() = 0;
    virtual void quitSoon() = 0;
    virtual void quit() = 0;
    virtual void stopUpdates( bool stop = true ) = 0;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_APPLICATIONBASE_H