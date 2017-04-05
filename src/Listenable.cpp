#include "Listenable.h"
#include "Editor.h"

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


bool Listenable::addListener(Editor *listener)
{
	if (mNumListeners >= maxListeners)
		return false;
	
	mListeners[mNumListeners++] = listener;
	
	return true;
}


