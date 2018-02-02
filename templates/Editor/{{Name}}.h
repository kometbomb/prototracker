#pragma once

#include "Editor.h"

class {{Name}}: public Editor
{
public:
	{{Name}}(EditorState& editorState);
	virtual ~{{Name}}();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
