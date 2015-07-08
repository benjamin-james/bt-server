#include <SDL2/SDL.h>
#include "input.h"
void init(struct input *i, const char *title)
{
	i->w = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	i->r = SDL_CreateRenderer(i->w, -1, 0);
	SDL_WarpMouseInWindow(i->w, 320, 240);
}

void reset(struct input *i)
{
	int width, height;
	SDL_GetWindowSize(i->w, &width, &height);
	SDL_SetRenderDrawColor(i->r, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(i->r);
	SDL_RenderPresent(i->r);
	SDL_WarpMouseInWindow(i->w, width / 2, height / 2);
	i->x = 0.0;
	i->y = 0.0;
	i->throttle = 1.0;
}
int windowevent(struct input *i, SDL_WindowEvent e)
{
	switch(e.type) {
	case SDL_WINDOWEVENT_ENTER:
		reset(i);
	case SDL_WINDOWEVENT_LEAVE:
		SDL_SetRenderDrawColor(i->r, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(i->r);
		SDL_RenderPresent(i->r);
		i->x = 0.0;
		i->y = 0.0;
		i->throttle = 1.0;
	case SDL_WINDOWEVENT_CLOSE:
		SDL_DestroyRenderer(i->r);
		SDL_DestroyWindow(i->w);
		SDL_Quit();
		return 1;
	default:
		break;
	}
	return 0;
}
	void mousemotion(struct input *i, int mx, int my, int maxx, int maxy)
	{
		int w, h;
		SDL_GetWindowSize(i->w, &w, &h);
		i->x = mx - maxx;
		i->y = my - maxy;
		i->x /= maxx;
		i->y /= maxy;
		SDL_SetRenderDrawColor(i->r, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(i->r);
		SDL_SetRenderDrawColor(i->r, 0xFF, 0, 0, 0xFF);
		SDL_RenderDrawLine(i->r, mx, my, w/2, h/2);
		SDL_RenderPresent(i->r);
	}
	void mousewheel(struct input *i, int wheel)
	{
		i->throttle += wheel;
	}
	int input(struct input *i)
	{
		SDL_Event event;
		int width, height;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_SPACE)
					reset(i);
				else if (event.key.keysym.sym == SDLK_ESCAPE) {
					SDL_DestroyRenderer(i->r);
					SDL_DestroyWindow(i->w);
					SDL_Quit();
					return 1;
				}
				break;
			case SDL_WINDOWEVENT:
				if (windowevent(i, event.window) == 1)
					return 1;
				break;
			case SDL_CONTROLLERAXISMOTION:
				break;
			case SDL_JOYAXISMOTION:
				break;
			case SDL_FINGERMOTION:
				break;
			case SDL_MOUSEMOTION:
				SDL_GetWindowSize(i->w, &width, &height);
				mousemotion(i, event.motion.x, event.motion.y, width / 2, height / 2);
				break;
			case SDL_MULTIGESTURE:
				break;
			case SDL_MOUSEWHEEL:
				mousewheel(i, event.wheel.y);
				break;
			case SDL_QUIT:
				SDL_DestroyRenderer(i->r);
				SDL_DestroyWindow(i->w);
				SDL_Quit();
				return 1;
			default:
				break;
			}
		}
		return 0;
	}
