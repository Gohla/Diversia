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

#ifndef DIVERSIA_OGRECLIENT_CONFIG_H
#define DIVERSIA_OGRECLIENT_CONFIG_H

#ifdef DIVERSIA_DEPENDENCIES_STATIC
// Link ogre as a static library.
#   define DIVERSIA_OGRECLIENT_OGRE_STATIC
// Link MyGUI as a static library.
#   define MYGUI_STATIC
// Link curl as a static library.
#   define CURL_STATICLIB
// Link curlpp as a static library.
#   define CURLPP_STATICLIB
// Link FreeImage as a static library.
#   define FREEIMAGE_LIB
// Link cAudio as a static library.
#   define CAUDIO_STATIC_LIB
#endif

// Choose ogre plugins to statically link.
#ifdef DIVERSIA_OGRECLIENT_OGRE_STATIC
#   define OGRE_STATIC_LIB
#   define ENABLE_PLUGIN_CgProgramManager
#   define ENABLE_PLUGIN_OctreeSceneManager
#   define ENABLE_PLUGIN_ParticleFX
//#   define ENABLE_PLUGIN_BSPSceneManager
#   if DIVERSIA_PLATFORM ==  DIVERSIA_PLATFORM_WIN32
#       define ENABLE_PLUGIN_Direct3D9
#   elif DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX || DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE
#       define ENABLE_PLUGIN_GL
#   endif
#endif

#endif // DIVERSIA_OGRECLIENT_CONFIG_H