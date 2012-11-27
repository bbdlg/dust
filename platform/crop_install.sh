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

