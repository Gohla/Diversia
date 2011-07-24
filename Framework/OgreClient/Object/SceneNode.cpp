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

#include "OgreClient/Platform/StableHeaders.h"

#include "OgreClient/Object/SceneNode.h"
#include "Util/Math/Node.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

SceneNode::SceneNode( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, SceneNode::getTypeStatic(), source, localOverride, 
        rObject ),
    mNode( 0 ),
    mNoUpdates( false )
{
    PropertySynchronization::storeUserObject();

    if( !ClientComponent::getObject().hasParent() )
    {
        // Root object, create parent scene node.
        mNode = GlobalsBase::mScene->getRootSceneNode()->createChildSceneNode( 
            Component::getGUIDString() );
    }
    else
    {
        // Not a root object, attach a new scene node to the parent node.
        mNode = ClientComponent::getObject().getParentObject()->getComponent<SceneNode>().getNode()->
            createChildSceneNode( Component::getGUIDString() );
    }

    mNode->setUserAny( Ogre::Any( &ClientComponent::getClientObject() ) );

    // Subscribe for parent updates.
    rObject.connectParentChange( sigc::mem_fun( this, &SceneNode::parentChange ) );
}

SceneNode::~SceneNode()
{
    if( mNode ) GlobalsBase::mScene->destroySceneNode( Component::getGUIDString() );
}

void SceneNode::setNoUpdates( bool noUpdates )
{
    if( noUpdates != mNoUpdates )
    {
        if( noUpdates )
            mTransformConnection.disconnect();
        else
            mTransformConnection = 
                ClientComponent::getObject().connectLocalTransformChange( sigc::mem_fun( this, 
                &SceneNode::transformChange ) );

        mNoUpdates = noUpdates;
    }
}

void SceneNode::setTransform( const Node& rNode )
{
    mNode->setPosition( toVector3<Ogre::Vector3>( rNode.getPosition() ) );
    mNode->setOrientation( toQuaternion<Ogre::Quaternion>( rNode.getOrientation() ) );
    mNode->setScale( toVector3<Ogre::Vector3>( rNode.getScale() ) );
}

void SceneNode::create()
{
    if( !mNoUpdates )
    {
        // Get initial transform.
        SceneNode::setTransform( Component::getObject() );

        // Subscribe for transform updates.
        if( mTransformConnection.empty() )
            mTransformConnection = ClientComponent::getObject().connectLocalTransformChange( 
                sigc::mem_fun( this, &SceneNode::transformChange ) );
    }
}

void SceneNode::parentChange( Object* pParent )
{
    if( pParent )
    {
        // Detach this node from old parent.
        Ogre::SceneNode* parentNode = mNode->getParentSceneNode();
        if( parentNode )
            parentNode->removeChild( mNode );

        // Attach this node to the new parent.
        pParent->getComponent<SceneNode>().getNode()->addChild( mNode );
    }
    else
    {
        // Detach this node from old parent.
        Ogre::SceneNode* parentNode = mNode->getParentSceneNode();
        if( parentNode )
            parentNode->removeChild( mNode );

        // Attach to root scene node.
        GlobalsBase::mScene->getRootSceneNode()->addChild( mNode );
    }
}

void SceneNode::transformChange( const Node& rNode )
{
    SceneNode::setTransform( rNode );
}

void SceneNode::selected( bool selected )
{
    mNode->showBoundingBox( selected );
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia