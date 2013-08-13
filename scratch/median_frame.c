#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define NPIX 2000
#define NFRAMES 500

typedef unsigned char pixel_t;

void bad_alloc() {
    exit(-1);
}

pixel_t *get_next_frame() {
    static int state = 0;
    pixel_t *frame = malloc(NPIX*sizeof(pixel_t));
    if (frame == NULL) bad_alloc();
    memset(frame, state, NPIX*sizeof(pixel_t));
    state = (state + 1) % NFRAMES;
    return frame;
}

void test_guess(int *n_geq, pixel_t *guess, pixel_t *frame) {
    for (int i = 0; i < NPIX; i++) {
	n_geq[i] += (frame[i] >= guess[i]);
    }
}

void update_guess(int *n_geq, pixel_t *guess, int last_bit_pos) {
    pixel_t increase_mask = 0x01, decrease_mask = 0x3;
    increase_mask = (last_bit_pos) ? increase_mask << (last_bit_pos - 1) : 0x0;
    decrease_mask = (last_bit_pos) ? decrease_mask << (last_bit_pos - 1) : 0x1;
    for (int i = 0; i < NPIX; i++) {
	guess[i] ^= (n_geq[i] < (NFRAMES / 2 + 1)) ? decrease_mask : increase_mask;
    }
}

pixel_t *median_frame() {
    unsigned char startval = 128;
    pixel_t *guess = malloc(NPIX*sizeof(pixel_t));
    int *n_geq = malloc(NPIX*sizeof(int));
    if (guess == NULL || n_geq == NULL) bad_alloc();
    memset(guess, startval, NPIX);
    for (int bit_pos = 7; bit_pos >= 0; bit_pos--) {
	memset(n_geq, 0, NPIX*sizeof(int));
	for (int f = 0; f < NFRAMES; f++) {
	    pixel_t *frame = get_next_frame();
	    test_guess(n_geq, guess, frame);
	    free(frame);
	}
	update_guess(n_geq, guess, bit_pos);
    }
    free(n_geq);
    return guess;
}
	    
int main(int argc, char **argv) {
    pixel_t *median = median_frame();
    free(median);
}
