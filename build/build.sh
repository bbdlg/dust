#!/bin/sh

exec_target=dfc
package_script=`dirname $PWD`/platform/dust2farm.sh

function help()
{
   echo "Usage: $0 [help|dust|debug|farm|clean|distclean]"
   echo "   help,       show this information;"
   echo "   dust,       generate executable file: ${exec_target};"
   echo "   farm,       compile all module files without '`dirname $PWD`/platform/userapp.c/h', "
   echo "               and generate FARM develop platform."
   echo "               you may add a version after 'farm', eg: $0 farm 1.0.0;"
   echo "   clean,      just clean current project temp files;"
   echo "   distclean,  clean all make/cmake files;"
   echo "   without any parameter will be equal to <$0 dust>."
   echo
}

if [ "$1" = "farm" -o "$1" = "test" ]; then
   echo "Start to compile files ..."
   cmake -Wno-dev -Ddust2farm:bool=on --build .. && make
   mv bin/lib${exec_target}.a .
   echo "Finished!"
   echo

   if [ -x ${package_script} ]; then
      echo "Generate FARM platform ..."
      if [ -z $2 ]; then
         echo -e "\033[7mWarning: version is null, it will be default. You can do like this: '$0 farm 1.0.1'\033[0m"
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
   cmake -Wno-dev -Ddebug_mode=on -Ddust2farm:bool=off --build .. && make
   mv bin/${exec_target} .
elif [ "$1" = "dust" -o "$1" = "" ]; then
   cmake -Wno-dev -Ddust2farm:bool=off --build .. && make
   mv bin/${exec_target} .
else
   help
fi

