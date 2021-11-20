#include <SDL2/SDL.h>
#include <stdio.h>
#include "wireworld.c"
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

void init_sdl();

void close_sdl();

void createSurface(void * pixels, unsigned int width, unsigned int height);

SDL_Window * window = NULL;
	
SDL_Surface * screen_surface = NULL;

SDL_Surface * cell_surface = NULL;

void init_sdl()
{
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("WireWorld", SDL_WINDOWPOS_UNDEFINED, 
	SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	screen_surface = SDL_GetWindowSurface(window);
}

void close_sdl()
{
	SDL_FreeSurface(cell_surface);
	cell_surface = NULL;
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
}

void createSurface(void * pixels, unsigned int width, unsigned int height)
{
	int channels = 1;
	unsigned char res;

	SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(
		pixels,
		width,
		height,
		8,          			// bits per pixel
		width,  				// pitch
		0,             			// red mask
		0,             			// green mask
		0,             			// blue mask
		0);                    	// alpha mask (no alpha mask)
	
	surface->format->palette->colors[0b00000001].r = 0;
	surface->format->palette->colors[0b00000001].g = 0;
	surface->format->palette->colors[0b00000001].b = 255;
	surface->format->palette->colors[0b00100000].r = 255;
	surface->format->palette->colors[0b00100000].g = 0;
	surface->format->palette->colors[0b00100000].b = 0;
	surface->format->palette->colors[0b01000000].r = 255;
	surface->format->palette->colors[0b01000000].g = 255;
	surface->format->palette->colors[0b01000000].b = 0;
	surface->format->palette->colors[0b10000000].r = 0;
	surface->format->palette->colors[0b10000000].g = 0;
	surface->format->palette->colors[0b10000000].b = 0;
	
	cell_surface = surface;
}

int main( int argc, char* args[] )
{
	unsigned char quit = 0;
	Uint32 last, now;
	float fps = 0;
	
	init_sdl();	

	SDL_Event e;
	
	createSurface((void *) cells, WIDTH, HEIGHT);
	
	init();
	
	last = SDL_GetTicks();
	
	while( !quit )
	{
		while( SDL_PollEvent( &e ) != 0 )
		{
			if( e.type == SDL_QUIT )
			{
				quit = 1;
			}
		}
		
		//iterate automaton
		iterate(1);

		SDL_BlitScaled(SDL_ConvertSurfaceFormat(cell_surface, screen_surface->format->format, 0), NULL, screen_surface, NULL );
	
		SDL_UpdateWindowSurface( window );
		
		now = SDL_GetTicks();
		fps = (float)1000/(now-last);
		//printf("FPS = %f\n", fps);
		last = now;
		SDL_Delay(100);
		
		//printf(SDL_GetError());
	}

	close_sdl();

	return 0;
}