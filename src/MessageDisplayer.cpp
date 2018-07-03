#include "MessageDisplayer.h"
#include "MessageManager.h"
#include "Renderer.h"
#include "Color.h"

MessageDisplayer::MessageDisplayer(EditorState& editorState, MessageManager& manager)
	: Editor(editorState, false), mMessageManager(manager)
{
}


void MessageDisplayer::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	const MessageManager::Message *message = mMessageManager.getVisibleMessage();

	if (message)
	{
		/* Center text inside the display area
		 */

		SDL_Rect textArea = renderer.getTextRect(message->text.c_str());
		SDL_Rect finalArea = area;
		finalArea.x = area.x + area.w / 2 - textArea.w / 2;
		finalArea.y = area.y + area.h / 2 - textArea.h / 2;
		finalArea.w = textArea.w;
		finalArea.h = textArea.h;

		float scale = message->getVisibility();
		SDL_Rect frameArea;
		frameArea.x = finalArea.x + finalArea.w / 2 - (finalArea.w / 2 + 2) * scale;
		frameArea.y = finalArea.y + finalArea.h / 2 - (finalArea.h / 2 + 2) * scale;
		frameArea.w = (finalArea.w + 4) * scale;
		frameArea.h = (finalArea.h + 4) * scale;

		if (frameArea.w > 0 && frameArea.h > 0)
		{
			renderer.setClip(frameArea);
			renderer.clearRect(frameArea, Theme::ColorType::ModalBackground);
			renderer.drawRect(frameArea, Theme::ColorType::ModalBorder);

			renderer.renderText(finalArea, Theme::ColorType::NormalText, message->text.c_str());
		}
	}
}


void MessageDisplayer::onUpdate(int ms)
{
	if (mMessageManager.getVisibleMessage() != NULL || mMessageManager.getWasMessageVisibleLastFrame())
		setDirty(true);
}
