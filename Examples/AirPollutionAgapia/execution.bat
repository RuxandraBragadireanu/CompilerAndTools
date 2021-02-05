echo off
set GENPATH=%AGAPIAPATH%\GenerateApp\Release\GenerateApp.exe
%GENPATH% exectype=iterative Def.txt agapia.txt MainInput.txt AirQualityIndex.h CsvTools.h MeasurementInfo.h AirQualityIndex.cpp MeasurementInfo.cpp CsvTools.cpp

