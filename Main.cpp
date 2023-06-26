#include <iostream>
#include <SDL.h>
#include "Screen.h"
#include "gl.h"
#include "Shader.h"
#include "Quad.h"
#include "Camera.h"
#include "FileDialog.h"

bool isAppRunning = true;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int PROPERTIES_WINDOW_WIDTH = 400;

/// <summary>
/// renders the properties window, updating the properties of the quad according the input by the user
/// </summary>
/// <param name="quad">the quad contaning the image texture</param>
void RenderPropertiesWindow(Quad& quad)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Properties", nullptr,
		ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);

	auto windowPos = ImVec2(SCREEN_WIDTH - PROPERTIES_WINDOW_WIDTH, 0);
	auto windowSize = ImVec2(PROPERTIES_WINDOW_WIDTH, SCREEN_HEIGHT);

	ImGui::SetWindowPos("Properties", windowPos);
	ImGui::SetWindowSize("Properties", windowSize);

	static bool imageLoaded = false;
	static bool isInvert = false;
	static float blurPercent = 0.0f;


	//buttons for loading and saving images//////////////////////////////////////
	if (ImGui::Button("Load new image"))
	{
		char filename[MAX_PATH];
		if (OpenFileDialog(filename) >= 0)
		{
			if (AcceptibleFormat(filename))
			{
				imageLoaded = true;
				isInvert = false;
				blurPercent = 0.0f;
				quad.LoadNewTexture(std::string(filename));
			}
			else
			{
				MessageBoxW(NULL, L"File type not supported", L"File Path", MB_OK);
			}
		}
		
	}

	if (ImGui::Button("Save image without effects"))
	{
		if (imageLoaded)
		{
			char filename[MAX_PATH];
			if (SaveFileDialog(filename) >= 0)
			{
				quad.SaveTextureImage(std::string(filename));
			}
		}
	}

	if (ImGui::Button("Save image with effects"))
	{
		if (imageLoaded)
		{
			char filename[MAX_PATH];
			if (SaveFileDialog(filename) >= 0)
			{
				quad.SaveTextureImageWithEffects(filename);
			}
		}
	}

	ImGui::Separator();
	//sliders for controling the position, rotation and scale of the images
	auto position = quad.GetPosition(); 
	if (ImGui::SliderFloat3("Position", &position.x, -10.0f, 10.0f, "%.2f"))
	{
		quad.SetPosition(position.x, position.y, position.z);
	}

	auto rotation = quad.GetRotation();
	if (ImGui::SliderFloat3("Rotation", &rotation.x, -360.0f, 360.0f, "%.2f"))
	{
		quad.SetRotation(rotation.x, rotation.y, rotation.z);
	}

	auto scale = quad.GetScale();
	if (ImGui::SliderFloat2("Scale", &scale.x, 0.001f, 10.0f, "%.2f"))
	{
		quad.SetScale(scale.x, scale.y, scale.z);
	}

	if (ImGui::Button("Default position"))
	{
		quad.SetDefaultPosition();
	}

	ImGui::Separator();
	/////////////post processing effects: color inversion and guassian blur///////////////////////

	if (ImGui::Checkbox("Invert colors", &isInvert))
	{
		if (imageLoaded)
		{
			quad.InvertColors();
		}
		else
		{
			isInvert = false;
		}
	}

	if (ImGui::SliderFloat("Gaussian Blur", &blurPercent, 0.00f, 5.0f, "%.2f"))
	{
		if (imageLoaded)
		{
			quad.Blur(blurPercent, isInvert);
		}
		else
		{
			blurPercent = 0.0f;
		}
	}

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/// <summary>
/// processes any left mouse click event
/// </summary>
/// <returns>returns false if 'x' was clicked, otherwise returns true</returns>
bool ProcessEvent()
{
	SDL_Event event;

	SDL_PollEvent(&event);
	ImGui_ImplSDL2_ProcessEvent(&event);

	if (event.type == SDL_QUIT)
	{
		return false;
	}
	return true;
}

int main(int argc, char* argv[])
{

	Screen::Instance()->Initialize();
	
	if (!Shader::Instance()->CreateProgram())
	{
		return 0;
	}

	if (!Shader::Instance()->CreateShaders())
	{
		return 0;
	}
	
	if (!Shader::Instance()->CompileShaders("Shaders/Main.vert", Shader::ShaderType::VERTEX_SHADER))
	{
		return 0;
	}

	if (!Shader::Instance()->CompileShaders("Shaders/Main.frag", Shader::ShaderType::FRAGMENT_SHADER))
	{
		return 0;
	}

	Shader::Instance()->AttachShaders();
	
	if (!Shader::Instance()->LinkProgram())
	{
		return 0;
	}

	//================================================================
	//objects in the 3d space: quad and camera
	Quad quad;
	Camera camera;
	camera.Set3DView();
	camera.SetViewport(0, 0, SCREEN_WIDTH - PROPERTIES_WINDOW_WIDTH, SCREEN_HEIGHT);

	//================================================================
	while (isAppRunning)
	{
		Screen::Instance()->ClearScreen();

		isAppRunning = ProcessEvent();

		RenderPropertiesWindow(quad);

		quad.Update();
		quad.Render();

		Screen::Instance()->Present();
	}

	Shader::Instance()->DetachShaders();
	Shader::Instance()->DestroyShaders();
	Shader::Instance()->DestroyProgram();

	Screen::Instance()->Shutdown();	

	return 0;
}