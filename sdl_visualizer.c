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

unsigned char poll_events()
{
	SDL_Event e;

	while( SDL_PollEvent( &e ) != 0 )
	{
		if( e.type == SDL_QUIT )
		{
			return 1;
		}
	}

	return 0;
}

int main( int argc, char* args[] )
{
	unsigned char quit = 0;
	Uint32 last = 0, now;
	float fps = 0, gps = 0;
	char title[100] = "WireWorld (Generation: 0; FPS: 0)";
	unsigned long long int last_gen = 0;
	
	init_sdl();	
	
	createSurface((void *) cells, WIDTH, HEIGHT);
	
	init();
	
	while( !quit )
	{
		//iterate automaton
		iterate(1);
		
		now = SDL_GetTicks();
		if ((now-last) >= 16)
		{
			//drawing
			SDL_BlitScaled(SDL_ConvertSurfaceFormat(cell_surface, screen_surface->format->format, 0), NULL, screen_surface, NULL );
			SDL_UpdateWindowSurface(window);

			//FPS and GPS
			fps = (float)1000/(now-last);
			gps = fps * (current_gen-last_gen);

			//title
			sprintf(title, "WireWorld (Generation: %llu; GPS: %.2f; FPS: %.2f)", current_gen, gps, fps);
			SDL_SetWindowTitle(window, title);

			//events
			quit = poll_events();

			//update last
			last = now;
			last_gen = current_gen;
		}

		//prevent software from going too fast and hogging CPU power
		//SDL_Delay(1);
	}

	close_sdl();

	return 0;
}