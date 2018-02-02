#include "Lockable.h"

Lockable::Lockable()
    : mLockCounter(0), mSpinlock(0)
{
    mMutex = SDL_CreateMutex();
}


Lockable::~Lockable()
{
    SDL_DestroyMutex(mMutex);
}


void Lockable::lock()
{
	SDL_AtomicLock(&mSpinlock);
	if (++mLockCounter > 0)
	{
		SDL_AtomicUnlock(&mSpinlock);
		SDL_LockMutex(mMutex);
	}
	else
	{
		SDL_AtomicUnlock(&mSpinlock);
	}
}


void Lockable::unlock()
{
	SDL_AtomicLock(&mSpinlock);
	if (mLockCounter-- > 0)
	{
		SDL_AtomicUnlock(&mSpinlock);
		SDL_UnlockMutex(mMutex);
	}
	else
	{
		SDL_AtomicUnlock(&mSpinlock);
	}
}
