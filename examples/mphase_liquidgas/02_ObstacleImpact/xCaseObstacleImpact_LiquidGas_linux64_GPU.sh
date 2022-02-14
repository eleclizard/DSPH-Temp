#!/bin/bash 

fail () { 
 echo Execution aborted. 
 read -n1 -r -p "Press any key to continue..." key 
 exit 1 
}

# "name" and "dirout" are named according to the testcase

export name=CaseObstacleImpact_LiquidGas
export dirout=${name}_out

# "executables" are renamed and called from their directory

export dirbin=../../../bin/linux
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${dirbin}
export gencase="${dirbin}/GenCase_linux64"
export dualsphysicscpu="${dirbin}/DualSPHysics4.0_LiquidGasCPU_linux64"
export dualsphysicsgpu="${dirbin}/DualSPHysics4.0_LiquidGas_linux64"
export boundaryvtk="${dirbin}/BoundaryVTK_linux64"
export partvtk="${dirbin}/PartVTK_linux64"
export partvtkout="${dirbin}/PartVTKOut_linux64"
export measuretool="${dirbin}/MeasureTool_linux64"
export computeforces="${dirbin}/ComputeForces_linux64"
export isosurface="${dirbin}/IsoSurface_linux64"
export flowtool="${dirbin}/FlowTool_linux64"
export floatinginfo="${dirbin}/FloatingInfo_linux64"

option=-1
 if [ -e $dirout ]; then
 while [ "$option" != 1 -a "$option" != 2 -a "$option" != 3 ] 
 do 

	echo -e "The folder "${dirout}" already exists. Choose an option.
  [1]- Delete it and continue.
  [2]- Execute post-processing.
  [3]- Abort and exit.
"
 read -n 1 option 
 done 
  else 
   option=1 
fi 

if [ $option -eq 1 ]; then
# "dirout" is created to store results or it is removed if it already exists

if [ -e ${dirout} ]; then rm -r ${dirout}; fi
mkdir ${dirout}
if [ $? -ne 0 ] ; then fail; fi

# CODES are executed according the selected parameters of execution in this testcase

${gencase} ${name}_Def ${dirout}/${name} -save:all
if [ $? -ne 0 ] ; then fail; fi

${dualsphysicsgpu} -gpu ${dirout}/${name} ${dirout} -svres
if [ $? -ne 0 ] ; then fail; fi

fi

if [ $option -eq 2 -o $option -eq 1 ]; then
export dirout2=${dirout}/particles
mkdir ${dirout2}
${partvtk} -dirin ${dirout} -savevtk ${dirout2}/PartFluid -onlytype:-all,+fluid -vars:+press,+mk
if [ $? -ne 0 ] ; then fail; fi

${partvtkout} -dirin ${dirout} -savevtk ${dirout2}/PartFluidOut -SaveResume ${dirout2}/_ResumeFluidOut
if [ $? -ne 0 ] ; then fail; fi

export dirout2=${dirout}/elevation
mkdir ${dirout2}
${measuretool} -dirin ${dirout} -points ${name}_PointsElevation1.txt -onlytype:-all,+fluid -elevation -savevtk ${dirout2}/PointsElevation_1 -savecsv ${dirout2}/_PointsElevation_1 -onlymk:1
if [ $? -ne 0 ] ; then fail; fi

${measuretool} -dirin ${dirout} -points ${name}_PointsElevation2.txt -onlytype:-all,+fluid -elevation -savevtk ${dirout2}/PointsElevation_2 -savecsv ${dirout2}/_PointsElevation_2 -onlymk:1
if [ $? -ne 0 ] ; then fail; fi

${measuretool} -dirin ${dirout} -points ${name}_PointsElevation3.txt -onlytype:-all,+fluid -elevation -savevtk ${dirout2}/PointsElevation_3 -savecsv ${dirout2}/_PointsElevation_3 -onlymk:1
if [ $? -ne 0 ] ; then fail; fi

${measuretool} -dirin ${dirout} -points ${name}_PointsElevation4.txt -onlytype:-all,+fluid -elevation -savevtk ${dirout2}/PointsElevation_4 -savecsv ${dirout2}/_PointsElevation_4 -onlymk:1
if [ $? -ne 0 ] ; then fail; fi

fi
if [ $option != 3 ];then
 echo All done
 else
 echo Execution aborted
fi

read -n1 -r -p "Press any key to continue..." key
