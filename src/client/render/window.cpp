#include "window.hpp"
#include <cassert>

window::window() {
	bool init_succ = !SDL_Init(SDL_INIT_VIDEO);
	assert(init_succ);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window_ = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	assert(window_);

	context_ = SDL_GL_CreateContext(window_);

	SDL_GL_SetSwapInterval(0);
}

window::~window() {
	SDL_GL_DeleteContext(context_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

void window::swap() {
	SDL_GL_SwapWindow(window_);
}
