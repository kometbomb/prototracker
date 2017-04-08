#include "Player.h"
#include "TrackState.h"
#include "SequenceRow.h"

Player::Player(Song& song)
	: IPlayer(song)
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		trackState[i] = new TrackState();
}

Player::~Player()
{
}
