#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
struct input {
	SDL_Window *w;
	SDL_Renderer *r;
	double x, y, throttle;
};

void init(struct input *i, const char *windowtitle);
void reset(struct input *i);
int input(struct input *i);
#endif
