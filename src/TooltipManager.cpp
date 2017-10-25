#include "TooltipManager.h"

TooltipManager::Tooltip::Tooltip(const SDL_Rect& _area, const std::string& _text)
	: area(_area), text(_text)
{
}


TooltipManager::Tooltip::Tooltip() 
	: area(), text()
{

}


TooltipManager::TooltipManager()
	:  mVisible(false), mDidJustAppear(false), mHoveredMs(0)
{
}


void TooltipManager::update(int ms)
{
	if (mTooltip.area.w == 0)
	{
		return;
	}

	mHoveredMs += ms;
	
	if (mHoveredMs >= HoverDelayMs)
	{
		if (!mVisible) 
		{
			mDidJustAppear = true;
		}
		
		mVisible = true;
	}
}


void TooltipManager::setTooltip(const SDL_Rect& area, const std::string& message)
{
	if (mTooltip.area.x == area.x && mTooltip.area.y == area.y 
		&& mTooltip.area.w == area.w && mTooltip.area.h == area.h 
		&& message == mTooltip.text) 
	{
		return;
	}

	mTooltip = Tooltip(area, message);
	mHoveredMs = 0;
	mVisible = false;
}


void TooltipManager::updateTooltipMotion(int x, int y)
{
	SDL_Point point = { x, y };
	if (!Editor::pointInRect(point, mTooltip.area)) 
	{
		mTooltip.area.w = 0;
		mTooltip.area.h = 0;
		mDidJustAppear = true;
	}
}


const TooltipManager::Tooltip* TooltipManager::getVisibleTooltip() const
{
	if (mVisible && mTooltip.area.w > 0)  
	{
		return &mTooltip;
	}

	return NULL;
}


bool TooltipManager::isDirty() const
{
	return mDidJustAppear;
}


void TooltipManager::setDirty(bool state)
{
	mDidJustAppear = state;
}
