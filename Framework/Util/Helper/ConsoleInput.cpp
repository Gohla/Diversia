/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "Util/Platform/StableHeaders.h"

#include "Util/Helper/ConsoleInput.h"
#include "Util/Helper/Kbhit.h"

namespace Diversia
{
namespace Util 
{
//------------------------------------------------------------------------------

String                      ConsoleInput::mInputBuffer = String();
sigc::slot<void, String>    ConsoleInput::mCommandCallback = sigc::slot<void, String>();
sigc::slot<void>            ConsoleInput::mConsoleClosedCallback = sigc::slot<void>();

void ConsoleInput::setCommandCallback( const sigc::slot<void, String>& rCallback )
{
    mCommandCallback = rCallback;
}

void ConsoleInput::setConsoleClosedCallback( const sigc::slot<void>& rCallback )
{
    mConsoleClosedCallback = rCallback;

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
    if ( SetConsoleCtrlHandler( &ConsoleInput::consoleHandler, TRUE ) == FALSE )
        ULOGE << "Could not install console handler.";
#else
    ULOGW << "Cannot install console input handler, unsupported OS".;
#endif
}

void ConsoleInput::setUpdateSignal( sigc::signal<void>& rUpdateSignal )
{
    rUpdateSignal.connect( sigc::ptr_fun( &ConsoleInput::parse ) );
}

void ConsoleInput::parse()
{
    if ( kbhit() )
    {
        char ch = getch();

        if( (int)ch == 13 && !mInputBuffer.empty() )
        {
            // Enter
            std::cout << "Lua: " << mInputBuffer << std::endl;
            mCommandCallback( mInputBuffer );
            String spaces( mInputBuffer.size(), ' ' );
            std::cout << spaces << "\r" << std::flush;
            mInputBuffer.clear();
        }
        else if( (int)ch == 8 && !mInputBuffer.empty() )
        {
            // Backspace
            mInputBuffer[ mInputBuffer.size() - 1 ] = ' ';
            std::cout << mInputBuffer << "\r" << std::flush;
            mInputBuffer.erase( mInputBuffer.size() - 1 );
        }
        else if( (int)ch >= 32 && (int)ch <= 126 )
        {
            // Normal input
            mInputBuffer.append( 1, ch );
        }

        std::cout << mInputBuffer << "\r" << std::flush;
    }
}

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
BOOL WINAPI ConsoleInput::consoleHandler( DWORD CEvent )
{
    switch( CEvent )
    {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            mConsoleClosedCallback();
            break;
    }

    return TRUE;

}
#endif

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia