#!/bin/sh

exec_target=dfc

function help()
{
   echo "Usage: $0 [help|dust|debug|farm|clean|distclean]"
   echo "   help,       show this information;"
   echo "   dust,       generate executable file: ${exec_target};"
   echo "   farm,       generate FARM develop platform, you may add a version after 'farm', eg: $0 farm 1.0.0;"
   echo "   clean,      just clean current project temp files;"
   echo "   distclean,  clean all make/cmake files;"
   echo "   without any parameter will be equal to <$0 dust>."
   echo
}

if [ "$1" = "farm" ]; then
   cmake -Wno-dev -Ddust2farm:bool=on --build .. && make
   mv bin/lib${exec_target}.a .
   ../platform/dust2farm.sh $1 $2
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

