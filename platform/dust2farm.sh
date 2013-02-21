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
VER="debug"

if [ $# != 2 ]; then
   echo -n "Use default version, "
else
   echo -n "Use version <$2>, "
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

echo "   Creating directory ..."
mkdir ${TARGETDIR} 
mkdir ${TARGETDIR}/include/ 
mkdir ${TARGETDIR}/lib/ 
mkdir ${TARGETDIR}/bin
mkdir ${TARGETDIR}/src
mkdir ${TARGETDIR}/conf 
mkdir ${TARGETDIR}/build
#mkdir ${TARGETDIR}/doc 
#mkdir ${TARGETDIR}/log 
#mkdir ${TARGETDIR}/data 

echo "   Coping *.h, *.c, *.a, *.sh files ..."
cp ../platform/farm_README             ${TARGETDIR}/README
cp ../platform/farm_build.sh           ${TARGETDIR}/build/build.sh
cp ../platform/farm_CMakeLists.txt     ${TARGETDIR}/CMakeLists.txt
cp ../platform/farm_src_CMakeLists.txt ${TARGETDIR}/src/CMakeLists.txt
for header in `find ../module* -type f -name "*.h"`
do
   cp ${header} ${TARGETDIR}/include
done
cp ../platform/userapp.h         ${TARGETDIR}/include/
cp ../platform/main.h            ${TARGETDIR}/include/
cp ../build/libdfc.a             ${TARGETDIR}/lib/
cp ../moduleLog/seelog.sh        ${TARGETDIR}/bin/
cp ../moduleLg/lp.sh             ${TARGETDIR}/bin/
cp ../platform/farm2crop.sh      ${TARGETDIR}/bin/
cp ../platform/crop_*            ${TARGETDIR}/bin/
cp ../platform/userapp.c         ${TARGETDIR}/src/userapp.c

echo "   Coping config files ..."
cp ../platform/dfc.conf          ${TARGETDIR}/conf/

### package
if [ "$1" = "test" ]; then
   echo "You can check <${TARGETDIR}> in current directory."
else
   echo "   Packing ..."
   tar czf ${TARGET} ${TARGETDIR}
   echo "   Remove temp files ..."
   rm -rf ${TARGETDIR}
   echo "   Move target ..."
   mv ${TARGET} ..
   echo -e "\033[7mNote: Target <${TARGET}> will be found in <`dirname $PWD`>.\033[0m"
fi

