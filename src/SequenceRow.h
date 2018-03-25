#pragma once

#include "App.h"

struct SequenceRow
{
	static const int maxTracks = SONG_TRACKS;

	int pattern[maxTracks];

	void clear();
	bool isEmpty() const;

	SequenceRow();
};
