#include "qubicle.h"

#include <stdio.h>
#include <stdlib.h>

unsigned char get_qalpha(qcolor *matrix, qsize *qs, int x, int y, int z) {
	if (x<0 || y<0 || z<0) return 0;
	if (x>=qs->size_x || y>=qs->size_y || z>=qs->size_z) return 0;
	return matrix[(x + (y*qs->size_x) + (z*qs->size_x*qs->size_y))].a;
}

unsigned char get_qthing(qcolor *matrix, qsize *qs, int x, int y, int z) {
	if (x<0 || y<0 || z<0) return 0;
	if (x>=qs->size_x || y>=qs->size_y || z>=qs->size_z) return 0;
	qcolor c = matrix[(x + (y*qs->size_x) + (z*qs->size_x*qs->size_y))];
	return c.b;
}

void get_qcolor(qcolor *matrix, qsize *qs, int x, int y, int z, qcolor *qc) {
	if (x<0 || y<0 || z<0 || x>=qs->size_x || y>=qs->size_y || z>=qs->size_z) {
		qc->r = 0;
		qc->g = 0;
		qc->b = 0;
		qc->a = 0;
		return;
	}
	qcolor c = matrix[(x + (y*qs->size_x) + (z*qs->size_x*qs->size_y))];
	qc->r = c.r;
	qc->g = c.g;
	qc->b = c.b;
	qc->a = c.a;
}

int has_qthing(qcolor *matrix, qsize *qs, int x, int y, int z, int samexy) {
	if (x<0 || y<0 || x>=qs->size_x || y>=qs->size_y) {
		return 1;
	}
	if ((z < 0 || z>=qs->size_z)) return 0;
	if (z < 0) z = 0;
	if (z>=qs->size_z) z = qs->size_z-1;
	qcolor c = matrix[(x + (y*qs->size_x) + (z*qs->size_x*qs->size_y))];
	return (c.a == 0) ? 0 : 1;
}


unsigned int get_surround(qcolor *matrix, qsize *qs, int x, int y, int z) {
	unsigned int n = 0;
	int shft = 0;
	for (int qz=z-1; qz<z+2; qz++) {
		for (int qy=y-1; qy<y+2; qy++) {
			for (int qx=x-1; qx<x+2; qx++) {
				int samexy = (qx == x && qy == y);
				n = n | (has_qthing(matrix, qs, qx, qy, qz, samexy) << shft);
				shft++;
			}
		}
	}
	return n;
}

qbox *get_boxes(qcolor *matrix, qsize *qs, int *size) {
	int cnt = 0;
	for (int z=0; z<qs->size_z; z++) {
		for (int y = 0; y < qs->size_y; y++) {
			for (int x = (qs->size_x - 1); x >= 0; x--) {
				unsigned char c = get_qalpha(matrix, qs, x, y, z);
				if (c != 0 && get_qalpha(matrix, qs, x, y, z - 1) == 0) {
					cnt++;
				}
			}
		}
	}
	qbox *boxes = (qbox *)malloc(sizeof(qbox) * cnt);
	*size = cnt;
	int idx = 0;
	for (int z=0; z<qs->size_z; z++) {
		for (int y = 0; y < qs->size_y; y++) {
			for (int x = (qs->size_x - 1); x >= 0; x--) {
				unsigned char c = get_qalpha(matrix, qs, x, y, z);
				if (c != 0 && get_qalpha(matrix, qs, x, y, z - 1) == 0) {
					unsigned char t = get_qthing(matrix, qs, x, y, z);
					if (t == 141) {
						boxes[idx].thing = 0;
					} else {
						boxes[idx].thing = 1;
					}
					boxes[idx].x = x;
					boxes[idx].y = y;
					boxes[idx].z = -z;
					boxes[idx].zsize = 1;
					int zz = z + 1;
					while (zz<qs->size_z && get_qalpha(matrix, qs, x, y, zz) != 0) {
						boxes[idx].zsize += 1;
						zz++;
					}
					idx++;
				}
			}
		}
	}
	return boxes;
}

qbox *load_qubicle_boxes(const char *filename, int *size) {
	qsize qs;
	qcolor *m = load_qubicle_file(filename, &qs);
	qbox *boxes = get_boxes(m, &qs, size);
	free(m);
	return boxes;
}


qcolor *load_qubicle_file(const char *filename, qsize *qs) {
	FILE *f = fopen(filename, "rb");
	if (f == NULL) {
		printf("failed to open %s\n", filename);
		return NULL;
	}
	qheader qh;
	size_t br = fread(&qh, sizeof(qheader), 1, f);
	if (br != 1) {
		printf("no bytes read from %s\n", filename);
		return NULL;
	}
	char name_len;
	char name[512];
	fread(&name_len, sizeof(char), 1, f);
	fread(name, (size_t)name_len, 1, f);

	qpos qp;
	br = fread(qs, sizeof(qsize), 1, f);
	br = fread(&qp, sizeof(qpos), 1, f);

	printf("version: %d\n", qh.version);
	printf("color_format: %d\n", qh.color_format);
	printf("z_axis_orientation: %d\n", qh.z_axis_orientation);
	printf("compressed: %d\n", qh.compressed);
	printf("visibility_mask_encoded: %d\n", qh.visibility_mask_encoded);
	printf("num_matrices: %d\n", qh.num_matrices);
	printf("name: %s\n", name);
	printf("size_x: %d\n", qs->size_x);
	printf("size_y: %d\n", qs->size_y);
	printf("size_z: %d\n", qs->size_z);
	printf("pos_x: %d\n", qp.pos_x);
	printf("pos_y: %d\n", qp.pos_y);
	printf("pos_z: %d\n", qp.pos_z);

	//printf("sizeof(qcolor) is %d\n", sizeof(qcolor));
	//int bytes_needed = sizeof(qcolor)*qs.size_x*qs.size_y*qs.size_z;
	//printf("bytes needed: %d\n", bytes_needed);
	qcolor *m = (qcolor *)malloc(sizeof(qcolor)*qs->size_x*qs->size_y*qs->size_z);
	for (int z=0; z<qs->size_z; z++) {
		for (int y=0; y<qs->size_y; y++) {
			for (int x=(qs->size_x-1); x>=0; x--) {
				int idx = (x + (y*qs->size_x) + (z*qs->size_x*qs->size_y));
				fread(&m[idx], sizeof(qcolor), 1, f);
				/*
				if (m[idx].a != 0) {
					printf("found at (%d,%d,%d): (%d,%d,%d,%d)\n", x, y, z, m[idx].r, m[idx].g, m[idx].b, m[idx].a);
				}
				*/
			}
		}
	}
	fclose(f);
	return m;
}

