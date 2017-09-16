#ifndef UTIL_H
#define UTIL_H

#define TWO_PI (2.0f * M_PI)
#define ONE_DEG_IN_RAD  (2.0f * M_PI) / 360.0f

#if defined __cplusplus
extern "C" {
#endif

const char* load_file(const char *input_file_name);
float rand_float();
unsigned char rand_ubyte(int mod);
int rand_int(int mod);
unsigned int rand_uint(unsigned int mod);
float oscillate(float val, float min, float range);
float elastic(float p);
float signum(float x);
float intbound(float s, float ds);

#ifdef __cplusplus
}
#endif

#endif //UTIL_H
