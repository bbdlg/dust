#!/bin/sh
TARGET="MANIFEST"
cd ..

echo "DFC" > ${TARGET}
echo "=============" >> ${TARGET}
git ls-files >> ${TARGET}

cd -

