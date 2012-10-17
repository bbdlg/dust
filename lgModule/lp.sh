#!/bin/bash
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

