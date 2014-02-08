FLAGS=

all: extractbitrate.js

extractbitrate.js: extractbitrate.cpp mp4v2/.libs/libmp4v2.a
	emcc -Imp4v2/include ${FLAGS} extractbitrate.cpp -o extractbitrate.bc
	emcc ${FLAGS} extractbitrate.bc mp4v2/.libs/libmp4v2.a -o extractbitrate.html --preload-file tt3.mp4 -s EXPORTED_FUNCTIONS="['_extractbitrate']"
