@echo off

mkdir ..\..\build
pushd ..\..\build
cl -FC -Zi ..\HandMadeHero\code\Win32_handmade.cpp /link user32.lib Gdi32.lib dsound.lib xaudio2.lib
popd