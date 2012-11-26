#!/bin/sh

if [ $# -lt 1 ]; then
   echo "Usage: `basename $0` [destination directory]"
   echo
fi

DFCHOME=`pwd`
CUR_DIR_NAME=`basename ${DFCHOME}`
if [ ! -z "$1" -a -d $1 ]; then
   echo "<$1> is a valid directory, now <${CUR_DIR_NAME}> will be installed there."
   rm -rf $1/${CUR_DIR_NAME}
   cp -rf ${DFCHOME} $1
   DFCHOME=$1/${CUR_DIR_NAME}
else
   echo "current directory <`pwd`> will be the DFCHOME."
fi

echo "DFCHOME=${DFCHOME}" >> ~/.bash_profile
echo "PATH=$PATH:${DFCHOME}/bin:." >> ~/.bash_profile
echo "${DFCHOME}"

