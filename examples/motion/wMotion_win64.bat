@echo off
set dirbin=../../bin/windows
set gencase="%dirbin%/GenCase_win64.exe"
set boundaryvtk="%dirbin%/BoundaryVTK_win64.exe"

set name=Motion01
rd /s /q %name%
%gencase% %name% %name%/%name%
if not "%ERRORLEVEL%" == "0" goto fail
%boundaryvtk% -loadvtk AutoActual -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk 
if not "%ERRORLEVEL%" == "0" goto fail

set name=Motion02
rd /s /q %name%
%gencase% %name% %name%/%name%
if not "%ERRORLEVEL%" == "0" goto fail
%boundaryvtk% -loadvtk AutoActual -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk 
if not "%ERRORLEVEL%" == "0" goto fail

set name=Motion03
rd /s /q %name%
%gencase% %name% %name%/%name%
if not "%ERRORLEVEL%" == "0" goto fail
%boundaryvtk% -loadvtk AutoActual -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk 
if not "%ERRORLEVEL%" == "0" goto fail

set name=Motion04
rd /s /q %name%
%gencase% %name% %name%/%name%
if not "%ERRORLEVEL%" == "0" goto fail
%boundaryvtk% -loadvtk AutoActual -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk 
if not "%ERRORLEVEL%" == "0" goto fail

set name=Motion05
rd /s /q %name%
%gencase% %name% %name%/%name%
if not "%ERRORLEVEL%" == "0" goto fail
%boundaryvtk% -loadvtk AutoActual -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk 
if not "%ERRORLEVEL%" == "0" goto fail

set name=Motion06
rd /s /q %name%
%gencase% %name% %name%/%name%
if not "%ERRORLEVEL%" == "0" goto fail
%boundaryvtk% -loadvtk AutoActual -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk 
if not "%ERRORLEVEL%" == "0" goto fail

set name=Motion07
rd /s /q %name%
%gencase% %name% %name%/%name%
if not "%ERRORLEVEL%" == "0" goto fail
rem %boundaryvtk% -loadvtk %name%/%name%__Actual.vtk -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk
%boundaryvtk% -loadvtk AutoActual -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk 
if not "%ERRORLEVEL%" == "0" goto fail

set name=Motion08
rd /s /q %name%
%gencase% %name% %name%/%name%
if not "%ERRORLEVEL%" == "0" goto fail
rem %boundaryvtk% -loadvtk %name%/%name%__Actual.vtk -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk
%boundaryvtk% -loadvtk AutoActual -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk 
if not "%ERRORLEVEL%" == "0" goto fail

set name=Motion09
rd /s /q %name%
%gencase% %name% %name%/%name%
if not "%ERRORLEVEL%" == "0" goto fail
rem %boundaryvtk% -loadvtk %name%/%name%__Actual.vtk -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk
%boundaryvtk% -loadvtk AutoActual -filexml %name%/%name%.xml -savevtkdata %name%/%name%.vtk 
if not "%ERRORLEVEL%" == "0" goto fail


:success
echo All done
goto end

:fail
echo Execution aborted.

:end
pause

