#include "DefaultClient/Platform/StableHeaders.h"

#include "ClientApplication.h"

int main( int argc, char* argv[] )
{
    Diversia::DefaultClient::ClientApplication application;
    application.init();
    return 0;
}

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    main( 0, 0 );
}
#endif
