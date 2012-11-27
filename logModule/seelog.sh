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
# filename: seelog.sh
# version:  v1.0
# time:     2012-9-26 16:04:37
# author:   bbdlg
# blog:     http://my.oschina.net/bbdlg
# function: see log file
#
# log file must like this:
#     sec        usec   INFO/WARNING/ERROR function_name              file_name        line_no. message
#     1348644724 415327 ERROR              test_log_switchFilesByTime test_logModule.c 193      this is a test message~

# if use ./seelog.sh -f example.log -ev, we will see:
#     ERROR: 2012-09-26 15:32:04-415327 this is a test message~
#
# use ./seelog.h -h to get more help infomation
##############################################################################

function help()
{
   echo "usage:   `basename $0` [-f logFileName|logFilePath] [-i, -w, -e, -a] [-t|-v] [-d] [-h]"
   echo "example: `basename $0` -f /tmp/log/20120921/144623112233.log -iw -t"
   echo "         -i, info; -w, warning; -e, error; -a, all of above"
   echo "         -t, trace, if para after '-f' is a path of log, here will be only see the lastest file"
   echo "         -v, view, this is a default option against '-t'"
   echo "         -d, see the original log without any format"
   echo "         -h, show this help infomation."
   echo
}

FILE=""
FILEDIR=""
SORT=""
HOW="view"
INDETAIL=""

if [ $# = 0 ]; then
   help
   exit 1
fi

SUMOPT=1
while getopts :iweatvdhf: OPTION
do
   SUMOPT=${SUMOPT}+1
   case $OPTION in 
      a)SORT="|INFO|WARNING|ERROR"     #all log levels
         ;;
      i)
         if [ "${SORT}" = "*INFO*" ]; then
            continue
         fi
         SORT=${SORT}"|INFO" #include info level
         ;;
      w)
         if [ "${SORT}" = "*WARNING*" ]; then
            continue
         fi
         SORT=${SORT}"|WARNING" #include warning level
         ;;
      e)
         if [ "${SORT}" = "*ERROR*" ]; then
            continue
         fi
         SORT=${SORT}"|ERROR" #include error level
         ;;
      t)HOW="trace"
         ;;
      v)HOW="view"
         ;;
      d)INDETAIL="indetail"
         ;;
      h)help
         exit 0
         ;;
      f)
         if [ -f $OPTARG ]; then
            FILE=`basename $OPTARG`
            FILEDIR=`dirname $OPTARG`
         elif [ -d $OPTARG ]; then
            TMPCMD="eval ls -rt $OPTARG | sed ':a;$!N;/.$/s/\n/ /;ta;P;D'"
            FILE=`${TMPCMD}`
            FILEDIR=${OPTARG}
         else 
            echo "$OPTARG is not a file or directory"
            exit 2
         fi
         ;;
      \?)echo "unknown option -$OPTARG"
         help
         exit 3
         ;;
   esac
done

if [ ${SUMOPT} = 0 ]; then
   help
   exit 4
fi

# delete first '|'
SORT=${SORT#*|}
if [ -z "$FILEDIR" -o -z "$FILE" ]; then
   help
   exit 5
fi

echo "how is $HOW, filedir is <$FILEDIR> file is <$FILE>, sort is $SORT"

if [ $HOW = "trace" ]; then
   SEECMD="tail -f"
elif [ $HOW = "view" ]; then
   SEECMD="more"
else
   SEECMD="more"
fi

for X in $FILE
do
   LINENO=2
   if [ ${HOW} = "view" ]; then
      echo "now process <${X}> ..."
      echo "=================================================="
      if [ "${INDETAIL}" = "indetail" ]; then
         ${SEECMD} ${FILEDIR}/${X} | grep -E "${SORT}"
      else
         #${SEECMD} ${FILEDIR}/${X} | grep -E "${SORT}" | awk '{printf $1" "$2" "; for(i=7;i<=NF;i++){printf $i" "}printf "\n"}' 
         ${SEECMD} ${FILEDIR}/${X} | grep -E "${SORT}" | awk '{printf $3": "strftime("%F %T", $1)"-"$2" "; for(i=7;i<=NF;i++){printf $i" "}printf "\n"}' 
      fi
      echo "=================================================="
      echo "processed <${X}>."
      echo
   fi
done

if [ ${HOW} = "trace" ]; then
   echo "now process <${X}> ..."
   if [ "${INDETAIL}" = "indetail" ]; then
      ${SEECMD} ${FILEDIR}/${X} | grep -E "${SORT}"
   else
      ${SEECMD} ${FILEDIR}/${X} | grep -E "${SORT}" | awk '{printf $1" "; for(i=6;i<=NF;i++){printf $i" "}printf "\n"}' 
   fi
   echo "processed <${X}>."
   echo
fi

