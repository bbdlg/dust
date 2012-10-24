#!/bin/sh
################################
# find key value in config file
# a config file maybe like this:
#     [FIELD]
#     KEY1 = VALUE1
#     KEY2 = VALUE2
#     [FIELD]
#     KEY1 = VALUE3
# if you want to get 'VALUE3', you maybe use it like this:
# ./find.sh file.conf 2 FIELD KEY1
################################

if [ $# -lt 4 ]; then
   echo "usage: `basename $0` filename times field key"
   echo "return sum of field appears in filename if times is 0"
   echo
   exit
fi

filename=$1
times=$2
field=$3
key=$4

if [ ! -f ${filename} ]; then
   #echo "$filename is not a file"
   exit
fi
if [ `expr ${times} : '[0-9]*$'` -eq 0 ]; then
   #echo "times is not digital number"
   exit
fi
if [ ${times} -eq 0 ]; then
   cat ${filename} | grep -E "\[${field}\]" | wc -l
   exit
fi

start_line=`cat -n ${filename} | grep -E "\[${field}\]" | cat -n | sed -n ''$(echo ${times})'p' | awk '{print $2}'`
end_line=`cat -n ${filename} | grep -E "\[${field}\]" | cat -n | sed -n ''$(expr ${times} + 1)'p' | awk '{print $2 - 1}'`
if [ -z $start_line ]; then
   #start_line=1
   exit
fi
if [ -z $end_line ]; then
   end_line=$
fi

sed -n ''$(echo ${start_line})','$(echo ${end_line})'p' ${filename} | grep -E "^${key}" | cut -d= -f2 | sed 's/[[:space:]]*//g' 

