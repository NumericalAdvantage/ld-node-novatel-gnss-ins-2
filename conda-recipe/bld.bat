REM This file is part of project link.developers/ld-node-novatel-gnss-ins-2.
REM It is copyrighted by the contributors recorded in the version control history of the file,
REM available from its original location https://gitlab.com/link.developers/ld-node-novatel-gnss-ins-2.
REM
REM SPDX-License-Identifier: MPL-2.0

mkdir build
cd build

cmake .. -G "%CMAKE_GENERATOR%" ^
	-DCMAKE_INSTALL_PREFIX="%LIBRARY_PREFIX%" ^
	-DCMAKE_PREFIX_PATH="%LIBRARY_PREFIX%"
if errorlevel 1 exit 1

cmake --build . --target INSTALL --config Release
if errorlevel 1 exit 1
