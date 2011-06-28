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

#ifndef DIVERSIA_UTIL_CONSOLEINPUT_H
#define DIVERSIA_UTIL_CONSOLEINPUT_H

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

class DIVERSIA_UTIL_API ConsoleInput : public sigc::trackable
{
public:
    /**
    Sets the update signal. Console input will be parsed after calling this.
    **/
    static void setUpdateSignal( sigc::signal<void>& rUpdateSignal );
    /**
    Sets the command received callback.
    **/
    static void setCommandCallback( const sigc::slot<void, String>& rCallback );
    /**
    Sets the console closed callback.
    **/
    static void setConsoleClosedCallback( const sigc::slot<void>& rCallback );

private:
    /**
    Parses the console for input.
    **/
	static void parse();

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
    static BOOL WINAPI consoleHandler( DWORD CEvent );
#endif

    static String                      mInputBuffer;
    static sigc::slot<void, String>    mCommandCallback;
    static sigc::slot<void>            mConsoleClosedCallback;
};

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_CONSOLEINPUT_H