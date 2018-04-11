#include "Listenable.h"
#include "Listener.h"
#include <cstring>

Listenable::Listenable()
	: mNumListeners(0)
{
}


void Listenable::notify()
{
	for (int i = 0 ; i < mNumListeners ; ++i)
	{
		mListeners[i]->onListenableChange(this);
	}
}


bool Listenable::addListener(Listener *listener)
{
	if (mNumListeners >= maxListeners)
		return false;

	mListeners[mNumListeners++] = listener;

	return true;
}


void Listenable::removeListener(Listener *listener)
{
	for (int i = 0 ; i < mNumListeners ; ++i)
	{
		if (listener == mListeners[i])
		{
			memmove(&mListeners[i], &mListeners[i + 1], mNumListeners - i - 1);
			mNumListeners--;
			return;
		}
	}
}
