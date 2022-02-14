#!/bin/bash 

fail () { 
 echo Execution aborted. 
 read -n1 -r -p "Press any key to continue..." key 
 exit 1 
}

# Altomare, Tagliafierro, Dominguez, Suzuki, Viccione. Improved relaxation zone method in SPH-based model for coastal engineering applications, Applied Ocean Research, Volume 81, 2018, Pages 15-33, ISSN 0141-1187, https://doi.org/10.1016/j.apor.2018.09.013.

export name=CaseRZ_Coupling
export dirout=${name}_out
export diroutdata=${dirout}/data
export swash_name=Case_SWASH_8L
export outdata=${swash_name}_out
export dirswash=swash_results

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

# "dirout" is created to store results or it is removed if it already exists
if [ -e ${dirout} ]; then rm -r ${dirout}; fi
mkdir ${dirout}
if [ $? -ne 0 ] ; then fail; fi
mkdir ${diroutdata}

# copy .csv file for DualSPHysics in dirout
export dircopy="../../${dirout}"
cd ${outdata}/${dirswash}

if [ ! -e ${outdata}/${dirswash} ]; then echo ***Error: CSV files are missing, run the script wRZ_coupling_SWASH first.; fi

cp Case_SWASH_8L_corr_velx_x00_y00.csv ${dircopy}
cp Case_SWASH_8L_corr_velx_x01_y00.csv ${dircopy}
cp Case_SWASH_8L_corr_velx_x02_y00.csv ${dircopy}
cp Case_SWASH_8L_corr_velx_x03_y00.csv ${dircopy}
cp Case_SWASH_8L_corr_velx_x04_y00.csv ${dircopy}
if [ $? -ne 0 ] ; then fail; fi
cd ..
cd ..

# CODES are executed according the selected parameters of execution in this testcase

# Executes GenCase to create initial files for simulation.
${gencase} ${name}_Def ${dirout}/${name} -save:all
if [ $? -ne 0 ] ; then fail; fi

# Executes DualSPHysics to simulate SPH method.
${dualsphysicsgpu} -gpu ${dirout}/${name} ${dirout} -dirdataout data -svres
if [ $? -ne 0 ] ; then fail; fi

# Executes PartVTK to create VTK files with particles.
export dirout2=${dirout}/particles
${partvtk} -dirin ${diroutdata} -savevtk ${dirout2}/PartFluid -onlytype:-all,fluid -vars:+idp,+vel,+rhop,+press,+vor
if [ $? -ne 0 ] ; then fail; fi

# WG measurements
export dirout2=${dirout}/measures
${measuretool} -dirin ${diroutdata} -points elevation.txt -onlytype:-all,+fluid -elevation:0.4 -savecsv ${dirout2}/_Elevation
if [ $? -ne 0 ] ; then fail; fi

:success

read -n1 -r -p "Press any key to continue..." key
