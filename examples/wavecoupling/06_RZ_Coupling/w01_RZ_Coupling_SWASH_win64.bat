@echo off
REM Altomare, Tagliafierro, Dominguez, Suzuki, Viccione. Improved relaxation zone method in SPH-based model for coastal engineering applications, Applied Ocean Research, Volume 81, 2018, Pages 15-33, ISSN 0141-1187, https://doi.org/10.1016/j.apor.2018.09.013.

set swash_name=Case_SWASH_8L
set output=%swash_name%_out
rem value that identify the SWASH file to correct
set xvalue=x10_y00.csv 
set filesws=%swash_name%.sws
set dirswash=swash_results

rem "SWASH executables" and gencase are renamed and called from their directory
set dirbin=../../../bin/windows
set readswash="%dirbin%/ReadSwash_win64.exe"
set Swash="../%dirbin%/swash_win64.exe"

rem runs SWASH
if exist %output% rd /s /q %output%
mkdir %output%
copy %swash_name%\*.* %output%
copy %swash_name%\%swash_name%.sws %output%\INPUT
cd %output%
%Swash%
if not "%ERRORLEVEL%" == "0" goto fail

if not exist %dirswash% mkdir %dirswash%
cd ..

rem Defines the file to convert and correct and the rate of correction. Creates a converted .csv file for DualSPHysics
%readswash% -savecsv %output%/%filesws% %output%/%dirswash%/%swash_name%
if not "%ERRORLEVEL%" == "0" goto fail

set filein=%output%/%dirswash%/%swash_name%
set fileout=%output%\%dirswash%\%swash_name%_corr

%readswash% -correctcsv:50:0 %filein%_velx_x00_y00.csv %fileout%_velx_x00_y00.csv -tclip:0:50
%readswash% -correctcsv:50:0 %filein%_velz_x00_y00.csv %fileout%_velz_x00_y00.csv -tclip:0.0000:50
%readswash% -correctcsv:50:0 %filein%_velx_x01_y00.csv %fileout%_velx_x01_y00.csv -tclip:0.0000:50
%readswash% -correctcsv:50:0 %filein%_velz_x01_y00.csv %fileout%_velz_x01_y00.csv -tclip:0.0000:50
%readswash% -correctcsv:50:0 %filein%_velx_x02_y00.csv %fileout%_velx_x02_y00.csv -tclip:0.0000:50
%readswash% -correctcsv:50:0 %filein%_velz_x02_y00.csv %fileout%_velz_x02_y00.csv -tclip:0.0000:50
%readswash% -correctcsv:50:0 %filein%_velx_x03_y00.csv %fileout%_velx_x03_y00.csv -tclip:0.0000:50
%readswash% -correctcsv:50:0 %filein%_velz_x03_y00.csv %fileout%_velz_x03_y00.csv -tclip:0.0000:50
%readswash% -correctcsv:50:0 %filein%_velx_x04_y00.csv %fileout%_velx_x04_y00.csv -tclip:0.0000:50
%readswash% -correctcsv:50:0 %filein%_velz_x04_y00.csv %fileout%_velz_x04_y00.csv -tclip:0.0000:50
if not "%ERRORLEVEL%" == "0" goto fail

:success
echo All done
goto end

:fail
echo Execution aborted.

:end
pause
