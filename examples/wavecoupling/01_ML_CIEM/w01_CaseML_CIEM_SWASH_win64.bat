@echo off
REM **** Experimental data provided by Dr. Ivan Caceres, UPC, Barcelona
REM **** for experiments, refer to:
REM Scandura & Foti, 2011. Measurements of wave-induced steady currents outside the surfzone. J. Hydraulic Res. 49, 64–71.
REM **** for MLpistons (coupling technique) refer to
REM Altomare, Domínguez, Crespo, Suzuki, Caceres, Gómez-Gesteira, 2015. Hybridization of the Wave Propagation Model SWASH and the Meshfree Particle Method SPH for Real Coastal Applications, Coast. Eng. J. 57 (2015) 1550024. doi:doi:10.1142/S0578563415500242.

rem Runs SWASH simulation, "output"  and output directories are named according to the SWASH (v.5.01)
rem Defines "name" refers to the .xml for DualSPHysics

set name=CaseML_CIEM
set dirout=%name%_out
set swash_name=CIEM_SWASH
set output=%swash_name%_out
rem value that identify the SWASH file to correct
set xvalue=x09_y00.csv 
set filesws=%swash_name%.sws
set dirswash=swash_results
set diroutparts=%output%\MovingParts_out
set diroutvel=%output%\Velocity_out

rem "SWASH executables" and gencase are renamed and called from their directory

set dirbin=../../../bin/windows
set readswash="%dirbin%/ReadSwash_win64.exe"
set Swash="../%dirbin%/swash_win64.exe"
set gencase="%dirbin%/GenCase_win64.exe"

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
set filein=%output%/%dirswash%/%swash_name%
set fileout=%output%\%dirswash%\%swash_name%_corr
%readswash% -correctcsv:400:1 %filein%_velx_%xvalue% %fileout%_velx_%xvalue% -tclip:0

rem Creates VTK to show the piston velocity.
if exist %diroutvel% rd /s /q %diroutvel%
%readswash% -savevtkveldp:0.04 %diroutvel%/velocity -file_velx %fileout%_velx_%xvalue% -tmin:0 -tmax:80 -dt:0.5 -smooth:10
if not "%ERRORLEVEL%" == "0" goto fail

rem Creates VTK with simulation of piston motion.
if exist %diroutparts% rd /s /q %diroutparts%
%gencase% %name%_Def %diroutparts%/%name% -save:vtkall
if not "%ERRORLEVEL%" == "0" goto fail

copy %fileout%_velx_%xvalue% %diroutparts%
if not "%ERRORLEVEL%" == "0" goto fail

%readswash% -savevtk_case %diroutparts%/piston -loadcase %diroutparts%/%name% -tmin:0 -tmax:80 -dt:0.5
if not "%ERRORLEVEL%" == "0" goto fail

:success
echo All done
goto end

:fail
echo Execution aborted.

:end
pause
