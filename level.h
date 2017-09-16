#ifndef PECTIN_LEVEL_H
#define PECTIN_LEVEL_H

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t size_x;
	uint32_t size_y;
	uint32_t size_z;
	int32_t *vox;
} lev;

lev *load_level(const char *filename);
void save_level(lev *l, const char *filename);
void free_level(lev *l);
int32_t vox_at(lev *l, int x, int y, int z);
void set_vox_at(lev *l, int x, int y, int z, int32_t v);
unsigned int vox_surround(lev *l, int x, int y, int z);

#ifdef __cplusplus
}
#endif

#endif //PECTIN_LEVEL_H
