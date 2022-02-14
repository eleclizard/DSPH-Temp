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
export swash_name=CIEM_SWASH
export output=${swash_name}_out
# value that identify the SWASH file to correct
export xvalue=x09_y00.csv 
export filesws=${swash_name}.sws
export dirswash=swash_results
export diroutparts=${output}/MovingParts_out
export diroutvel=${output}/Velocity_out

# "SWASH executables" and gencase are renamed and called from their directory

export dirbin=../../../bin/linux
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${dirbin}
export readswash="${dirbin}/ReadSwash_linux64"
export Swash="../${dirbin}/swash_linux64"
export gencase="${dirbin}/GenCase_linux64"

# runs SWASH
if [ -e ${output} ]; then rm -r ${output}; fi
mkdir ${output}
cp ${swash_name}/* ${output}
cp ${swash_name}/${swash_name}.sws ${output}/INPUT
cd ${output}
${Swash}
if [ $? -ne 0 ] ; then fail; fi

if [ ! -e ${dirswash} ]; then mkdir ${dirswash}; fi
cd ..

# Defines the file to convert and correct and the rate of correction. Creates a converted .csv file for DualSPHysics

${readswash} -savecsv ${output}/${filesws} ${output}/${dirswash}/${swash_name}
export filein=${output}/${dirswash}/${swash_name}
export fileout=${output}/${dirswash}/${swash_name}_corr
${readswash} -correctcsv:400:1 ${filein}_velx_${xvalue} ${fileout}_velx_${xvalue} -tclip:0

# Creates VTK to show the piston velocity.
if [ -e ${diroutvel} ]; then rm -r ${diroutvel}; fi
${readswash} -savevtkveldp:0.04 ${diroutvel}/velocity -file_velx ${fileout}_velx_${xvalue} -tmin:0 -tmax:80 -dt:0.5 -smooth:10
if [ $? -ne 0 ] ; then fail; fi

# Creates VTK with simulation of piston motion.
if [ -e ${diroutparts} ]; then rm -r ${diroutparts}; fi
${gencase} ${name}_Def ${diroutparts}/${name} -save:vtkall
if [ $? -ne 0 ] ; then fail; fi

cp ${fileout}_velx_${xvalue} ${diroutparts}
if [ $? -ne 0 ] ; then fail; fi

${readswash} -savevtk_case ${diroutparts}/piston -loadcase ${diroutparts}/${name} -tmin:0 -tmax:80 -dt:0.5
if [ $? -ne 0 ] ; then fail; fi

:success

read -n1 -r -p "Press any key to continue..." key
