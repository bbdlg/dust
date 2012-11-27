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

# call generate_test_runner.rb to process all test files
# and start test

if [ $# -eq 0 ]; then
   echo "usage: $0 test1.c test2.c ..."
   echo
   exit 1
fi

while [ $# -gt 0 ]
do
   #echo "now process $1 ..."
   ruby ../../unity/auto/generate_test_runner.rb $1
   shift
done

make clean
make
make clean

