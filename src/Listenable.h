#pragma once

struct Editor;

/*

This interface defines the Listenable object that Listeners can subscribe to
and are notified when the Listenable changes. User by the GUI elements to 
know when they need a redraw.

*/

class Listenable
{
protected:
	static const int maxListeners = 128;
	
	Editor *mListeners[maxListeners];
	int mNumListeners;
public:
	Listenable();
	
	bool addListener(Editor *editor);
	void notify();
};
