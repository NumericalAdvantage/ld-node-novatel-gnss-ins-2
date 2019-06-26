#!/bin/bash
#
# This file is part of project link.developers/ld-node-novatel-gnss-ins-2.
# It is copyrighted by the contributors recorded in the version control history of the file,
# available from its original location https://gitlab.com/link.developers/ld-node-novatel-gnss-ins-2.
#
# SPDX-License-Identifier: MPL-2.0

mkdir build
cd build

cmake .. \
	-DCMAKE_PREFIX_PATH="$PREFIX" \
	-DCMAKE_INSTALL_PREFIX="$PREFIX" \
	-DCMAKE_BUILD_TYPE=Release

make -j $CPU_COUNT
make install
