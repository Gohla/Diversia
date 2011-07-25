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

#ifndef DIVERSIA_OGRECLIENT_FORCEFIELD_H
#define DIVERSIA_OGRECLIENT_FORCEFIELD_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API ForceField : public ClientComponent, public sigc::trackable
{
public:
    ForceField( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~ForceField();

    /**
    Enables or disables the application of force.
    
    @param  enabled True to enable, false to disable. 
    **/
    void setEnabled( bool enabled );
    /**
    Query if force application is enabled.
    
    @return True if enabled, false if not. 
    **/
    inline bool isEnabled() const { return mEnabled; }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_FORCEFIELD; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_FORCEFIELD; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_FORCEFIELD; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_FORCEFIELD; }

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    inline void create() {}
    inline bool delayedDestruction() { return false; }
    void update();
    void componentChange( Component& rComponent, bool created );
    void pluginStateChanged( PluginState state, PluginState prevState );

    const ObjectsInArea* mObjectsInArea;

    bool    mEnabled;
    Vector3 mForce;

    sigc::connection mUpdateConnection;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::ForceField, 
    &Diversia::OgreClient::Bindings::CampBindings::bindForceField );

#endif // DIVERSIA_OGRECLIENT_FORCEFIELD_H