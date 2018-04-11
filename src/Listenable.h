#pragma once

struct Listener;

/*

This interface defines the Listenable object that Listeners can subscribe to
and are notified when the Listenable changes. User by the GUI elements to
know when they need a redraw.

*/

class Listenable
{
protected:
	static const int maxListeners = 128;

	Listener *mListeners[maxListeners];
	int mNumListeners;
public:
	Listenable();

	bool addListener(Listener *listener);
	void removeListener(Listener *listener);
	void notify();
};
