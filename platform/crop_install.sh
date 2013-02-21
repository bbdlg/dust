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

echo "Installing ..."
if [ ${DFCHOME} != ${PWD} ]; then
   echo "export DFCHOME=${PWD}" >> ~/.bash_profile
   echo "export PATH=$PATH:${DFCHOME}/bin:." >> ~/.bash_profile
   source ~/.bash_profile
fi
chmod +x ${DFCHOME}/bin/*

echo "Finished!"
echo -e "<\033[7m${DFCHOME}\033[0m> will be the workspace."
echo

