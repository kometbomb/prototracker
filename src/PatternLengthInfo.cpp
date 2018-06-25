#include "PatternLengthInfo.h"
#include "Song.h"
#include "Renderer.h"
#include "Color.h"
#include "EditorState.h"


PatternLengthInfo::PatternLengthInfo(EditorState& editorState, Song& song)
	: Label(editorState), mSong(song)
{
	mEditorState.macroEditor.currentRow.addListener(this);
}


PatternLengthInfo::~PatternLengthInfo()
{
}


void PatternLengthInfo::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	renderer.renderBackground(area);
	renderer.renderTextV(area, renderer.getTheme().getColor(Theme::ColorType::NormalText), "%02x", static_cast<int>(mSong.getPatternLength()));
}
