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
struct Mixer;
struct FileSelector;
struct Listenable;
struct Theme;
struct MessageManager;
struct MessageDisplayer;
struct TooltipManager;
struct TooltipDisplayer;
struct AudioDeviceSelector;
struct MIDIDeviceSelector;
struct MIDIHandler;

class MainEditor: public Editor
{
	IPlayer& mPlayer;
	PlayerState& mPlayerState;
	Song& mSong;
	ISynth& mSynth;
	Mixer& mMixer;
	MIDIHandler& mMIDIHandler;
	Listenable *mOscillatorsProbePos;
	PatternEditor *patternEditor;
	SequenceRowEditor *sequenceRowEditor;
	TextEditor *songNameEditor;
	TextEditor *macroNameEditor;
	FileSelector *fileSelector;
	AudioDeviceSelector *audioDeviceSelector;
	MIDIDeviceSelector *midiDeviceSelector;
	MessageManager *mMessageManager;
	MessageDisplayer *mMessageDisplayer;
	TooltipManager *mTooltipManager;
	TooltipDisplayer *mTooltipDisplayer;

	int mDragStartX, mDragStartY;
	bool mIsDragging;

	enum
	{
		FileSelectionLoad,
		FileSelectionSave,
		AudioDeviceSelection,
		MIDIDeviceSelection,
	};

	void displayLoadDialog();
	void displaySaveDialog();
	void displayAudioDeviceDialog();
	void displayMIDIDeviceDialog();

	std::string mBase64Encoded;

	std::string getUserFile(const char *file) const;
	void deleteChildren();

	void startDragging(int x, int y);
	void stopDragging();

public:
	MainEditor(EditorState& editorState, IPlayer& player, PlayerState& playerState, Song& song, ISynth& synth, Mixer& mixer, MIDIHandler& midiHandler);
	virtual ~MainEditor();

	virtual bool onEvent(SDL_Event& event);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual void onFileSelectorEvent(const Editor& fileSelector, bool accept);
	virtual void showTooltip(const SDL_Rect& area, const char* message);
	virtual int showMessageInner(MessageClass messageClass, int messageId, const char* message);

	void onExternalKeyStateChange(int key, bool keyDown);

	void cycleFocus();
	void syncPlayerState();

	virtual void onUpdate(int ms);

	void setMacro(int index);
	void syncSongParameters(const Song& song);
	void refreshAll();

	void playSong();
	void stopSong();
	void togglePlayStop();

	void setAudioDevice(const char *device);
	void setMIDIDevice(const char *device);

	bool saveSong(const char *path);
	bool loadSong(const char *path);
	bool exportSong();
	void newSong();
	const std::string& getSongBase64();

	bool loadState();
	void saveState();

	bool loadElements(const Theme& theme);
};
