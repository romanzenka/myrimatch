#!/bin/bash

root=$(dirname $0)
pushd $root > /dev/null

./freicore/clean.sh

if (ls build-*-* > /dev/null 2>&1); then rm -fr build-*-*; fi;

popd > /dev/null