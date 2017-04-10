#pragma once

#include "Editor.h"
#include <string>

struct IPlayer;
struct Song;
struct PatternEditor;
struct TextEditor;
struct SequenceRowEditor;
struct Oscilloscope;
struct ISynth;
struct FileSelector;
struct Listenable;
struct Theme;

class MainEditor: public Editor
{
	IPlayer& mPlayer;
	PlayerState& mPlayerState;
	Song& mSong;
	ISynth& mSynth;
	Listenable *mOscillatorsUpdated;
	PatternEditor *patternEditor;
	SequenceRowEditor *sequenceRowEditor;
	TextEditor *songNameEditor;
	TextEditor *macroNameEditor;
	FileSelector *fileSelector;
	int mDragStartX, mDragStartY;
	bool mIsDragging;
	
	enum
	{
		FileSelectionLoad,
		FileSelectionSave
	};
	
	void displayLoadDialog();
	void displaySaveDialog();
	
	std::string mBase64Encoded;
	
	std::string getUserFile(const char *file) const;
	void deleteChildren();
	
	void startDragging(int x, int y);
	void stopDragging();
	
public:
	MainEditor(EditorState& editorState, IPlayer& player, PlayerState& playerState, Song& song, ISynth& synth);
	virtual ~MainEditor();
	
	virtual bool onEvent(SDL_Event& event);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual void onFileSelectorEvent(const Editor& fileSelector, bool accept);
	
	void cycleFocus();
	void syncPlayerState();
	
	void setMacro(int index);
	void syncSongParameters(const Song& song);
	void refreshAll();
	
	void playSong();
	void stopSong();
	void togglePlayStop();
	
	bool saveSong(const char *path);
	bool loadSong(const char *path);
	bool exportSong();
	void newSong();
	const std::string& getSongBase64();
	
	bool loadState();
	void saveState();
	
	void loadElements(const Theme& theme);
};
