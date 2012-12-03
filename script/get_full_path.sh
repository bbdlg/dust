#!/bin/sh

### change relative path to absolute path
### or get absolute path of a file
### return HOME of current account if no parameter given
function get_fullpath() 
{
   _PWD=`pwd`
   if [ -d $1 ]; then
      cd $1
   elif [ -f $1 ]; then
      cd `dirname $1`
   else
      cd 
   fi
   echo $(cd ..; cd -)
   cd ${_PWD} >/dev/null
}

### test
FILEARRAY="/root/ /root / ./../../not_exist_file ../../ . .. "
for FILE in ${FILEARRAY}
do
   PATH=`get_fullpath ${FILE}`
   echo "path of <${FILE}> is <${PATH}>"
done

