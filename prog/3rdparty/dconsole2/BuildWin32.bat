echo off
call "%VS90COMNTOOLS%vsvarsall.bat"

echo +
echo + Building sdl_example [Debug]
echo +
devenv samples/sdl_example/sdl_example.sln /build "Debug|Win32"
echo +
echo + Built sdl_example [Debug]
echo +

echo +
echo + Building sdl_example [Release]
echo +
devenv samples/sdl_example/sdl_example.sln /build "Release|Win32"
echo +
echo + Built sdl_example [Release]
echo +


echo +
echo + Building dx9_example [Debug]
echo +
devenv samples/dx9_example/dx9_example.sln /build "Debug|Win32"
echo +
echo + Built dx9_example [Debug]
echo +

echo +
echo + Building dx9_example [Release]
echo +
devenv samples/dx9_example/dx9_example.sln /build "Release|Win32"
echo +
echo + Built dx9_example [Release]
echo +


echo +
echo + Building console_cmdline_example [Debug]
echo +
devenv samples/console_cmdline_example/console_cmdline_example.sln /build "Debug|Win32"
echo +
echo + Built console_cmdline_example [Debug]
echo +

echo +
echo + Building console_cmdline_example [Release]
echo +
devenv samples/console_cmdline_example/console_cmdline_example.sln /build "Release|Win32"
echo +
echo + Built console_cmdline_example [Release]
echo +

