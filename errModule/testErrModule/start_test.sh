#!/bin/sh
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

