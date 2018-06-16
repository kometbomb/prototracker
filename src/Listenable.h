#pragma once

struct Listener;

#include <vector>

/*

This interface defines the Listenable object that Listeners can subscribe to
and are notified when the Listenable changes. User by the GUI elements to
know when they need a redraw.

*/

class Listenable
{
protected:
	std::vector<Listener*> mListeners;

public:
	Listenable();

	bool addListener(Listener *listener);
	void removeListener(Listener *listener);
	void notify();
};
