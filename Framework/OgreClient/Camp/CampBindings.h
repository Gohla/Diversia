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

#ifndef DIVERSIA_OGRECLIENT_CAMPBINDINGS_H
#define DIVERSIA_OGRECLIENT_CAMPBINDINGS_H

namespace Diversia
{
namespace OgreClient
{
namespace Bindings
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API CampBindings
{
public:
    static void bindTextureFilterOptions();
    static void bindOgreResourceParams();
    static void bindGraphicsManager();
    static void bindResourceManager();
    static void bindURLArchiveParams();
    static void bindURLArchiveParamsHolder();
    static void bindPrecipitationType();
    static void bindSkyPlugin();
    static void bindSceneNode();
    static void bindEntity();
    static void bindCamera();
    static void bindGameModePlugin();
    static void bindLuaGameModeScriptEvent();
    static void bindTerrain();
    static void bindAnimation();
    static void bindTextCanvas();
    static void bindText();
    static void bindPhysicsManager();
    static void bindRigidBody();
    static void bindAudio();
    static void bindAudioManager();
    static void bindLuaObjectScript();
    static void bindLuaObjectScriptEvent();
    static void bindParticle();
    static void bindMouseButton();
    static void bindKeyboardButton();
    static void bindAxis();
    static void bindMouseState();
    static void bindKeyboardState();
    static void bindMouseListener();
    static void bindKeyboardListener();
    static void bindInputManager();
    static void bindMesh();
    static void bindCollisionShape();
    static void bindSceneManagerPlugin();
    static void bindAreaTrigger();
    static void bindForceField();
    static void bindLight();
    static void bindGizmo();
    static void bindRotationGizmo();
    static void bindScaleGizmo();
    static void bindTranslationGizmo();

};

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_CAMPBINDINGS_H