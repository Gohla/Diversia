#include "Platform/StableHeaders.h"

#include "Application.h"

int main( int argc, char* argv[] )
{
    Diversia::Server::Application application;
    application.init();
    return 0;
}