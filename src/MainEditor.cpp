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
#include "SequenceRow.h"
#include "ITrackState.h"
#include "FileSection.h"
#include "OctaveInfo.h"
#include "TouchRegion.h"
#include "FileSelector.h"
#include "Emscripten.h"
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

MainEditor::MainEditor(EditorState& editorState, IPlayer& player, PlayerState& playerState, Song& song, ISynth& synth)
	: Editor(editorState), mPlayer(player), mPlayerState(playerState), mSong(song), mSynth(synth), mIsDragging(false)
{
	mOscillatorsUpdated = new Listenable();
	
	fileSelector = new FileSelector(editorState);
}


MainEditor::~MainEditor()
{
	delete mOscillatorsUpdated;
	delete fileSelector;
	
	deleteChildren();
}


void MainEditor::deleteChildren()
{
	for (int index = 0; index < mNumChildren ; ++index)	
		delete mChildren[index];
	
	mNumChildren = 0;
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

	
bool MainEditor::onEvent(SDL_Event& event)
{
	if (mModal != NULL)
	{
		return mModal->onEvent(event);
	}
	
	Editor* target = getFocus();

	
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		SDL_Point point = {event.button.x/SCALE, event.button.y/SCALE};
		
		for (int index = 0 ; index < mNumChildren ; ++index)
		{
			if (pointInRect(point, mChildrenArea[index]))
			{
				target = mChildren[index];
				break;
			}
		}
		
		startDragging(event.button.x, event.button.y);
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		stopDragging();
	}
#if MOUSEDRAG
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
		return true;
	}
	
	if (target->isFocusable())
	{
		setFocus(target);
	}
	
	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{	
			case SDLK_F1:
				mEditorState.octave = std::max(0, mEditorState.octave - 1);
				return true;
				
			case SDLK_F2:
				mEditorState.octave = std::min(15, mEditorState.octave + 1);
				return true;
				
			case SDLK_F9:
				mSong.setPatternLength(std::max(1, mSong.getPatternLength() - 1));
				refreshAll();
				return true;
				
			case SDLK_F10:
				mSong.setPatternLength(std::min(Pattern::maxRows, mSong.getPatternLength() + 1));
				refreshAll();
				return true;
			
			case SDLK_1:
			case SDLK_2:
			case SDLK_3:
			case SDLK_4:
			
				if (event.key.keysym.mod & KMOD_ALT)
				{
					mPlayer.getTrackState(event.key.keysym.sym - SDLK_1).enabled ^= true;
					return true;
				}
				
				break;
				
			case SDLK_F5:
			case SDLK_RCTRL:
				mPlayer.play(mEditorState.sequenceEditor.currentRow);
				mEditorState.editMode = false;
				refreshAll();
				return true;
			
			case SDLK_F6:
			case SDLK_RSHIFT:
				mPlayer.play(mEditorState.sequenceEditor.currentRow, PlayerState::PlaySequenceRow);
				mEditorState.editMode = false;
				refreshAll();
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
						mEditorState.editMode = !mEditorState.editMode;
						refreshAll();
					}
					mPlayer.muteTracks();
				}
				else
				{
					mEditorState.editMode = !mEditorState.editMode;
					refreshAll();
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
				mEditorState.followPlayPosition = !mEditorState.followPlayPosition;
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
							exportSong();
							break;
							
						case SDLK_o:
							displayLoadDialog();
							break;
							
						case SDLK_n:
							newSong();
							break;
					}
					
					return true;
				}
				
				break;
		}
		
		
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
	
	for (; index < mNumChildren ; ++index)
		if (mChildren[index] == currentFocus)
		{
			break;
		}
		
	if (index >= mNumChildren)
		index = 0;
	
	do
	{
		index = (index + 1) % mNumChildren;
	} 
	while (!mChildren[index]->isFocusable());
	
	setFocus(mChildren[index]);
	
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
		mOscillatorsUpdated->notify();
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
			printf("Error!\n");
			delete section;
			delete[] data;
			return false;
		}
		
		mPlayer.stop();
		mPlayer.reset();
				
		if (!mSong.unpack(*section))
		{
			// popup here
			printf("Error!\n");
			delete section;
			delete[] data;
			return false;
		}
	
		delete section;
		delete[] data;
		
		mEditorState = EditorState();
		
		syncPlayerState();
		syncSongParameters(mSong);
		refreshAll();
		
		return true;
	}
	else
	{
		printf("Error!\n");
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


void MainEditor::onFileSelectorEvent(const Editor& fileSelector, bool accept)
{
	if (accept)
	{
		int id = reinterpret_cast<const FileSelector&>(fileSelector).getId();
		switch (id)
		{
			case FileSelectionLoad:
				loadSong(reinterpret_cast<const FileSelector&>(fileSelector).getSelectedPath());
				break;
				
			case FileSelectionSave:
				saveSong(reinterpret_cast<const FileSelector&>(fileSelector).getSelectedPath());
				break;
		}
	}
		
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
		printf("Error!\n");
		
		delete section;
		delete[] data;
		
		return false;
	}
	
	if (!mEditorState.unpack(*section))
	{
		printf("EditorState load failed\n");
	}
	
	delete section;
	delete[] data;
	
	setMacro(mEditorState.macro);
	
	return true;
}


void MainEditor::saveState()
{
	saveSong(getUserFile("worktune" SONG_EXT).c_str());
	
	FileSection *section = mEditorState.pack();
	
	FILE *f = fopen(getUserFile("editorstate").c_str(), "wb");
	fwrite(section->getPackedData(), section->getPackedSize(), 1, f);
	fclose(f);	
	
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
				mOscillatorsUpdated->addListener(oscilloscope);
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
				sequenceRowEditor = new SequenceRowEditor(mEditorState, mSong);
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
	
	return true;
}


bool MainEditor::exportSong()
{
	FileSection *section = mSong.pack();
	
#ifndef __EMSCRIPTEN__
	if (SDL_SetClipboardText(section->getBase64()))
	{
		printf("err: %s\n", SDL_GetError());
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


void MainEditor::stopSong()
{
	mPlayer.stop();
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
	mEditorState = EditorState();
	refreshAll();
}
