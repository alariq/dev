echo off
call "%VS90COMNTOOLS%vsvars32.bat" x86

echo +
echo + Cleaning sdl_example [Debug]
echo +
devenv samples/sdl_example/sdl_example.sln /clean "Debug|Win32"
echo +
echo + Cleaned sdl_example [Debug]
echo +

echo +
echo + Cleaning sdl_example [Release]
echo +
devenv samples/sdl_example/sdl_example.sln /clean "Release|Win32"
echo +
echo + Cleaned sdl_example [Release]
echo +


echo +
echo + Cleaning dx9_example [Debug]
echo +
devenv samples/dx9_example/dx9_example.sln /clean "Debug|Win32"
echo +
echo + Cleaned dx9_example [Debug]
echo +

echo +
echo + Cleaning dx9_example [Release]
echo +
devenv samples/dx9_example/dx9_example.sln /clean "Release|Win32"
echo +
echo + Cleaned dx9_example [Release]
echo +


echo +
echo + Cleaning console_cmdline_example [Debug]
echo +
devenv samples/console_cmdline_example/console_cmdline_example.sln /clean "Debug|Win32"
echo +
echo + Cleaned console_cmdline_example [Debug]
echo +

echo +
echo + Cleaning console_cmdline_example [Release]
echo +
devenv samples/console_cmdline_example/console_cmdline_example.sln /clean "Release|Win32"
echo +
echo + Cleaned console_cmdline_example [Release]
echo +

