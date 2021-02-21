#pragma once

#include <SDL2/SDL.h>

struct window {
	window();

	window(const window &) = delete;
	window &operator=(const window &) = delete;

	~window();

	void swap();
private:
	SDL_Window *window_;
	SDL_GLContext context_;
};
