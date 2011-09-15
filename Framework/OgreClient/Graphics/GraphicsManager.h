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

#ifndef DIVERSIA_OGRECLIENT_GRAPHICSMANAGER_H
#define DIVERSIA_OGRECLIENT_GRAPHICSMANAGER_H

#include "OgreClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

// Static logger for Ogre
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > ogreLogger
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "Ogre"
);

/**
Ogre log listener. 
**/
class OgreLogListener : public Ogre::LogListener
{
    void messageLogged( const String& rMessage, Ogre::LogMessageLevel lml, bool maskDebug, 
        const String &rLogName, bool& skipThisMessage )
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

/**
Ogre resource parameters.
**/
struct OgreResourceParams
{
public:
    OgreResourceParams( const Path& rLocation = "", const String& rGroup = "General",
        const String& rArchive = "FileSystem", bool recursive = false ): mLocation( rLocation ), 
        mGroup( rGroup ), mArchive( rArchive ), mRecursive( recursive ) {}

    Path    mLocation;
    String  mGroup;
    String  mArchive;
    bool    mRecursive;
};

typedef std::list<OgreResourceParams> Resources;

/**
Ogre graphics manager.
**/
class DIVERSIA_OGRECLIENT_API GraphicsManager : public Ogre::WindowEventListener
{
public:
    /**
    Default constructor. 
    **/
    GraphicsManager();
    /**
    Destructor. 
    **/
    virtual ~GraphicsManager();

    /**
    Initialises the graphics.
    **/
    void init( unsigned int width = 0, unsigned int height = 0, const String& rWidgetHandle = "", 
        const String& rRootResourceLocation = "" );
    /**
    Adds a resource location to the settings. Doesn't add it to Ogre!
    
    @param  rParams Resource params.
    **/
    void addResourceLocation( const OgreResourceParams& rParams );

    static ResourceSet getTextureNamesFromMaterial( const String& rMaterialName, 
        const String& rGroupName );

    inline Ogre::Root* getRoot() const { return mRoot; }
    inline Ogre::SceneManager* getSceneMgr() const { return mSceneMgr; }
    inline Ogre::RenderWindow* getWindow() const { return mWindow; }
    inline Ogre::RenderSystem* getRenderSystem() const { return mRenderSystem; }
    inline Fader* getFader() const { return mFader.get(); }
    inline Atlas* getAtlas() const { return mAtlas; }
    inline const String& getAtlasFont() const { return mAtlasFont; }
    inline const Path& getRootResourceLocation() const { return mRootResourceLocation; }

    /**
    Connects a slot to the window moved signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(int, int)) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    sigc::connection connectWindowMoved( const sigc::slot<void, int, int>& rSlot ) 
    {
        return mWindowMovedEvent.connect( rSlot );
    }
    /**
    Connects a slot to the window resized signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(unsigned int, unsigned int)) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    sigc::connection connectWindowResized( const sigc::slot<void, unsigned int, unsigned int>& rSlot ) 
    {
        return mWindowResizedEvent.connect( rSlot );
    }
    /**
    Connects a slot to the window focus signal. 
    
    @param [in,out] rSlot   The slot (signature: void func(bool)) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    sigc::connection connectWindowFocus( const sigc::slot<void, bool>& rSlot ) 
    {
        return mWindowFocusEvent.connect( rSlot );
    }
    /**
    Connects a slot to the window closing signal. 
    
    @param [in,out] rSlot   The slot (signature: void func()) to connect. 
    
    @return Connection object to block or disconnect the connection.
    **/
    sigc::connection connectWindowClosing( const sigc::slot<void>& rSlot ) 
    {
        return mWindowClosingEvent.connect( rSlot );
    }

    void windowMoved( Ogre::RenderWindow* pRenderWindow );
    void windowResized( Ogre::RenderWindow* pRenderWindow );
    void windowFocusChange( Ogre::RenderWindow* pRenderWindow );
    bool windowClosing( Ogre::RenderWindow* pRenderWindow );
    
private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.
    
    Ogre::Root*                 mRoot;
    Ogre::SceneManager*         mSceneMgr;
    Ogre::RenderWindow*         mPrimaryWindow;
    Ogre::RenderWindow*         mWindow;
    Ogre::RenderSystem*         mRenderSystem;
    OgreLogListener             mLogListener;
    boost::scoped_ptr<Fader>    mFader;
    Atlas*                      mAtlas;
    String                      mAtlasFont;

    sigc::signal<void, int, int>                    mWindowMovedEvent;
    sigc::signal<void, unsigned int, unsigned int>  mWindowResizedEvent;
    sigc::signal<void, bool>                        mWindowFocusEvent;
    sigc::signal<void>                              mWindowClosingEvent;

    // Options
    std::vector<String>             mPlugins;
    Path                            mRootResourceLocation;
    Resources                       mResourceLocations;
    String                          mRendererName;
    unsigned int                    mWindowWidth;
    unsigned int                    mWindowHeight;
    String                          mWindowName;
    bool                            mFullscreen;
    bool                            mVSync;
    unsigned short                  mAliasing;
    unsigned short                  mNumMipmaps;
    Ogre::TextureFilterOptions      mTextureFilteringType;
    unsigned short                  mAnisotropyLevel;
    
};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::GraphicsManager, 
    &Diversia::OgreClient::Bindings::CampBindings::bindGraphicsManager );

CAMP_AUTO_TYPE( Diversia::OgreClient::OgreResourceParams, 
    &Diversia::OgreClient::Bindings::CampBindings::bindOgreResourceParams );

CAMP_AUTO_TYPE( Ogre::TextureFilterOptions, 
    &Diversia::OgreClient::Bindings::CampBindings::bindTextureFilterOptions );

#endif // DIVERSIA_OGRECLIENT_GRAPHICSMANAGER_H