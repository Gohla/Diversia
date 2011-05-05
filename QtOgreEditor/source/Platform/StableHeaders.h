/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_STABLEHEADERS_H
#define DIVERSIA_QTOGREEDITOR_STABLEHEADERS_H

#include "Platform/Prerequisites.h"

// Framework stable headers
#include "Util/UtilIncludes.h"
#include "Object/ObjectIncludes.h"
#include "Shared/SharedIncludes.h"
#include "Client/ClientIncludes.h"

// RakNet
#include <RakNet/RakPeerInterface.h>
#include <RakNet/NetworkIDManager.h>
#include <RakNet/ReplicaManager3.h>
#include <RakNet/BitStream.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/Kbhit.h>
#include <RakNet/RakSleep.h>
#include <RakNet/GetTime.h>

// Ogre
#include <OGRE/Ogre.h>
#include <OGRE/Terrain/OgreTerrain.h>
#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>

// Bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

// OIS
#include <OIS/OIS.h>

// MyGUI
#include <MYGUI/MyGUI.h>
#include <MYGUI/MyGUI_Delegate.h>
#include <MYGUI/MyGUI_OgrePlatform.h>

// curl
#include <curl/curl.h>
#include <curl/easy.h>

// Lua
#include <camp-lua/lua.hpp>

// Camp
#include <camp/qt/qt.hpp>

// cAudio
#include <cAudio.h>

// Qt
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QSettings>
#include <QWidget>
#include <QEvent>
#include <QResource>
#include <QTimer>
#include <QTextStream>
#include <QPointer>
#include "qevent.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include <QString>

#include <QtTreePropertyBrowser>
#include <QtVariantProperty>
#include <QtProperty>
#include "qtpropertymanager.h"
#include "qteditorfactory.h"

#endif // DIVERSIA_QTOGREEDITOR_STABLEHEADERS_H