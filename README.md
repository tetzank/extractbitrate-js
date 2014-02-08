extractbitrate-js
=================

javascript function to extract the bitrate of a mp4 file

It uses libmp4v2 for extracting the bitrate. The library was ported to
javascript with emscripten.


Compile
-------

- make sure you have emscripten installed and emcc, emconfigure and emmake in
  your PATH
- run build-lib.sh to compile libmp4v2
- run make to build the javascript file extractbitrate.js
- site.html shows how to call and use the javascript file

License
-------

All the code in mp4v2/ is MPL 1.1 as described in mp4v2/COPYING.
All other code is MPL 2.0 as described in COPYING.
