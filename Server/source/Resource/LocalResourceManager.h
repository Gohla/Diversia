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

#ifndef DIVERSIA_OGRECLIENT_RESOURCEMANAGER_H
#define DIVERSIA_OGRECLIENT_RESOURCEMANAGER_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace Server
{
//------------------------------------------------------------------------------

typedef std::map<String, sigc::signal<void, const Path&> > ExtensionSignals;

class LocalResourceManager
{
public:
    /**
    Default constructor. 
    **/
    LocalResourceManager();
    /**
    Destructor. 
    **/
    ~LocalResourceManager();

    /**
    Gets the root resource location. 
    **/
    inline const Path& getRootResourceLocation() const { return mRootResourceLocation; }

    /**
    Initializes the resource manager, searching the media directory for resources.
    **/
    void init();

    /**
    Connects a slot to the resource found signal for a certain file extension.
    
    @param [in,out] rSlot   The slot (signature: void func(const Path& [resource])) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    sigc::connection connect( const String& rExtension, const sigc::slot<void, const Path&>& rSlot ) 
    {
        return mExtensionSignals[ rExtension ].connect( rSlot );
    }

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void load( const Path& rPath );

    Path                mRootResourceLocation;
    ExtensionSignals    mExtensionSignals;

};

//------------------------------------------------------------------------------
} // Namespace Server
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Server::LocalResourceManager, 
    &Diversia::Server::Bindings::CampBindings::bindLocalResourceManager );

#endif // DIVERSIA_SERVER_RESOURCEMANAGER_H