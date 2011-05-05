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

#include "OgreClient/Platform/StableHeaders.h"

#include "OgreClient/Graphics/Atlas.h"
#include "OgreClient/Graphics/Fader.h"
#include "OgreClient/Graphics/GraphicsManager.h"
#include "OgreClient/Graphics/StaticPluginLoader.h"
#include "OgreClient/Resource/GenericResourceManager.h"
#include "OgreClient/Resource/URLArchive.h"
#include "Util/Helper/Transition.h"
#include <Ogre/OgreMaterial.h>
#include <Ogre/OgrePass.h>
#include <Ogre/OgreTechnique.h>
#include <Ogre/OgreTextureUnitState.h>

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

GraphicsManager::GraphicsManager():
    mRoot( 0 ),
    mSceneMgr( 0 ),
    mWindow( 0 ),
    mRenderSystem( 0 ),
    mFader( 0 ),
    mAtlasFont( "LucidaSans.ttf" ),
    mRootResourceLocation( "media" ),
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
    mRendererName( "Direct3D9 Rendering Subsystem" ),
#elif
    mRendererName( "OpenGL Rendering Subsystem" ),
#endif
    mWindowWidth( 800 ),
    mWindowHeight( 600 ),
    mWindowName( "Diversia OgreClient" ),
    mFullscreen( false ),
    mVSync( true ),
    mAliasing( 0 ),
    mNumMipmaps( 4 ),
    mTextureFilteringType( Ogre::TFO_TRILINEAR ),
    mAnisotropyLevel( 8 )
{
    // Set default plugins
#ifndef DIVERSIA_OGRECLIENT_OGRE_STATIC
#   ifdef DIVERSIA_PLATFORM_WIN32
    mPlugins.push_back( "RenderSystem_Direct3D9" );
#   endif
    mPlugins.push_back( "RenderSystem_GL" );
    mPlugins.push_back( "Plugin_ParticleFX" );
    mPlugins.push_back( "Plugin_CgProgramManager" );
    mPlugins.push_back( "Plugin_OctreeSceneManager" );

#   ifdef DIVERSIA_DEBUG
    // Append _d in debug mode.
    for( std::vector<String>::iterator i = mPlugins.begin(); i != mPlugins.end(); ++i )
    {
        i->append( "_d" );
    }
#   endif
#endif

    // Set default resource locations.
    mResourceLocations.push_back( OgreResourceParams() );
    mResourceLocations.push_back( OgreResourceParams( "Ogre" ) );
    mResourceLocations.push_back( OgreResourceParams( "RTShaderLib" ) );

    GlobalsBase::mGraphics = this;
}

GraphicsManager::~GraphicsManager()
{
    if( mRoot ) delete mRoot;

    GlobalsBase::mScene = 0;
    GlobalsBase::mGraphics = 0;
}

void GraphicsManager::init( unsigned int width /*= 0*/, unsigned int height /*= 0*/, 
    const String& rWidgetHandle /*= ""*/ )
{
    // Root resource location
    if( boost::filesystem::exists( "media" ) )
    {
        mRootResourceLocation = "media";
    }
    else if( boost::filesystem::exists( "../../media" ) )
    {
        mRootResourceLocation = "../../media";
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_FILE_NOT_FOUND, 
            "Media directory cannot be found at 'media' or '../../media', please get the media \
directory or set the correct directory in config.xml under 'RootResourceLocation'.", 
            "GraphicsManager::init" );
    }

    mRoot = new Ogre::Root( "", "", "" );

    // Custom logging
    Ogre::Log* log = Ogre::LogManager::getSingletonPtr()->createLog( "Ogre.log", true, false, true );
    log->addListener( &mLogListener );

#if defined(DIVERSIA_OGRECLIENT_OGRE_STATIC)
    // Load plugins statically.
    Ogre::StaticPluginLoader staticPluginLoader;
    staticPluginLoader.load( *mRoot );
    mPlugins.clear();
#else
    // Load plugins dynamically.
    for( std::vector<String>::iterator i = mPlugins.begin(); i != mPlugins.end(); ++i )
    {
        mRoot->loadPlugin( *i );
    }
#endif

    // Pick render system.
    Ogre::RenderSystemList renderers = mRoot->getAvailableRenderers();
    if( renderers.size() == 0 )
    {
        DIVERSIA_EXCEPT( Exception::ERR_INTERNAL_ERROR, "No render systems available.", 
            "GraphicsManager::init" );
    }

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
    Ogre::RenderSystem* renderer = 0;
    for( size_t i = 0; i < renderers.size(); ++i )
    {
        const std::string currentName( renderers.at( i )->getName() );
        const size_t length = strlen( mRendererName.c_str() );
        if( strncmp( currentName.c_str(), mRendererName.c_str(), length ) == 0 )
        {
            renderer = renderers.at( i );
            break;
        }
    }
    if( renderer == 0 )
    {
        renderer = renderers.front();
    }
    mRoot->setRenderSystem( renderer );
#else
    mRoot->setRenderSystem( renderers.front() );
#endif

    // Initialize root, don't auto create a render window.
    mRoot->initialise( false );

    Ogre::NameValuePairList ogreParams;
    if( mVSync )
        ogreParams["vsync"] = "Yes";

    ogreParams["FSAA"] = boost::lexical_cast<String>( mAliasing );

    // Editor specific variables
    if( !rWidgetHandle.empty() )
    {
        ogreParams["externalWindowHandle"] = rWidgetHandle;
        mFullscreen = false;
        mWindowWidth = width;
        mWindowHeight = height;
    }

    // Create render window.
    mWindow = Ogre::Root::getSingleton().createRenderWindow(
        mWindowName, mWindowWidth, mWindowHeight, mFullscreen, &ogreParams );

    if( rWidgetHandle.empty() )
        Ogre::WindowEventUtilities::addWindowEventListener( mWindow, this );

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps( mNumMipmaps );
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering( mTextureFilteringType );
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy( mAnisotropyLevel );

    // Create scene manager.
    mSceneMgr = mRoot->createSceneManager( Ogre::ST_GENERIC );
    GlobalsBase::mScene = mSceneMgr;

    // Load resources
    GenericResourceManager::getSingletonPtr();
    std::set<String> groups;

    Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
    for( Resources::const_iterator i = mResourceLocations.begin(); i != mResourceLocations.end(); ++i )
    {
        Path resourcePath = mRootResourceLocation / i->mLocation;
        rgm.addResourceLocation( resourcePath.string(), i->mArchive, i->mGroup, i->mRecursive );
        groups.insert( i->mGroup );
    }

    for( std::set<String>::const_iterator i = groups.begin(); i != groups.end(); ++i )
    {
        Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup( *i );
    }

    // Add URLAchive
    Path cachePath = mRootResourceLocation / "cache";
    URLArchive::setCaching( cachePath.directory_string(), true );
    Ogre::ArchiveManager::getSingleton().addArchiveFactory( new URLArchiveFactory() );

    // Create fader
    mFader.reset( new Fader( "Overlays/FadeInOut", "Materials/OverlayMaterial", 
        *GlobalsBase::mFrameSignal ) );

    // Create atlas
    FontFaceDefinition titleFont( mAtlasFont );
    for( unsigned int size = 10; size <= 20; ++size )
        titleFont.addSize( size );

    mAtlas = new Atlas( std::vector<String>(), 
        initializer< std::vector<FontFaceDefinition> >( titleFont ), "General" );

    // Create RTShader system
    if ( Ogre::RTShader::ShaderGenerator::initialize() )
    {
        Ogre::RTShader::ShaderGenerator* shaderGenerator = 
            Ogre::RTShader::ShaderGenerator::getSingletonPtr();

        // Set shader cache path.
        Path cachePath = mRootResourceLocation / "ShaderCache";
        boost::filesystem::create_directories( cachePath );
        shaderGenerator->setShaderCachePath( cachePath.string() );

        // Set the scene manager.
        shaderGenerator->addSceneManager( mSceneMgr );
    }

    // Set zero values for transitions
    TransitionZeroHelper<Ogre::ColourValue>::mZero = Ogre::ColourValue::ZERO;
    TransitionZeroHelper<Ogre::Vector2>::mZero = Ogre::Vector2::ZERO;
    TransitionZeroHelper<Ogre::Vector3>::mZero = Ogre::Vector3::ZERO;
    TransitionZeroHelper<Ogre::Vector4>::mZero = Ogre::Vector4::ZERO;
    TransitionZeroHelper<Ogre::Quaternion>::mZero = Ogre::Quaternion::ZERO;
}

void GraphicsManager::addResourceLocation( const OgreResourceParams& rParams )
{
    mResourceLocations.push_back( rParams );
}

ResourceSet GraphicsManager::getTextureNamesFromMaterial( const String& rMaterialName,
    const String& rGroupName )
{
    ResourceSet textureNames;

    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingletonPtr()->getByName( 
        rMaterialName, rGroupName );
    if( material.isNull() ) return textureNames;
    material->load();

    Ogre::Material::TechniqueIterator ti = material->getSupportedTechniqueIterator();
    while( ti.hasMoreElements() )
    {
        Ogre::Technique* technique = ti.getNext();
        Ogre::Technique::PassIterator pi = technique->getPassIterator();
        while( pi.hasMoreElements() )
        {
            const Ogre::Pass* pass = pi.getNext();
            Ogre::Pass::ConstTextureUnitStateIterator tui = pass->getTextureUnitStateIterator();
            while( tui.hasMoreElements() )
            {
                Ogre::TextureUnitState* textureUnitState = tui.getNext();
                textureNames.insert( ResourceInfo( textureUnitState->getTextureName(), "Texture" ) );

                for( unsigned int i = 0; i < textureUnitState->getNumFrames(); ++i )
                {
                    textureNames.insert( ResourceInfo( textureUnitState->getFrameTextureName( i ), 
                        "Texture" ) );
                }
            }
        }
    }

    return textureNames;
}

void GraphicsManager::windowMoved( Ogre::RenderWindow* pRenderWindow )
{
    unsigned int ignore;
    int left;
    int top;

    pRenderWindow->getMetrics( ignore, ignore, ignore, left, top );

    mWindowMovedEvent( left, top );
}

void GraphicsManager::windowResized( Ogre::RenderWindow* pRenderWindow )
{
    unsigned int width;
    unsigned int height;
    unsigned int ignore;
    int ignore2;

    pRenderWindow->getMetrics( width, height, ignore, ignore2, ignore2 );

    mWindowResizedEvent( width, height );
}

void GraphicsManager::windowFocusChange( Ogre::RenderWindow* pRenderWindow )
{
    mWindowFocusEvent( pRenderWindow->isActive() || pRenderWindow->isVisible() );
}

bool GraphicsManager::windowClosing( Ogre::RenderWindow* pRenderWindow )
{
    mWindowClosingEvent();
    return false;
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia