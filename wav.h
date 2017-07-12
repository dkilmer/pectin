#ifndef WAV_H
#define WAV_H

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t ckid;
	uint32_t cksize;
	uint32_t waveid;
} wav_header;

typedef struct {
	uint32_t ckid;
	uint32_t cksize;
	uint16_t format_tag;
	uint16_t n_channels;
	uint32_t samples_sec;
	uint32_t bytes_sec;
	uint16_t block_align;
	uint16_t bits_sample;
} fmt_chunk;

typedef struct {
	uint32_t ckid;
	uint32_t cksize;
} data_chunk;

int16_t *load_wav(const char *filename, unsigned int *num_samples);

#ifdef __cplusplus
}
#endif

#endif //WAV_H
