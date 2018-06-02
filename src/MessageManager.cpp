#include "MessageManager.h"
#include <algorithm>

MessageManager::Message::Message(int _id, Editor::MessageClass _messageClass, const std::string& _message)
	: id(_id), messageClass(_messageClass), text(_message)
{
	resetDelay();
}


void MessageManager::Message::resetDelay()
{
	delay = MessageAnimationMs * 2 + MessageVisibleMs;
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
	: mWasVisible(false), mMessageIDCounter(1)
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
		mMessageQueue.pop_front();
	}

	mWasVisible = true;
}


int MessageManager::pushMessage(Editor::MessageClass messageClass, const std::string& message, int messageId)
{
	auto foundItem = std::find_if(mMessageQueue.begin(), mMessageQueue.end(),
		[&messageId](const Message& message) -> bool { return message.id == messageId; });

	if (foundItem != mMessageQueue.end())
	{
		// Replace old

		foundItem->messageClass = messageClass;
		foundItem->text = message;
		foundItem->resetDelay();
		return messageId;
	}
	else
	{
		// Insert new

		mMessageQueue.push_back(Message(mMessageIDCounter, messageClass, message));
		return mMessageIDCounter++;
	}
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
