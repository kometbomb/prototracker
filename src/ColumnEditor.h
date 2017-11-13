#pragma once

#include "Editor.h"
#include "SDL.h"

struct TrackEditorState;

class ColumnEditor: public Editor
{
protected:
	TrackEditorState& mTrackEditorState;
	int maxTracks, maxRows, mColumns;
	int mRowNumberMargin, mTrackMargin;

	int getNoteFromKey(const SDL_Keysym& sym) const;
	int getHexFromKey(const SDL_Keysym& sym) const;
	int getCharFromKey(const SDL_Keysym& sym) const;
	void changeTrack(int d);

	virtual void changeColumn(int d);

	void scrollView(int d, bool wrap = true);

	// Return true if play position is at this row
	virtual bool isRowActive(int track, int row) const;

public:
	ColumnEditor(EditorState& editorState, TrackEditorState& trackEditorState, int tracks, int columns);
	virtual ~ColumnEditor();

	void setRowNumberMargin(int margin);
	void setTrackMargin(int margin);

	void setMaxRows(int rows);
};
