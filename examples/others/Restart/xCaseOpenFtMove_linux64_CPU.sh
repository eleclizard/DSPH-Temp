#!/bin/bash 

fail () { 
 echo Execution aborted. 
 read -n1 -r -p "Press any key to continue..." key 
 exit 1 
}

# "name" and "dirout" are named according to the testcase

export name=CaseOpenFtMove
export dirout=${name}_out
export diroutdata=${dirout}/data

# "executables" are renamed and called from their directory

export dirbin=../../../bin/linux
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${dirbin}
export gencase="${dirbin}/GenCase_linux64"
export dualsphysicscpu="${dirbin}/DualSPHysics5.0CPU_linux64"
export dualsphysicsgpu="${dirbin}/DualSPHysics5.0_linux64"
export boundaryvtk="${dirbin}/BoundaryVTK_linux64"
export partvtk="${dirbin}/PartVTK_linux64"
export partvtkout="${dirbin}/PartVTKOut_linux64"
export measuretool="${dirbin}/MeasureTool_linux64"
export computeforces="${dirbin}/ComputeForces_linux64"
export isosurface="${dirbin}/IsoSurface_linux64"
export flowtool="${dirbin}/FlowTool_linux64"
export floatinginfo="${dirbin}/FloatingInfo_linux64"

# RUN FIRST PART OF SIMULATION (0-1 seconds)

# "dirout" is created to store results or it is removed if it already exists

if [ -e ${dirout} ]; then rm -r ${dirout}; fi
if [ $? -ne 0 ] ; then fail; fi

# CODES are executed according the selected parameters of execution in this testcase

# Executes GenCase to create initial files for simulation.
${gencase} ${name}_Def ${dirout}/${name} -save:all
if [ $? -ne 0 ] ; then fail; fi

# Executes DualSPHysics to simulate first second.
${dualsphysicscpu} ${dirout}/${name} ${dirout} -dirdataout data -svres -tmax:1
if [ $? -ne 0 ] ; then fail; fi

# Executes post-processing tools...
export dirout2=${dirout}/particles
${partvtk} -dirin ${diroutdata} -savevtk ${dirout2}/PartFluid -onlytype:-all,+fluid
if [ $? -ne 0 ] ; then fail; fi

${partvtkout} -dirin ${diroutdata} -savevtk ${dirout2}/PartFluidOut -SaveResume ${dirout2}/_ResumeFluidOut
if [ $? -ne 0 ] ; then fail; fi

export dirout2=${dirout}/boundary
${boundaryvtk} -loadvtk ${dirout}/${name}__Actual.vtk -motiondata ${diroutdata} -savevtkdata ${dirout2}/Floatings.vtk
if [ $? -ne 0 ] ; then fail; fi

export dirout2=${dirout}/floatinginfo
${floatinginfo} -dirin ${diroutdata} -savedata ${dirout2}/FloatingMotion 
if [ $? -ne 0 ] ; then fail; fi

export dirout2=${dirout}/elevation
${measuretool} -dirin ${diroutdata} -points ${name}_PointsElevation.txt -onlytype:-all,+fluid -elevation -savevtk ${dirout2}/PointsElevation -savecsv ${dirout2}/_Elevation
if [ $? -ne 0 ] ; then fail; fi

# RESTART SIMULATION AND RUN LAST PART OF SIMULATION (1-4 seconds)

export olddiroutdata=${diroutdata}
export dirout=${name}_restart_out
export diroutdata=${dirout}/data

# "redirout" is created to store results of restart simulation

if [ -e ${dirout} ]; then rm -r ${dirout}; fi
if [ $? -ne 0 ] ; then fail; fi

# CODES are executed according the selected parameters of execution in this testcase

# Executes GenCase to create initial files for simulation.
${gencase} ${name}_Def ${dirout}/${name} -save:all
if [ $? -ne 0 ] ; then fail; fi

# Executes DualSPHysics to simulate the last 3 seconds.
${dualsphysicscpu} ${dirout}/${name} ${dirout} -dirdataout data -svres -partbegin:100 ${olddiroutdata}
if [ $? -ne 0 ] ; then fail; fi

# Executes post-processing tools for restart simulation...
export dirout2=${dirout}/particles
${partvtk} -dirin ${diroutdata} -savevtk ${dirout2}/PartFluid -onlytype:-all,+fluid
if [ $? -ne 0 ] ; then fail; fi

${partvtkout} -dirin ${diroutdata} -savevtk ${dirout2}/PartFluidOut -SaveResume ${dirout2}/ResumeFluidOut
if [ $? -ne 0 ] ; then fail; fi

export dirout2=${dirout}/boundary
${boundaryvtk} -loadvtk ${dirout}/${name}__Actual.vtk -motiondata0 ${olddiroutdata} -motiondata ${diroutdata} -savevtkdata ${dirout2}/Floatings.vtk
if [ $? -ne 0 ] ; then fail; fi

export dirout2=${dirout}/fluidslices
${isosurface} -dirin ${diroutdata} -saveslice ${dirout2}/Slices
if [ $? -ne 0 ] ; then fail; fi

export dirout2=${dirout}/floatinginfo
${floatinginfo} -dirin ${diroutdata}  -savedata ${dirout2}/FloatingMotion 
if [ $? -ne 0 ] ; then fail; fi

export dirout2=${dirout}/elevation
${measuretool} -dirin ${diroutdata} -points ${name}_PointsElevation.txt -onlytype:-all,+fluid -elevation -savevtk ${dirout2}/PointsElevation -savecsv ${dirout2}/_Elevation
if [ $? -ne 0 ] ; then fail; fi

:success

read -n1 -r -p "Press any key to continue..." key
