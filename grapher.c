// gcc `pkg-config --cflags sdl2` grapher.c `pkg-config --libs sdl2` -lm; ./a.out

#include <SDL.h>
#include <math.h>

#define WIDTH 1280
#define HIDTH 960

Uint8 handle_event (SDL_Event * eventp, double * subp, Uint8 * textureChangedp)
{
	if (eventp->type == SDL_QUIT)
	{
		return 0;
	}
	else if (eventp->type == SDL_KEYDOWN)
	{
		if (eventp->key.keysym.sym == SDLK_f)
		{
			*subp += 1;
			*textureChangedp = 1;
		}
		else if (eventp->key.keysym.sym == SDLK_s)
		{
			*subp -= 1;
			*textureChangedp = 1;
		}
	}
	return 1;
}

void update_texture (Uint32 * myPixels, SDL_Renderer * renderer, SDL_Texture * texture, double sub)
{
	int i;
	double x, y;
	Uint8 val;
	double vald;
	i = 0;
	for (y = 0; y < HIDTH; ++y)
	{
		for (x = 0; x < WIDTH; ++x)
		{
			vald =  ( sqrt(pow (x - 100, 2) + pow (y - 100, 2))
				+ sqrt(pow (x - 500, 2) + pow (y - 300, 2))
				+ sqrt(pow (x - 300, 2) + pow (y - 600, 2))
				) - sub;
			val = vald < 0 ? 0 : 255;//vald > 255 ? 0 : 255 - (Uint8) vald;
			myPixels [i] = 0xff000000 | (val << 16) | (val << 8) | val;
			++i;
		}
	}
	SDL_UpdateTexture (texture, NULL, myPixels, WIDTH * sizeof (Uint32));
	SDL_RenderClear (renderer);
	SDL_RenderCopy (renderer, texture, NULL, NULL);
	SDL_RenderPresent (renderer);
}

int main (int argc, char **argv)
{
	SDL_Window * screen;
	SDL_Renderer * renderer;
	SDL_Texture * texture;
	Uint32 myPixels [WIDTH*HIDTH];
	double sub = 800;
	SDL_Event event;
	Uint8 keepGoing = 1;
	Uint8 textureChanged = 1;
	int c_event = 0;
	if (SDL_Init (SDL_INIT_EVERYTHING) < 0)
		return 1;
	screen = SDL_CreateWindow (
		"hi",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH, HIDTH, 0);
	renderer = SDL_CreateRenderer (screen, -1, 0);
	texture = SDL_CreateTexture (
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WIDTH, HIDTH);
	while (keepGoing)
	{
		if (textureChanged)
		{
			printf ("updated texture\n");
			update_texture (myPixels, renderer, texture, sub);
			textureChanged = 0;
		}
		// if there are events in the queue, then handle them
		// (these events are due to the update_texture taking a long time to run, presumably because it takes a long time to calculate the texture)
		// c_event is the number of polled events.  it could be a bool, but in case i want to use it as a count someday, it's already a count.  an int should be able to hold the count, right ?
		// but in case it doesn't, it is decremented, and a message is displayed telling you that you've done something wrong
		// "no" bugs here uwu
		while (SDL_PollEvent (&event))
		{
			keepGoing &= handle_event (&event, &sub, &textureChanged);
			++c_event;
			if (c_event == 0)
			{
				printf ("what are you doing with your life\n");
				--c_event;
			}
		}
		// if not, then wait for an event
		// this is so that, when there are no events, it will get stuck on WaitEvent instead of a PollEvent loop
		// and if there were events, it will also go here if the program has not been exited and the texture was not changed
		// which is not necessary may cause problems in a program with more types of events
		if (c_event == 0 || (keepGoing && !textureChanged))
		{
			SDL_WaitEvent (&event);
			keepGoing &= handle_event (&event, &sub, &textureChanged);
		}
		c_event = 0;
	}
	SDL_Quit ();
	return 0;
}
