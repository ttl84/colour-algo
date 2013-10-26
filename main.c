#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "colour_name.h"
#include "filters.h"
#define SCREEN_W (320)
#define SCREEN_H (320)
static int running = 1;
static int mousedown;
static unsigned mouse_x;
static unsigned mouse_y;
static int keydown;
static int key;
void event_handle(void)
{
	keydown = 0;
	mousedown = 0;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if(e.type == SDL_QUIT)
			running = 0;
		else if(e.type == SDL_MOUSEBUTTONDOWN)
		{
			mousedown = 1;
			mouse_x = e.button.x;
			mouse_y = e.button.y;
		}
		else if(e.type == SDL_KEYDOWN)
		{
			keydown = 1;
			switch(e.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					running = 0;
					break;
				
				default:
					key = e.key.keysym.sym;
					break;
			}
		}
		else if(e.type == SDL_KEYUP)
		{
			switch(e.key.keysym.sym)
			{
				default:
					break;
			}
		}
	}
}
SDL_Surface * img_load(char const * file)
{
	SDL_Surface * pic = IMG_Load(file);
	if(pic != NULL)
	{
		SDL_Surface * optimized = SDL_DisplayFormat(pic);
		if(optimized != NULL)
		{
			SDL_FreeSurface(pic);
			pic = optimized;
		}
	}
	return pic;
}
void blit(SDL_Surface * dst, SDL_Surface * src, SDL_Rect * clip, int x, int y)
{
	SDL_Rect pos = {.x = x, .y = y};
	SDL_BlitSurface(src, clip, dst, &pos);
}
unsigned pixel_diff(SDL_PixelFormat * format, Uint32 p1, Uint32 p2)
{
	Uint8 r1, g1, b1, r2, g2, b2;
	SDL_GetRGB(p1, format, &r1, &g1, &b1);
	SDL_GetRGB(p2, format, &r2, &g2, &b2);
	Uint8 dr, dg, db;
	dr = (r1 > r2 ? r1 - r2 : r2 - r1);
	dg = (g1 > g2 ? g1 - g2 : g2 - g1);
	db = (b1 > b2 ? b1 - b2 : b2 - b1);
	return dr + dg + db;
}
void pixel_set(SDL_Surface * dst, unsigned x, unsigned y, Uint32 pix)
{
	Uint32 * pixels = dst->pixels;
	pixels[y * dst->w + x] = pix;
}
Uint32 pixel_get(SDL_Surface * dst, unsigned x, unsigned y)
{
	Uint32 * pixels = dst->pixels;
	return pixels[y * dst->w + x];
}
Uint32 region_rgb(SDL_Surface * dst, unsigned x, unsigned y, unsigned tolerance)
{
	if(x >= dst->w || y >= dst->h)
		return 0;

	// breadth first search
	unsigned bufsiz = dst->w * dst->h;
	struct pos{
		unsigned x, y;
	} * queue = calloc(bufsiz, sizeof *queue);
	unsigned head = 0;
	unsigned len = 0;
	unsigned max_len = 0;
	char * visited = calloc(dst->w * dst->h, 1);

	// sums of r, g and b
	unsigned rt = 0, gt = 0, bt = 0, iter = 0;
	
	// starting pixel
	Uint32 origin = pixel_get(dst, x, y);

	queue[head].x = x;
	queue[head].y = y;
	len++;

	SDL_LockSurface(dst);
	while(len > 0)
	{
		struct pos current = queue[head];
		head = (head + 1) % bufsiz;
		len--;

		unsigned x = current.x;
		unsigned y = current.y;

	// skip this pixel if it has been visited or its colour is too different
		if(visited[y * dst->w + x] ||
		pixel_diff(dst->format, pixel_get(dst, x, y), origin) > tolerance)
			continue;

	// otherwise add neighbors to queue
		struct pos nei[] = {
			{x + 1, y}, {x - 1, y}, {x, y + 1}, {x, y - 1}
		};
		for(int i = 0; i < 4; i++)
		{
			unsigned x2 = nei[i].x;
			unsigned y2 = nei[i].y;
			if(x2 < dst->w && y2 < dst->h &&
			!visited[y2 * dst->w + x2])
			{
				queue[(head + len) % bufsiz] = nei[i];
				len++;
			}
			
		}
		// do work on pixel: add rgb to their sums
		{
			Uint8 r, g, b;
			SDL_GetRGB(pixel_get(dst, x, y), dst->format, &r, &g, &b);
			rt += r;
			gt += g;
			bt += b;
			iter++;
		}
		visited[y * dst->w + x] = 1;
		if(len > max_len)
			max_len = len;
	}
	SDL_UnlockSurface(dst);
	free(visited);
	free(queue);
	assert(max_len < bufsiz);

	// then return an average pixel
	return SDL_MapRGB(dst->format, rt / iter, gt / iter, bt / iter);
}
void colour_filter(
	SDL_Surface * dst, SDL_Rect * clip,
	filter_callback convert, void * args)
{
	unsigned minx, miny, maxx, maxy;
	if(clip != NULL)
	{
		minx = clip->x > 0 ? clip->x : 0;
		miny = clip->y > 0 ? clip->y : 0;
		maxx = clip->x + clip->w;
		maxx = maxx > dst->w ? dst->w : maxx;
		maxy = clip->y + clip->h;
		maxy = maxy > dst->h ? dst->h : maxy;
	}
	else
	{
		minx = 0;
		miny = 0;
		maxx = dst->w;
		maxy = dst->h;
	}
	SDL_LockSurface(dst);
	for(unsigned y = miny; y < maxy; y++)
	for(unsigned x = minx; x < maxx; x++)
	{

		Uint32 pix = pixel_get(dst, x, y);
		pix = (*convert)(dst->format, pix, args);
		pixel_set(dst, x, y, pix);
	}
	SDL_UnlockSurface(dst);
}




// transformation matrix for rgb, in thousandths
int red_green_matrix[3][3] = {
	{560, 440, 0},
	{550, 450, 0},
	{0, 240, 750}
};
int help_matrix[3][3] = {
	{1000, 0, 500},
	{0, 1000, 0},
	{0, 0, 1000}
};
int colour_matrix[3][3] = {
	{1000, 00, 0},
	{00, 1000, 0},
	{0, 0, 1000}
};
int main(int argc, char ** argv)
{
	char * filename = "spectrum2.jpg";
	if(argc == 2)
		filename = argv[1];
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_Surface* screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32, SDL_SWSURFACE );
	
	SDL_Surface * pic = img_load(filename);
	screen = SDL_SetVideoMode(pic->w + 10, pic->h, 32, SDL_SWSURFACE);
	
	SDL_Rect pic_area = {
		.x = 0, .y = 0,
		.w = pic->w, .h = pic->h
	};
	SDL_Rect stripe_area = {
		.x = pic->w, .y = 0,
		.w = 10, .h = pic->h
	};
	
	unsigned tick = 0;
	unsigned dt = 0;
	unsigned avdt = 0;
	
	char const * col_name = "";
	int filter_i = 0;
	int colour_blind = 0;
	while(running)
	{
		event_handle();
		if(mousedown)
		{
			Uint32 avg = region_rgb(pic, mouse_x, mouse_y, 0);
			SDL_FillRect(screen, &stripe_area, avg);
			Uint8 r, g, b;
			SDL_GetRGB(avg, pic->format, &r, &g, &b);
			col_name = colour_string(colour_name(r, g, b));
		}
		blit(screen, pic, NULL, 0, 0);
		if(keydown)
		{
			if(key == SDLK_DOWN)
				filter_i++;
			if(key == SDLK_SPACE)
				colour_blind ^= 1;
		}
		switch(filter_i)
		{
		case 1:
			colour_filter(screen, &pic_area, &ired_filter, NULL);
			break;
		case 2:
			colour_filter(screen, &pic_area, &igreen_filter, NULL);
			break;
		case 3:
			colour_filter(screen, &pic_area, &iblue_filter, NULL);
			break;
		case 4:
			colour_filter(screen, &pic_area, &ired_filter, NULL);
			colour_filter(screen, &pic_area, &igreen_filter, NULL);
			break;
		case 5:
			colour_filter(screen, &pic_area, &ired_filter, NULL);
			colour_filter(screen, &pic_area, &iblue_filter, NULL);
			break;
		case 6:
			colour_filter(screen, &pic_area, &igreen_filter, NULL);
			colour_filter(screen, &pic_area, &iblue_filter, NULL);
			break;
		case 7:
			colour_filter(screen, &pic_area, &ired_filter, NULL);
			colour_filter(screen, &pic_area, &igreen_filter, NULL);
			colour_filter(screen, &pic_area, &iblue_filter, NULL);
			break;
		case 8:
			colour_filter(screen, &pic_area, &help_filter, NULL);
			break;
		default:
			filter_i = 0;
		}
		if(colour_blind)
			colour_filter(screen, &pic_area, &matrix_filter, red_green_matrix);
		SDL_Flip(screen);
		unsigned end = SDL_GetTicks();
		dt = end - tick;
		tick = end;
		avdt = (avdt * 3 + dt) / 4;
		
		char title[200];
		snprintf(title, sizeof title, "%s | mspf:%d",
				col_name,
				avdt);
		SDL_WM_SetCaption(title, NULL);
		
	}

	SDL_Quit();
	return 0;
}
