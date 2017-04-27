#ifndef COLORS_H
#define COLORS_H

#if defined __cplusplus
extern "C" {
#endif

typedef struct {
	float r;       // a fraction between 0 and 1
	float g;       // a fraction between 0 and 1
	float b;       // a fraction between 0 and 1
} rgb;

typedef struct {
	float h;       // angle in degrees
	float s;       // a fraction between 0 and 1
	float v;       // a fraction between 0 and 1
} hsv;

hsv rgb2hsv(rgb in);
rgb hsv2rgb(hsv in);

#ifdef __cplusplus
}
#endif

#endif //COLORS_H
