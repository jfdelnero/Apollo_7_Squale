#!/bin/bash
#
# Squale Cross compiler generation scripts
# (c)2019 Jean-François DEL NERO
#
# Cross compiler build script
#

export CUR_FOLDER=`pwd`

#
# Download sources
#

mkdir -p ${CUR_FOLDER}/download

wget http://lwtools.projects.l-w.ca/releases/lwtools/lwtools-4.17.tar.gz -nc --directory-prefix=download || exit 1
wget ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-4.6.4/gcc-4.6.4.tar.bz2 -nc --directory-prefix=download || exit 1
wget http://ftp.gnu.org/gnu/mpfr/mpfr-2.4.2.tar.xz -nc --directory-prefix=download || exit 1
wget https://gmplib.org/download/gmp/gmp-5.0.2.tar.bz2 -nc --directory-prefix=download || exit 1
wget http://ftp.gnu.org/gnu/mpc/mpc-1.0.3.tar.gz -nc --directory-prefix=download || exit 1
wget http://www.linuxfromscratch.org/patches/downloads/gcc/gcc-4.6.3-cross_compile-1.patch -nc --directory-prefix=download || exit 1

mkdir -p ${CUR_FOLDER}/bin || exit 1
mkdir -p ${CUR_FOLDER}/src || exit 1

#
# Build lwtools
#

cd ${CUR_FOLDER}/src || exit 1

tar zxvf ${CUR_FOLDER}/download/lwtools-4.17.tar.gz || exit 1

cd ${CUR_FOLDER}/src/lwtools-4.17   || exit 1

# Fix ifl_head multiple definitions build error
sed 's/struct\ ifl\ \*ifl_head\;/extern\ struct\ ifl\ \*ifl_head\;/' -i lwasm/input.h || exit 1

make  || exit 1
make INSTALLDIR="${CUR_FOLDER}/bin" install  || exit 1
cp ${CUR_FOLDER}/src/lwtools-4.17/extra/ar ${CUR_FOLDER}/bin/m6809-unknown-ar || exit 1
cp ${CUR_FOLDER}/src/lwtools-4.17/extra/as ${CUR_FOLDER}/bin/m6809-unknown-as || exit 1
cp ${CUR_FOLDER}/src/lwtools-4.17/extra/ld ${CUR_FOLDER}/bin/m6809-unknown-ld || exit 1
ln -s /bin/true ${CUR_FOLDER}/bin/m6809-unknown-nm        || exit 1
ln -s /bin/true ${CUR_FOLDER}/bin/m6809-unknown-objdump   || exit 1
ln -s /bin/true ${CUR_FOLDER}/bin/m6809-unknown-ranlib    || exit 1

#
# Build gcc 6809
#

cd ${CUR_FOLDER}/src || exit 1

tar jxvf ${CUR_FOLDER}/download/gcc-4.6.4.tar.bz2 || exit 1

cd gcc-4.6.4 || exit 1

tar xvf ${CUR_FOLDER}/download/mpfr-2.4.2.tar.xz || exit 1
tar xvf ${CUR_FOLDER}/download/gmp-5.0.2.tar.bz2 || exit 1
tar xvf ${CUR_FOLDER}/download/mpc-1.0.3.tar.gz  || exit 1

mv gmp-5.0.2 gmp    || exit 1
mv mpc-1.0.3 mpc    || exit 1
mv mpfr-2.4.2 mpfr  || exit 1

patch -p1 < ${CUR_FOLDER}/download/gcc-4.6.3-cross_compile-1.patch        || exit 1
patch -p1 < ${CUR_FOLDER}/src/lwtools-4.17/extra/gcc6809lw-4.6.4-9.patch  || exit 1

#sed -i 's/install_to_$(INSTALL_DEST) //' libiberty/Makefile.in

export PATH="$CUR_FOLDER/bin:$PATH"

mkdir -p ${CUR_FOLDER}/build   || exit 1
cd ${CUR_FOLDER}/build         || exit 1

${CUR_FOLDER}/src/gcc-4.6.4/configure --enable-languages=c --target=m6809-unknown --program-prefix=m6809-unknown- --enable-obsolete --srcdir=${CUR_FOLDER}/src/gcc-4.6.4 --disable-threads --disable-nls --disable-libssp --disable-doc --without-htmldir --without-pdfdir --without-docdir --prefix=${CUR_FOLDER} --with-as=${CUR_FOLDER}/bin/m6809-unknown-as --with-ld=${CUR_FOLDER}/bin/m6809-unknown-ld --with-ar=${CUR_FOLDER}/bin/m6809-unknown-ar || exit 1

make           || exit 1
make install   || exit 1

#
# Build Squale image tools
#

cd ${CUR_FOLDER}/Squale_Tools
make clean
make mrproper
make

#
# Build Squale loaders
#

cd ${CUR_FOLDER}/Squale_entry
make clean
make mrproper
make

