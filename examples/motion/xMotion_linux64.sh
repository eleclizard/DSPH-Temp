#!/bin/bash

dirbin=../../bin/linux
gencase="${dirbin}/GenCase_linux64"
boundaryvtk="${dirbin}/BoundaryVTK_linux64"
errcode=0

c=1
while [ $c -le 9 ] && [ $errcode -eq 0 ]; do
  name=Motion0$c
  if [ -e $name ]; then
    rm -f -r $name
  fi
  $gencase $name $name/$name
  errcode=$?
  if [ $errcode -eq 0 ]; then
    $boundaryvtk -loadvtk AutoActual -filexml $name/${name}.xml -savevtkdata $name/${name}.vtk	
    errcode=$?
  fi

  let c=c+1
done 

if [ $errcode -eq 0 ]; then
  echo All done
else
  echo Execution aborted
fi
read -n1 -r -p "Press any key to continue..." key
echo
