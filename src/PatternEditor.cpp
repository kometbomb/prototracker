#include "PatternEditor.h"
#include "PatternRow.h"
#include "Sequence.h"
#include "SequenceRow.h"
#include "EditorState.h"
#include "CommandOptionSelector.h"
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


void PatternEditor::setCurrentPattern(int pattern)
{
	setPattern(mTrackEditorState.currentTrack, pattern);
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
	Pattern& pattern = mSong.getPattern(track, mSong.getSequence().getRow(mEditorState.sequenceEditor.currentRow).pattern[track]);
	return pattern.getRow(row);
}


PatternRow& PatternEditor::getCurrentPatternRow()
{
	SequenceRow seqRow = mSong.getSequence().getRow(mEditorState.sequenceEditor.currentRow);
	Pattern& pattern = mSong.getPattern(mTrackEditorState.currentTrack, seqRow.pattern[mTrackEditorState.currentTrack]);

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
	Pattern& pattern = mSong.getPattern(track, seqRow.pattern[mTrackEditorState.currentTrack]);

	return pattern;
}


void PatternEditor::findUnusedTrack(int track)
{
	SequenceRow& seqRow = mSong.getSequence().getRow(mEditorState.sequenceEditor.currentRow);
	int orig = seqRow.pattern[track];

	while (!mSong.getPattern(track, seqRow.pattern[track]).isEmpty())
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


void PatternEditor::onRequestCommandRegistration()
{
	registerCommand("Pattern", "Next pattern", [this]() {
		this->setCurrentPattern(this->getCurrentSequenceRow().pattern[this->mTrackEditorState.currentTrack] + 1);
	}, SDLK_UP, KMOD_SHIFT);
	registerCommand("Pattern", "Previous pattern", [this]() {
		this->setCurrentPattern(this->getCurrentSequenceRow().pattern[this->mTrackEditorState.currentTrack] - 1);
	}, SDLK_DOWN, KMOD_SHIFT);
	registerCommand("Pattern", "Next sequence row", [this]() {
		this->setSequenceRow(mEditorState.sequenceEditor.currentRow + 1);
	}, SDLK_RIGHT, KMOD_SHIFT);
	registerCommand("Pattern", "Previous sequence row", [this]() {
		this->setSequenceRow(mEditorState.sequenceEditor.currentRow - 1);
	}, SDLK_LEFT, KMOD_SHIFT);

	registerCommand("Pattern", "Mark block start", [this]() { this->setBlockStartToCurrentRow(); }, SDLK_b, KMOD_CTRL);
	registerCommand("Pattern", "Mark block end", [this]() { this->setBlockEndToCurrentRow(); }, SDLK_e, KMOD_CTRL);
	registerCommand("Pattern", "Find unused pattern", [this]() { this->findCurrentUnusedTrack(); }, SDLK_u, KMOD_CTRL);
	registerCommand("Pattern", "Kill current pattern", [this]() { this->killCurrentTrack(); }, SDLK_k, KMOD_CTRL);
	registerCommand("Pattern", "Copy pattern", [this]() { this->copyCurrentTrack(); }, SDLK_F3);
	registerCommand("Pattern", "Paste pattern", [this]() { this->pasteCurrentTrack(); }, SDLK_F4);
	registerCommand("Pattern", "Copy pattern block", [this]() { this->copyCurrentBlock(); }, SDLK_c, KMOD_CTRL);
	registerCommand("Pattern", "Paste pattern block", [this]() { this->pasteCurrentBlock(); }, SDLK_v, KMOD_CTRL);
	registerCommand("Pattern", "Set edit skip", [this](int value) {
		this->setEditSkip(value);
	}, [this](CommandOptionSelector& selector) {
		for (int i = 0 ; i <= 16 ; ++i)
			selector.addIntItem(i);
	});
}
