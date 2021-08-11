// g++ `pkg-config --cflags sdl2` sdl.cpp `pkg-config --libs sdl2` -lm; ./a.out

#include <SDL.h>
#include <math.h>

#define WIDTH 1280
#define HIDTH 960

struct Vec3
{
	double x;
	double y;
	double z;
};

void vecSubeq (struct Vec3 * a, const struct Vec3 * b)
{
	a->x -= b->x;
	a->y -= b->y;
	a->z -= b->z;
}

double vecMagSquare (const struct Vec3 * a)
{
	return a->x * a->x + a->y * a->y + a->z * a->z;
}

double vecMag (struct Vec3 * a)
{
	return sqrt (vecMagSquare (a));
}

void vecNormeq (struct Vec3 * a)
{
	double mag = vecMag (a);
	a->x /= mag;
	a->y /= mag;
	a->z /= mag;
}

Uint8 handle_event (SDL_Event * eventp, double * subp, Uint8 * textureChangedp)
{
	if (eventp->type == SDL_QUIT)
	{
		return 0;
	}
	return 1;
}

void dv_ellipse (Uint8 * collisionp, double * distp, struct Vec3 raypos, const struct Vec3 rayvec, const struct Vec3 pos, const struct Vec3 r)
{
	double a;
	double bota;
	double coa;
	double hit;
	vecSubeq (&raypos, &pos);
	a =
		rayvec.x * rayvec.x / r.x / r.x +
		rayvec.y * rayvec.y / r.y / r.y +
		rayvec.z * rayvec.z / r.z / r.z;
	bota = (
		raypos.x * rayvec.x / r.x / r.x +
		raypos.y * rayvec.y / r.y / r.y +
		raypos.z * rayvec.z / r.z / r.z
		) / a;
	coa = (
		raypos.x * raypos.x / r.x / r.x +
		raypos.y * raypos.y / r.y / r.y +
		raypos.z * raypos.z / r.z / r.z
		- 1
		) / a;
	hit = bota * bota - coa;
	if (hit >= 0)
	{
		*collisionp = 1;
		*distp = -(bota + sqrt (bota * bota - coa));
	}
	else
	{
		*collisionp = 0;
	}
}

void normal_ellipse (struct Vec3 * normal, const struct Vec3 * point, const struct Vec3 * center, const struct Vec3 * radius)
{
	struct Vec3 pointCenter = *point;
	vecSubeq (&pointCenter, radius);
	
}

Uint32 get_ray_color (struct Vec3 raypos, struct Vec3 rayvec)
{
	double mindist;
	double tempdist;
	Uint8 collision;
	Uint8 tempcollision;
	const struct Vec3 ellPos = {0, 0, 0};
	const struct Vec3 ellRad = {1, 1, 1};
	dv_ellipse (&tempcollision, &tempdist, raypos, rayvec, ellPos, ellRad);
	collision = tempcollision;
	mindist = tempdist;
	if (collision)
		return 0xff0000ff;
	else
		return 0xffffffff;
}

Uint32 get_pixel_color (int u, int v)
{
	struct Vec3 raypos = {0, 0, -5};
	struct Vec3 rayvec;
	rayvec.x = (double) (u - WIDTH / 2) / (double) WIDTH;
	rayvec.y = (double) (v - HIDTH / 2) / (double) WIDTH;
	rayvec.z = 1;
	vecNormeq (&rayvec);
	return get_ray_color (raypos, rayvec);
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
			myPixels [i] = get_pixel_color (x, y);
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
	double sub = 300000;
	SDL_Event event;
	Uint8 keepGoing = 1;
	Uint8 textureChanged = 1;
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
		while (SDL_PollEvent (&event))
			keepGoing &= handle_event (&event, &sub, &textureChanged);
		if (textureChanged)
			update_texture (myPixels, renderer, texture, sub);
		SDL_Delay (60);
	}
	SDL_Quit ();
	return 0;
}
