#include "Gamepad.h"
#include <cstdio>

typedef struct { int axis; int direction; int idx; int button; } AxisTransTab;

void Gamepad::initControllers()
{
	controllerAxis[0] = 0;
	controllerAxis[1] = 0;
	
	for (int i = 0; i < SDL_NumJoysticks(); ++i) 
	{
		if (SDL_IsGameController(i)) 
		{
			SDL_GameController *newController = SDL_GameControllerOpen(i);
			
			if (newController)
			{
				printf("Opening controller %s\n", SDL_GameControllerName(newController));
				controller.push_back(newController);
			}
			else
			{
				printf("Opening controller failed\n");
			}
		}
	}

}


void Gamepad::deinitControllers()
{
	for (auto ctrl : controller)
		SDL_GameControllerClose(ctrl);
	
	controller.clear();
}


bool Gamepad::loadDefinitions(const char *path)
{
	return SDL_GameControllerAddMappingsFromFile(path) != -1;
}


void Gamepad::translateAxisToDPad(const SDL_Event& event)
{
	const AxisTransTab transTab[] = {
		{ SDL_CONTROLLER_AXIS_LEFTX, -1, 0, SDL_CONTROLLER_BUTTON_DPAD_LEFT },
		{ SDL_CONTROLLER_AXIS_LEFTX, 1, 0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT },
		{ SDL_CONTROLLER_AXIS_LEFTY, -1, 1, SDL_CONTROLLER_BUTTON_DPAD_UP },
		{ SDL_CONTROLLER_AXIS_LEFTY, 1, 1, SDL_CONTROLLER_BUTTON_DPAD_DOWN }
	};
	
	int i;
	
	for (i = 0 ; i < 4 ; ++i)
	{
		const AxisTransTab trans = transTab[i];
		
		if (trans.axis == event.caxis.axis)
		{
			bool pressed = event.caxis.value < 1000 * trans.direction;
			bool wasPressed = controllerAxis[trans.idx] < 1000 * trans.direction;
				
			if (wasPressed && !pressed)
			{
				SDL_Event event = {0};
				event.type = SDL_CONTROLLERBUTTONUP;
				event.cbutton.timestamp = event.caxis.timestamp;
				event.cbutton.which = event.caxis.which;
				event.cbutton.button = trans.button;
				event.cbutton.state = SDL_RELEASED;
				
				SDL_PushEvent(&event);
			}
			
			if (pressed && !wasPressed)
			{
				SDL_Event event = {0};
				event.type = SDL_CONTROLLERBUTTONDOWN;
				event.cbutton.timestamp = event.caxis.timestamp;
				event.cbutton.which = event.caxis.which;
				event.cbutton.button = trans.button;
				event.cbutton.state = SDL_PRESSED;
				
				SDL_PushEvent(&event);
			}
		}
	}
	
	for (i = 0 ; i < 4 ; ++i)
	{
		const AxisTransTab trans = transTab[i];
		
		if (trans.axis == event.caxis.axis)
		{
			controllerAxis[trans.idx] = event.caxis.value;
			break;
		}
	}
}
