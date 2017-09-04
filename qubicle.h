#ifndef QUBICLE_H
#define QUBICLE_H

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t version;
	uint32_t color_format;
	uint32_t z_axis_orientation;
	uint32_t compressed;
	uint32_t visibility_mask_encoded;
	uint32_t num_matrices;
} qheader;

typedef struct {
	uint32_t size_x;
	uint32_t size_y;
	uint32_t size_z;
} qsize;

typedef struct {
	int32_t pos_x;
	int32_t pos_y;
	int32_t pos_z;
} qpos;

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} qcolor;

typedef struct {
	int thing;
	int x;
	int y;
	int z;
	int zsize;
} qbox;

qcolor *load_qubicle_file(const char *filename, qsize *qs);
qbox *load_qubicle_boxes(const char *filename, int *size);
unsigned char get_qalpha(qcolor *matrix, qsize *qs, int x, int y, int z);
unsigned char get_qthing(qcolor *matrix, qsize *qs, int x, int y, int z);
void get_qcolor(qcolor *matrix, qsize *qs, int x, int y, int z, qcolor *qc);
int has_qthing(qcolor *matrix, qsize *qs, int x, int y, int z, int samexy);
unsigned int get_surround(qcolor *matrix, qsize *qs, int x, int y, int z);

#ifdef __cplusplus
}
#endif

#endif //QUBICLE_H
