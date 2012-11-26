#!/bin/sh
### This script is used to install Crop including set DFCHOME.

if [ "$1" = "-h" ]; then
   echo "Usage: `basename $0` [destination directory]"
   echo
   exit
fi

DFCHOME=`pwd`
CUR_DIR_NAME=`basename ${DFCHOME}`
if [ -d "$1" ]; then
   echo "<$1> is a valid directory, now <${CUR_DIR_NAME}> will be installed there."
   rm -rf $1/${CUR_DIR_NAME}
   cp -rf ${DFCHOME} $1
   DFCHOME=$1/${CUR_DIR_NAME}
else
   echo "current directory <`pwd`> will be the DFCHOME."
fi

echo "export DFCHOME=${DFCHOME}" >> ~/.bash_profile
echo "export PATH=$PATH:${DFCHOME}/bin:." >> ~/.bash_profile
source ~/.bash_profile
chmod +x ${DFCHOME}/bin/*

echo "${DFCHOME} is the work space."
echo

