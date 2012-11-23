#!/bin/sh

APP="crop"
VER="1.0.0"
TARGETDIR=${APP}_${VER}
TARGET=${TARGETDIR}.tar.gz

echo "Usage:   `basename $0` [appname][version]"
echo "Example: `basename $0` ${APP} ${VER}"
echo "         target name will be ${TARGET}"
echo

if [ $# != 2 ]; then
   echo -n "Parameter maybe wrong, run me again?(y/n):"
   read RUNAGAIN
   case $RUNAGAIN in
      n|N) 
         echo "Now use default value, target name will be ${TARGET}"
         ;;
      y|Y|*) 
         echo "exit ..."
         exit
         ;;
   esac
fi

APP=$1
VER=$2
TARGETDIR=${APP}_${VER}
TARGET=${TARGETDIR}.tar.gz

if [ ! -d ${DUSTHOME} ]; then
   echo "Failed: DUSTHOME is not set, now exit ..."
   echo
   exit
fi

rm -rf ${TARGETDIR}
mkdir ${TARGETDIR}
mkdir ${TARGETDIR}/bin
mkdir ${TARGETDIR}/conf

cp -rf `ls ${DUSTHOME}/bin/ | grep -E -v "^(\`basename $0\`|crop_*|${TARGETDIR})$"` ${TARGETDIR}/bin/
cp -rf ${DUSTHOME}/conf/*     ${TARGETDIR}/conf
#cp -rf ${DUSTHOME}/crop_install.sh ${TARGETDIR}/install.sh
#cp -rf ${DUSTHOME}/crop_README     ${TARGETDIR}/README

tar czf ${TARGET} ${TARGETDIR}
rm -rf ${TARGETDIR}

mv ${TARGET} ${DUSTHOME}
echo "Target<${TARGET}> will be found in ${DUSTHOME}."
echo

