@ECHO OFF

echo Copying DLLs

xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\sigc-vc90-2_0.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_filesystem-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_system-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_log-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_date_time-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_thread-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_regex-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\RakNet.dll ReleaseWin32\ /d /y

xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\sigc-vc90-d-2_0.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_filesystem-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_system-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_log-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_date_time-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_thread-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_regex-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\RakNetDebug.dll DebugWin32\ /d /y

pause
