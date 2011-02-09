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

#ifndef DIVERSIA_OGRECLIENT_SCENENODE_H
#define DIVERSIA_OGRECLIENT_SCENENODE_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API SceneNode : public ClientComponent, public sigc::trackable
{
public:
    SceneNode( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~SceneNode();

    /**
    Gets the ogre scene node. 
    **/
    inline Ogre::SceneNode* getNode() { return mNode; }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_SCENENODE; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_SCENENODE; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_SCENENODE; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_SCENENODE; }
    /**
    Gets if the node shouldn't receive transform updates from it's object.
    **/
    inline bool getNoUpdates() const { return mNoUpdates; }
    /**
    Sets if the node shouldn't receive transform updates from it's object.
    **/
    void setNoUpdates( bool noUpdates );

    /**
    Sets a transformation to the scene node. 
    
    @param  rTransform  The transformation to set.
    **/
    void setTransform( const Node& rNode );
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    inline bool delayedDestruction() { return false; }
    void create();
    void parentChange( Object* pParent );
    void transformChange( const Node& rNode );
    void setSelected( bool selected );

    Ogre::SceneNode* mNode;

    sigc::connection    mTransformConnection;
    bool                mNoUpdates;

    CAMP_RTTI()

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::SceneNode, 
    &Diversia::OgreClient::Bindings::CampBindings::bindSceneNode );

#endif // DIVERSIA_OGRECLIENT_SCENENODE_H