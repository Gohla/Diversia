/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_STABLEHEADERS_H
#define DIVERSIA_QTOGREEDITOR_STABLEHEADERS_H

// Platform
#include "Util/Build/Platform.h"
#include "Util/Build/StdHeaders.h"
#include "Platform/Forward.h"
#include "Platform/Prerequisites.h"

// Libraries
// RakNet
#include <RakNet/RakPeerInterface.h>
#include <RakNet/NetworkIDManager.h>
#include <RakNet/ReplicaManager3.h>
#include <RakNet/RakString.h>
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
#include <QtGui/QtGui>
#include <QtCore/QtCore>

// Framework
#include "Util/UtilIncludes.h"
#include "Object/ObjectIncludes.h"
#include "Shared/SharedIncludes.h"
#include "Client/ClientIncludes.h"

#endif // DIVERSIA_QTOGREEDITOR_STABLEHEADERS_H