#include "PatternEditor.h"
#include "PatternRow.h"
#include "Sequence.h"
#include "SequenceRow.h"
#include "EditorState.h"
#include "Song.h"
#include "Pattern.h"
#include "Renderer.h"
#include "Color.h"
#include "PlayerState.h"
#include "IPlayer.h"
#include "SDL.h"

PatternEditor::PatternEditor(EditorState& editorState, IPlayer& player, Song& song)
	: TrackEditor(editorState, editorState.patternEditor, player, song, SequenceRow::maxTracks)
{
	editorState.sequenceEditor.currentRow.addListener(this);
}


PatternEditor::~PatternEditor()
{
}


void PatternEditor::setPattern(int track, int pattern)
{
	if (pattern < 0)
		pattern = 0;

	if (pattern >= Song::maxPatterns)
		pattern = Song::maxPatterns - 1;

	getCurrentSequenceRow().pattern[track] = pattern;

	/*
	 * Trigger sequence display update
	 */

	mEditorState.sequenceEditor.currentRow.notify();

	setDirty(true);
}


bool PatternEditor::onEvent(SDL_Event& event)
{
	switch (event.type)
	{
		case SDL_KEYDOWN:
			if (event.key.keysym.mod & KMOD_SHIFT)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_DOWN:
						setPattern(mTrackEditorState.currentTrack, getCurrentSequenceRow().pattern[mTrackEditorState.currentTrack] - 1);
						return true;

					case SDLK_UP:
						setPattern(mTrackEditorState.currentTrack, getCurrentSequenceRow().pattern[mTrackEditorState.currentTrack] + 1);
						return true;

					case SDLK_LEFT:
						setSequenceRow(mEditorState.sequenceEditor.currentRow - 1);
						return true;

					case SDLK_RIGHT:
						setSequenceRow(mEditorState.sequenceEditor.currentRow + 1);
						return true;
				}
			}

			break;
	}

	return TrackEditor::onEvent(event);
}


PatternRow& PatternEditor::getPatternRow(int track, int row)
{
	Pattern& pattern = mSong.getPattern(mSong.getSequence().getRow(mEditorState.sequenceEditor.currentRow).pattern[track]);
	return pattern.getRow(row);
}


PatternRow& PatternEditor::getCurrentPatternRow()
{
	SequenceRow seqRow = mSong.getSequence().getRow(mEditorState.sequenceEditor.currentRow);
	Pattern& pattern = mSong.getPattern(seqRow.pattern[mTrackEditorState.currentTrack]);

	return pattern.getRow(mTrackEditorState.currentRow);
}


SequenceRow& PatternEditor::getCurrentSequenceRow()
{
	return mSong.getSequence().getRow(mEditorState.sequenceEditor.currentRow);
}


void PatternEditor::setPatternRow(int row)
{
	mTrackEditorState.currentRow = row;

	setDirty(true);
}


void PatternEditor::setSequenceRow(int row)
{
	if (row < 0)
		row = 0;

	if (row >= mSong.getSequenceLength())
		row = mSong.getSequenceLength() - 1;

	mEditorState.sequenceEditor.currentRow = row;

	setDirty(true);
}


Pattern& PatternEditor::getCurrentPattern(int track)
{
	SequenceRow seqRow = mSong.getSequence().getRow(mEditorState.sequenceEditor.currentRow);
	Pattern& pattern = mSong.getPattern(seqRow.pattern[mTrackEditorState.currentTrack]);

	return pattern;
}


void PatternEditor::findUnusedTrack(int track)
{
	SequenceRow& seqRow = mSong.getSequence().getRow(mEditorState.sequenceEditor.currentRow);
	int orig = seqRow.pattern[track];

	while (!mSong.getPattern(seqRow.pattern[track]).isEmpty())
	{
		seqRow.pattern[track] = (seqRow.pattern[track] + 1) % Song::maxPatterns;

		// Check if we are back at starting position

		if (orig == seqRow.pattern[track])
			break;
	}

	mEditorState.sequenceEditor.currentRow.notify();
}


bool PatternEditor::isRowActive(int track, int row) const
{
	const PlayerState& playerState = mPlayer.getPlayerState();
	const SequenceRow& playRow = mSong.getSequence().getRow(playerState.sequenceRow);
	const SequenceRow& seqRow = mSong.getSequence().getRow(mEditorState.sequenceEditor.currentRow);
	return playerState.isPlaying() && playerState.patternRow == row && playRow.pattern[track] == seqRow.pattern[track];
}
