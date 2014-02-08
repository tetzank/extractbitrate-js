#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <mp4v2/mp4v2.h>

extern "C"{

unsigned char *gbuf;
int64_t gbuf_pos;
int64_t gbuf_size;

void *p_open(const char *name, MP4FileMode mode){
	//puts("open called");
	if(mode == FILEMODE_READ){
		return gbuf; // do nothing
	}else{
		return NULL; // only allow read, fail for everything else
	}
}

int p_seek(void *handle, int64_t pos){
	//puts("seek called");
	if(pos > gbuf_size){
		printf("pos: %lli; gbuf_size: %lli\n", pos, gbuf_size);
		return -1;
	}else{
		gbuf_pos = pos;
		return 0; //
	}
}

int p_read(void *handle, void *buffer, int64_t size, int64_t *nin, int64_t maxChunkSize){
	//puts("read called");
	// ignores maxChunkSize
	if((gbuf_pos + size) > gbuf_size){
		printf("size: %lli; gbuf_pos: %lli; gbuf_size: %lli\n", size, gbuf_pos, gbuf_size);
		size = gbuf_size - gbuf_pos;
	}
	memcpy(buffer, gbuf+gbuf_pos, size);
	gbuf_pos += size;
	*nin = size;
	if(*nin > 0){
		return 0;
	}else{
		return -1;
	}
}

int p_write(void *handle, const void *buffer, int64_t size, int64_t *nout, int64_t maxChunkSize){
	//puts("write called");
	return -1; //HACK: just fail, don't write anything
}

int p_close(void *handle){
	//puts("close called");
	return 0; // nothing to do
}

int extractbitrate(unsigned char *buf, int size){
	printf("size: %i; head: %c%c%c%c\n",
		size, buf[4], buf[5], buf[6], buf[7]);

	// HACK: ugly workaround
	// this isn't used, but makes the other buffer work for whatever reason
	// needs tt.mp4 preloaded, is the same size as buf, not sure if that's important
// 	FILE *fd = fopen("tt.mp4", "rb");
// 	fseek(fd, 0L, SEEK_END);
// 	size_t fsize = ftell(fd);
// 	fseek(fd, 0L, SEEK_SET);
// 	unsigned char *buffer = (unsigned char*)malloc(/*f*/size);
// 	fread(buffer, 1, fsize, fd);
// 	fclose(fd);

	gbuf = /*buffer*/buf;
	gbuf_pos = 0;
	gbuf_size = /*fsize*/size;

	MP4FileProvider provider;
	provider.open = p_open;
	provider.seek = p_seek;
	provider.read = p_read;
	provider.write = p_write;
	provider.close = p_close;


	MP4FileHandle mp4handle = MP4ReadProvider("tt3.mp4", &provider);
	char *info = MP4Info(mp4handle);
	if(info){
		printf("%s\n", info);
		free(info);
	}


	int32_t tracks = MP4GetNumberOfTracks(mp4handle);
	printf("tracks: %i\n", tracks);
	int32_t overall_bitrate=0;
	for(int i=0; i<tracks; ++i){
		MP4TrackId tid = MP4FindTrackId(mp4handle, i);
		int32_t bitrate = MP4GetTrackBitRate(mp4handle, tid);
		printf("track %i bitrate: %i\n", i, bitrate);
		overall_bitrate += bitrate;
	}

	MP4Close(mp4handle);

// 	free(buffer);

	return overall_bitrate;
}

}
