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

#ifndef DIVERSIA_SHARED_DEFINES_H
#define DIVERSIA_SHARED_DEFINES_H

namespace Diversia
{
//------------------------------------------------------------------------------

// Replica type
//#define REPLICATYPE_OBJECT            0x00
//#define REPLICATYPE_COMPONENT         0x01
#define REPLICATYPE_PLUGIN  0x02
//#define REPLICATYPE_OBJECTTEMPLATE    0x03
//#define REPLICATYPE_COMPONENTTEMPLATE 0x04

// Client-server plugins, when changed also update CampBindings::bindPluginTypeEnum!
typedef unsigned char PluginType;
enum PluginTypeEnum
{
    PLUGINTYPE_PERMISSIONMANAGER = 0x00,
    PLUGINTYPE_RESOURCEMANAGER = 0x01,
    PLUGINTYPE_LUA = 0x02,
    PLUGINTYPE_SERVERNEIGHBORS = 0x03,
    PLUGINTYPE_SKY = 0x04,
    PLUGINTYPE_OBJECTTEMPLATEMANAGER = 0x05,
    PLUGINTYPE_OBJECTMANAGER = 0x06,
    PLUGINTYPE_TERRAIN = 0x07,
    PLUGINTYPE_GAMEMODE = 0x08,
    PLUGINTYPE_SCENEMANAGER = 0x09,
    PLUGINTYPE_LEVELMANAGER = 0x10
};
#define PLUGINNAME_PERMISSIONMANAGER "PermissionManager"
#define PLUGINNAME_RESOURCEMANAGER "ResourceManager"
#define PLUGINNAME_LUA "Lua"
#define PLUGINNAME_SERVERNEIGHBORS "ServerNeighbors"
#define PLUGINNAME_SKY "Sky"
#define PLUGINNAME_OBJECTTEMPLATEMANAGER "ObjectTemplateManager"
#define PLUGINNAME_OBJECTMANAGER "ObjectManager"
#define PLUGINNAME_TERRAIN "Terrain"
#define PLUGINNAME_GAMEMODE "GameMode"
#define PLUGINNAME_SCENEMANAGER "SceneManager"
#define PLUGINNAME_LEVELMANAGER "LevelManager"

// Components, when changed also update CampBindings::bindComponentTypeEnum!
enum ComponentTypeEnum
{
    COMPONENTTYPE_SCENENODE = 0x00,
    COMPONENTTYPE_ENTITY = 0x01,
    COMPONENTTYPE_MESH = 0x02,
    COMPONENTTYPE_LIGHT = 0x03,
    COMPONENTTYPE_ANIMATION = 0x04,
    COMPONENTTYPE_PARTICLE = 0x05,
    COMPONENTTYPE_COLLISIONSHAPE = 0x06,
    COMPONENTTYPE_RIGIDBODY = 0x07,
    COMPONENTTYPE_AREATRIGGER = 0x08,
    COMPONENTTYPE_FORCEFIELD = 0x09,
    COMPONENTTYPE_AUDIO = 0x0A,
    COMPONENTTYPE_TEXT = 0x0B,
    COMPONENTTYPE_CAMERA = 0x0C,
    COMPONENTTYPE_LUAOBJECTSCRIPT = 0x0D
};

#define COMPONENTNAME_SCENENODE "SceneNode"
#define COMPONENTNAME_ENTITY "Entity"
#define COMPONENTNAME_MESH "Mesh"
#define COMPONENTNAME_LIGHT "Light"
#define COMPONENTNAME_ANIMATION "Animation"
#define COMPONENTNAME_PARTICLE "Particle"
#define COMPONENTNAME_COLLISIONSHAPE "CollisionShape"
#define COMPONENTNAME_RIGIDBODY "RigidBody"
#define COMPONENTNAME_AREATRIGGER "AreaTrigger"
#define COMPONENTNAME_FORCEFIELD "ForceField"
#define COMPONENTNAME_AUDIO "Audio"
#define COMPONENTNAME_TEXT "Text"
#define COMPONENTNAME_CAMERA "Camera"
#define COMPONENTNAME_LUAOBJECTSCRIPT "LuaObjectScript"

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::PluginTypeEnum, 
    &Diversia::Shared::Bindings::CampBindings::bindPluginTypeEnum );

CAMP_AUTO_TYPE( Diversia::ComponentTypeEnum, 
    &Diversia::Shared::Bindings::CampBindings::bindComponentTypeEnum );

#endif // DIVERSIA_SHARED_DEFINES_H
