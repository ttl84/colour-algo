#include "filters.h"
Uint32 ired_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	return SDL_MapRGB(format, 255 - r, g, b);
}
Uint32 igreen_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	return SDL_MapRGB(format, r, 255 - g, b);
}
Uint32 iblue_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	return SDL_MapRGB(format, r, g, 255 - b);
}
Uint32 red_green_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	unsigned avg = (r + g) / 2;
	return SDL_MapRGB(format, avg, avg, b);
}
Uint32 help_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	unsigned B = b;
	B += r;
	if(B > 255)
		B = 255;
	b = B;
	return SDL_MapRGB(format, r, g, b);
}
Uint32 colour_vision_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	return pixel;
}
Uint32 matrix_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	int (*A)[3] = args;
	int X[3] = {r, g, b};
	int B[3];
	for(int y = 0; y < 3; y++)
	{
		int dot = 0;
		for(int x = 0; x < 3; x++)
			dot += A[y][x] * X[x];
		dot /= 1000;
		if(dot > 255)
			dot = 255;
		else if(dot < 0)
			dot = 0;
		B[y] = dot;
		
	}
	r = B[0];
	g = B[1];
	b = B[2];
	return SDL_MapRGB(format, r, g, b);
}
Uint32 order_rgb_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	if(r >= g && g >= b)
		return pixel;
	else
		return 0;
}
Uint32 order_rbg_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	if(r >= b && b >= g)
		return pixel;
	else
		return 0;
}Uint32 order_grb_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	if(g >= r && r >= b)
		return pixel;
	else
		return 0;
}Uint32 order_gbr_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	if(g >= b && b >= r)
		return pixel;
	else
		return 0;
}
Uint32 order_bgr_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	if(b >= g && g >= r)
		return pixel;
	else
		return 0;
}Uint32 order_brg_filter(SDL_PixelFormat * format, Uint32 pixel, void * args)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel, format, &r, &g, &b);
	if(b >= r && r >= g)
		return pixel;
	else
		return 0;
}