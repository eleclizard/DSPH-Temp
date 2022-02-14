#!/bin/bash 

fail () { 
 echo Execution aborted. 
 read -n1 -r -p "Press any key to continue..." key 
 exit 1 
}

# **** Experimental data provided by Dr. Ivan Caceres, UPC, Barcelona
# **** for experiments, refer to:
# Scandura & Foti, 2011. Measurements of wave-induced steady currents outside the surfzone. J. Hydraulic Res. 49, 64–71.
# **** for MLpistons (coupling technique) refer to
# Altomare, Domínguez, Crespo, Suzuki, Caceres, Gómez-Gesteira, 2015. Hybridization of the Wave Propagation Model SWASH and the Meshfree Particle Method SPH for Real Coastal Applications, Coast. Eng. J. 57 (2015) 1550024. doi:doi:10.1142/S0578563415500242.

# Runs SWASH simulation, "output"  and output directories are named according to the SWASH (v.5.01)
# Defines "name" refers to the .xml for DualSPHysics

export name=CaseML_CIEM
export dirout=${name}_out
export diroutdata=${dirout}/data
# name of file created and corrected using wMLpistons_SWASH.bat
export swash_name=CIEM_SWASH
export nameswcor=CIEM_SWASH_corr_velx_x09_y00.csv
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
if [ ! -e ${nameswcor} ]; then echo ***Error: CSV file is missing ${nameswcor}, run the script wMLpistons_SWASH first.; fi
cp ${nameswcor} ${dircopy}
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

${partvtk} -dirin ${diroutdata} -savevtk ${dirout2}/PartMov -onlytype:-all,moving
if [ $? -ne 0 ] ; then fail; fi

# WG measurements
export dirout2=${dirout}/measures
${measuretool} -dirin ${diroutdata} -points AWG5.txt -onlytype:-all,+fluid -elevation:0.4  -savecsv ${dirout2}/AWG5
if [ $? -ne 0 ] ; then fail; fi

:success

read -n1 -r -p "Press any key to continue..." key
