#!/bin/sh
#
# script for getting bjam and bumbershoot up and running
#
# Get the location of quickbuild.sh and drop trailing slash
ROOT=$(pwd)
if [ ! -e $ROOT/quickbuild.sh ]; then
    echo "quickbuild.sh must be run from the directory it resides in - quitting"
    exit 1
fi

# per platform in case of multi OS shared volume (VMware etc)
BOOST_BUILD_PATH=$ROOT/freicore/libraries/boost-build
BJAM_PATH=$BOOST_BUILD_PATH/engine/bin/$(uname -s)
BJAM=$BJAM_PATH/bjam

# Build local copy of bjam
if [ ! -e $BJAM ]; then
    echo "Building bjam..."
    cd $BOOST_BUILD_PATH/engine
    LOCATE_TARGET=bin sh build.sh
    mkdir -p $BJAM_PATH
    cp -f $BOOST_BUILD_PATH/engine/bin/bjam $BJAM
fi

cd $ROOT
if ! BOOST_BUILD_PATH=$BOOST_BUILD_PATH $BJAM "$@"; then
   echo "This bumbershoot target failed to build."
   exit 1
fi
