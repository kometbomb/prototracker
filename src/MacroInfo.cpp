#include "MacroInfo.h"
#include "Song.h"
#include "Renderer.h"
#include "Color.h"
#include "EditorState.h"


MacroInfo::MacroInfo(EditorState& editorState, Song& song)
	: Label(editorState), mSong(song)
{
	mEditorState.macro.addListener(this);
}


MacroInfo::~MacroInfo()
{
}


void MacroInfo::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	renderer.renderBackground(area);
	renderer.renderTextV(area, Theme::ColorType::NormalText, "%02x", static_cast<int>(mEditorState.macro));
}
