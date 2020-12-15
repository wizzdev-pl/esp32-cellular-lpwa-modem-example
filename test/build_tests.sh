#!/bin/bash

# Prepares and build tests in a working directory (please run it in an out of source build directory)

set -x  # Print every command before execution
set -e  # Exit on the very first error

SOURCE_DIR_RELATIVE="`dirname \"$0\"`"
SOURCE_DIR="`realpath ${SOURCE_DIR_RELATIVE}`"
BIN_DIR="`pwd`"
NUMBER_OF_PROCESSORS=$(grep -c ^processor /proc/cpuinfo)


mkdir -p externals/googletest/
pushd externals/googletest/

GOOGLE_TEST_SRC_DIR="${SOURCE_DIR}/../externals/googletest/"
cmake ${GOOGLE_TEST_SRC_DIR} -DBUILD_GMOCK=OFF -DCMAKE_INSTALL_PREFIX=bin
make all
make install

popd


cmake ${SOURCE_DIR}
make
./esp32_test_on_pc
