#!/bin/bash 

fail () { 
 echo Execution aborted. 
 read -n1 -r -p "Press any key to continue..." key 
 exit 1 
}

# Altomare, Tagliafierro, Dominguez, Suzuki, Viccione. Improved relaxation zone method in SPH-based model for coastal engineering applications, Applied Ocean Research, Volume 81, 2018, Pages 15-33, ISSN 0141-1187, https://doi.org/10.1016/j.apor.2018.09.013.

export swash_name=Case_SWASH_8L
export output=${swash_name}_out
# value that identify the SWASH file to correct
export xvalue=x10_y00.csv 
export filesws=${swash_name}.sws
export dirswash=swash_results

# "SWASH executables" and gencase are renamed and called from their directory
export dirbin=../../../bin/linux
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${dirbin}
export readswash="${dirbin}/ReadSwash_linux64"
export Swash="../${dirbin}/swash_linux64"

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
if [ $? -ne 0 ] ; then fail; fi

export filein=${output}/${dirswash}/${swash_name}
export fileout=${output}/${dirswash}/${swash_name}_corr

${readswash} -correctcsv:50:0 ${filein}_velx_x00_y00.csv ${fileout}_velx_x00_y00.csv -tclip:0:50
${readswash} -correctcsv:50:0 ${filein}_velz_x00_y00.csv ${fileout}_velz_x00_y00.csv -tclip:0.0000:50
${readswash} -correctcsv:50:0 ${filein}_velx_x01_y00.csv ${fileout}_velx_x01_y00.csv -tclip:0.0000:50
${readswash} -correctcsv:50:0 ${filein}_velz_x01_y00.csv ${fileout}_velz_x01_y00.csv -tclip:0.0000:50
${readswash} -correctcsv:50:0 ${filein}_velx_x02_y00.csv ${fileout}_velx_x02_y00.csv -tclip:0.0000:50
${readswash} -correctcsv:50:0 ${filein}_velz_x02_y00.csv ${fileout}_velz_x02_y00.csv -tclip:0.0000:50
${readswash} -correctcsv:50:0 ${filein}_velx_x03_y00.csv ${fileout}_velx_x03_y00.csv -tclip:0.0000:50
${readswash} -correctcsv:50:0 ${filein}_velz_x03_y00.csv ${fileout}_velz_x03_y00.csv -tclip:0.0000:50
${readswash} -correctcsv:50:0 ${filein}_velx_x04_y00.csv ${fileout}_velx_x04_y00.csv -tclip:0.0000:50
${readswash} -correctcsv:50:0 ${filein}_velz_x04_y00.csv ${fileout}_velz_x04_y00.csv -tclip:0.0000:50
if [ $? -ne 0 ] ; then fail; fi

:success

read -n1 -r -p "Press any key to continue..." key
