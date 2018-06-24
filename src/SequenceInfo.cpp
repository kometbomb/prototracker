#include "SequenceInfo.h"
#include "Song.h"
#include "Renderer.h"
#include "Color.h"
#include "EditorState.h"


SequenceInfo::SequenceInfo(EditorState& editorState)
	: Label(editorState)
{
	mEditorState.sequenceEditor.currentRow.addListener(this);
}


SequenceInfo::~SequenceInfo()
{
}


void SequenceInfo::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	renderer.renderBackground(area);
	renderer.renderTextV(area, renderer.getTheme().getColor(Theme::ColorType::NormalText), "%02x", static_cast<int>(mEditorState.sequenceEditor.currentRow));
}
