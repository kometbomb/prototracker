#pragma once

#include "PlayerState.h"

struct Song;
struct ITrackState;
struct SDL_mutex;
struct PatternRow;

/*

IPlayer handles most of the player logic.

You need to override IPlayer() to setup your own ITrackStates for the tracks.

*/

class IPlayer
{
protected:
	SDL_mutex *mMutex;
	int mLockCounter;
	Song& mSong;
	PlayerState state;
	ITrackState **trackState;

	void processZeroTick();
	void processAnyTick();

	/*
	Return false if processLastTick handled sequenceRow advance etc.
	*/
	bool processLastTick();
	void advanceSequenceRow();
	void advanceSequenceTick();
	void advanceTrackTick(int track);
	void advancePatternRow();
	void handleNoteOn();
	void handleMacroNote(int track);
	void handleMacroTick(int track);
	void processZeroTick(int track, const PatternRow& row);

public:
	IPlayer(Song& song);
	virtual ~IPlayer();

	void reset();

	void lock();
	void unlock();

	int getTick() const;

	void play(int sequenceRow);
	void play(int sequenceRow, int mode);
	void stop();
	void muteTracks();
	void triggerNote(int track, int note);
	void triggerNote(int track, const PatternRow& row);
	void triggerNoteWithReset(int track, int note, int macro = -1);

	void setSequenceRow(int row);
	void setPatternRow(int row);
	PlayerState& getPlayerState();
	ITrackState& getTrackState(int track);
	void runTick();
	void advanceTick();
};
