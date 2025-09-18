@echo off
IF exist build ( echo build exists ) ELSE ( mkdir build && echo build created)

cls
cd build
cmake ..
:: cmake --build .