@echo off
setlocal EnableDelayedExpansion
rem Don't remove the two jump line after than the next line [set NL=^]
set NL=^


rem "name" and "dirout" are named according to the testcase

set name=CaseSloshingAcc
set dirout=%name%_out
set diroutdata=%dirout%\data

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

:menu
if exist %dirout% ( 
	set /p option="The folder "%dirout%" already exists. Choose an option.!NL!  [1]- Delete it and continue.!NL!  [2]- Execute post-processing.!NL!  [3]- Abort and exit.!NL!"
	if "!option!" == "1" goto run else (
		if "!option!" == "2" goto postprocessing else (
			if "!option!" == "3" goto fail else ( 
				goto menu
			)
		)
	)
)

:run
rem "dirout" to store results is removed if it already exists
if exist %dirout% rd /s /q %dirout%


REM CODES are executed according the selected parameters of execution in this testcase

REM Executes GenCase4 to create initial files for simulation.
%gencase% %name%_Def %dirout%/%name% -save:all
if not "%ERRORLEVEL%" == "0" goto fail

REM Executes DualSPHysics to simulate SPH method.
%dualsphysicscpu% %dirout%/%name% %dirout% -dirdataout data -svres
if not "%ERRORLEVEL%" == "0" goto fail

:postprocessing
REM Executes PartVTK4 to create VTK files with particles.
set dirout2=%dirout%\particles
%partvtk% -dirin %diroutdata% -savevtk %dirout2%/PartAll
if not "%ERRORLEVEL%" == "0" goto fail

REM Executes PartVTKOut4 to create VTK files with excluded particles.
%partvtkout% -dirin %diroutdata% -savevtk %dirout2%/PartFluidOut -SaveResume %dirout2%/_ResumeFluidOut
if not "%ERRORLEVEL%" == "0" goto fail

REM Measures pressure value at point close to the boundary and 1.5h away.
set dirout2=%dirout%\measuretool
REM Executes MeasureTool4 to create a CSV file with pressure information at point close to the boundary and 1.5h away.
%measuretool% -dirin %diroutdata% -points CaseSloshingAcc_PointsPressure_Incorrect.txt -onlytype:-all,+fluid -vars:-all,+press,+kcorr -kcusedummy:0 -kclimit:0.5 -savevtk %dirout2%/PointsPressure_Incorrect -savecsv %dirout2%/_PointsPressure_Incorrect
if not "%ERRORLEVEL%" == "0" goto fail
%measuretool% -dirin %diroutdata% -points CaseSloshingAcc_PointsPressure_Correct.txt   -onlytype:-all,+fluid -vars:-all,+press,+kcorr -kcusedummy:0 -kclimit:0.5 -savevtk %dirout2%/PointsPressure_Correct   -savecsv %dirout2%/_PointsPressure_Correct
if not "%ERRORLEVEL%" == "0" goto fail

REM Computes forces and moment about an axis
set dirout2=%dirout%\forces
%computeforces% -dirin %diroutdata% -savecsv %dirout2%/_TankM0 -onlymk:10 -momentaxis:0:0.1:0.0:0:-0.1:0.0
if not "%ERRORLEVEL%" == "0" goto fail


:success
echo All done
goto end

:fail
echo Execution aborted.

:end
pause
