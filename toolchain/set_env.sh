#!/bin/bash
#
# Squale Cross compiler generation scripts
# (c)2019 Jean-François DEL NERO
#
# Set env script
#

CROSS_BUILD_SIGN=${CROSS_BUILD_SIGN:-"UNDEF"}

export CUR_FOLDER=`pwd`

if [ $CROSS_BUILD_SIGN = "CROSS_ENV_SET" ]; then

	echo "***********************************"
	echo "* Build environment already set ! *"
	echo "***********************************"
	echo
	echo "use the command exit to leave this build environment."
	echo
else
	export CROSS_BUILD_SIGN="CROSS_ENV_SET"
	export PATH="${CUR_FOLDER}/bin:${PATH}:"
	export CROSS_BUILD_SIGN="CROSS_ENV_SET"

	echo "************************************************************************"
	echo "* Squale 6809 Environment set !"
	echo "*"
	echo "* Commands:"
	echo "* m6809-unknown-gcc, m6809-unknown-as, m6809-unknown-ld"
	echo "************************************************************************"
	echo

	echo "Current folder :"
	pwd
	echo

	bash

	echo "************************************************************"
	echo "* You just leaved the Squale 6809 build Environment"
	echo "************************************************************"

fi
