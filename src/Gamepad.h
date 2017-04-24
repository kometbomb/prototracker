#pragma once

/* 
 Helper object to handle SDL_GameControllers and joystick axis to d-pad button translation
 */
 
#include "SDL.h"
#include <vector>

class Gamepad
{
	bool controllerDefsLoaded;
	int controllerAxis[2];
	
	std::vector<SDL_GameController*> controller;
	
public:

	void translateAxisToDPad(const SDL_Event& event);
	bool loadDefinitions(const char *path);
	void initControllers();
	void deinitControllers();
};
