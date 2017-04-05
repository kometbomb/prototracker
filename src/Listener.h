#pragma once

struct Listenable;

class Listener
{
public:
	virtual void onListenableChange(Listenable *listenable) = 0;
};
