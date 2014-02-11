#!/bin/sh

pushd mp4v2

emconfigure ./configure --disable-debug --disable-util --disable-gch
emmake make -s -j8

popd

