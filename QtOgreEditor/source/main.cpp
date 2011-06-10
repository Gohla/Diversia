#include "Platform/StableHeaders.h"

#include "EditorApplication.h"

int main( int argc, char* argv[] )
{
    Diversia::QtOgreEditor::EditorApplication app( argc, argv );
    app.init( argc, argv );
    app.run();
    return 0;
}

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    main( 0, 0 );
}
#endif
