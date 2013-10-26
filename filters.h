#ifndef FILTERS_H
#define FILTERS_H
#include "SDL/SDL.h"
typedef Uint32 (*filter_callback)(SDL_PixelFormat *, Uint32, void *);

// filters to invert r g b
Uint32 ired_filter(SDL_PixelFormat * format, Uint32 pixel, void * args);
Uint32 igreen_filter(SDL_PixelFormat * format, Uint32 pixel, void * args);
Uint32 iblue_filter(SDL_PixelFormat * format, Uint32 pixel, void * args);

// filter to simulate colour blindness
Uint32 red_green_filter(SDL_PixelFormat * format, Uint32 pixel, void * args);

Uint32 help_filter(SDL_PixelFormat * format, Uint32 pixel, void * args);

// normal colour vision
Uint32 colour_vision_filter(SDL_PixelFormat * format, Uint32 pixel, void * args);

/* uses a matrix to transform rgb
matrix is an int array of length 9.
each entry of the matrix should be scaled up 1000x.
*/
Uint32 matrix_filter(SDL_PixelFormat * format, Uint32 pixel, void * args);
#endif