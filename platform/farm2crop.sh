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


APP="crop"
VER="1.0.0"
TARGETDIR=${APP}_${VER}
TARGET=${TARGETDIR}.tar.gz

echo "Usage:   `basename $0` [appname][version]"
echo "Example: `basename $0` ${APP} ${VER}"
echo "         target name will be ${TARGET}"
echo

if [ $# != 2 ]; then
   echo -n "Are u sure to use default appname and version?(y/n):"
   read RUNAGAIN
   case $RUNAGAIN in
      y|Y) 
         echo "Now use default value, target name will be ${TARGET}"
         ;;
      n|N|*) 
         echo "exit ..."
         exit
         ;;
   esac
else
   APP=$1
   VER=$2
fi

TARGETDIR=${APP}_${VER}
TARGET=${TARGETDIR}.tar.gz

if [[ ! -d ${DFCHOME} ]] || [[ "${DFCHOME}" != `dirname \`pwd\`` ]]; then
   echo "Failed: DFCHOME may be wrong, now exit ..."
   echo
   exit
fi

rm -rf ${TARGETDIR}
mkdir ${TARGETDIR}
mkdir ${TARGETDIR}/bin
mkdir ${TARGETDIR}/conf

cp -rf `ls ${DFCHOME}/bin/ | grep -E -v "^(\`basename $0\`|crop_*|${TARGETDIR})"` ${TARGETDIR}/bin/
cp -rf ${DFCHOME}/conf/*               ${TARGETDIR}/conf
cp -rf ${DFCHOME}/bin/crop_README      ${TARGETDIR}/README
cp -rf ${DFCHOME}/bin/crop_install.sh  ${TARGETDIR}/install.sh

tar czf ${TARGET} ${TARGETDIR}
rm -rf ${TARGETDIR}

mv ${TARGET} ${DFCHOME}
echo "Target<${TARGET}> will be found in ${DFCHOME}."
echo

