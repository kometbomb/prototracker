#pragma once

#include "Editor.h"

struct TrackEditorState;

class ColumnEditor: public Editor
{
protected:
	TrackEditorState& mTrackEditorState;
	int maxTracks, maxRows, mColumns;
	int mRowNumberMargin, mTrackMargin;
	
	int getNoteFromKey(int sym) const;
	int getHexFromKey(int sym) const;
	int getCharFromKey(int sym) const;
	void changeTrack(int d);
	
	virtual void changeColumn(int d);
	
	void scrollView(int d, bool wrap = true);
	
public:
	ColumnEditor(EditorState& editorState, TrackEditorState& trackEditorState, int tracks, int columns);
	virtual ~ColumnEditor();
	
	void setRowNumberMargin(int margin);
	void setTrackMargin(int margin);
	
	void setMaxRows(int rows);
};
