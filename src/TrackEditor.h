#pragma once

#include "ColumnEditor.h"

struct PatternRow;
struct Song;
struct IPlayer;
struct Color;
struct Pattern;
struct TrackEditorState;

class TrackEditor: public ColumnEditor
{
protected:
	IPlayer& mPlayer;
	Song& mSong;
	bool mTriggerNotes;
	bool mAddMacroEffect;

	int getColumnFlagsFromModifier(int mod) const;

protected:
	virtual Pattern& getCurrentPattern(int track) = 0;
	virtual PatternRow& getCurrentPatternRow() = 0;
	virtual PatternRow& getPatternRow(int track, int row) = 0;

	virtual void changeColumn(int d);

	void playRow();
	void insertRow(bool allTracks, int flags);
	void deleteRow(bool allTracks, int flags);
	void emptyRow(bool allTracks, int flags);

	void setBlockStart(int row);
	void setBlockEnd(int row);
	void copyBlock(int track);
	void pasteBlock(int track);
	void copyTrack(int track);
	void pasteTrack(int track);
	void killTrack(int track);
	virtual void findUnusedTrack(int track) = 0;

	void renderPatternRow(Renderer& renderer, const SDL_Rect& area, const PatternRow& row, const Color& color, int columnFlags = -1);

public:
	TrackEditor(EditorState& editorState, TrackEditorState& trackEditorState, IPlayer& player, Song& song, int tracks);
	virtual ~TrackEditor();

	void setTriggerNotes(bool state);
	void setAddMacroEffect(bool state);

	virtual bool onEvent(SDL_Event& event);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
