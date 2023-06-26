#pragma once

#include <SDL.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl2.h"

class Screen
{

public:

	static Screen* Instance();

	bool Initialize();
	void ClearScreen();
	void Present();
	void Shutdown();

private:

	Screen();
	Screen(const Screen&);

	SDL_Window* window;
	SDL_GLContext context;

};