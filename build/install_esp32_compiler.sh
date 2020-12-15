#!/bin/bash

set -x  # Print every command before execution
set -e  # Exit on the very first error

SCRIPT_DIR_RELATIVE="`dirname \"$0\"`"
SCRIPT_DIR="`realpath ${SCRIPT_DIR_RELATIVE}`"

EXTERNALS_DIR="${SCRIPT_DIR}/../externals"
INSTALL_COMPILER_SCRIPT=${EXTERNALS_DIR}/esp-idf/install.sh

${INSTALL_COMPILER_SCRIPT}
