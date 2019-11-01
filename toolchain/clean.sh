#!/bin/bash
#
# Squale Cross compiler generation scripts
# (c)2019 Jean-François DEL NERO
#
# Set env script
#

CROSS_BUILD_SIGN=${CROSS_BUILD_SIGN:-"UNDEF"}

export CUR_FOLDER=`pwd`

echo "***********************************"
echo "* Cleaning...                     *"
echo "***********************************"

cd Squale_entry
make clean

cd ${CUR_FOLDER}

cd Squale_Tools
make clean
make mrproper

cd ${CUR_FOLDER}

rm -Rf bin build lib libexec share src m6809-unknown

