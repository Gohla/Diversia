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

#include "Platform/StableHeaders.h"

#include "Application.h"
#include "Ogre/StaticPluginLoader.h"
#include "BtOgre/BtOgreGP.h"
#include "Shared/Physics/Physics.h"

#include <bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <meshmagick/MmOptimiseToolFactory.h>
#include <meshmagick/MmTransformToolFactory.h>

using namespace boost::program_options;
using namespace std;

namespace Diversia
{
namespace BulletMeshGenerator 
{
//------------------------------------------------------------------------------

void conflicting_options(const variables_map& vm, 
                         const char* opt1, const char* opt2)
{
    if (vm.count(opt1) && !vm[opt1].defaulted() 
        && vm.count(opt2) && !vm[opt2].defaulted())
        throw logic_error(string("Conflicting options '") 
                          + opt1 + "' and '" + opt2 + "'.");
}

/* Function used to check that of 'for_what' is specified, then
   'required_option' is specified too. */
void option_dependency(const variables_map& vm,
                        const char* for_what, const char* required_option)
{
    if (vm.count(for_what) && !vm[for_what].defaulted())
        if (vm.count(required_option) == 0 || vm[required_option].defaulted())
            throw logic_error(string("Option '") + for_what 
                              + "' requires option '" + required_option + "'.");
}


Application::Application():
    mRoot( 0 ),
    mShapeString( "Sphere" ),
    mShape( 0 ),
    mMeshOptimize( false ),
    mMeshCenter( false )
{

}

Application::~Application()
{
    if( mRoot ) delete mRoot;
}

int Application::parse( int ac, char* av[] )
{
    // Init logging
    boost::log::init_log_to_console
    (
        std::cout, 
        boost::log::keywords::filter = 
        boost::log::filters::attr< LogLevel >( "Severity" ) >= LOG_INFO,
        boost::log::keywords::format = boost::log::formatters::format( "%1%" )
        % boost::log::formatters::message()
    );

    // Init program options
    options_description desc( "Allowed options" );
    desc.add_options()
        ( "help", "Shows help" )
        ( "input-dir,d", value( &mInputDir ), "Directory with the input files" )
        ( "input-file,f", value( &mInputFiles ), "Input files to convert" )
        ( "output-dir,o", value( &mOutputDir ), "Output directory for bullet collision shape files" )
        ( "shape,s", value( &mShapeString ), "Collision shape type (Sphere, Box, Cylinder, ConvexHull, Trimesh)" )
        ( "mesh-center,c", bool_switch( &mMeshCenter ), "Center the Ogre mesh (recommended)" )
        //( "mesh-optimize,p", bool_switch( &mMeshOptimize ), "Optimize the Ogre mesh" )
        
    ;

    positional_options_description p;
    p.add( "input-file", -1 );

    variables_map vm;        
    store( command_line_parser( ac, av ).options( desc ).positional( p ).run(), vm );
    notify( vm );    

    if ( vm.count( "help" ) || !vm.size() ) {
        LOGI << "Usage: " << Path( av[0] ).filename() << " [files] [options]";
        LOGI << desc;
        return 1;
    }

    if( vm.count( "input-file" ) )
    {
        // Input path
        if( vm.count( "input-dir" ) )
        {
            if( !boost::filesystem::exists( mInputDir ) )
            {
                DIVERSIA_EXCEPT( Exception::ERR_FILE_NOT_FOUND, "Input directory does not exist.", 
                    "Application::parse" );
            }
        }

        // Output path
        if( vm.count( "output-dir" ) )
        {
            boost::filesystem::create_directories( mOutputDir );
        }
        
        // Physics shape
        if( vm.count( "shape" ) )
        {
            mShape = camp::enumByType<PhysicsShape>().value( mShapeString );
        }

        // Do the files exist?
        for( vector<Path>::iterator i = mInputFiles.begin(); i != mInputFiles.end(); ++i )
        {
            Path file = mInputDir / *i;

            if( !boost::filesystem::is_regular_file( file ) )
            {
                DIVERSIA_EXCEPT( Exception::ERR_FILE_NOT_FOUND, 
                    "File " + file.string() + " does not exist.", 
                    "Application::parse" );
            }
        }

        // Init ogre
        Application::initOgre();

        LOGI << "Converting Ogre meshes to bullet meshes";

        // Load resources into ogre.
        Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
        rgm.addResourceLocation( mInputDir.string(), "FileSystem" );
        rgm.initialiseResourceGroup( Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
        rgm.loadResourceGroup( Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );

        // Perform mesh operations
        if( mMeshCenter )
        {
            Ogre::StringVector files;
            for( vector<Path>::iterator i = mInputFiles.begin(); i != mInputFiles.end(); ++i )
            {
                String fileName = (*i).filename();
                files.push_back( fileName );
            }
            meshmagick::OptionList options;
            
            if( mMeshCenter )
            {
                LOGI << "Centering all meshes";
                options.push_back( meshmagick::Option( std::make_pair( "xalign", Ogre::String( "center" ) ) ) );
                options.push_back( meshmagick::Option( std::make_pair( "yalign", Ogre::String( "center" ) ) ) );
                options.push_back( meshmagick::Option( std::make_pair( "zalign", Ogre::String( "center" ) ) ) );
            }

            mTransformTool->invoke( options, options, files, files );
        }

        // Reload resources.
        rgm.unloadResourceGroup( Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
        rgm.initialiseResourceGroup( Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
        rgm.loadResourceGroup( Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );

        // Convert and saveCollisionShape all meshes.
        for( vector<Path>::iterator i = mInputFiles.begin(); i != mInputFiles.end(); ++i )
        {
            String fileName = (*i).filename();

            // Load mesh
            Ogre::Entity* entity = mSceneManager->createEntity( fileName );

            // Convert mesh to bullet shape.
            btCollisionShape* collisionShape;
            BtOgre::StaticMeshToShapeConverter converter;
            converter.addEntity( entity );

            switch( mShape )
            {
                case PHYSICSSHAPE_SPHERE:
                    collisionShape = converter.createSphere();
                    break;
                case PHYSICSSHAPE_BOX:
                    collisionShape = converter.createBox();
                    break;
                case PHYSICSSHAPE_CYLINDER:
                    collisionShape = converter.createCylinder();
                    break;
                case PHYSICSSHAPE_CONVEXHULL:
                    collisionShape = converter.createConvex();
                    break;
                case PHYSICSSHAPE_BVHTRIANGLEMESH:
                    collisionShape = converter.createTrimesh();
                    break;
                default:
                    DIVERSIA_EXCEPT( Exception::ERR_INVALIDPARAMS, 
                        "Unknown or unhandled collision shape chosen", 
                    	"Application::parse" );
                    break;
            }

            // Save collision shape
            Application::saveCollisionShape( collisionShape, fileName );
            delete collisionShape;

            // Unload mesh
            mSceneManager->destroyEntity( entity );
        }
    }

    return 0;
}

void Application::initOgre()
{
    mRoot = new Ogre::Root( "", "", "" );

    // Custom logging
    Ogre::Log* log = Ogre::LogManager::getSingletonPtr()->createLog( "Ogre.log", true, false, true );
    log->addListener( &mLogListener );
    Ogre::LogManager::getSingletonPtr()->setLogDetail( Ogre::LL_LOW );

#ifdef DIVERSIA_BULLETMESHGENERATOR_OGRE_STATIC
    // Load plugins statically
    Ogre::StaticPluginLoader staticPluginLoader;
    staticPluginLoader.load( *mRoot );
#else
    // Load plugins dynamically
#   ifdef DIVERSIA_PLATFORM_WIN32
    mRoot->loadPlugin( "RenderSystem_Direct3D9" );
#   else
    mRoot->loadPlugin( "RenderSystem_GL" );
#   endif
#endif

    // Pick render system
    Ogre::RenderSystemList renderers = mRoot->getAvailableRenderers();
    if( renderers.size() == 0 )
    {
        DIVERSIA_EXCEPT( Exception::ERR_INTERNAL_ERROR, "No render systems available.", 
            "GraphicsManager::init" );
    }
    mRoot->setRenderSystem( renderers.front() );

    // Initialize root, don't auto create a render window.
    mRoot->initialise( false );

    // Create 'hidden' window
    // TODO: Use hidden param in new ogre.
    Ogre::NameValuePairList hiddenParams; hiddenParams["border"] = "none";
    Ogre::RenderWindow* window = mRoot->createRenderWindow( "Hidden", 1, 1, false, &hiddenParams );
    window->setVisible( false );
    window->setAutoUpdated( false );

    mSceneManager = mRoot->createSceneManager( Ogre::ST_GENERIC );

    // Init mesh magick
    Application::initMeshMagick( log );
}

void Application::saveCollisionShape( btCollisionShape* pShape, const String& rFilename )
{
    int maxSerializeBufferSize = 1024*1024*5;
    btDefaultSerializer* serializer = new btDefaultSerializer( maxSerializeBufferSize );

    serializer->startSerialization();
    pShape->serializeSingleShape( serializer );
    serializer->finishSerialization();

    String saveName = String( rFilename.substr( 0, rFilename.find_last_of( "." ) ) ) + 
        ".bullet";
    Path savePath = mOutputDir / saveName;
    LOGI << "Converting mesh " << rFilename << " to a " << mShapeString << " collision shape.";

    FILE* file = fopen( savePath.string().c_str(), "wb" );
    fwrite( serializer->getBufferPointer(), serializer->getCurrentBufferSize(), 1, file );
    fclose( file );

    delete serializer;
}

void Application::initMeshMagick( Ogre::Log* log )
{
    mOgreEnvironment = new meshmagick::OgreEnvironment();
    mOgreEnvironment->initialize( false, log );
    mToolManager = new meshmagick::ToolManager();

    mToolManager->registerToolFactory( new meshmagick::OptimiseToolFactory() );
    mToolManager->registerToolFactory( new meshmagick::TransformToolFactory() );

    mTransformTool = static_cast<meshmagick::TransformTool*>( mToolManager->createTool( 
        "transform" ) );
    mOptimizeTool = static_cast<meshmagick::OptimiseTool*>( mToolManager->createTool( 
        "optimise" ) );
}

//------------------------------------------------------------------------------
} // Namespace BulletMeshGenerator
} // Namespace Diversia