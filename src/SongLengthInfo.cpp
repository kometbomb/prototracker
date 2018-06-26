#include "SongLengthInfo.h"
#include "Song.h"
#include "Renderer.h"
#include "Color.h"
#include "EditorState.h"


SongLengthInfo::SongLengthInfo(EditorState& editorState, Song& song)
	: Label(editorState), mSong(song)
{
	mEditorState.sequenceEditor.currentRow.addListener(this);
}


SongLengthInfo::~SongLengthInfo()
{
}


void SongLengthInfo::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	renderer.renderBackground(area);
	renderer.renderTextV(area, Theme::ColorType::NormalText, "%02x", mSong.getSequenceLength());
}
