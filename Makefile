FLAGS=-O3

all: extractbitrate.js

extractbitrate.js: extractbitrate.cpp mp4v2/.libs/libmp4v2.a
	emcc -Imp4v2/include ${FLAGS} extractbitrate.cpp -o extractbitrate.bc
	emcc ${FLAGS} --llvm-lto 3 --closure 1 extractbitrate.bc mp4v2/.libs/libmp4v2.a -o extractbitrate.js -s DISABLE_EXCEPTION_CATCHING=0 -s EXPORTED_FUNCTIONS="['_extractbitrate']"

mp4check.js: mp4check.cpp mp4v2/.libs/libmp4v2.a
	emcc -Imp4v2/include ${FLAGS} --llvm-lto 3 --closure 1 mp4check.cpp mp4v2/.libs/libmp4v2.a -o mp4check.js -s DISABLE_EXCEPTION_CATCHING=0 -s EXPORTED_FUNCTIONS="['_process','_getBitrate','_getAudioCodec','_getVideoCodec']"
