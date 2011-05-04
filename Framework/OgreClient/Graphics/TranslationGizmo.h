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

#ifndef DIVERSIA_OGRECLIENT_TRANSLATIONGIZMO_H
#define DIVERSIA_OGRECLIENT_TRANSLATIONGIZMO_H

#include "OgreClient/Platform/Prerequisites.h"

#include "OgreClient/Graphics/Gizmo.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API TranslationGizmo : public Gizmo
{
public:
    /**
    Constructor. 
    
    @param [in,out] rControlledObject   The object this gizmo should control. 
    **/
    TranslationGizmo( ClientObject& rControlledObject );
    /**
    Destructor. 
    **/
    virtual ~TranslationGizmo();
    
private:
    Ogre::Entity* createSelectionHelperBox( const Ogre::Vector3& center, 
        const Ogre::Vector3& size );

    Real mMouseIncrementToMovementFactor;
    Real mSelectionHelperSize;
    bool mMoveX;
    bool mMoveY;
    bool mMoveZ;
    Ogre::ManualObject* mTransformGizmoX;
    Ogre::ManualObject* mTransformGizmoX2;
    Ogre::ManualObject* mTransformGizmoY;
    Ogre::ManualObject* mTransformGizmoY2;
    Ogre::ManualObject* mTransformGizmoZ;
    Ogre::ManualObject* mTransformGizmoZ2;
    Ogre::Entity* mSelectionHelperX;
    Ogre::Entity* mSelectionHelperY;
    Ogre::Entity* mSelectionHelperZ;
    Ogre::SceneNode* mXNode;
    Ogre::SceneNode* mYNode;
    Ogre::SceneNode* mZNode;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::TranslationGizmo, 
    &Diversia::OgreClient::Bindings::CampBindings::bindTranslationGizmo );

#endif // DIVERSIA_OGRECLIENT_TRANSLATIONGIZMO_H