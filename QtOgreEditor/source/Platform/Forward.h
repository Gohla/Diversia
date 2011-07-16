/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_FORWARD_H
#define DIVERSIA_QTOGREEDITOR_FORWARD_H

#include "Util/Platform/Forward.h"
#include "Object/Platform/Forward.h"
#include "Shared/Platform/Forward.h"
#include "Client/Platform/Forward.h"
#include "OgreClient/Platform/Forward.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class EditorApplication;

// Camp
class CampPropertyManager;

// Graphics
class OgreWidget;

// Input
class QtInputManager;

// Model
class ObjectComponentModel;
class ObjectItem;
class ComponentItem;
class ObjectComponentTemplateModel;
class ObjectTemplateItem;
class ComponentTemplateItem;
class PluginModel;
class PluginItem;
class ResourceModel;
class ResourceItem;
class UndoModel;
class UndoItem;

// Object
class EditorObject;
class EditorObjectManager;

// Log
class QtLogger;

// State
class EditState;
class InitialState;
class LoadingState;
class PauseState;
class PlayState;

// UI
class ConnectDialog;
class MainWindow;
class NewGameDialog;
class ObjectTreeView;
class PluginTreeView;
class PropertyBrowser;
class ResourceTreeView;
class UndoListView;

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_FORWARD_H
