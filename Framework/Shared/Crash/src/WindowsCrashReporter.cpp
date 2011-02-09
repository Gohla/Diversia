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

#include "Shared/Platform/StableHeaders.h"

#include "Shared/Crash/WindowsCrashReporter.h"

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32

#include <DbgHelp.h>
#include <RakNet/FileOperations.h>
#include <RakNet/SimpleMutex.h>

namespace Diversia
{
//------------------------------------------------------------------------------

bool WindowsCrashReporter::mEnabled = true;
bool WindowsCrashReporter::mSilent = false;
String WindowsCrashReporter::mMiniDumpPath = "";
String WindowsCrashReporter::mAppName = "Diversia";
String WindowsCrashReporter::mAppVersion = "";
int WindowsCrashReporter::mMiniDumpType = 
    MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory;

// More info at:
// http://www.codeproject.com/debug/postmortemdebug_standalone1.asp
// http://www.codeproject.com/debug/XCrashReportPt3.asp
// http://www.codeproject.com/debug/XCrashReportPt1.asp
// http://www.microsoft.com/msj/0898/bugslayer0898.aspx

LONG ProcessException(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    if( !WindowsCrashReporter::mEnabled ) return EXCEPTION_CONTINUE_EXECUTION;

    char appDescriptor[_MAX_PATH];
    if ( !WindowsCrashReporter::mSilent )
    {
        sprintf( appDescriptor, "%s has crashed. Generate a MiniDump?", WindowsCrashReporter::mAppName.c_str() );
        if ( ::MessageBoxA( NULL, appDescriptor, "Crash Reporter", MB_YESNO ) == IDNO )
        {
            return EXCEPTION_CONTINUE_SEARCH;
        }
    }

    char dumpFilepath[_MAX_PATH];
    char dumpFilename[_MAX_PATH];
    sprintf(appDescriptor, "%s %s - %s %s", WindowsCrashReporter::mAppName.c_str(), 
        WindowsCrashReporter::mAppVersion.c_str(), __DATE__, __TIME__);

    strcpy(dumpFilepath, WindowsCrashReporter::mMiniDumpPath.c_str());
    WriteFileWithDirectories(dumpFilepath,0,0);
    AddSlash(dumpFilepath);
    unsigned i, dumpFilenameLen;
    strcpy(dumpFilename, appDescriptor);
    dumpFilenameLen=(unsigned) strlen(appDescriptor);
    for (i=0; i < dumpFilenameLen; i++)
        if (dumpFilename[i]==':' || dumpFilename[i]=='/' || dumpFilename[i]=='\\')
            dumpFilename[i]='.'; // Remove illegal characters from filename
    strcat(dumpFilepath, dumpFilename);
    strcat(dumpFilepath, ".dmp");

    HANDLE hFile = CreateFileA(dumpFilepath,GENERIC_WRITE, FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hFile==INVALID_HANDLE_VALUE)
        return EXCEPTION_CONTINUE_SEARCH;

    MINIDUMP_EXCEPTION_INFORMATION eInfo;
    eInfo.ThreadId = GetCurrentThreadId();
    eInfo.ExceptionPointers = ExceptionInfo;
    eInfo.ClientPointers = FALSE;

    if (MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        (MINIDUMP_TYPE)WindowsCrashReporter::mMiniDumpType,
        ExceptionInfo ? &eInfo : NULL,
        NULL,
        NULL)==false)
        return EXCEPTION_CONTINUE_SEARCH;

    CloseHandle(hFile);

    return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI CrashExceptionFilter( struct _EXCEPTION_POINTERS *ExceptionInfo )
{
    // Mutex here due to http://www.jenkinssoftware.com/raknet/forum/index.php?topic=2305.0;topicseen
    static RakNet::SimpleMutex crashExceptionFilterMutex;
    crashExceptionFilterMutex.Lock();
    LONG retVal = ProcessException(ExceptionInfo); 
    crashExceptionFilterMutex.Unlock();
    return retVal;
}

void DumpMiniDump(PEXCEPTION_POINTERS excpInfo)
{
    if (excpInfo == NULL) 
    {
        // Generate exception to get proper context in dump
        __try 
        {
            RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
        } 
        __except(DumpMiniDump(GetExceptionInformation()),EXCEPTION_EXECUTE_HANDLER) 
        {
        }
    } 
    else
    {
        ProcessException(excpInfo); 
    }
}

WindowsCrashReporter::WindowsCrashReporter()
{
#if !defined( DIVERSIA_DEBUG ) && defined( DIVERSIA_DEBUGINFO )
    SetUnhandledExceptionFilter( CrashExceptionFilter );
#endif
}

WindowsCrashReporter::~WindowsCrashReporter()
{
#if !defined( DIVERSIA_DEBUG ) && defined( DIVERSIA_DEBUGINFO )
    SetUnhandledExceptionFilter( NULL );
#endif
}

void WindowsCrashReporter::setCreateLargeMiniDump( bool large )
{
    if( large )
        mMiniDumpType = MiniDumpWithPrivateReadWriteMemory
                        | MiniDumpWithDataSegs
                        | MiniDumpWithHandleData
                        | MiniDumpWithFullMemoryInfo;
    else
        mMiniDumpType = MiniDumpWithIndirectlyReferencedMemory
                        | MiniDumpScanMemory;
}

bool WindowsCrashReporter::isCreateLargeMiniDump() const
{
    return mMiniDumpType != (MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory);
}

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
