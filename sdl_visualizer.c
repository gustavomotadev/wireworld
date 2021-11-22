#include <SDL2/SDL.h>
#include <stdio.h>
#include "wireworld.c"
#define MIN_SCREEN_WIDTH 600
#define MIN_SCREEN_HEIGHT 600

void init_sdl();

void close_sdl();

void createSurface(void * pixels, unsigned int width, unsigned int height);

SDL_Window * window = NULL;
	
SDL_Surface * screen_surface = NULL;

SDL_Surface * cell_surface = NULL;

void init_sdl(unsigned int width, unsigned int height)
{
	if (width < 256 || height < 256)
	{
		width = MIN_SCREEN_WIDTH;
		height = MIN_SCREEN_HEIGHT;
	}
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("WireWorld", SDL_WINDOWPOS_UNDEFINED, 
	SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
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

	if (argc != 2) 
    {
        printf("Wrong number of arguments, pass filename.");
        return -1;
    }

	//allocate_and_fill(".\\csv\\diode.csv");
	allocate_and_fill(args[1]);

	init_automaton();

	init_sdl(board_width, board_height);
	
	createSurface((void *) cells, board_width, board_height);

	//initial iterations if necessary
	//iterate(1);
	
	while( !quit )
	{
		//iterate automaton (independent from frame)
		//iterate(1);
		
		now = SDL_GetTicks();
		if ((now-last) >= 100) //frametime
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

			//iterate on frame (to slow down)
			iterate(1);
		}
	}

	close_sdl();

	return 0;
}