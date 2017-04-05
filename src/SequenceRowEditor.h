#pragma once

#include "ColumnEditor.h"

struct Song;
struct TrackEditorState;

class SequenceRowEditor: public ColumnEditor
{
	Song& mSong;
	
	void duplicateRow();
	void insertRow(bool allTracks, int flags);
	void deleteRow(bool allTracks, int flags);
	void emptyRow(bool allTracks, int flags);
	
public:
	SequenceRowEditor(EditorState& editorState, Song& song);
	virtual ~SequenceRowEditor();
	
	virtual bool onEvent(SDL_Event& event);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
