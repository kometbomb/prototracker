#include "MessageManager.h"

MessageManager::Message::Message(Editor::MessageClass _messageClass, const std::string& _message)
	: delay(MessageAnimationMs * 2 + MessageVisibleMs), messageClass(_messageClass), text(_message)
{
}


float MessageManager::Message::getVisibility() const
{
	if (delay > MessageAnimationMs && delay < MessageVisibleMs + MessageAnimationMs)
		return 1.0;
	
	if (delay <= MessageAnimationMs)
		return static_cast<float>(delay) / MessageAnimationMs;
	
	if (delay > MessageAnimationMs + MessageVisibleMs)
		return 1.0f - static_cast<float>(delay - MessageAnimationMs - MessageVisibleMs) / MessageAnimationMs;
	
	return 0.0f;
}


MessageManager::MessageManager()
	: mWasVisible(false)
{
}


void MessageManager::update(int ms)
{
	if (mMessageQueue.empty())
	{
		mWasVisible = false;
		return;
	}
	
	mMessageQueue.front().delay -= ms;
	
	if (mMessageQueue.front().delay <= 0)
	{
		mMessageQueue.pop();
	}
	
	mWasVisible = true;
}


void MessageManager::pushMessage(Editor::MessageClass messageClass, const std::string& message)
{
	mMessageQueue.push(Message(messageClass, message));
}


const MessageManager::Message* MessageManager::getVisibleMessage() const
{
	if (mMessageQueue.empty())
		return NULL;
	
	return &mMessageQueue.front();
}


bool MessageManager::getWasMessageVisibleLastFrame() const
{
	return mWasVisible;
}
