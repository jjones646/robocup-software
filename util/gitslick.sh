#!/bin/bash

UTIL_DIR=$(readlink -f $(dirname $0))
pushd "$UTIL_DIR" >/dev/null

echo "making sure submodules are updated"
pushd .. >/dev/null
git submodule update --init

echo "configuring a recommended set of git configurations"
./external/gitslick-gist/git-tweaks.sh

echo "setting up submodule hooks"
cp ./util/git-hooks/* .git/hooks/

dirs -c >/dev/null

echo "done"

