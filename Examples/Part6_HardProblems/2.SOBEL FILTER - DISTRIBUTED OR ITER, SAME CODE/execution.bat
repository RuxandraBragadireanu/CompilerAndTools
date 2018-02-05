echo off
set GENPATH=%AGAPIAPATH%\GenerateApp\Release\GenerateApp.exe
%GENPATH% exectype=distributed Def.txt agapia.txt MainInput.txt TGAReader.h TGAReader.cpp images.tga 

