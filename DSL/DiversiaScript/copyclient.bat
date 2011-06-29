@ECHO OFF

echo Copying client

echo \ShaderCache\ > exclude.txt

xcopy ..\..\DefaultClient\media\*.* client\media\ /e /d /y /EXCLUDE:exclude.txt
xcopy ..\..\DefaultClient\bin\ReleaseWin32\Diversia-DefaultClient-DbgInfo.exe client\ /d /y
xcopy ..\..\DefaultClient\bin\ReleaseWin32\*.dll client\ /d /y

del exclude.txt