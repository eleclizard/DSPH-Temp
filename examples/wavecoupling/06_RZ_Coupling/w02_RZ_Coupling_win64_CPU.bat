@echo off
REM Altomare, Tagliafierro, Dominguez, Suzuki, Viccione. Improved relaxation zone method in SPH-based model for coastal engineering applications, Applied Ocean Research, Volume 81, 2018, Pages 15-33, ISSN 0141-1187, https://doi.org/10.1016/j.apor.2018.09.013.

set name=CaseRZ_Coupling
set dirout=%name%_out
set diroutdata=%dirout%\data
set swash_name=Case_SWASH_8L
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
set dircopy="..\..\%dirout%"
cd %outdata%/%dirswash%

if not exist %outdata%/%dirswash% echo ***Error: CSV files are missing, run the script wRZ_coupling_SWASH first.

copy Case_SWASH_8L_corr_velx_x00_y00.csv %dircopy%
copy Case_SWASH_8L_corr_velx_x01_y00.csv %dircopy%
copy Case_SWASH_8L_corr_velx_x02_y00.csv %dircopy%
copy Case_SWASH_8L_corr_velx_x03_y00.csv %dircopy%
copy Case_SWASH_8L_corr_velx_x04_y00.csv %dircopy%
if not "%ERRORLEVEL%" == "0" goto fail
cd ..
cd ..

rem CODES are executed according the selected parameters of execution in this testcase

rem Executes GenCase to create initial files for simulation.
%gencase% %name%_Def %dirout%/%name% -save:all
if not "%ERRORLEVEL%" == "0" goto fail

rem Executes DualSPHysics to simulate SPH method.
%dualsphysicscpu% %dirout%/%name% %dirout% -dirdataout data -svres
if not "%ERRORLEVEL%" == "0" goto fail

rem Executes PartVTK to create VTK files with particles.
set dirout2=%dirout%\particles
%partvtk% -dirin %diroutdata% -savevtk %dirout2%/PartFluid -onlytype:-all,fluid -vars:+idp,+vel,+rhop,+press,+vor
if not "%ERRORLEVEL%" == "0" goto fail

rem WG measurements
set dirout2=%dirout%\measures
%measuretool% -dirin %diroutdata% -points elevation.txt -onlytype:-all,+fluid -elevation:0.4 -savecsv %dirout2%/_Elevation
if not "%ERRORLEVEL%" == "0" goto fail

:success
echo All done
goto end

:fail
echo Execution aborted.

:end
pause

