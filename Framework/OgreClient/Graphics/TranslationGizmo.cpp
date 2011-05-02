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

#include "OgreClient/Platform/StableHeaders.h"

#include "OgreClient/Graphics/TranslationGizmo.h"
#include "OgreClient/Graphics/GeometryHelper.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

TranslationGizmo::TranslationGizmo():
    Gizmo(),
    mMouseIncrementToMovementFactor( 0.2f ),
    mSelectionHelperQueryFlag( 0x20 ),
    mSelectionHelperSize( 0.2f ),
    mMoveX( false ),
    mMoveY( false ),
    mMoveZ( false ),
    mTransformGizmoX( GeometryHelper::createLine("Axis_X", Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_X) ),
    mTransformGizmoY( GeometryHelper::createLine("Axis_Y", Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_Y) ),
    mTransformGizmoZ( GeometryHelper::createLine("Axis_Z", Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_Z) ),
    mTransformGizmoX2( GeometryHelper::createCone("Axis_X", Ogre::Vector3::UNIT_X, Ogre::Vector3::UNIT_X, 0.03f, 8, 0.1f) ),
    mTransformGizmoY2( GeometryHelper::createCone("Axis_Y", Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Y, 0.03f, 8, 0.1f) ),
    mTransformGizmoZ2( GeometryHelper::createCone("Axis_Z", Ogre::Vector3::UNIT_Z, Ogre::Vector3::UNIT_Z, 0.03f, 8, 0.1f) ),
    mSelectionHelperX( TranslationGizmo::createSelectionHelperBox(Ogre::Vector3(0.5f + mSelectionHelperSize * 0.5f, 0.0f, 0.0f), Ogre::Vector3(1.0f + mSelectionHelperSize, mSelectionHelperSize, mSelectionHelperSize)) ),
    mSelectionHelperY( TranslationGizmo::createSelectionHelperBox(Ogre::Vector3(0.0f, 0.5f + mSelectionHelperSize * 0.5f, 0.0f), Ogre::Vector3(mSelectionHelperSize, 1.0f + mSelectionHelperSize, mSelectionHelperSize)) ),
    mSelectionHelperZ( TranslationGizmo::createSelectionHelperBox(Ogre::Vector3(0.0f, 0.0f, 0.5f + mSelectionHelperSize * 0.5f), Ogre::Vector3(mSelectionHelperSize, mSelectionHelperSize, 1.0f + mSelectionHelperSize)) ),
    mXNode( Gizmo::getSceneNode()->createChildSceneNode() ),
    mYNode( Gizmo::getSceneNode()->createChildSceneNode() ),
    mZNode( Gizmo::getSceneNode()->createChildSceneNode() )
{
    // Create axis geometry
    mTransformGizmoX->setVisibilityFlags( 2 );
    mTransformGizmoY->setVisibilityFlags( 2 );
    mTransformGizmoZ->setVisibilityFlags( 2 );
    Gizmo::getSceneNode()->attachObject(mTransformGizmoX);
    Gizmo::getSceneNode()->attachObject(mTransformGizmoY);
    Gizmo::getSceneNode()->attachObject(mTransformGizmoZ);
    mTransformGizmoX2->setVisibilityFlags( 2 );
    mTransformGizmoY2->setVisibilityFlags( 2 );
    mTransformGizmoZ2->setVisibilityFlags( 2 );
    Gizmo::getSceneNode()->attachObject(mTransformGizmoX2);
    Gizmo::getSceneNode()->attachObject(mTransformGizmoY2);
    Gizmo::getSceneNode()->attachObject(mTransformGizmoZ2);

    // Create selection helpers
    Gizmo::getSceneNode()->attachObject( mSelectionHelperX );
    Gizmo::getSceneNode()->attachObject( mSelectionHelperY );
    Gizmo::getSceneNode()->attachObject( mSelectionHelperZ );

    // Create transformation helper nodes.
    mXNode->translate(1.0f, 0.0f, 0.0f);
    mYNode->translate(0.0f, 1.0f, 0.0f);
    mZNode->translate(0.0f, 0.0f, 1.0f);
}

TranslationGizmo::~TranslationGizmo()
{
    GlobalsBase::mScene->destroyManualObject( mTransformGizmoX );
    GlobalsBase::mScene->destroyManualObject( mTransformGizmoY );
    GlobalsBase::mScene->destroyManualObject( mTransformGizmoZ );
    GlobalsBase::mScene->destroyManualObject( mTransformGizmoX2 );
    GlobalsBase::mScene->destroyManualObject( mTransformGizmoY2 );
    GlobalsBase::mScene->destroyManualObject( mTransformGizmoZ2 );
    GlobalsBase::mScene->destroyEntity( mSelectionHelperX );
    GlobalsBase::mScene->destroyEntity( mSelectionHelperY );
    GlobalsBase::mScene->destroyEntity( mSelectionHelperZ );
}

Ogre::Entity* TranslationGizmo::createSelectionHelperBox( const Ogre::Vector3& center, 
    const Ogre::Vector3& size )
{
    Ogre::Entity* entity = GeometryHelper::createAxisAlignedBoxEntity("BaseWhiteNoLighting", center, 
        size);
    entity->setVisible( false );
    entity->setQueryFlags( mSelectionHelperQueryFlag );
    return entity;
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia