#include "Listenable.h"
#include "Listener.h"
#include <cstring>
#include <algorithm>

Listenable::Listenable()
{
}


void Listenable::notify()
{
	for (auto listener : mListeners)
	{
		listener->onListenableChange(this);
	}
}


bool Listenable::addListener(Listener *listener)
{
	mListeners.push_back(listener);

	return true;
}


void Listenable::removeListener(Listener *listener)
{
	auto iter = std::find(mListeners.begin(), mListeners.end(), listener);
	if (iter != mListeners.end())
		mListeners.erase(iter);
}
