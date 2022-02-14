#!/bin/bash 

fail () { 
 echo Execution aborted. 
 read -n1 -r -p "Press any key to continue..." key 
 exit 1 
}

# "name" and "dirout" are named according to the testcase

export name=CaseSloshingMotion
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
# "dirout" to store results is removed if it already exists
if [ -e ${dirout} ]; then rm -r ${dirout}; fi

# CODES are executed according the selected parameters of execution in this testcase

# Executes GenCase4 to create initial files for simulation.
${gencase} ${name}_Def ${dirout}/${name} -save:all
if [ $? -ne 0 ] ; then fail; fi

# Executes DualSPHysics to simulate SPH method.
${dualsphysicsgpu} -gpu ${dirout}/${name} ${dirout} -dirdataout data -svres
if [ $? -ne 0 ] ; then fail; fi

fi

if [ $option -eq 2 -o $option -eq 1 ]; then
# Executes PartVTK4 to create VTK files with particles.
export dirout2=${dirout}/particles
${partvtk} -dirin ${diroutdata} -savevtk ${dirout2}/PartAll
if [ $? -ne 0 ] ; then fail; fi

# Executes PartVTKOut4 to create VTK files with excluded particles.
${partvtkout} -dirin ${diroutdata} -savevtk ${dirout2}/PartFluidOut -SaveResume ${dirout2}/_ResumeFluidOut
if [ $? -ne 0 ] ; then fail; fi

# Measures pressure value at point close to the boundary and 1.5h away.
export dirout2=${dirout}/measuretool
# Executes BoundaryVTK4 to create a CSV file with positions where pressure is measured using MeasureTool4.
${boundaryvtk} -motiondata ${diroutdata} -saveposmotion:10:-0.4461:0:0.093 ${dirout2}/PosCorrect -saveposmotion:10:-0.45:0:0.093 ${dirout2}/PosIncorrect
if [ $? -ne 0 ] ; then fail; fi
# Executes MeasureTool4 to create a CSV file with pressure information at point close to the boundary and 1.5h away.
${measuretool} -dirin ${diroutdata} -pointspos ${dirout2}/PosCorrect_mk0010.csv   -onlytype:-all,+fluid -vars:-all,+press,+kcorr -kcusedummy:0 -kclimit:0.5 -savevtk ${dirout2}/PointsPressure_Correct   -savecsv ${dirout2}/_PointsPressure_Correct
if [ $? -ne 0 ] ; then fail; fi
${measuretool} -dirin ${diroutdata} -pointspos ${dirout2}/PosIncorrect_mk0010.csv -onlytype:-all,+fluid -vars:-all,+press,+kcorr -kcusedummy:0 -kclimit:0.5 -savevtk ${dirout2}/PointsPressure_Incorrect -savecsv ${dirout2}/_PointsPressure_Incorrect
if [ $? -ne 0 ] ; then fail; fi

# Computes forces and moment about an axis
export dirout2=${dirout}/forces
${computeforces} -dirin ${diroutdata} -savecsv ${dirout2}/_TankM0 -onlymk:10 -momentaxis:0:0.1:0.0:0:-0.1:0.0
if [ $? -ne 0 ] ; then fail; fi

fi
if [ $option != 3 ];then
 echo All done
 else
 echo Execution aborted
fi

read -n1 -r -p "Press any key to continue..." key
