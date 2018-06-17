#include "MainEditor.h"
#include "IPlayer.h"
#include "PatternEditor.h"
#include "Oscilloscope.h"
#include "MacroEditor.h"
#include "EditorState.h"
#include "SequenceRowEditor.h"
#include "SequenceInfo.h"
#include "SongLengthInfo.h"
#include "MacroInfo.h"
#include "PatternLengthInfo.h"
#include "Macro.h"
#include "Pattern.h"
#include "TextEditor.h"
#include "PlayerState.h"
#include "Color.h"
#include "Song.h"
#include "Renderer.h"
#include "Label.h"
#include "ISynth.h"
#include "Mixer.h"
#include "Value.h"
#include "SequenceRow.h"
#include "ITrackState.h"
#include "FileSection.h"
#include "OctaveInfo.h"
#include "TouchRegion.h"
#include "FileSelector.h"
#include "AudioDeviceSelector.h"
#include "CommandSelector.h"
#include "CommandOptionSelector.h"
#include "Emscripten.h"
#include "MessageManager.h"
#include "MessageDisplayer.h"
#include "TooltipManager.h"
#include "TooltipDisplayer.h"
#include "App.h"
#include "SDL.h"
#include "Theme.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// TODO: Move this somewhere

#ifndef SCALE
#define SCALE 2
#endif

MainEditor::MainEditor(EditorState& editorState, IPlayer& player, PlayerState& playerState, Song& song, ISynth& synth, Mixer& mixer)
	: Editor(editorState), mPlayer(player), mPlayerState(playerState), mSong(song), mSynth(synth), mMixer(mixer), mIsDragging(false),
	mSelectedCommand(NULL)
{
	mOscillatorsProbePos = new Value();

	fileSelector = new FileSelector(editorState);
	audioDeviceSelector = new AudioDeviceSelector(editorState);
	commandSelector = new CommandSelector(editorState, *this);
	commandOptionSelector = NULL;

	mMessageManager = new MessageManager();
	mTooltipManager = new TooltipManager();

	// This is a special case because MainEditor is never added as a child so we trigger this here.
	onRequestCommandRegistration();
}


MainEditor::~MainEditor()
{
	delete mOscillatorsProbePos;
	delete fileSelector;
	delete audioDeviceSelector;
	delete commandSelector;

	if (commandOptionSelector != NULL)
		delete commandOptionSelector;

	deleteChildren();

	delete mMessageManager;

	for (auto desc : mCommands)
	{
		delete desc;
	}
}


void MainEditor::deleteChildren()
{
	for (auto child : mChildren)
		delete child.editor;

	mChildren.clear();
}


void MainEditor::startDragging(int x, int y)
{
	mIsDragging = true;

	mDragStartX = x;
	mDragStartY = y;
}


void MainEditor::stopDragging()
{
	mIsDragging = false;
}


void MainEditor::togglePositionFollowing()
{
	mEditorState.followPlayPosition = !mEditorState.followPlayPosition;
	showMessage(MessageInfo, mEditorState.followPlayPosition ? "Cursor now follows play position" : "Disabled play position following");
}


bool MainEditor::onEvent(SDL_Event& event)
{
	/* If a modal dialog (FileSelector etc.) is visible,
	 * send events to that Editor instead.
	 */
	if (mModal != NULL)
	{
		return mModal->onEvent(event);
	}

	Editor* target = getFocus();

	/* Focus on GUI elements when user clicks on them.
	 * Also, start drag (used by drag scroll below this)
	 */
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		SDL_Point point = {event.button.x/SCALE, event.button.y/SCALE};

		for (auto child : mChildren)
		{
			if (pointInRect(point, child.area))
			{
				target = child.editor;
				break;
			}
		}

		startDragging(event.button.x, event.button.y);
	}
	else if (event.type == SDL_MOUSEMOTION)
	{
		SDL_Point point = {event.motion.x/SCALE, event.motion.y/SCALE};

		for (auto child : mChildren)
		{
			if (pointInRect(point, child.area))
			{
				target = child.editor;
				break;
			}
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		stopDragging();
	}
#if MOUSEDRAG
	/* Emulate dragging by mouse (or touch events) by sending cursor key
	 * events per every 8 pixels (should be font width/height to be accurate)
	 */

	else if (event.type == SDL_MOUSEMOTION && mIsDragging)
	{
		if (abs(event.motion.y - mDragStartY) >= SCALE * 8)
		{
			SDL_Event keyhit = {0};
			keyhit.type = SDL_KEYDOWN;
			keyhit.key.keysym.sym = event.motion.y > mDragStartY ? SDLK_UP : SDLK_DOWN;

			SDL_PushEvent(&keyhit);

			mDragStartX = event.motion.x;
			mDragStartY = event.motion.y;

			return true;
		}
		else if (abs(event.motion.x - mDragStartX) >= SCALE * 8)
		{
			SDL_Event keyhit = {0};
			keyhit.type = SDL_KEYDOWN;
			keyhit.key.keysym.sym = event.motion.x < mDragStartX ? SDLK_LEFT : SDLK_RIGHT;

			SDL_PushEvent(&keyhit);

			mDragStartX = event.motion.x;
			mDragStartY = event.motion.y;

			return true;
		}
	}
#endif

	if (target->onEvent(event))
	{
		// Target Editor consumed the event, we don't need to process it here.
		return true;
	}

	if (event.type == SDL_MOUSEMOTION)
	{
		mTooltipManager->updateTooltipMotion(event.motion.x/SCALE, event.motion.y/SCALE);
	}

	if (target->isFocusable() && event.type != SDL_MOUSEMOTION)
	{
		setFocus(target);
	}

	switch (event.type)
	{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case SDLK_F1:
					mEditorState.octave = std::max(0, mEditorState.octave - 1);
					return true;

				case SDLK_F2:
					mEditorState.octave = std::min(15, mEditorState.octave + 1);
					return true;

				case SDLK_F9:
					setPatternLength(std::max(1, mSong.getPatternLength() - 1));
					refreshAll();
					return true;

				case SDLK_F10:
					setPatternLength(std::min(Pattern::maxRows, mSong.getPatternLength() + 1));
					refreshAll();
					return true;

				/* Mute tracks */
				case SDLK_1:
				case SDLK_2:
				case SDLK_3:
				case SDLK_4:
				case SDLK_5:
				case SDLK_6:
				case SDLK_7:
				case SDLK_8:
				case SDLK_9:

					if (event.key.keysym.mod & KMOD_ALT)
					{
						int track = event.key.keysym.sym - SDLK_1;

						if (track < SequenceRow::maxTracks)
						{
							mPlayer.getTrackState(track).enabled ^= true;

							showMessageV(MessageInfo, "%s track %d", mPlayer.getTrackState(track).enabled ? "Unmuted" : "Muted", track);

							return true;
						}
					}

					break;

				/* F5 and F6 also used for laptops etc. keyboards with (very) limited key layout */
				case SDLK_F5:
				case SDLK_RCTRL:
					playSong();
					return true;

				/* F5 and F6 also used for laptops etc. keyboards with (very) limited key layout */
				case SDLK_F6:
				case SDLK_RSHIFT:
					playPattern();
					return true;

				case SDLK_SPACE:
					if (mEditorState.followPlayPosition)
					{
						if (mPlayerState.mode != PlayerState::Stop)
						{
							mPlayer.stop();
						}
						else
						{
							toggleEditMode();
						}

						muteTracks();
					}
					else
					{
						toggleEditMode();

						// Should only mute tracks when stopped, i.e.
						// the user has played a note and wants to stop it
						// and not when editing while playing the song
						if (mPlayerState.mode == PlayerState::Stop)
							muteTracks();
					}

					return true;

				case SDLK_F7:
					saveState();
					return true;

				case SDLK_ESCAPE:
					cycleFocus();
					return true;

				case SDLK_PERIOD:
					if (!(event.key.keysym.mod & KMOD_SHIFT))
						break;
				case SDLK_KP_PLUS:
					if (mEditorState.macro < Song::maxMacros - 1)
						++mEditorState.macro;
					setMacro(mEditorState.macro);
					return true;

				case SDLK_COMMA:
					if (!(event.key.keysym.mod & KMOD_SHIFT))
						break;
				case SDLK_KP_MINUS:
					if (mEditorState.macro > 0)
						--mEditorState.macro;
					setMacro(mEditorState.macro);
					return true;

				case SDLK_CAPSLOCK:
				case SDLK_SCROLLLOCK:
					togglePositionFollowing();
					break;

				default:
					if (event.key.keysym.mod & (KMOD_CTRL|KMOD_ALT))
					{
						switch (event.key.keysym.sym)
						{
							case SDLK_s:
								displaySaveDialog();
								break;

							case SDLK_p:
								if (event.key.keysym.mod & KMOD_SHIFT)
									displayCommandPalette();
								else
									exportSong();
								break;

							case SDLK_o:
								displayLoadDialog();
								break;

							case SDLK_a:
								displayAudioDeviceDialog();
								break;

							case SDLK_n:
								newSong();
								showMessage(MessageInfo, "Song reset");
								break;
						}

						return true;
					}

					break;
			}

			break;

		case SDL_CONTROLLERBUTTONDOWN:
			switch (event.cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_START:
					if (mPlayerState.mode != PlayerState::Play)
						mPlayer.play(mEditorState.sequenceEditor.currentRow);
					else
						mPlayer.stop();
					return true;

				case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
					cycleFocus();
					return true;
			}

			break;
	}

	return false;
}


void MainEditor::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	if (shouldRedrawBackground())
		renderer.renderBackground(area);
}


void MainEditor::setMacro(int index)
{
	mEditorState.macro = index;
	macroNameEditor->setBuffer(mSong.getMacro(index).getName(), Macro::macroNameLength + 1);
}


void MainEditor::cycleFocus()
{
	int index = 0;
	Editor *currentFocus = getFocus();
	int numChildren = mChildren.size();

	for (; index < numChildren ; ++index)
		if (mChildren[index].editor == currentFocus)
		{
			break;
		}

	if (index >= numChildren)
		index = 0;

	do
	{
		index = (index + 1) % numChildren;
	}
	while (!mChildren[index].editor->isFocusable());

	setFocus(mChildren[index].editor);

	setDirty(true);
}


void MainEditor::syncPlayerState()
{
	if (mPlayerState.getUpdated() & PlayerState::Updated::PatternRelevant)
	{
		patternEditor->setDirty(true);

		if (mPlayerState.isPlaying() && mEditorState.followPlayPosition)
		{
			patternEditor->setSequenceRow(mPlayerState.sequenceRow);
			patternEditor->setPatternRow(mPlayerState.patternRow);
		}
	}

	if (mPlayerState.getUpdated() & PlayerState::Updated::OscillatorProbe)
	{
		*static_cast<Value*>(mOscillatorsProbePos) = mSynth.getProbePosition();
	}

	mPlayerState.ackUpdated(PlayerState::Updated::SequenceRow | PlayerState::Updated::PatternRow | PlayerState::Updated::OscillatorProbe);
}


void MainEditor::syncSongParameters(const Song& song)
{
	patternEditor->setMaxRows(song.getPatternLength());
	sequenceRowEditor->setMaxRows(song.getSequenceLength());
}


bool MainEditor::loadSong(const char *path)
{
	FILE *f = fopen(path, "rb");

	if (f)
	{
		fseek(f, 0, SEEK_END);
		int fileSize = ftell(f);
		fseek(f, 0, SEEK_SET);

		char *data = new char[fileSize];

		fread(data, fileSize, 1, f);
		fclose(f);

		FileSection *section = FileSection::openSection(data, fileSize);

		if (!section)
		{
			showMessage(MessageError, "Failed to read song data");

			delete section;
			delete[] data;
			return false;
		}

		mPlayer.stop();
		mPlayer.reset();

		Song::UnpackError result = mSong.unpack(*section);

		if (result != Song::Success)
		{
			switch (result)
			{
				default:
					break;

				case Song::NotASong:
					showMessage(MessageError, "File is not a song");
					break;

				case Song::ErrorVersion:
					showMessage(MessageError, "Song version is unsupported");
					break;

				case Song::ErrorRead:
					showMessage(MessageError, "Failed to read song data");
					break;
			}

			delete section;
			delete[] data;
			return false;
		}

		delete section;
		delete[] data;

		// Use reset() instead of assigning to avoid CopyBuffer double free
		mEditorState.reset();

		syncPlayerState();
		syncSongParameters(mSong);
		refreshAll();
		onLoaded();

		return true;
	}
	else
	{
		showMessageV(MessageError, "Could not open %s", path);
		return false;
	}
}


bool MainEditor::saveSong(const char *path)
{
	FileSection *section = mSong.pack();

	FILE *f = fopen(path, "wb");
	fwrite(section->getPackedData(), section->getPackedSize(), 1, f);
	fclose(f);

/*#ifndef __EMSCRIPTEN__
	FILE *f = fopen(path, "wb");
	fwrite(section->getPackedData(), section->getPackedSize(), 1, f);
	fclose(f);
#else
	const char urlStub[] = "window.open('data:application/octet-stream;base64,";
	char *urlOpen = new char[strlen(section->getBase64()) + strlen(urlStub) + 10];
	sprintf(urlOpen, "%s%s')", urlStub, section->getBase64());

	emscripten_run_script(urlOpen);

	delete[] urlOpen;
#endif*/
#ifdef __EMSCRIPTEN__
	emSyncFs();
#endif

	delete section;

	return true;
}


void MainEditor::refreshAll()
{
	mEditorState.patternEditor.currentRow.notify();
	mEditorState.macroEditor.currentRow.notify();
	mEditorState.sequenceEditor.currentRow.notify();
	songNameEditor->setDirty(true);
}


void MainEditor::onFileSelectorEvent(const Editor& selector, bool accept)
{


	// Close modal after accept - disable for CommandSelector so a new modal opened by
	// the command is not closed immediately.

	bool closeModal = true;

	if (accept)
	{
		int id = reinterpret_cast<const GenericSelector&>(selector).getId();
		switch (id)
		{
			case FileSelectionLoad:
				if (loadSong(reinterpret_cast<const FileSelector&>(selector).getSelectedPath()))
					showMessage(MessageInfo, "Song loaded");
				break;

			case FileSelectionSave:
				if (saveSong(reinterpret_cast<const FileSelector&>(selector).getSelectedPath()))
					showMessage(MessageInfo, "Song saved");
				else
					showMessage(MessageError, "Song was not saved");
				break;

			case AudioDeviceSelection:
				setAudioDevice(reinterpret_cast<const AudioDeviceSelector&>(selector).getSelectedDevice());
				break;

			case CommandSelection: {
				const CommandDescriptor& command = reinterpret_cast<const CommandSelector&>(selector).getSelectedCommand();
				setModal(NULL);
				closeModal = false;
				if (command.option)
					displayCommandOptionDialog(command);
				else
					command.func();
			} break;

			case CommandOptionSelection: {
				const CommandOptionSelector& optionSelector = reinterpret_cast<const CommandOptionSelector&>(selector);
				const CommandOptionSelector::CommandOption& option = optionSelector.getSelectedOption();
				setModal(NULL);
				closeModal = false;
				mSelectedCommand->funcWithOption(option.value);
			} break;
		}
	}

	if (closeModal)
		setModal(NULL);
}


void MainEditor::displayLoadDialog()
{
	fileSelector->setId(FileSelectionLoad);
	fileSelector->setTitle("Load song");
	fileSelector->setFilter(".song");
	fileSelector->setOverwriteCheck(false);
	fileSelector->populate();
	setModal(fileSelector);
}


void MainEditor::displaySaveDialog()
{
	fileSelector->setId(FileSelectionSave);
	fileSelector->setTitle("Save song");
	fileSelector->setFilter(".song");
	fileSelector->setOverwriteCheck(true);
	fileSelector->populate();
	setModal(fileSelector);
}


void MainEditor::displayAudioDeviceDialog()
{
	mMixer.buildDeviceList();
	audioDeviceSelector->setId(AudioDeviceSelection);
	audioDeviceSelector->setTitle("Select output device");
	audioDeviceSelector->populate(mMixer);
	setModal(audioDeviceSelector);
}


std::string MainEditor::getUserFile(const char *file) const
{
#ifndef __EMSCRIPTEN__
	return std::string(SDL_GetPrefPath(APP_NAME, APP_NAME)) + file;
#else
	return std::string("/persistent/") + file;
#endif
}


bool MainEditor::loadState()
{
	loadSong(getUserFile("worktune" SONG_EXT).c_str());

	FILE *f = fopen(getUserFile("editorstate").c_str(), "rb");

	if (!f)
		return false;

	fseek(f, 0, SEEK_END);
	int fileSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *data = new char[fileSize];

	fread(data, fileSize, 1, f);
	fclose(f);

	FileSection *section = FileSection::openSection(data, fileSize);

	if (!section)
	{
		showMessage(MessageError, "Could not restore editor state");

		delete section;
		delete[] data;

		return false;
	}

	if (!mEditorState.unpack(*section))
	{
		showMessage(MessageError, "Could not restore editor state");
	}
	else
	{
		showMessage(MessageInfo, "Editor state restored");
		setMacro(mEditorState.macro);
	}

	delete section;
	delete[] data;

	return true;
}


void MainEditor::saveState()
{
	saveSong(getUserFile("worktune" SONG_EXT).c_str());

	FileSection *section = mEditorState.pack();

	FILE *f = fopen(getUserFile("editorstate").c_str(), "wb");
	fwrite(section->getPackedData(), section->getPackedSize(), 1, f);
	fclose(f);

	showMessage(MessageInfo, "Editor state saved");

	//printf("%s\n", section->getBase64());

#ifdef __EMSCRIPTEN__
	emSyncFs();
#endif

	delete section;
}


bool MainEditor::loadElements(const Theme& theme)
{
	deleteChildren();

	setFocus(NULL);

	for (int index = 0 ; index < theme.getElementCount() ; ++index)
	{
		const Theme::Element& element = theme.getElement(index);

		switch (element.type)
		{
			case Theme::Oscilloscope:
			{
				Oscilloscope * oscilloscope = new Oscilloscope(mEditorState, mPlayer, element.parameters[4]);
				oscilloscope->setBuffer(mSynth.getOscillatorProbe(element.parameters[4]), ISynth::oscillatorProbeLength);
				mOscillatorsProbePos->addListener(oscilloscope);
				addChild(oscilloscope, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::PatternEditor:
			{
				patternEditor = new PatternEditor(mEditorState, mPlayer, mSong);
				patternEditor->setRowNumberMargin(element.parameters[4]);
				patternEditor->setTrackMargin(element.parameters[5]);
				addChild(patternEditor, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::MacroEditor:
			{
				MacroEditor *macroEditor = new MacroEditor(mEditorState, mPlayer, mSong);
				macroEditor->setRowNumberMargin(element.parameters[4]);
				macroEditor->setTrackMargin(element.parameters[5]);
				addChild(macroEditor, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::SequenceEditor:
			{
				sequenceRowEditor = new SequenceRowEditor(mEditorState, mPlayer, mSong);
				sequenceRowEditor->setRowNumberMargin(element.parameters[4]);
				sequenceRowEditor->setTrackMargin(element.parameters[5]);
				addChild(sequenceRowEditor, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::MacroNumber:
			{
				MacroInfo *macroInfo = new MacroInfo(mEditorState, mSong);
				addChild(macroInfo, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::OctaveNumber:
			{
				OctaveInfo *octaveInfo = new OctaveInfo(mEditorState);
				addChild(octaveInfo, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::SongName:
			{
				songNameEditor = new TextEditor(mEditorState);
				songNameEditor->setBuffer(mSong.getSongName(), Song::songNameLength + 1);
				songNameEditor->setSolidBackground(false);
				addChild(songNameEditor, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::MacroName:
			{
				macroNameEditor = new TextEditor(mEditorState);
				macroNameEditor->setSolidBackground(false);
				mEditorState.macro.addListener(macroNameEditor);
				addChild(macroNameEditor, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::SequencePosition:
			{
				SequenceInfo *sequenceInfo = new SequenceInfo(mEditorState);
				addChild(sequenceInfo, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::SequenceLength:
			{
				SongLengthInfo *songLength = new SongLengthInfo(mEditorState, mSong);
				addChild(songLength, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::PatternLength:
			{
				PatternLengthInfo *patternLength = new PatternLengthInfo(mEditorState, mSong);
				addChild(patternLength, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::TouchRegion:
			{
				TouchRegion *region = new TouchRegion(mEditorState, SDL_GetScancodeFromName(element.strParameters[0]), static_cast<SDL_Keymod>(0));
				addChild(region, element.parameters[0], element.parameters[1], element.parameters[2], element.parameters[3]);
			}
			break;

			case Theme::Unknown:
				break;
		}
	}

	if (patternEditor)
		setFocus(patternEditor);

	setMacro(0);

	mMessageDisplayer = new MessageDisplayer(mEditorState, *mMessageManager);
	addChild(mMessageDisplayer, 0, theme.getHeight() - 16, theme.getWidth(), 16);
	mTooltipDisplayer = new TooltipDisplayer(mEditorState, *mTooltipManager);
	addChild(mTooltipDisplayer, 0, 0, theme.getWidth(), theme.getHeight());

	return true;
}


bool MainEditor::exportSong()
{
	FileSection *section = mSong.pack();

#ifndef __EMSCRIPTEN__
	if (SDL_SetClipboardText(section->getBase64()))
	{
		showMessageV(MessageClass::MessageError, "SDL_GetError: %s", SDL_GetError());
	}

#else
	const char urlStub[] = "window.open('data:application/octet-stream;base64,";
	char *urlOpen = new char[strlen(section->getBase64()) + strlen(urlStub) + 10];
	sprintf(urlOpen, "%s%s')", urlStub, section->getBase64());

	emscripten_run_script(urlOpen);

	delete[] urlOpen;
#endif

	delete section;
	return true;
}


void MainEditor::playSong()
{
	mPlayer.play(mEditorState.sequenceEditor.currentRow);
	mEditorState.editMode = false;
	refreshAll();
}


void MainEditor::playPattern()
{
	mPlayer.play(mEditorState.sequenceEditor.currentRow, PlayerState::PlaySequenceRow);
	mEditorState.editMode = false;
	refreshAll();
}


void MainEditor::stopSong()
{
	mPlayer.stop();
	refreshAll();
}


void MainEditor::muteTracks()
{
	mPlayer.muteTracks();
}


void MainEditor::toggleEditMode()
{
	mEditorState.editMode = !mEditorState.editMode;
	refreshAll();
}


void MainEditor::togglePlayStop()
{
	if (mPlayerState.mode == PlayerState::Stop)
		playSong();
	else
		stopSong();
}


const std::string& MainEditor::getSongBase64()
{
	FileSection *section = mSong.pack();

	mBase64Encoded = section->getBase64();

	delete section;
	return mBase64Encoded;
}


void MainEditor::newSong()
{
	mSong.clear();
	mEditorState.reset();
	mSynth.reset();
	refreshAll();
}


int MainEditor::showMessageInner(MessageClass messageClass, int messageId, const char* message)
{
	return mMessageManager->pushMessage(messageClass, message, messageId);
}


void MainEditor::showTooltip(const SDL_Rect& area, const char* message)
{
	mTooltipManager->setTooltip(area, message);
}


void MainEditor::onUpdate(int ms)
{
	mMessageManager->update(ms);
	mTooltipManager->update(ms);
}


void MainEditor::setAudioDevice(const char *device)
{
	mMixer.stopThread();

	if (!mMixer.runThread(device))
	{
		showMessage(Editor::MessageError, "Could not open audio device");

		if (!mMixer.runThread(NULL))
		{
			showMessageV(Editor::MessageError, "No audio device found.");
		}
	}

	if (mMixer.getCurrentDeviceID() > 0)
	{
		showMessageV(Editor::MessageInfo, "Using %s", mMixer.getCurrentDeviceName());
	}

	const char *currentDevice = mMixer.getCurrentDeviceName();
	mEditorState.audioDevice = currentDevice ? currentDevice : "";
}

void MainEditor::setPatternLength(int length)
{
	mSong.setPatternLength(length);
}


bool MainEditor::registerCommand(const char *context, const char *commandName, Command command)
{
	mCommands.push_back(new CommandDescriptor(context, commandName, command));
	return true;
}


bool MainEditor::registerCommand(const char *context, const char *commandName, CommandWithOption command, CommandOptionFunc option)
{
	mCommands.push_back(new CommandDescriptor(context, commandName, command, option));
	return true;
}


int MainEditor::getNumCommands() const
{
	return mCommands.size();
}


const Editor::CommandDescriptor& MainEditor::getCommand(int index) const
{
	return *mCommands[index];
}


void MainEditor::displayCommandPalette()
{
	commandSelector->setId(CommandSelection);
	commandSelector->setTitle("Select command");
	commandSelector->populate();
	setModal(commandSelector);
}


void MainEditor::displayCommandOptionDialog(const CommandDescriptor& command)
{
	mSelectedCommand = &command;

	if (commandOptionSelector != NULL)
		delete commandOptionSelector;

	commandOptionSelector = new CommandOptionSelector(mEditorState, command);
	commandOptionSelector->setId(CommandOptionSelection);
	commandOptionSelector->setTitle(command.name);
	commandOptionSelector->populate();
	setModal(commandOptionSelector);
}


void MainEditor::onRequestCommandRegistration()
{
	registerCommand("Editor", "Toggle play position following", [this]() { this->togglePositionFollowing(); });
	registerCommand("Editor", "Toggle edit mode", [this]() { this->toggleEditMode(); });
	registerCommand("Song", "Reset song", [this]() { this->newSong(); this->showMessage(MessageInfo, "Song reset"); });
	registerCommand("Song", "Load song", [this]() { this->displayLoadDialog(); });
	registerCommand("Song", "Save song", [this]() { this->displaySaveDialog(); });
	registerCommand("Song", "Play song", [this]() { this->playSong(); });
	registerCommand("Song", "Play and loop pattern", [this]() { this->playPattern(); });
	registerCommand("Song", "Stop song", [this]() { this->stopSong(); });
	registerCommand("Editor", "Mute all tracks", [this]() { this->muteTracks(); });
	registerCommand("Editor", "Select output device", [this]() { this->displayAudioDeviceDialog(); });
	registerCommand("Song", "Set pattern length", [this](int value) {
		this->setPatternLength(value);
	}, [this](CommandOptionSelector& selector) {
		const int lengths[] = { 4, 16, 32, 48, 64, 128 };
		for (auto length : lengths)
			selector.addIntItem(length);
	});
}
