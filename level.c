#include "level.h"

#include <stdio.h>
#include <stdlib.h>

lev *load_level(const char *filename) {
	FILE *f = fopen(filename, "rb");
	if (f == NULL) {
		printf("failed to open %s\n", filename);
		return NULL;
	}
	lev *l = (lev *)malloc(sizeof(lev));
	size_t br = fread(&l->size_x, sizeof(uint32_t), 1, f);
	if (br != 1) {
		printf("error size_x read from %s\n", filename);
		fclose(f);
		return NULL;
	}
	br = fread(&l->size_y, sizeof(uint32_t), 1, f);
	if (br != 1) {
		printf("error size_y read from %s\n", filename);
		fclose(f);
		return NULL;
	}
	br = fread(&l->size_z, sizeof(uint32_t), 1, f);
	if (br != 1) {
		printf("error size_z read from %s\n", filename);
		fclose(f);
		return NULL;
	}
	int lev_size = l->size_x * l->size_y * l->size_z;
	l->vox = (int32_t *)malloc(sizeof(int32_t) * lev_size);
	for (int i=0; i<lev_size; i++) {
		br = fread(&l->vox[i], sizeof(int32_t), 1, f);
		if (br != 1) {
			printf("error vox read at %d from %s\n", i, filename);
			fclose(f);
			return NULL;
		}
	}
	fclose(f);
	return l;
}

void save_level(lev *l, const char *filename) {
	FILE *f = fopen(filename, "wb");
	if (f == NULL) {
		printf("failed to open %s\n", filename);
		return;
	}
	size_t br = fwrite(&l->size_x, sizeof(uint32_t), 1, f);
	if (br != 1) {
		printf("error size_x write to %s\n", filename);
		fclose(f);
		return;
	}
	br = fwrite(&l->size_y, sizeof(uint32_t), 1, f);
	if (br != 1) {
		printf("error size_y write to %s\n", filename);
		fclose(f);
		return;
	}
	br = fwrite(&l->size_z, sizeof(uint32_t), 1, f);
	if (br != 1) {
		printf("error size_z write to %s\n", filename);
		fclose(f);
		return;
	}
	int lev_size = l->size_x * l->size_y * l->size_z;
	for (int i=0; i<lev_size; i++) {
		br = fwrite(&l->vox[i], sizeof(int32_t), 1, f);
		if (br != 1) {
			printf("error vox write at %d to %s\n", i, filename);
			fclose(f);
			return;
		}
	}
	fclose(f);
}

void free_level(lev *l) {
	free(l->vox);
	free(l);
}

int32_t vox_at(lev *l, int x, int y, int z) {
	if (z == l->size_z) {
		return 0;
	}
	if (x < 0) {
		x += l->size_x;
	} else if (x >= l->size_x) {
		x -= l->size_x;
	}
	if (x<0 || y<0 || z<0 || x>=l->size_x || y>=l->size_y || z>=l->size_z) {
		return -1;
	}
	return l->vox[(x + (y*l->size_x) + (z*l->size_x*l->size_y))];
}

void set_vox_at(lev *l, int x, int y, int z, int32_t v) {
	if (x<0 || y<0 || z<0 || x>=l->size_x || y>=l->size_y || z>=l->size_z) {
		printf("not setting\n");
		return;
	}
	l->vox[(x + (y*l->size_x) + (z*l->size_x*l->size_y))] = v;
}

int has_thing(lev *l, int x, int y, int z, int samexy) {
	if (x < 0) {
		x += (int)l->size_x;
	} else if (x >= (int)l->size_x) {
		x -= (int)l->size_x;
	}
	if (y<0 || y>=l->size_y) {
		return 1;
	}
	if ((z < 0 || z>=l->size_z)) return 0;
	if (z < 0) z = 0;
	if (z>=l->size_z) z = l->size_z-1;
	int32_t  c = l->vox[(x + (y*l->size_x) + (z*l->size_x*l->size_y))];
	return (c < 0) ? 0 : 1;
}

unsigned int vox_surround(lev *l, int x, int y, int z) {
	unsigned int n = 0;
	int shft = 0;
	for (int qz=z-1; qz<z+2; qz++) {
		for (int qy=y-1; qy<y+2; qy++) {
			for (int qx=x-1; qx<x+2; qx++) {
				int samexy = (qx == x && qy == y);
				n = n | (has_thing(l, qx, qy, qz, samexy) << shft);
				shft++;
			}
		}
	}
	return n;
}
