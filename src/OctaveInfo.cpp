#include "OctaveInfo.h"
#include "Song.h"
#include "Renderer.h"
#include "Color.h"
#include "EditorState.h"


OctaveInfo::OctaveInfo(EditorState& editorState)
	: Label(editorState)
{
	mEditorState.octave.addListener(this);
}


OctaveInfo::~OctaveInfo()
{
}


void OctaveInfo::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	renderer.renderBackground(area);
	renderer.renderTextV(area, Theme::ColorType::NormalText, "%02x", static_cast<int>(mEditorState.octave));
}
