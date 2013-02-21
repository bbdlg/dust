#!/bin/sh

exec_target=dfc
package_script=`dirname $PWD`/bin/farm2crop.sh

function help()
{
   echo "Usage: $0 [help|farm|debug|crop|clean|distclean]"
   echo "   help,       show this information;"
   echo "   farm,       generate executable file: ${exec_target};"
   echo "   crop,       Generate CROP, which can be run in real environment."
   echo "               you may add a version after 'crop', eg: $0 crop 1.0.0;"
   echo "   clean,      just clean current project temp files;"
   echo "   distclean,  clean all make/cmake files;"
   echo "   without any parameter will be equal to <$0 farm>."
   echo
}

if [ "$1" = "crop" -o "$1" = "test" ]; then
   echo "Start to compile files ..."
   cmake -Wno-dev --build .. && make
   mv bin/${exec_target} ../bin
   echo "Finished!"
   echo

   if [ -x ${package_script} ]; then
      echo "Generate FARM platform ..."
      if [ -z $2 ]; then
         echo -e "\033[7mWarning: version is null, it will be default. You can do like this: '$0 crop 1.0.1'\033[0m"
      fi
      ${package_script} $1 $2
      echo "Finished!"
      echo
   else
      echo -e "\033[7merror: script<${package_script}> is not executable.\033[0m"
      echo
      exit
   fi
   #add other option
elif [ "$1" = "clean" ]; then
   make clean
elif [ "$1" = "distclean" ]; then
   ls | grep -v `basename $0` | xargs rm -rf 
elif [ "$1" = "debug" ]; then
   cmake -Wno-dev -Ddebug_mode=on --build .. && make
   mv bin/${exec_target} .
elif [ "$1" = "farm" -o "$1" = "" ]; then
   cmake -Wno-dev --build .. && make
   mv bin/${exec_target} .
else
   help
fi

