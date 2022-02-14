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
set diroutdata=%dirout%\data
rem name of file created and corrected using wMLpistons_SWASH.bat
set swash_name=CIEM_SWASH
set nameswcor=CIEM_SWASH_corr_velx_x09_y00.csv
set outdata=%swash_name%_out
set dirswash=swash_results

rem "executables" are renamed and called from their directory

set dirbin=../../../bin/windows
set gencase="%dirbin%/GenCase_win64.exe"
set dualsphysicscpu="%dirbin%/DualSPHysics5.0CPU_win64.exe"
set dualsphysicsgpu="%dirbin%/DualSPHysics5.0_win64.exe"
set boundaryvtk="%dirbin%/BoundaryVTK_win64.exe"
set partvtk="%dirbin%/PartVTK_win64.exe"
set partvtkout="%dirbin%/PartVTKOut_win64.exe"
set measuretool="%dirbin%/MeasureTool_win64.exe"
set computeforces="%dirbin%/ComputeForces_win64.exe"
set isosurface="%dirbin%/IsoSurface_win64.exe"
set flowtool="%dirbin%/FlowTool_win64.exe"
set floatinginfo="%dirbin%/FloatingInfo_win64.exe"

rem "dirout" is created to store results or it is removed if it already exists

if exist %dirout% rd /s /q %dirout%
mkdir %dirout%
if not "%ERRORLEVEL%" == "0" goto fail
mkdir %diroutdata%

rem copy .csv file for DualSPHysics in dirout

set dircopy="../../%dirout%"
cd %outdata%/%dirswash%
if not exist %nameswcor% echo ***Error: CSV file is missing %nameswcor%, run the script wMLpistons_SWASH first.
copy %nameswcor% %dircopy%
if not "%ERRORLEVEL%" == "0" goto fail
cd ..
cd ..

rem CODES are executed according the selected parameters of execution in this testcase

rem Executes GenCase to create initial files for simulation.
%gencase% %name%_Def %dirout%/%name% -save:all
if not "%ERRORLEVEL%" == "0" goto fail

rem Executes DualSPHysics to simulate SPH method.
%dualsphysicsgpu% -gpu %dirout%/%name% %dirout% -dirdataout data -svres
if not "%ERRORLEVEL%" == "0" goto fail

rem Executes PartVTK to create VTK files with particles.
set dirout2=%dirout%\particles
%partvtk% -dirin %diroutdata% -savevtk %dirout2%/PartFluid -onlytype:-all,fluid -vars:+idp,+vel,+rhop,+press,+vor
if not "%ERRORLEVEL%" == "0" goto fail

%partvtk% -dirin %diroutdata% -savevtk %dirout2%/PartMov -onlytype:-all,moving
if not "%ERRORLEVEL%" == "0" goto fail

rem WG measurements
set dirout2=%dirout%\measures
%measuretool% -dirin %diroutdata% -points AWG5.txt -onlytype:-all,+fluid -elevation:0.4  -savecsv %dirout2%/AWG5
if not "%ERRORLEVEL%" == "0" goto fail


:success
echo All done
goto end

:fail
echo Execution aborted.

:end
pause

