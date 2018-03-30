#include "Player.h"
#include "Song.h"
#include "Pattern.h"
#include "ITrackState.h"
#include "PatternRow.h"
#include "SequenceRow.h"
#include "Sequence.h"
#include "Macro.h"
#include "SDL.h"
#include <cstdio>

IPlayer::IPlayer(Song& song)
	: Lockable(), mSong(song), state()
{
	trackState = new ITrackState*[SequenceRow::maxTracks];
}


IPlayer::~IPlayer()
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		delete trackState[i];

	delete[] trackState;
}


void IPlayer::reset()
{
	state = PlayerState();
}



void IPlayer::handleMacroTick(int track)
{
	handleMacroNote(track);

	ITrackState& trState = *trackState[track];

	if (trState.tick == 0)
	{
		int tries = 0;

		/*
		If the effect forces reprocessing, do this max. 2 times to prevent infinite loops with Jxx etc.
		*/

		while (tries < 2)
		{
			bool jumpHappened = false;

			for (int i = 0 ; !jumpHappened && i < PatternRow::effectParams + 1 ; ++i) {
				jumpHappened |= trState.macroState.handleEffectZeroTick(mSong.getMacro(trState.macro).getRow(trState.macroRow).getAnyParam(i), trState, state);
			}

			if (!jumpHappened)
				break;

			handleMacroNote(track);
			tries++;
		}
	}

	for (int i = 0 ; i < PatternRow::effectParams + 1 ; ++i) {
		trState.macroState.handleEffectAnyTick(mSong.getMacro(trState.macro).getRow(trState.macroRow).getAnyParam(i), trState, state);
	}
}


void IPlayer::advanceSequenceRow()
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

void IPlayer::advancePatternRow()
{
	++state.patternRow;
	state.setUpdated(PlayerState::Updated::PatternRow);

	if (state.patternRow >= mSong.getPatternLength())
	{
		advanceSequenceRow();
	}
}


void IPlayer::advanceTrackTick(int track)
{
	ITrackState& trState = *trackState[track];

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


void IPlayer::advanceSequenceTick()
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


void IPlayer::runTick()
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


void IPlayer::advanceTick()
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		advanceTrackTick(i);

	if (state.isPlaying())
		advanceSequenceTick();
}


void IPlayer::handleNoteOn()
{
	const SequenceRow& seqRow = mSong.getSequence().getRow(state.sequenceRow);

	if (state.tick == 0)
	{
		for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		{
			PatternRow& patternRow = mSong.getPattern(i, seqRow.pattern[i]).getRow(state.patternRow);
			int note = patternRow.getNoteWithOctave();

			if (note != PatternRow::NoNote)
			{
				for (int effectParam = 0 ; effectParam < PatternRow::effectParams ; ++effectParam)
				{
					if (patternRow.getEffect(effectParam).effect != '3')
					{
						if (patternRow.getEffect(effectParam).effect == 'm')
							trackState[i]->macro = patternRow.getEffect(effectParam).getParamsAsByte();

						triggerNote(i, note);
					}
					else
					{
						trackState[i]->trackState.setSlideTargetFromNote(note);
					}
				}
			}
		}
	}
}


void IPlayer::handleMacroNote(int track)
{
	ITrackState& trState = *trackState[track];

	if (trState.tick == 0)
	{
		int note = mSong.getMacro(trState.macro).getRow(trState.macroRow).getNoteWithOctave();

		if (note != PatternRow::NoNote)
		{
			trState.macroState.setFrequencyFromNote(note - 3);
		}
	}
}


void IPlayer::triggerNoteWithReset(int track, int note, int macro)
{
	if (macro != -1)
		trackState[track]->macro = macro;

	triggerNote(track, note);

	if (!state.isPlaying())
		state.tick = 0;
}


void IPlayer::triggerNote(int track, int note)
{
	ITrackState& trState = *trackState[track];
	trState.triggered = true;
	trState.trackState.setFrequencyFromNote(note);
	trState.tick = 0;
	trState.trackState.volume = ITrackState::maxVolume;
	trState.macroState.frequency = 1.0f;
	trState.macroState.volume = ITrackState::maxVolume;
	trState.macroRow = 0;
	trState.macroSpeed = 1;
	handleMacroTick(track);
}


void IPlayer::triggerNote(int track, const PatternRow& row)
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


void IPlayer::processZeroTick(int track, const PatternRow& row)
{
	ITrackState& trState = *trackState[track];

	for (int effectParam = 0 ; effectParam < PatternRow::effectParams + 1 ; ++effectParam)
	{
		trState.trackState.handleEffectZeroTick(row.getAnyParam(effectParam), trState, state);
	}
}


void IPlayer::processZeroTick()
{
	const SequenceRow& seqRow = mSong.getSequence().getRow(state.sequenceRow);

	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		PatternRow&	row = mSong.getPattern(i, seqRow.pattern[i]).getRow(state.patternRow);

		processZeroTick(i, row);
	}
}


void IPlayer::processAnyTick()
{
	const SequenceRow& seqRow = mSong.getSequence().getRow(state.sequenceRow);

	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		ITrackState& trState = *trackState[i];
		PatternRow&	row = mSong.getPattern(i, seqRow.pattern[i]).getRow(state.patternRow);

		for (int effectParam = 0 ; effectParam < PatternRow::effectParams + 1 ; ++effectParam) {
			trState.trackState.handleEffectAnyTick(row.getAnyParam(effectParam), trState, state);
		}
	}
}


bool IPlayer::processLastTick()
{
	const SequenceRow& seqRow = mSong.getSequence().getRow(state.sequenceRow);

	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		for (int effectParam = 0 ; effectParam < PatternRow::effectParams + 1 ; ++effectParam)
		{
			const EffectParam& effect = mSong.getPattern(i, seqRow.pattern[i]).getRow(state.patternRow).getAnyParam(effectParam);
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
	}

	return true;
}


void IPlayer::setSequenceRow(int row)
{
	state.sequenceRow = row;
	state.setUpdated(PlayerState::Updated::SequenceRow);
}


void IPlayer::setPatternRow(int row)
{
	state.patternRow = row;
	state.setUpdated(PlayerState::Updated::PatternRow);
}


PlayerState& IPlayer::getPlayerState()
{
	return state;
}


ITrackState& IPlayer::getTrackState(int track)
{
	return *trackState[track];
}


void IPlayer::play(int sequenceRow)
{
	play(sequenceRow, PlayerState::Play);
}


void IPlayer::play(int sequenceRow, int mode)
{
	state.setUpdated(PlayerState::Updated::SequenceRow|PlayerState::Updated::PatternRow);
	muteTracks();
	state.sequenceRow = sequenceRow;
	state.patternRow = 0;
	state.tick = 0;
	state.mode = static_cast<PlayerState::Mode>(mode);
	runTick();
}


void IPlayer::stop()
{
	state.mode = PlayerState::Stop;
	muteTracks();
}


void IPlayer::muteTracks()
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		ITrackState& trState = *trackState[i];
		trState.trackState.volume = 0;
	}
}


int IPlayer::getTick() const
{
	return state.tick;
}
