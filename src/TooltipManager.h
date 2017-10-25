#pragma once

#include <queue>
#include <string>
#include "Editor.h"
#include "SDL.h"

class TooltipManager 
{
public:
	struct Tooltip {
		SDL_Rect area;
		std::string text;
		
		Tooltip(const SDL_Rect& area, const std::string& message);
		Tooltip();
	};

private:
	static const int HoverDelayMs = 500;

	Tooltip mTooltip;
	bool mVisible, mWasVisible, mDidJustAppear;
	int mHoveredMs;
	
public:
	TooltipManager();
	void update(int ms);
	void setTooltip(const SDL_Rect& area, const std::string& message);
	void updateTooltipMotion(int x, int y);

	/* Get tooltip if visible
	 */
	const Tooltip* getVisibleTooltip() const;
	
	bool isDirty() const;
	void setDirty(bool state);
};
