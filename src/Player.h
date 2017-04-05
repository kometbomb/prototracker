#pragma once

#include "PlayerState.h"

struct Song;
struct TrackState;
struct SDL_mutex;
struct PatternRow;

class Player
{
	SDL_mutex *mMutex;
	Song& mSong;
	PlayerState state;
	TrackState *trackState;
	
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
	Player(Song& song);
	~Player();
	
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
	TrackState& getTrackState(int track);
	void runTick();	
	void advanceTick();
};
