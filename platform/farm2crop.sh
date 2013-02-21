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
CROP="crop"
VER="debug"

if [ $# != 2 ]; then
   echo -n "Use default version, "
else
   echo -n "Use version <$2>, "
   CROP=$1
   VER=$2
fi

TARGETDIR=${CROP}_${VER}
TARGET=${TARGETDIR}.tar.gz
echo "target name will be ${TARGET}"

if [ -d ${TARGETDIR} ]; then
   #mv ${TARGETDIR} ${TARGETDIR}_`date +%Y-%m-%d_%H_%M_%S`_bak
   rm -rf ${TARGETDIR}
fi

echo "   Creating directory ..."
mkdir ${TARGETDIR} 
mkdir ${TARGETDIR}/bin
mkdir ${TARGETDIR}/conf 
#mkdir ${TARGETDIR}/doc 
#mkdir ${TARGETDIR}/log 
#mkdir ${TARGETDIR}/data 

echo "   Coping doc, *.sh files ..."
cp ../bin/crop_README            ${TARGETDIR}/README
cp ../bin/crop_install.sh        ${TARGETDIR}/install.sh
cp ../bin/*                      ${TARGETDIR}/bin/
rm -rf ${TARGETDIR}/bin/crop_README
rm -rf ${TARGETDIR}/bin/crop_install.sh

echo "   Coping config files ..."
cp ../conf/dfc.conf              ${TARGETDIR}/conf/

### package
if [ "$1" = "test" ]; then
   echo -e "\033[7mNote: You can check <${TARGETDIR}> in current directory.\033[0m"
else
   echo "   Packing ..."
   tar czf ${TARGET} ${TARGETDIR}
   echo "   Remove temp files ..."
   rm -rf ${TARGETDIR}
   echo "   Move target ..."
   mv ${TARGET} ..
   echo -e "\033[7mNote: Target <${TARGET}> will be found in <`dirname $PWD`>.\033[0m"
fi

