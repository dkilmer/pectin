#include <stdio.h>
#include <stdlib.h>
#include "wav.h"

int16_t *load_wav(const char *filename, unsigned int *num_samples) {
	FILE *f = fopen(filename, "rb");
	if (f == NULL) {
		printf("failed to open %s\n", filename);
		return NULL;
	}
	wav_header header;
	fmt_chunk fmt;
	data_chunk data_header;
	fread(&header, sizeof(wav_header), 1, f);
	printf("ckid: %08x\n", header.ckid);
	printf("cksize: %08x\n", header.cksize);
	printf("waveid: %08x\n", header.waveid);
	fread(&fmt, sizeof(fmt_chunk), 1, f);
	printf("ckid: %08x\n", fmt.ckid);
	printf("cksize: %08x\n", fmt.cksize);
	printf("format_tag: %08x\n", fmt.format_tag);
	printf("n_channels: %08x\n", fmt.n_channels);
	printf("samples_sec: %08x\n", fmt.samples_sec);
	printf("block_align: %08x\n", fmt.block_align);
	fread(&data_header, sizeof(data_chunk), 1, f);
	printf("ckid: %08x\n", data_header.ckid);
	printf("cksize: %08x\n", data_header.cksize);

	if (header.ckid != 0x46464952 || header.waveid != 0x45564157) {
		printf("%s is not a wav file\n", filename);
		return NULL;
	}
	if (fmt.format_tag != 1) {
		printf("%s is does not contain PCM data\n", filename);
		return NULL;
	}
	if (fmt.n_channels != 2) {
		printf("%s is does not contain stereo data\n", filename);
		return NULL;
	}
	if (fmt.samples_sec != 0x0000ac44) {
		printf("%s is does not contain 44100Hz audio\n", filename);
		return NULL;
	}
	if (data_header.ckid != 0x61746164) {
		printf("%s is does not contain a data header\n", filename);
		return NULL;
	}
	int16_t *data = (int16_t *)malloc(data_header.cksize);
	fread(data, data_header.cksize, 1, f);
	*num_samples = data_header.cksize / 4;
	fclose(f);
	return data;
}
