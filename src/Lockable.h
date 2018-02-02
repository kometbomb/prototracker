#pragma once

#include "SDL.h"

class Lockable
{
    SDL_mutex *mMutex;
	SDL_SpinLock mSpinlock;
	int mLockCounter;

public:
    Lockable();
    ~Lockable();

    void lock();
	void unlock();
};
