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

##############################################################################
# filename: lp.sh
# version:  v1.0
# time:     
# author:   bbdlg
# blog:     http://my.oschina.net/bbdlg
# function: inspect app program which inserted lgModule
#
##############################################################################

function help()
{
   echo "usage:   `basename $0` [lgModule's ip][lgModule's port]"
   echo "example: `basename $0` 127.0.0.1 1234"
   echo
}

if [ $# != 2 ]; then
   help
   exit 1
fi

nc $1 $2

