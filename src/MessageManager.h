#pragma once

#include <list>
#include <string>
#include "Editor.h"

class MessageManager
{
public:
	static const int MessageVisibleMs = 2000;
	static const int MessageAnimationMs = 100;

	struct Message
	{
		int id;
		// Remaining milliseconds the message should still be displayed
		// value (will decrease)
		int delay;
		Editor::MessageClass messageClass;
		std::string text;

		Message(int id, Editor::MessageClass messageClass, const std::string& message);
		void resetDelay();

		float getVisibility() const;
	};

private:
	bool mWasVisible;
	std::list<Message> mMessageQueue;
	int mMessageIDCounter;

public:
	MessageManager();
	void update(int ms);

	/* Push a new message in the queue or replace the message with the matching ID
	 * Returns the message ID for the new message
	 */
	int pushMessage(Editor::MessageClass messageClass, const std::string& message, int messageId = 0);

	/* Get topmost queued message
	 */
	const Message* getVisibleMessage() const;

	/* Returns true if there was a message in the queue since last update()
	 */
	bool getWasMessageVisibleLastFrame() const;
};
