#include <iostream>
#include "Screen.h"
#include "gl.h"

Screen* Screen::Instance()
{
	static Screen* screen = new Screen;
	return screen;
}

Screen::Screen()
{
	window = nullptr;
	context = nullptr;
}

/// <summary>
/// initializes the screen, including the display windlow's location and size
/// </summary>
/// <returns></returns>
bool Screen::Initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cout << "Error initializing SDL" << std::endl;
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	window = SDL_CreateWindow("Graphics Engine",
							  SDL_WINDOWPOS_UNDEFINED,
							  SDL_WINDOWPOS_UNDEFINED,
							  1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

	if (!window)
	{
		std::cout << "Error creating SDL window" << std::endl;
		return false;
	}

	context = SDL_GL_CreateContext(window);

	if (!context)
	{
		std::cout << "Error creating OpenGL context." << std::endl;
		return false;
	}

	if (!gladLoaderLoadGL())
	{
		std::cout << "Error loading extensions!" << std::endl;
	}

	ImGui::CreateContext();
	ImGui_ImplOpenGL3_Init("#version 460");
	ImGui_ImplSDL2_InitForOpenGL(window, context);

	return true;
}

void Screen::ClearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Screen::Present()
{
	SDL_GL_SwapWindow(window);
}

void Screen::Shutdown()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

