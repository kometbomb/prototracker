#pragma once

#include "IPlayer.h"

class Player: public IPlayer
{
public:
	Player(Song& song);
	virtual ~Player();
};
