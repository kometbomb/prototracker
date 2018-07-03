#include "{{Name}}.h"
#include "Renderer.h"
#include "Color.h"
#include "EditorState.h"


{{Name}}::{{Name}}(EditorState& editorState)
	: Editor(editorState)
{
	mEditorState.octave.addListener(this);
}


{{Name}}::~{{Name}}()
{
}


void {{Name}}::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	renderer.renderBackground(area);
	renderer.renderText(area, Theme::ColorType::NormalText, "{{Name}}");
}
