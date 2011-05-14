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
    
    @param [in,out] pControlledObject   The object this gizmo should control or 0 to control no 
                                        object. Defaults to 0.
    **/
    TranslationGizmo( ClientObject* pControlledObject = 0 );
    /**
    Destructor. 
    **/
    ~TranslationGizmo();

    /**
    Sets gizmo visibility.
    
    @param  visible True to show, false to hide. 
    **/
    void setVisible( bool visible );
    /**
    Call this if the transform is changed and the gizmo needs to update this.
    **/
    void transformChanged();
    
private:
    Ogre::Entity* createSelectionHelperBox( const Ogre::Vector3& center, 
        const Ogre::Vector3& size );
    
    void hover( bool hoverIn, int param );
    void drag( bool dragStart, int param, const Vector3& rPosition );
    void controlGizmo( bool control, int param, const Vector3& rPosition, bool duplicate, 
        Gizmo* pController );
    void checkHighlight();
    void update();
    inline void controlUpdate() { TranslationGizmo::update(); }
    void transformChange( const Node& rNode );

    static const Real mSelectionHelperSize;

    Gizmo::Axis mHoverAxis;
    Gizmo::Axis mDragAxis;
    Ogre::ManualObject* mLineX;
    Ogre::ManualObject* mConeX;
    Ogre::ManualObject* mLineY;
    Ogre::ManualObject* mConeY;
    Ogre::ManualObject* mLineZ;
    Ogre::ManualObject* mConeZ;
    Ogre::Entity* mSelectionHelperX;
    Ogre::Entity* mSelectionHelperY;
    Ogre::Entity* mSelectionHelperZ;
    Ogre::SceneNode* mXNode;
    Ogre::SceneNode* mYNode;
    Ogre::SceneNode* mZNode;
    Vector3 mDesiredPosition;
    camp::UserObject mUserObject;
    sigc::connection mUpdateConnection;
    sigc::connection mPositionChangeConnection;
    TranslationGizmo* mController;
    bool mClearMouseState;

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::TranslationGizmo, 
    &Diversia::OgreClient::Bindings::CampBindings::bindTranslationGizmo );

#endif // DIVERSIA_OGRECLIENT_TRANSLATIONGIZMO_H