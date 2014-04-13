#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <mp4v2/mp4v2.h>

extern "C"{

unsigned char *gbuf;
int64_t gbuf_pos;
int64_t gbuf_size;

void *p_open(const char *name, MP4FileMode mode){
	if(mode == FILEMODE_READ){
		return gbuf; // do nothing
	}else{
		return NULL; // only allow read, fail for everything else
	}
}

int p_seek(void *handle, int64_t pos){
	if(pos > gbuf_size){
		printf("pos: %lli; gbuf_size: %lli\n", pos, gbuf_size);
		return -1;
	}else{
		gbuf_pos = pos;
		return 0; //
	}
}

int p_read(void *handle, void *buffer, int64_t size, int64_t *nin, int64_t maxChunkSize){
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
	return -1; // just fail, don't write anything
}

int p_close(void *handle){
	return 0; // nothing to do
}

int64_t p_filesize(void *handle){
	return gbuf_size;
}

int g_bitrate;
const char *g_audioCodec;
const char *g_videoCodec;

int process(unsigned char *buf, int size){
	g_bitrate = 0;
	g_audioCodec = g_videoCodec = "Unsupported";

	gbuf = buf;
	gbuf_pos = 0;
	gbuf_size = size;

	MP4FileProvider provider;
	provider.open = p_open;
	provider.seek = p_seek;
	provider.read = p_read;
	provider.write = p_write;
	provider.close = p_close;
	provider.filesize = p_filesize;

	MP4FileHandle mp4handle = MP4ReadProvider("buffer", &provider);
	if(mp4handle == MP4_INVALID_FILE_HANDLE){
		printf("ERROR: invalid mp4 file\n");
		return -1;
	}
	// just for testing, probably remove later
	char *info = MP4Info(mp4handle);
	if(info){
		printf("%s\n", info);
		free(info);
	}

	int32_t tracks = MP4GetNumberOfTracks(mp4handle);
	g_bitrate = 0;
	for(int i=0; i<tracks; ++i){
		MP4TrackId tid = MP4FindTrackId(mp4handle, i);

		const char *trackType = MP4GetTrackType(mp4handle, tid);
		if(strcmp(trackType, MP4_AUDIO_TRACK_TYPE)==0){
			printf("found audio track %i\n", i);
			
			const char *mediaDataName = MP4GetTrackMediaDataName(mp4handle, tid);
			if(strcasecmp(mediaDataName, "mp4a")==0){
				uint8_t type = MP4GetTrackEsdsObjectTypeId(mp4handle, tid);
				if(type == MP4_MPEG4_AUDIO_TYPE){
					type = MP4GetTrackAudioMpeg4Type(mp4handle, tid);
					switch(type){
						//http://helpx.adobe.com/flash/kb/supported-codecs-flash-player.html
						case MP4_MPEG4_AAC_MAIN_AUDIO_TYPE:
						case MP4_MPEG4_AAC_LC_AUDIO_TYPE:
						case MP4_MPEG4_AAC_HE_AUDIO_TYPE:
							g_audioCodec = "AAC";
							printf("found suitable AAC audio track %i\n", i);
							break;

						default:
							printf("ERROR: unsupported audio codec %i\n", type);
					}
				}else{
					//FIXME: there are some files with MPEG2 AAC LC
					// type == MP4_MPEG2_AAC_LC_AUDIO_TYPE
					// maybe mpeg2 aac main too
					printf("ERROR: invalid mpeg4 audio on track %i (0x%x)\n", i, type);
				}
			}else{
				printf("ERROR: not the right audio codec on track %i (%s)\n", i, mediaDataName);
			}

		}else if(strcmp(trackType, MP4_VIDEO_TRACK_TYPE)==0){
			printf("found video track %i\n", i);

			const char *mediaDataName = MP4GetTrackMediaDataName(mp4handle, tid);
			if(strcasecmp(mediaDataName, "avc1")==0){
				uint8_t profile, level;
				if(MP4GetTrackH264ProfileLevel(mp4handle, tid, &profile, &level)){
					switch(profile){
						//http://helpx.adobe.com/flash/kb/supported-codecs-flash-player.html
						case  66: //Baseline
						case  77: //Main
						//case  88: //Extended
						case 100: //High
							g_videoCodec = "H264";
							printf("found suitable h264 video track %i\n", i);
							break;

						default: //High10, High 4:2:2, High 4:4:4 unsupported
							printf("ERROR: unsupported h264 profile on track %i (%i)", i, profile);
					}
					// don't care about level
				}else{
					printf("ERROR: failed to get profile and level on track %i\n", i);
				}
			}else{
				printf("ERROR: not the right video codec on track %i (%s)\n", i, mediaDataName);
			}

		}else{
			printf("ERROR: useless track %i, not audio or video (%s)\n", i, trackType);
		}

		g_bitrate += MP4GetTrackBitRate(mp4handle, tid);

	}

	MP4Close(mp4handle);

	return 0;
}

int getBitrate(){
	return g_bitrate;
}
const char *getAudioCodec(){
	return g_audioCodec;
}
const char *getVideoCodec(){
	return g_videoCodec;
}

} // extern "C"
