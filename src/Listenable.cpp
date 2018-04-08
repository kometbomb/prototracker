#include "Listenable.h"
#include "Listener.h"

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
