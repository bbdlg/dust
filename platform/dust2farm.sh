#!/bin/sh
### This file is part of the DFC.
###
### Copyright (C) 2012 by bbdlg <shiluo.110@163.com>
###
### The DFC is free software: you can redistribute it and/or modify
### it under the terms of the GNU Lesser General Public License as published by
### the Free Software Foundation, either version 3 of the License, or
### (at your option) any later version.
###
### The DFC is distributed in the hope that it will be useful,
### but WITHOUT ANY WARRANTY; without even the implied warranty of
### MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
### GNU Lesser General Public License for more details.
###
### You should have received a copy of the GNU Lesser General Public License
### along with DFC. If not, see <http://www.gnu.org/licenses/>.
###

### this script should be in "dust/platform/"

### package develop platform named "farm":)
FARM="farm"
VER="1.0.0"

if [ $# != 2 ]; then
   echo -n "Now use default value, "
   echo
else
   FARM=$1
   VER=$2
fi

TARGETDIR=${FARM}_${VER}
TARGET=${TARGETDIR}.tar.gz
echo "target name will be ${TARGET}"

if [ -d ${TARGETDIR} ]; then
   #mv ${TARGETDIR} ${TARGETDIR}_`date +%Y-%m-%d_%H_%M_%S`_bak
   rm -rf ${TARGETDIR}
fi
mkdir ${TARGETDIR} 
mkdir ${TARGETDIR}/include/ 
mkdir ${TARGETDIR}/lib/ 
mkdir ${TARGETDIR}/bin
mkdir ${TARGETDIR}/src
mkdir ${TARGETDIR}/conf 
#mkdir ${TARGETDIR}/doc 
#mkdir ${TARGETDIR}/log 
#mkdir ${TARGETDIR}/data 

cp ../platform/farm_README       ${TARGETDIR}/README
for header in `find ../module* -type f -name "*.h"`
do
   cp ${header} ${TARGETDIR}/include
done
cp ../platform/userapp.h         ${TARGETDIR}/include
cp ../platform/main.h            ${TARGETDIR}/include
cp ../moduleLog/seelog.sh        ${TARGETDIR}/bin
cp ../moduleLg/lp.sh             ${TARGETDIR}/bin
cp ../platform/farm2crop.sh      ${TARGETDIR}/bin
cp ../platform/crop_*            ${TARGETDIR}/bin
cp ../platform/userapp.c         ${TARGETDIR}/src/userapp.c
cp ../build/libdfc.a             ${TARGETDIR}/lib/
cp ../platform/farm_dust.conf    ${TARGETDIR}/conf/dust.conf

### package
if [ "$1" = "test" ]; then
   echo "You can check <${TARGETDIR}> in current directory."
else
   tar czf ${TARGET} ${TARGETDIR}
   #rm -rf ${TARGETDIR}
   echo "Target<${TARGET}> will be found in current directory."
fi

echo

