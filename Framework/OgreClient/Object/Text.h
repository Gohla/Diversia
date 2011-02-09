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

#ifndef DIVERSIA_OGRECLIENT_TEXT_H
#define DIVERSIA_OGRECLIENT_TEXT_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API Text : public ClientComponent, public sigc::trackable
{
public:
    Text( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~Text();

    /**
    Gets the text canvas. 
    **/
    inline TextCanvas* getTextCanvas() const { return mTextCanvas; }
    /**
    Query if the text canvas has been loaded. 
    **/
    inline bool isLoaded() const { return mTextCanvas != 0; }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_TEXT; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_TEXT; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_TEXT; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_TEXT; }
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void create();
    inline bool delayedDestruction() { return false; }
    void resourcesLoaded( Entity& rEntity );
    void componentChange( Component& rComponent, bool created );

    bool        mCreated;
    TextCanvas* mTextCanvas;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::Text, 
    &Diversia::OgreClient::Bindings::CampBindings::bindText );

#endif // DIVERSIA_OGRECLIENT_TEXT_H