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

#ifndef DIVERSIA_OGRECLIENT_APPLICATION_H
#define DIVERSIA_OGRECLIENT_APPLICATION_H

#include "Platform/Prerequisites.h"

#include <meshmagick/MeshMagick.h>

// Static logger for Ogre
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > ogreLogger
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "Ogre"
);

namespace Diversia
{
namespace BulletMeshGenerator
{
//------------------------------------------------------------------------------

/**
Ogre log listener. 
**/
class OgreLogListener : public Ogre::LogListener
{
    void messageLogged( const String& rMessage, Ogre::LogMessageLevel lml, bool maskDebug, 
        const String &rLogName )
    {
        LogLevel logLevel = LOG_INFO;
        switch( lml )
        {
            case Ogre::LML_TRIVIAL: logLevel = LOG_DEBUG; break;
            case Ogre::LML_NORMAL: logLevel = LOG_INFO; break;
            case Ogre::LML_CRITICAL: logLevel = LOG_CRITICAL; break;
        }
        BOOST_LOG_SEV( ogreLogger, logLevel ) << rMessage;
    }
};

class Application
{
public:
    Application();
    ~Application();

    int parse( int ac, char* av[] );
    
private:
    void initOgre();
    void initMeshMagick( Ogre::Log* log );
    void saveCollisionShape( btCollisionShape* pShape, const String& rFilename );

    // Ogre
    Ogre::Root*         mRoot;
    Ogre::SceneManager* mSceneManager;
    OgreLogListener     mLogListener;

    // MeshMagick
    meshmagick::ToolManager*        mToolManager;
    meshmagick::OgreEnvironment*    mOgreEnvironment;
    meshmagick::OptimiseTool*       mOptimizeTool;
    meshmagick::TransformTool*      mTransformTool;

    // Options
    Path                mInputDir;
    std::vector<Path>   mInputFiles;
    Path                mOutputDir;
    String              mShapeString;
    long                mShape;
    bool                mMeshCenter;
    bool                mMeshOptimize;

};

//------------------------------------------------------------------------------
} // Namespace BulletMeshGenerator
} // Namespace Diversia

#endif // DIVERSIA_BULLETMESHGENERATOR_APPLICATION_H