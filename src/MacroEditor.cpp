#include "MacroEditor.h"
#include "PatternRow.h"
#include "Song.h"
#include "Pattern.h"
#include "Macro.h"
#include "Color.h"
#include "EditorState.h"
#include "Renderer.h"
#include "CommandOptionSelector.h"

MacroEditor::MacroEditor(EditorState& editorState, IPlayer& player, Song& song)
	: TrackEditor(editorState, editorState.macroEditor, player, song, 1)
{
	editorState.macro.addListener(this);
	setTriggerNotes(false);
	setAddMacroEffect(false);
}


MacroEditor::~MacroEditor()
{

}


bool MacroEditor::onEvent(SDL_Event& event)
{
	return TrackEditor::onEvent(event);
}


PatternRow& MacroEditor::getCurrentPatternRow()
{
	Macro& macro = mSong.getMacro(mEditorState.macro);

	return macro.getRow(mTrackEditorState.currentRow);
}


PatternRow& MacroEditor::getPatternRow(int track, int row)
{
	Macro& macro = mSong.getMacro(mEditorState.macro);

	return macro.getRow(row);
}


Pattern& MacroEditor::getCurrentPattern(int track)
{
	return mSong.getMacro(mEditorState.macro);
}


void MacroEditor::findUnusedTrack(int track)
{
	int orig = mEditorState.macro;

	while (!mSong.getMacro(mEditorState.macro).isEmpty())
	{
		mEditorState.macro = (mEditorState.macro + 1) % Song::maxMacros;

		// Check if we are back at starting position

		if (orig == mEditorState.macro)
			break;
	}
}


void MacroEditor::onRequestCommandRegistration()
{
	registerCommand("Macro", "Mark block start", [this]() { this->setBlockStartToCurrentRow(); }, SDLK_b, KMOD_CTRL);
	registerCommand("Macro", "Mark block end", [this]() { this->setBlockEndToCurrentRow(); }, SDLK_e, KMOD_CTRL);
	registerCommand("Macro", "Find unused macro", [this]() { this->findCurrentUnusedTrack(); }, SDLK_u, KMOD_CTRL);
	registerCommand("Macro", "Kill current macro", [this]() { this->killCurrentTrack(); }, SDLK_k, KMOD_CTRL);
	registerCommand("Macro", "Copy macro", [this]() { this->copyCurrentTrack(); }, SDLK_F3);
	registerCommand("Macro", "Paste macro", [this]() { this->pasteCurrentTrack(); }, SDLK_F4);
	registerCommand("Macro", "Copy macro block", [this]() { this->copyCurrentBlock(); }, SDLK_c, KMOD_CTRL);
	registerCommand("Macro", "Paste macro block", [this]() { this->pasteCurrentBlock(); }, SDLK_v, KMOD_CTRL);
	registerCommand("Macro", "Set edit skip", [this](int value) {
		this->setEditSkip(value);
	}, [this](CommandOptionSelector& selector) {
		for (int i = 0 ; i <= 16 ; ++i)
			selector.addIntItem(i);
	});
}
