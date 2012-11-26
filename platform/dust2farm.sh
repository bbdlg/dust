#!/bin/sh
### this script should be in "dust/platform/"

IS_EXIT=YES
case $1 in
   help)
      echo "Usage: `basename $0` [platform_name version]"
      echo "       `basename $0` [help | clear | make | reset | test]"
      echo "       help,  show this info"
      echo "       clear, clear compiling temp files in dust/build/"
      echo "       make,  just make, if any error, try \"`basename $0` reset\""
      echo "       test,  generate platform without package"
      echo
      ;;
   clear)
      ### clear compile temp files, such as Makefile, *.o, etc.
      cd ../build/ && ls -a |grep -v configure.in |grep -v Makefile.am |grep -v "^\.$" | grep -v "^\.\.$"|xargs rm -rf && cd -
      ;;
   make)
      cd ../build/ && make && cd -
      ;;
   reset)
      cd ../build/ && aclocal && autoconf && autoheader && automake --add-missing && ./configure && make clean && make && cd -
      ;;
   *)
      IS_EXIT=NO
esac
if [ ${IS_EXIT} = YES ]; then
   exit 1
fi


### package develop platform named "farm":)
FARM="farm"
VER="1.0.0"
TARGETDIR=${FARM}_${VER}
TARGET=${TARGETDIR}.tar.gz

if [ $# != 2 ]; then
   echo "Now use default value, target name will be ${TARGET}"
   echo
else
   FARM=$1
   VER=$2
fi

TARGETDIR=${FARM}_${VER}
TARGET=${TARGETDIR}.tar.gz

cd ../build/ && aclocal && autoconf && autoheader && automake --add-missing && ./configure && make clean && make && cd -
echo

if [ -d ${TARGETDIR} ]; then
   #mv ${TARGETDIR} ${TARGETDIR}_`date +%Y-%m-%d_%H_%M_%S`_bak
   rm -rf ${TARGETDIR}
fi
mkdir ${TARGETDIR} 
mkdir ${TARGETDIR}/include/ 
mkdir ${TARGETDIR}/lib/ 
mkdir ${TARGETDIR}/bin
mkdir ${TARGETDIR}/src
mkdir ${TARGETDIR}/conf 
#mkdir ${TARGETDIR}/doc 
#mkdir ${TARGETDIR}/log 
#mkdir ${TARGETDIR}/data 

cp ./farm_README                 ${TARGETDIR}/README
cp ../commModule/commModule.h    ${TARGETDIR}/include
cp ../errModule/errModule.h      ${TARGETDIR}/include
cp ../logModule/logModule.h      ${TARGETDIR}/include
cp ../lgModule/lgModule.h        ${TARGETDIR}/include
cp ../toolsModule/toolsModule.h  ${TARGETDIR}/include
cp ./userapp.h                   ${TARGETDIR}/include
cp ./main.h                      ${TARGETDIR}/include
cp ../logModule/seelog.sh        ${TARGETDIR}/bin
cp ../lgModule/lp.sh             ${TARGETDIR}/bin
cp ./farm2crop.sh                ${TARGETDIR}/bin
cp ./crop_*                      ${TARGETDIR}/bin/
cp ./userapp.c                   ${TARGETDIR}/src
cp ./farm_Makefile.am            ${TARGETDIR}/src/Makefile.am
cp ./farm_makefile               ${TARGETDIR}/src/makefile
cp ./farm_configure.in           ${TARGETDIR}/src/configure.in
cp ../build/libdust.a            ${TARGETDIR}/lib/
cp ./farm_dust.conf              ${TARGETDIR}/conf/dust.conf

### package
if [ "$1" = "test" ]; then
   echo "You can check <${TARGETDIR}> in current directory."
else
   tar czf ${TARGET} ${TARGETDIR}
   rm -rf ${TARGETDIR}
   echo "Target<${TARGET}> will be found in current directory."
fi

echo

