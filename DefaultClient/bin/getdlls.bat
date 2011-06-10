@ECHO OFF

echo Copying DLLs

xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\sigc-vc90-2_0.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_filesystem-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_system-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_log-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_date_time-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_thread-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_regex-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\boost_program_options-vc100-mt-1_45.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\RakNet.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\caelum.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\cAudio.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\OpenAL32.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\wrap_oal.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\libcurl.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\MyGUIEngine.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\Ogre*.dll ReleaseWin32\*.dll /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\RenderSystem*.dll ReleaseWin32\*.dll /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\Plugin*.dll ReleaseWin32\*.dll /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\OIS.dll ReleaseWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\ReleaseWin32\cg.dll ReleaseWin32\ /d /y

xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\sigc-vc90-d-2_0.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_filesystem-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_system-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_log-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_date_time-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_thread-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_regex-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\boost_program_options-vc100-mt-gd-1_45.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\RakNetDebug.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\caelum_d.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\cAudio_d.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\OpenAL32.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\wrap_oal.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\libcurl.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\MyGUIEngine_d.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\Ogre*.dll DebugWin32\*.dll /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\RenderSystem*.dll DebugWin32\*.dll /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\Plugin*.dll DebugWin32\*.dll /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\OIS_d.dll DebugWin32\ /d /y
xcopy ..\..\Build\VC10\dependencies\lib\DebugWin32\cg.dll DebugWin32\ /d /y


pause