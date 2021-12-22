#include "PatchInfo.h"
#include "../Song.h"
#include "../Renderer.h"
#include "../Color.h"
#include "../EditorState.h"


PatchInfo::PatchInfo(EditorState& editorState)
	: Label(editorState)
{
	mEditorState.patch.addListener(this);
}


PatchInfo::~PatchInfo()
{
}


void PatchInfo::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	renderer.renderBackground(area);
	renderer.renderTextV(area, Theme::ColorType::NormalText, "%02x", static_cast<int>(mEditorState.patch));
}
