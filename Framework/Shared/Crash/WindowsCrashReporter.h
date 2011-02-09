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

#ifndef DIVERSIA_SHARED_WINDOWSCRASHDUMP_H
#define DIVERSIA_SHARED_WINDOWSCRASHDUMP_H

#include "Shared/Platform/Prerequisites.h"

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32

#include "Shared/Crash/CrashReporter.h"

namespace Diversia
{
//------------------------------------------------------------------------------

// This is a crash reporter that will send a minidump by email on unhandled exceptions
// This normally only runs if you are not currently debugging.
// To send reports while debugging (mostly to test this class), define _DEBUG_CRASH_REPORTER
// and put your code in a try/except block such as
//
// extern void DumpMiniDump(PEXCEPTION_POINTERS excpInfo);
//
// void main(void)
//{
//__try 
//{
//	RunGame();
//}
//__except(DumpMiniDump(GetExceptionInformation()),EXCEPTION_EXECUTE_HANDLER) 
//{
//}
//}

// The minidump can be opened in visual studio and will show you where the crash occurred and give you the local variable values.
//
// How to use the minidump:
//
// Put the minidump on your harddrive and double click it.  It will open Visual Studio.  It will look for the exe that caused the crash in the directory
// that the program that crashed was running at.  If it can't find this exe, or if it is different, it will look in the current
// directory for that exe.  If it still can't find it, or if it is different, it will load Visual Studio and indicate that it can't find
// the executable module.  No source code will be shown at that point.  However, you can specify modpath=<pathToExeDirectory> in the
// project properties window for "Command Arguments".
// The best solution is copy the .dmp to a directory containing a copy of the exe that crashed.
//
// On load, Visual Studio will look for the .pdb, which it uses to find the source code files and other information.  This is fine as long as the source
// code files on your harddrive match those that were used to create the exe.  If they don't, you will see source code but it will be the wrong code.
// There are three ways to deal with this.
//
// The first way is to change the path to your source code so it won't find the wrong code automatically.
// This will cause the debugger to not find the source code pointed to in the .pdb .  You will be prompted for the location of the correct source code.
//
// The second way is to build the exe on a different path than what you normally program with. For example, when you program you use c:/Working/Mygame
// When you release builds, you do at c:/Version2.2/Mygame .  After a build, you keep the source files, the exe, and the pdb
// on a harddrive at that location.  When you get a crash .dmp, copy it to the same directory as the exe, ( c:/Version2.2/Mygame/bin )
// This way the .pdb will point to the correct sources to begin wtih.
//
// The third way is save build labels or branches in source control and get that version (you only need source code + .exe + .pdb) before debugging.
// After debugging, restore your previous work.
//

// To use:
// #include "DbgHelp.h"
// Link with Dbghelp.lib ws2_32.lib

/// \brief On an unhandled exception, will save a minidump and email it.
/// A minidump can be opened in visual studio to give the callstack and local variables at the time of the crash.
/// It has the same amount of information as if you crashed while debugging in the relevant mode.  So Debug tends to give
/// accurate stacks and info while Release does not.
///
/// Minidumps are only accurate for the code as it was compiled at the date of the release.  So you should label releases in source control
/// and put that label number in the 'appVersion' field.
class DIVERSIA_SHARED_API WindowsCrashReporter : public CrashReporter
{
public:
    /**
    Default constructor. 
    **/
    WindowsCrashReporter();
    /**
    Destructor. 
    **/
    ~WindowsCrashReporter();

    /**
    Sets if a large or small minidump should be created. Defaults to false.
    **/
    void setCreateLargeMiniDump( bool large );
    /**
    Queries if a large or small minidump should be created. Defaults to false.
    **/
    bool isCreateLargeMiniDump() const;
    /**
    Setter/getter functions for CAMP, static variables cannot be set/get directly.
    **/
    inline void setEnabled( bool enabled ) { mEnabled = enabled; }
    inline bool isEnabled() const { return mEnabled; }
    inline void setSilent( bool silent ) { mSilent = silent; }
    inline bool isSilent() const { return mSilent; }
    inline void setAppName( const String& rName ) { mAppName = rName; }
    inline const String& getAppName() const { return mAppName; }
    inline void setAppVersion( const String& rVersion ) { mAppVersion = rVersion; }
    inline const String& getAppVersion() const { return mAppVersion; }

    // Set to false to disable minidump creation.
    static bool mEnabled;
   
    // Set to true to don't ask before creating a miniump.
    static bool mSilent;

    // Path to write to. Empty string means the current directory.
    static String mMiniDumpPath;

    // Used to generate the dump filename.
    static String mAppName;
    static String mAppVersion;

    // How much memory to write.
    static int mMiniDumpType;
};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::WindowsCrashReporter, 
    &Diversia::Shared::Bindings::CampBindings::bindWindowsCrashReporter );

#endif // DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32

#endif // DIVERSIA_SHARED_WINDOWSCRASHDUMP_H