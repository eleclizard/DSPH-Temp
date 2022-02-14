@echo off
setlocal EnableDelayedExpansion
rem Don't remove the two jump line after than the next line [set NL=^]
set NL=^


rem "name" and "dirout" are named according to the testcase

set name=CaseDEMInteraction_LiquidGas
set dirout=%name%_out

rem "executables" are renamed and called from their directory

set dirbin=../../../bin/windows
set gencase="%dirbin%/GenCase_win64.exe"
set dualsphysicscpu="%dirbin%/DualSPHysics4.0_LiquidGasCPU_win64.exe"
set dualsphysicsgpu="%dirbin%/DualSPHysics4.0_LiquidGas_win64.exe"
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
rem "dirout" is created to store results or it is removed if it already exists

if exist %dirout% rd /s /q %dirout%
mkdir %dirout%
if not "%ERRORLEVEL%" == "0" goto fail

rem CODES are executed according the selected parameters of execution in this testcase

%gencase% %name%_Def %dirout%/%name% -save:all
if not "%ERRORLEVEL%" == "0" goto fail

%dualsphysicscpu% %dirout%/%name% %dirout% -svres
if not "%ERRORLEVEL%" == "0" goto fail

:postprocessing
set dirout2=%dirout%\particles
mkdir %dirout2%
%partvtk% -dirin %dirout% -savevtk %dirout2%/PartFluid -onlytype:-all,+fluid -vars:+press,+mk
if not "%ERRORLEVEL%" == "0" goto fail

%partvtk% -dirin %dirout% -savevtk %dirout2%/PartBlocks -onlytype:-all,+floating 
if not "%ERRORLEVEL%" == "0" goto fail

%partvtkout% -dirin %dirout% -savevtk %dirout2%/PartFluidOut -SaveResume %dirout2%/_ResumeFluidOut
if not "%ERRORLEVEL%" == "0" goto fail

set dirout2=%dirout%\boundary
mkdir %dirout2%
%boundaryvtk% -loadvtk %dirout%/%name%__Actual.vtk -motiondata %dirout% -savevtkdata %dirout2%/Box.vtk -onlymk:10
if not "%ERRORLEVEL%" == "0" goto fail

%boundaryvtk% -loadvtk %dirout%/%name%__Actual.vtk -motiondata %dirout% -savevtkdata %dirout2%/MotionBlocks -onlymk:61-78
if not "%ERRORLEVEL%" == "0" goto fail

set dirout2=%dirout%\surface
mkdir %dirout2%
%isosurface% -dirin %dirout% -saveiso %dirout2%/Surface -onlymk:1
if not "%ERRORLEVEL%" == "0" goto fail


:success
echo All done
goto end

:fail
echo Execution aborted.

:end
pause

