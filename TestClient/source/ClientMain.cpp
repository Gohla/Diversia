#include "Platform/StableHeaders.h"

#include "Client/Object/ClientObjectManager.h"
#include "Object/ComponentFactoryManager.h"

#include "Shared/Object/TemplateComponentFactory.h"

#include "Shared/Communication/ReplicaManager.h"

#include "Shared/ClientServerPlugin/ClientServerPluginManager.h"
#include "Shared/ClientServerPlugin/ClientServerPluginFactoryManager.h"
#include "Shared/ClientServerPlugin/Factories/ObjectManagerFactory.h"
#include "Shared/ClientServerPlugin/Factories/TemplatePluginFactory.h"

#include "Client/Permission/PermissionManager.h" 

#include "Client/ClientServerPlugin/ServerPluginManager.h"
#include "Client/Communication/ServerNeighborsPlugin.h"
#include "Client/Communication/GridManager.h"
#include "Object/Object.h"

using namespace Diversia::Util;
using namespace Diversia::ObjectSystem;
using namespace Diversia;
using namespace Diversia::Client;
using namespace Diversia::TestClient;

sigc::signal<void> updateSignal;

void func()
{
    // Logging stuff.
    boost::log::add_common_attributes();
    boost::shared_ptr< boost::log::core > pCore = boost::log::core::get();
    pCore->add_global_attribute(
        "Scope", boost::make_shared< boost::log::attributes::named_scope >());
    const Util::LogLevel logLevel = LOG_DEBUG;

    boost::log::init_log_to_console
    (
        std::cout, 
        boost::log::keywords::filter = 
        boost::log::filters::attr< LogLevel >( "Severity" ) >= logLevel,
        boost::log::keywords::format = boost::log::formatters::format( "[%1%] [%2%] <%3%> %4%" )
        % boost::log::formatters::date_time( "TimeStamp", "%d.%m-%H:%M:%S.%f" )
        % boost::log::formatters::attr< String >( "Channel" )
        % boost::log::formatters::attr< LogLevel >( "Severity" )
        % boost::log::formatters::message()
    );
    boost::log::init_log_to_file
    (
        boost::log::keywords::file_name = "Log_%d.%m.%y.log",
        boost::log::keywords::open_mode = std::ios::app,
        boost::log::keywords::filter = 
        boost::log::filters::attr< LogLevel >( "Severity" ) >= logLevel,
        boost::log::keywords::format = boost::log::formatters::format( "[%1%] [%2%] <%3%> %4%" )
        % boost::log::formatters::date_time( "TimeStamp", "%H:%M:%S.%f" )
        % boost::log::formatters::attr< String >( "Channel" )
        % boost::log::formatters::attr< LogLevel >( "Severity" )
        % boost::log::formatters::message()
    );

    // Config load
    ConfigManager configManager;
    if( !configManager.load() )
        LOGE << "Config file was not loaded.";

    // Add component factories

    // Add plugin factories
    TemplatePluginFactory<PermissionManager, ServerPluginManager>::registerFactory();
    TemplatePluginFactory<ServerNeighborsPlugin, ServerPluginManager>::registerFactory();
    ObjectManagerFactory<ClientObjectManager, ServerPluginManager>::registerFactory( updateSignal );

    // Delay client startup for the server
    RakSleep( 1000 );

    // Set up communication
    GridManager* gridManager = new GridManager( updateSignal );
    Server& server = gridManager->createServer( GridPosition( 0, 0 ), ServerInfo( "127.0.0.1", 8500 ) );
    ServerPluginManager& pluginManager = server.getPluginManager();

    // Set up DelayedCall.
    DelayedCall::setUpdateSignal( updateSignal );

    // Enter infinite loop to run the system
    LOGI << "Client initialized.";

    static int counter = 1;
    srand( time( NULL ) );
    static int random = (int)Math::RangeRandom( -1000000, 1000000 );
    bool quit=false;
    while (!quit)
    {
        if ( kbhit() )
        {
            char ch;
            ch=getch();
            if (ch=='q' || ch=='Q')
            {
                LOGI << "Quitting.";
                quit=true;
            }
            if (ch=='c' || ch=='C')
            {
                // Create local object and make it remote if everything can be created.
                try
                {
                    StringStream ss;
                    ss << "ClientObject" << random << counter;

                    ss << "a";
                    Object& object1 = pluginManager.getPlugin<ClientObjectManager>().createObject( 
                        ss.str(), LOCAL );
                    ss << "b";
                    Object& object2 = pluginManager.getPlugin<ClientObjectManager>().createObject( 
                        ss.str(), LOCAL );
                    ss << "c";
                    Object& object3 = pluginManager.getPlugin<ClientObjectManager>().createObject( 
                        ss.str(), LOCAL );
                    ss << "d";
                    Object& object4 = pluginManager.getPlugin<ClientObjectManager>().createObject( 
                        ss.str(), LOCAL );
                    ss << "e";
                    Object& object5 = pluginManager.getPlugin<ClientObjectManager>().createObject( 
                        ss.str(), LOCAL );

                    object5.parent( &object4 );
                    object4.parent( &object3 );
                    object3.parent( &object2 );
                    object2.parent( &object1 );

                    /*try
                    {
                        Position& position = object1.createComponent<Position>( "Position" );
                        object1.createComponent<Position>( "Position2" );
                        position.set( "Position", Vector3( Math::RangeRandom( -1337.0, 1337.0 ) ) );
                        LOGD << "Position: " << position.get( "Position" ).to<Vector3>();
                    }
                    catch ( Exception e )
                    {
                        LOGE << "Could not create component: " << e.getDescription();
                        pluginManager.getPlugin<ClientObjectManager>().destroyObject( ss.str() );
                    }*/

                    try
                    {
                        object1.setNetworkingType( REMOTE );
                    }
                    catch ( Exception e )
                    {
                        LOGE << "Could not set object to remote: " << e.getDescription();
                        pluginManager.getPlugin<ClientObjectManager>().destroyObjectTree( object1 );
                    }

                    counter++;
                }
                catch ( Exception e )
                {
                    LOGE << "Could not create object: " << e.getDescription();
                }
            }
            if (ch=='v' || ch=='V')
            {
                // Create local object.
                try
                {
                    StringStream ss;
                    ss << "ClientObject" << random << counter;

                    Object& object = pluginManager.getPlugin<ClientObjectManager>().createObject( 
                        ss.str(), LOCAL );

                    /*try
                    {
                        object.createComponent<Position>( "Position" );
                    }
                    catch ( Exception e )
                    {
                        LOGE << "Could not create component: " << e.getDescription();

                        pluginManager.getPlugin<ClientObjectManager>().destroyObject( ss.str() );
                    }*/

                    counter++;
                }
                catch ( Exception e )
                {
                    LOGE << "Could not create object: " << e.getDescription();
                }
            }
            if (ch=='b' || ch=='B')
            {
                // Create remote object and make it local.
                try
                {
                    StringStream ss;
                    ss << "ClientObject" << random << counter;

                    Object& object = pluginManager.getPlugin<ClientObjectManager>().createObject( 
                        ss.str(), REMOTE );

                    /*try
                    {
                        object.createComponent<Position>( "Position" );
                    }
                    catch ( Exception e )
                    {
                        LOGE << "Could not create component: " << e.getDescription();
                        pluginManager.getPlugin<ClientObjectManager>().destroyObject( ss.str() );
                    }*/

                    try
                    {
                        object.setNetworkingType( LOCAL );
                    }
                    catch ( Exception e )
                    {
                        LOGE << "Could not set object to local: " << e.getDescription();
                        pluginManager.getPlugin<ClientObjectManager>().destroyObject( ss.str() );
                    }

                    counter++;
                }
                catch ( Exception e )
                {
                    LOGE << "Could not create object: " << e.getDescription();
                }
            }
            if (ch=='d' || ch=='D')
            {
                // Destroy all objects.
                LOGI << "Broadcasting destruction for all objects.";

                const Objects& objects = pluginManager.getPlugin<ClientObjectManager>().getObjects();
                for( Objects::const_iterator i = objects.begin(); i != objects.end(); ++i )
                {
                    try
                    {
                        pluginManager.getPlugin<ClientObjectManager>().destroyObject( i->first );
                    }
                    catch ( Exception e )
                    {
                        LOGE << "Could not destroy object: " << e.getDescription();
                    }
                }
            }
            if (ch=='f' || ch=='F')
            {
                // Destroy all componens.
                LOGI << "Broadcasting destruction for all components.";

                const Objects& objects = pluginManager.getPlugin<ClientObjectManager>().getObjects();
                for( Objects::const_iterator i = objects.begin(); i != objects.end(); ++i )
                {
                    try
                    {
                        const ComponentsByType& components = i->second->getComponentsByType();
                        for( ComponentsByType::const_iterator j = components.begin(); 
                            j != components.end(); ++j )
                        {
                            pluginManager.getPlugin<ClientObjectManager>().getObject( i->first ).destroyComponent( j->first );
                        }
                    }
                    catch ( Exception e )
                    {
                        LOGE << "Could not destroy object: " << e.getDescription();
                    }
                }
            }
            if (ch=='l' || ch=='L')
            {
                // List all objects.
                LOGI << "Listing all objects:";

                const Objects& objects = pluginManager.getPlugin<ClientObjectManager>().getObjects();
                for( Objects::const_iterator i = objects.begin(); i != objects.end(); ++i )
                {
                    LOGI    << "[" << i->second->getName() << "]"
                            << " T:" << i->second->getNetworkingType()
                            << " S:" << i->second->getSource()
                            << " SID:" << i->second->GetCreatingSystemGUID().g
                            //<< " Pos: " << ( i->second->hasComponent<Position>() ? i->second->getComponent<Position>().get( "Position" ).to<Vector3>() : "No position" )
                            << " Parent: " << ( i->second->hasParent() ? i->second->getParent()->getName() : "No parent" );
                }
            }
            if (ch=='r' || ch=='R')
            {
                // Change all objects to remote.
                LOGI << "Setting all objects to remote.";

                const Objects& objects = pluginManager.getPlugin<ClientObjectManager>().getObjects();
                for( Objects::const_iterator i = objects.begin(); i != objects.end(); ++i )
                {
                    try
                    {
                        i->second->setNetworkingType( REMOTE );
                    }
                    catch ( Exception e )
                    {
                        LOGE << "Could not set object to remote: " << e.getDescription();
                    }
                }
            }
            if (ch=='t' || ch=='T')
            {
                // Change all objects to local.
                LOGI << "Setting all objects to local.";

                const Objects& objects = pluginManager.getPlugin<ClientObjectManager>().getObjects();
                for( Objects::const_iterator i = objects.begin(); i != objects.end(); ++i )
                {
                    try
                    {
                        i->second->setNetworkingType( LOCAL );
                    }
                    catch ( Exception e )
                    {
                        LOGE << "Could not set object to local: " << e.getDescription();
                    }
                }
            }
            if (ch=='s' || ch=='S')
            {
                // Change position for all objects.
                LOGI << "Setting position for all position components.";

                const Objects& objects = pluginManager.getPlugin<ClientObjectManager>().getObjects();
                for( Objects::const_iterator i = objects.begin(); i != objects.end(); ++i )
                {
                    /*try
                    {
                        Vector3 random = Vector3( Math::RangeRandom( -1337.0, 1337.0 ) );
                        camp::Value val = random;
                        i->second->getComponent<Position>().set( "Position", 
                           val );
                    }
                    catch ( Exception e )
                    {
                        LOGE << "Could not set position: " << e.getDescription();
                    }*/
                }
            }
        }

        updateSignal();

        RakSleep( 10 );
    }

    // Remove grid.
    delete gridManager;

    // Config save
    if( configManager.save() )
        LOGE << "Config file was not saved.";
}

int main()
{
    using namespace Diversia::Util;

    try
    {
        func();
    }
    catch ( Exception e )
    {
        e.log( Diversia::TestClient::Log::lg );
        system ("pause");
    }

    return 0;
}