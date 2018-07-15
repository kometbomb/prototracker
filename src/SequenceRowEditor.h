#pragma once

#include "ColumnEditor.h"

struct Song;
struct IPlayer;
struct TrackEditorState;

class SequenceRowEditor: public ColumnEditor
{
	IPlayer& mPlayer;
	Song& mSong;

	void duplicateRow();
	void insertRow(bool allTracks, int flags);
	void deleteRow(bool allTracks, int flags);
	void emptyRow(bool allTracks, int flags);
	virtual bool isRowActive(int track, int row) const;

protected:
	virtual void onRequestCommandRegistration();

public:
	SequenceRowEditor(EditorState& editorState, IPlayer& player, Song& song);
	virtual ~SequenceRowEditor();

	virtual bool onEvent(SDL_Event& event);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
