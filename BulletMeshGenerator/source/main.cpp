#include "Platform/StableHeaders.h"

#include "Application.h"

int main( int ac, char* av[] )
{
    using namespace Diversia::BulletMeshGenerator;

    try
    {
        Application application;
        return application.parse( ac, av );
    }
    catch( Exception e )
    {
        e.log( Diversia::BulletMeshGenerator::Log::lg, true );
    }
    catch( Ogre::Exception e )
    {
        LOGC << "OGRE UNHANDLED EXCEPTION: " << e.what();
    }
    catch( std::logic_error e )
    {
        LOGC << e.what();
    }
    catch( std::exception e )
    {
        LOGC << "OTHER UNHANDLED EXCEPTION: " << e.what();
    }
    catch( ... )
    {
        LOGC << "UNKNOWN UNHANDLED EXCEPTION";
    }

    return 0;
}