#include "MacroEditor.h"
#include "PatternRow.h"
#include "Song.h"
#include "Pattern.h"
#include "Macro.h"
#include "Color.h"
#include "EditorState.h"
#include "Renderer.h"

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

