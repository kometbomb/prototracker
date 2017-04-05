#include "Player.h"
#include "Song.h"
#include "Pattern.h"
#include "TrackState.h"
#include "PatternRow.h"
#include "SequenceRow.h"
#include "Sequence.h"
#include "Macro.h"
#include "SDL.h"
#include <cstdio>

Player::Player(Song& song)
	: mSong(song), state()
{
	mMutex = SDL_CreateMutex();
	trackState = new TrackState[SequenceRow::maxTracks];
	stop();
}


Player::~Player()
{
	SDL_DestroyMutex(mMutex);
	delete[] trackState;
}


void Player::reset()
{
	state = PlayerState();
}



void Player::handleMacroTick(int track)
{
	handleMacroNote(track);
		
	TrackState& trState = trackState[track];
		
	if (trState.tick == 0)
	{
		int tries = 0;
		
		/*
		If the effect forces reprocessing, do this max. 2 times to prevent infinite loops with Jxx etc.
		*/
		
		while ((trState.macroState.handleEffectZeroTick(mSong.getMacro(trState.macro).getRow(trState.macroRow).note, trState, state) ||
			trState.macroState.handleEffectZeroTick(mSong.getMacro(trState.macro).getRow(trState.macroRow).effect, trState, state)) && tries < 2)
		{
			handleMacroNote(track);
			tries++;
		}
	}
	
	trState.macroState.handleEffectAnyTick(mSong.getMacro(trState.macro).getRow(trState.macroRow).note, trState, state);
	trState.macroState.handleEffectAnyTick(mSong.getMacro(trState.macro).getRow(trState.macroRow).effect, trState, state);
}


void Player::advanceSequenceRow()
{
	state.patternRow = 0;
	state.setUpdated(PlayerState::Updated::SequenceRow|PlayerState::Updated::PatternRow);
	
	if (state.shouldAdvanceSequenceRow())
	{
		state.sequenceRow++;	
		if (state.sequenceRow >= mSong.getSequenceLength())
			state.sequenceRow = 0;
	}
}

void Player::advancePatternRow()
{
	++state.patternRow;
	state.setUpdated(PlayerState::Updated::PatternRow);
			
	if (state.patternRow >= mSong.getPatternLength())
	{
		advanceSequenceRow();
	}
}


void Player::advanceTrackTick(int track)
{
	TrackState& trState = trackState[track];
	
	++trState.tick;
	
	if (trState.tick >= trState.macroSpeed)
	{
		trState.macroRow++;
		
		if (trState.macroRow >= Macro::maxMacroLength)
		{
			trState.macroRow = 0;
		}
		
		trState.tick = 0;
	}
}


void Player::advanceSequenceTick()
{
	++state.tick;
	
	if (state.tick >= state.songSpeed)
	{
		if (processLastTick())
		{
			advancePatternRow();
		}
		
		state.tick = 0;
	}
}


void Player::runTick()
{
	if (state.isPlaying())
	{
		handleNoteOn();
	}
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		handleMacroTick(i);
	}
	
	if (state.isPlaying())
	{
		if (state.tick == 0)
			processZeroTick();
		
		processAnyTick();
	}
}


void Player::advanceTick()
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		advanceTrackTick(i);
	
	if (state.isPlaying())
		advanceSequenceTick();
}


void Player::handleNoteOn()
{
	const SequenceRow& seqRow = mSong.getSequence().getRow(state.sequenceRow);
	
	if (state.tick == 0)
	{
		for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		{
			PatternRow& patternRow = mSong.getPattern(seqRow.pattern[i]).getRow(state.patternRow);
			int note = patternRow.getNoteWithOctave();
			
			if (note != PatternRow::NoNote)
			{
				if (patternRow.effect.effect != '3')
				{
					if (patternRow.effect.effect == 'm')
						trackState[i].macro = patternRow.effect.getParamsAsByte();
					
					triggerNote(i, note);
				}
				else
				{
					trackState[i].trackState.setSlideTargetFromNote(note);
				}
			}
		}
	}
}


void Player::handleMacroNote(int track)
{
	TrackState& trState = trackState[track];
	
	if (trState.tick == 0)
	{
		int note = mSong.getMacro(trState.macro).getRow(trState.macroRow).getNoteWithOctave();
			
		if (note != PatternRow::NoNote)
		{
			trState.macroState.setFrequencyFromNote(note - 3);
		}
	}
}


void Player::triggerNoteWithReset(int track, int note, int macro)
{
	if (macro != -1)
		trackState[track].macro = macro;
	
	triggerNote(track, note);
	state.tick = 0;
}


void Player::triggerNote(int track, int note)
{
	TrackState& trState = trackState[track];
	trState.trackState.setFrequencyFromNote(note);
	trState.tick = 0;
	trState.trackState.volume = TrackState::maxVolume;
	trState.macroState.frequency = 1.0f;
	trState.macroState.volume = TrackState::maxVolume;
	trState.macroRow = 0;
	trState.macroSpeed = 1;
	handleMacroTick(track);
}


void Player::triggerNote(int track, const PatternRow& row)
{
	int note = row.getNoteWithOctave();
	
	if (note != PatternRow::NoNote)
	{
		triggerNote(track, note);
		state.tick = 0;
		processZeroTick(track, row);
		handleMacroTick(track);
	}
}


void Player::processZeroTick(int track, const PatternRow& row)
{
	TrackState& trState = trackState[track]; 
	
	trState.trackState.handleEffectZeroTick(row.note, trState, state);
	trState.trackState.handleEffectZeroTick(row.effect, trState, state);
}


void Player::processZeroTick()
{
	const SequenceRow& seqRow = mSong.getSequence().getRow(state.sequenceRow);
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		PatternRow&	row = mSong.getPattern(seqRow.pattern[i]).getRow(state.patternRow);
		
		processZeroTick(i, row);
	}
}


void Player::processAnyTick()
{
	const SequenceRow& seqRow = mSong.getSequence().getRow(state.sequenceRow);
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		TrackState& trState = trackState[i]; 
		PatternRow&	row = mSong.getPattern(seqRow.pattern[i]).getRow(state.patternRow);
		trState.trackState.handleEffectAnyTick(row.note, trState, state);
		trState.trackState.handleEffectAnyTick(row.effect, trState, state);
	}
}


bool Player::processLastTick()
{
	const SequenceRow& seqRow = mSong.getSequence().getRow(state.sequenceRow);
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		const EffectParam& effect = mSong.getPattern(seqRow.pattern[i]).getRow(state.patternRow).effect;
		switch (effect.effect)
		{
			case 'b':
				advanceSequenceRow();
				state.sequenceRow = effect.getParamsAsByte();
				return false;
			
			case 'd':
				advanceSequenceRow();
				state.patternRow = effect.getParamsAsByte();
				return false;
				
		}
	}
	
	return true;
}	
	
	
void Player::setSequenceRow(int row)
{
	state.sequenceRow = row;
	state.setUpdated(PlayerState::Updated::SequenceRow);
}


void Player::setPatternRow(int row)
{
	state.patternRow = row;
	state.setUpdated(PlayerState::Updated::PatternRow);
}
	

PlayerState& Player::getPlayerState()
{
	return state;
}


void Player::lock()
{
	SDL_LockMutex(mMutex);
}


void Player::unlock()
{
	SDL_UnlockMutex(mMutex);
}


TrackState& Player::getTrackState(int track)
{
	return trackState[track];
}


void Player::play(int sequenceRow)
{
	play(sequenceRow, PlayerState::Play);
}


void Player::play(int sequenceRow, int mode)
{
	state.setUpdated(PlayerState::Updated::SequenceRow|PlayerState::Updated::PatternRow);
	muteTracks();
	state.sequenceRow = sequenceRow;
	state.patternRow = 0;
	state.tick = 0;
	state.mode = static_cast<PlayerState::Mode>(mode);
	runTick();
}


void Player::stop()
{
	state.mode = PlayerState::Stop;
	muteTracks();
}


void Player::muteTracks()
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		TrackState& trState = trackState[i];
		trState.trackState.volume = 0;
	}
}


int Player::getTick() const
{
	return state.tick;
}
