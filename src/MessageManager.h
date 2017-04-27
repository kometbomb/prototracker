#pragma once

#include <queue>
#include <string>
#include "Editor.h"

class MessageManager 
{
public:
	static const int MessageVisibleMs = 2000;
	static const int MessageAnimationMs = 100;

	struct Message 
	{
		// Remaining milliseconds the message should still be displayed 
		// value (will decrease)
		int delay;
		Editor::MessageClass messageClass;
		std::string text;
		
		Message(Editor::MessageClass messageClass, const std::string& message);
		
		float getVisibility() const;
	};
	
private:
	bool mWasVisible;
	std::queue<Message> mMessageQueue;
	
public:
	MessageManager();
	void update(int ms);
	void pushMessage(Editor::MessageClass messageClass, const std::string& message);
	
	/* Get topmost queued message
	 */
	const Message* getVisibleMessage() const;
	
	/* Returns true if there was a message in the queue since last update()
	 */
	bool getWasMessageVisibleLastFrame() const;
};
