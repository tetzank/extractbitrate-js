#!/bin/sh

pushd mp4v2

emconfigure ./configure --disable-debug --disable-util
emmake make

popd

