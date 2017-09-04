#include "example_common.h"
#include "example_port_audio.h"
#include "wav.h"
#include "portaudio.h"
#include "easing.h"
#include "window.h"
#include "render_util.h"
#include "game_util.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct {
	unsigned int num_beats;
	unsigned int num_samples;
	unsigned int real_samples;
	unsigned int offset;
	unsigned int cur_beat;
	unsigned int beat_offset;
	float samples_per_beat;
	float *beat_vals;
} pb_data;

int16_t *wav_data;

static float get_mult(pb_data *meta) {
	float cur_beat_exact = (float)(meta->offset / 2) / (meta->samples_per_beat);
	unsigned int cur_beat = (unsigned int)cur_beat_exact;
	float beat_frac = cur_beat_exact - cur_beat;
	unsigned int prev_beat = (((int)cur_beat - 1) < 0) ? meta->num_beats-1 : cur_beat-1;
	unsigned int next_beat = ((cur_beat + 1) == meta->num_beats) ? 0 : cur_beat+1;
	float mult;
	if (beat_frac < 0.5f) {
		float ri = SineEaseOut(0.5f + beat_frac);
		mult = meta->beat_vals[prev_beat] + (ri * (meta->beat_vals[cur_beat] - meta->beat_vals[prev_beat]));
	}  else {
		float ri = SineEaseIn(beat_frac - 0.5f);
		mult = meta->beat_vals[cur_beat] + (ri * (meta->beat_vals[next_beat] - meta->beat_vals[cur_beat]));
	}
	return mult;
}

static int pa_callback(const void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void *userData)
{
	/* Cast data passed through stream to our structure. */
	pb_data *meta = (pb_data *)userData;
	float *out = (float*)outputBuffer;
	unsigned int i;
	(void) inputBuffer; /* Prevent unused variable warning. */

	for( i=0; i<framesPerBuffer; i++ )
	{
		float mult = get_mult(meta);
		if ((meta->offset / 2) >= meta->num_samples) {
			*out++ = 0.0f;
			*out++ = 0.0f;
		} else {
			*out++ = ((float)wav_data[meta->offset] / (float)INT16_MAX) * mult;
			*out++ = ((float)wav_data[meta->offset+1] / (float)INT16_MAX) * mult;
		}
		meta->offset += 2;
		if ((meta->offset / 2) >= meta->real_samples) {
			meta->offset = 0;
		}
	}
	return 0;
}

void run_port() {
	font_kern = 1;
	unsigned int num_samples = 0;
	wav_data = load_wav("/Users/dmk/code/pectin/images/Untitled2.wav", &num_samples);
	printf("-=-= number of samples is %u\n", num_samples);
	float sps = 44100.0f;
	float bpm = 454;
	float seconds = (float)num_samples / sps;
	float minutes = seconds / 60.0f;
	unsigned int beats = (unsigned int)(roundf(minutes * bpm));
	float seconds_per_beat = 1.0f / (bpm / 60.0f);
	float samples_per_beat = (seconds_per_beat * sps);
	unsigned int real_samples = (unsigned int)roundf(samples_per_beat * (float)beats);

	printf("-=-= bpm: %f\n", bpm);
	printf("-=-= seconds: %f\n", seconds);
	printf("-=-= minutes: %f\n", minutes);
	printf("-=-= beats: %u\n", beats);
	printf("-=-= seconds per beat: %f\n", seconds_per_beat);
	printf("-=-= samples per beat: %f\n", samples_per_beat);
	printf("-=-= real samples: %u\n", real_samples);

	float *beat_vals = (float *)malloc(beats * sizeof(float));
	for (int i=0; i<beats; i++) {
		beat_vals[i] = 0.2f;//(float)(i % 2);
	}
	beat_vals[0] = 1.0f;
	beat_vals[4] = 1.0f;
	beat_vals[8] = 1.0f;
	beat_vals[10] = 1.0f;
	beat_vals[14] = 1.0f;
	beat_vals[18] = 1.0f;
	beat_vals[22] = 1.0f;
	beat_vals[24] = 1.0f;
	beat_vals[28] = 1.0f;

	beat_vals[32] = 1.0f;
	beat_vals[36] = 1.0f;
	beat_vals[40] = 1.0f;
	beat_vals[42] = 1.0f;
	beat_vals[46] = 1.0f;
	beat_vals[50] = 1.0f;
	beat_vals[54] = 1.0f;
	beat_vals[56] = 1.0f;
	beat_vals[60] = 1.0f;

	pb_data port_data = { beats, num_samples, real_samples, 0, 0, 0, samples_per_beat, beat_vals };
	pb_data mult_data = { beats, num_samples, real_samples, 0, 0, 0, samples_per_beat, beat_vals };

	timespec t1, t2;
	bool loop = true;

	// keyboard event variables
	bool kdown[NUM_KEYS];
	bool kpress[NUM_KEYS];
	for (int i=0; i<NUM_KEYS; i++) {
		kdown[i] = false;
		kpress[i] = false;
	}
	int key_map[NUM_KEYS];
	set_default_key_map(key_map);
	mouse_input mouse;
	float wmx = 0.0f;
	float wmy = 0.0f;

	// initialize the screen view
	int pix_per_unit = 32;
	screen_def sd;
	init_screen(&sd, SCREEN_W, SCREEN_H, pix_per_unit, true);
	print_screen_def(&sd);

	PaError paerr = Pa_Initialize();
	if( paerr != paNoError ) {
		printf("-=-= PortAudio init error: %s\n", Pa_GetErrorText(paerr));
	}
	PaStream *stream;
	paerr = Pa_OpenDefaultStream( &stream, 0, 2, paFloat32, 44100, 256, pa_callback, &port_data );
	if( paerr != paNoError ) {
		printf("-=-= PortAudio open stream error: %s\n", Pa_GetErrorText(paerr));
	}
	paerr = Pa_StartStream( stream );
	if( paerr != paNoError ) {
		printf("-=-= PortAudio start stream error: %s\n", Pa_GetErrorText(paerr));
	}

	const char *config_filename = "/Users/dmk/code/pectin/config/port_render_defs.cfg";
	// set up rendering for lines
	render_def *line_def = load_render_def(config_filename, "lines", (GLfloat *)&sd.vp_mat, NULL, NULL);
	render_def *rect_def = load_render_def(config_filename, "rects", (GLfloat *)&sd.vp_mat, NULL, NULL);
	render_def *wave_def = load_render_def(config_filename, "wave", (GLfloat *)&sd.vp_mat, NULL, NULL);
	// set up rendering for font
	render_def *font_def = load_render_def(config_filename, "font", (GLfloat *)&sd.vp_mat, NULL, NULL);

	line lnb = { {1, 15, 0, 0.5f, 0.5f, 0.5f}, {31, 19, 0, 0.5f, 0.5f, 0.5f} };
	line lbb = { {0, 12.9f, 0, 0.5f, 0.5f, 0.5f}, {0, 14.9f, 0, 0.5f, 0.5f, 0.5f} };
	line_box lb = { 1, 15, 31, 19, 0, 0.7f, 0.7f, 0.7f };
	line_box bb = { 0, 12.9f, 0, 14.9f, 0, 1.0f, 1.0f, 1.0f };
	float space_w = 0.08;
	float beat_w = ((30.0f - (space_w * (float)(beats - 1))) / (float)beats);
	printf("-=-= beat_w is %f\n", beat_w);

	float samples_per_pix = num_samples / ((float)pix_per_unit * 30.0f);
	float units_per_pix = (float)pix_per_unit / (float)SCREEN_W;
	printf("samples per pixel: %f\n", samples_per_pix);

	srect *btns = (srect *)malloc(beats * sizeof(srect));
	float bbx = 1.0f;
	for (unsigned int i=0; i<beats; i++) {
		btns[i].x1 = bbx;
		btns[i].y1 = 12.9f;
		btns[i].x2 = bbx + beat_w;
		btns[i].y2 = 14.9f;
		bbx += (beat_w + space_w);
	}

	//const char *txt = "This is an [example] of some {text}. Who knows if it has mojo?";
	const char *txt = "jitterbox v0.9";
	float txt_x = 1.0f;
	float txt_y = 19.0f;
	int txt_len = (int)strlen(txt);

	printf("sizeof(line) is %d\n", (int)sizeof(line));
	line *project = (line *)malloc(20 * sizeof(line));

	sprite *s = (sprite *)malloc(sizeof(sprite));
	init_render_environment();

	int advance = 0;
	float tx = txt_x;
	for (int i=0; i<txt_len; i++) {
		int ch = txt[i] - 32;
		int trow = ch / 24;
		int tcol = ch % 24;
		int back = font_back[ch];
		float kern = (((float)font_kern / 8.0f) * font_def->xscale);
		tx += (((float)advance / 8.0f) * font_def->xscale);
		tx += kern;
		if (back < 0) tx += (((float)back / 8.0f) * font_def->xscale);
		sprite_for(tx, txt_y, 0, trow, tcol, s, font_def);
		render_sprite(font_def, s);
		advance = font_space[ch];
	}
	float wmin = 0.0f;
	float wmax = 0.0f;
	int scnt = 0;
	line wln = { {1, 15, 0, 0.8f, 0.8f, 0.8f}, {31, 19, 0, 0.8f, 0.8f, 0.8f} };
	float wx = 1.0f;
	float lx = 1.0f;
	float ly = 0.0f;
	for (int i=0; i<num_samples; i++) {
		float samp = ((float)wav_data[i*2] / (float)INT16_MAX);
		if (samp < wmin) wmin = samp;
		if (samp > wmax) wmax = samp;
		if (i >= (int)roundf((float)(scnt+1)*samples_per_pix)) {
			wln.p1.x = wx;
			wln.p1.y = 17.0f + (wmin * 2.0f);
			wln.p2.x = wx;
			wln.p2.y = 17.0f + (wmax * 2.0f);
			render_line(wave_def, &wln);
			wmin = 0.0f;
			wmax = 0.0f;
			scnt++;
			wx += units_per_pix;

			mult_data.offset = (unsigned int)i * 2;
			float mult = get_mult(&mult_data);
			wln.p1.x = lx;
			wln.p1.y = 10.5f + (ly * 2.0f);
			wln.p2.x = wx;
			wln.p2.y = 10.5f + (mult * 2.0f);
			render_line(wave_def, &wln);
			lx = wx;
			ly = mult;
		}
	}

	clock_gettime(CLOCK_REALTIME, &t1);
	int frame = 0;
	while (loop) {
		get_input(kdown, kpress, key_map, &mouse);
		if (kpress[KEY_QUIT]) {
			loop = false;
		}
		clock_gettime(CLOCK_REALTIME, &t2);
		double dt = time_diff_d(t1, t2);

		if (mouse.ldown) {
			mouse_to_world(&sd, mouse.x, mouse.y, &wmx, &wmy);
			for (unsigned int i=0; i<beats; i++) {
				if (wmx >= btns[i].x1 && wmx <= btns[i].x2 && wmy >= btns[i].y1 && wmy <= btns[i].y2) {
					//printf("hit button %u\n", i);
					beat_vals[i] = (wmy - btns[i].y1) / (btns[i].y2 - btns[i].y1);
					break;
				}
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_advance(line_def);
		render_advance(rect_def);
		render_line(line_def, &lnb);
		for (unsigned int i=0; i<beats; i++) {
			lbb.p1.x = btns[i].x1;
			lbb.p1.y = btns[i].y1;
			lbb.p2.x = btns[i].x2;
			lbb.p2.y = btns[i].y2;

			lb.x1 = lbb.p1.x;
			lb.y1 = lbb.p1.y;
			lb.x2 = lbb.p2.x;
			lb.y2 = lbb.p1.y + (beat_vals[i] * (lbb.p2.y - lbb.p1.y));
			lb.z = 0;
			render_rect_box(rect_def, &lb);
			render_line(line_def, &lbb);
		}

		render_buffer(line_def);
		render_buffer(font_def);
		render_buffer(rect_def);
		render_buffer(wave_def);
		swap_window();
		frame += 1;
		if (frame == 60) frame = 0;
	}
	free(s);
	free_render_def(line_def);
	free_render_def(rect_def);
	free_render_def(font_def);
	free_render_def(wave_def);
	paerr = Pa_StopStream( stream );
	if( paerr != paNoError ) {
		printf("-=-= PortAudio stop stream error: %s\n", Pa_GetErrorText(paerr));
	}
	paerr = Pa_Terminate();
	if( paerr != paNoError ) {
		printf("-=-= PortAudio shutdown error: %s\n", Pa_GetErrorText(paerr));
	}
	free(wav_data);
	free(btns);
	free(beat_vals);
}

