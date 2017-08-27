#include "ColumnEditor.h"
#include "EditorState.h"
#include <cstdio>

ColumnEditor::ColumnEditor(EditorState& editorState, TrackEditorState& trackEditorState, int tracks, int columns)
	:Editor(editorState), mTrackEditorState(trackEditorState), maxTracks(tracks), maxRows(256), mColumns(columns), mRowNumberMargin(0), mTrackMargin(0)
{
	trackEditorState.currentRow.addListener(this);
	trackEditorState.currentTrack.addListener(this);
	trackEditorState.currentColumn.addListener(this);
}


ColumnEditor::~ColumnEditor()
{
}


/* SDL has trouble with AZERTY number keys (they are accessed with the shift key).
 * The following routines bypass this by determining the number by using scancodes
 * (on 95 % of keyboard layouts the number keys are in the same order and at same
 * location).
 */

int ColumnEditor::getCharFromKey(const SDL_Keysym& sym) const
{
	if (sym.sym >= SDLK_a && sym.sym <= SDLK_z)
		return sym.sym - SDLK_a + 'a';
	
	// Scancodes are ordered 1-9 and then 0 (as on the keyboard)
	
	if (sym.scancode == SDL_SCANCODE_0)
		return '0';
	
	if (sym.scancode >= SDL_SCANCODE_1 && sym.scancode <= SDL_SCANCODE_9)
		return sym.scancode - SDL_SCANCODE_1 + '1';
	
	return -1;
}


int ColumnEditor::getHexFromKey(const SDL_Keysym& sym) const
{
	if (sym.sym >= SDLK_a && sym.sym <= SDLK_f)
		return sym.sym - SDLK_a + 0xa;
	
	// Scancodes are ordered 1-9 and then 0 (as on the keyboard)
	
	if (sym.scancode == SDL_SCANCODE_0)
		return 0;
	
	if (sym.scancode >= SDL_SCANCODE_1 && sym.scancode <= SDL_SCANCODE_9)
		return sym.scancode - SDL_SCANCODE_1 + 1;
	
	return -1;
}


int ColumnEditor::getNoteFromKey(const SDL_Keysym& sym) const
{
	static const struct { SDL_Scancode scancode; int note; } syms[] = 
	{
		{SDL_SCANCODE_Z, 0},
		{SDL_SCANCODE_S, 1},
		{SDL_SCANCODE_X, 2},
		{SDL_SCANCODE_D, 3},
		{SDL_SCANCODE_C, 4},
		{SDL_SCANCODE_V, 5},
		{SDL_SCANCODE_G, 6},
		{SDL_SCANCODE_B, 7},
		{SDL_SCANCODE_H, 8},
		{SDL_SCANCODE_N, 9},
		{SDL_SCANCODE_J, 10},
		{SDL_SCANCODE_M, 11},
		{SDL_SCANCODE_COMMA, 12},
		{SDL_SCANCODE_L, 13},
		{SDL_SCANCODE_PERIOD, 14},
		
		{SDL_SCANCODE_Q, 12},
		{SDL_SCANCODE_2, 13},
		{SDL_SCANCODE_W, 14},
		{SDL_SCANCODE_3, 15},
		{SDL_SCANCODE_E, 16},
		{SDL_SCANCODE_R, 17},
		{SDL_SCANCODE_5, 18},
		{SDL_SCANCODE_T, 19},
		{SDL_SCANCODE_6, 20},
		{SDL_SCANCODE_Y, 21},
		{SDL_SCANCODE_7, 22},
		{SDL_SCANCODE_U, 23},
		{SDL_SCANCODE_I, 24},
		{SDL_SCANCODE_9, 25},
		{SDL_SCANCODE_O, 26},
		{SDL_SCANCODE_0, 27},
		{SDL_SCANCODE_P, 28},
		{SDL_SCANCODE_P, -1}
	};
		
	for (int i = 0 ; syms[i].note != -1 ; ++i)
		if (syms[i].scancode == sym.scancode)
			return syms[i].note;
			
	return -1;
}


void ColumnEditor::setMaxRows(int rows)
{
	maxRows = rows;
	
	if (mTrackEditorState.currentRow >= rows)
		mTrackEditorState.currentRow = rows - 1;
}


void ColumnEditor::scrollView(int d, bool wrap)
{
	mTrackEditorState.currentRow += d;
	
	if (wrap)
	{
		mTrackEditorState.currentRow = (mTrackEditorState.currentRow + maxRows) % maxRows;
	}
	else
	{
		if (mTrackEditorState.currentRow < 0)
			mTrackEditorState.currentRow = 0;
		else if (mTrackEditorState.currentRow >= maxRows)
			mTrackEditorState.currentRow = maxRows - 1;
	}
}


void ColumnEditor::changeColumn(int d)
{
	if (d < 0)
	{
		--mTrackEditorState.currentColumn;
		
		if (mTrackEditorState.currentColumn < 0)
		{
			mTrackEditorState.currentColumn = mColumns - 1;
			changeTrack(-1);
		}
	}
	else 
	{
		++mTrackEditorState.currentColumn;
		
		if (mTrackEditorState.currentColumn >= mColumns)
		{
			mTrackEditorState.currentColumn = 0;
			changeTrack(1);
		}
	}
}


void ColumnEditor::changeTrack(int d)
{
	int currentTrack = mTrackEditorState.currentTrack;
	currentTrack += d;
	
	if (currentTrack < 0)
	{
		currentTrack = 0;
		mTrackEditorState.currentColumn = 0;
	}
	
	if (currentTrack > maxTracks - 1)
	{
		currentTrack = maxTracks - 1;
		mTrackEditorState.currentColumn = mColumns - 1;
	}
	
	mTrackEditorState.currentTrack = currentTrack;
}


void ColumnEditor::setRowNumberMargin(int margin)
{
	mRowNumberMargin = margin;
}


void ColumnEditor::setTrackMargin(int margin)
{
	mTrackMargin = margin;
}
