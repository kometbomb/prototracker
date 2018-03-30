#pragma once

struct IPlayer;
struct ISynth;
struct SDL_Thread;
struct SDL_AudioCVT;
struct Sample16;

#include "SDL.h"

class Mixer
{
	static const int MAX_DEVICE_NAME_SIZE = 200;
	static const int MAX_DEVICES = 32;

	IPlayer& mPlayer;
	ISynth& mSynth;

	int mSampleRate;
	int mSamples;
	bool mThreadRunning;
	bool mAudioOpened;
	SDL_Thread *mThread;
	SDL_AudioCVT *mConvert;
	Sample16 *mBuffer;
	int mBufferSize;
	char mCurrentDevice[MAX_DEVICE_NAME_SIZE];
	char mDeviceList[MAX_DEVICES][MAX_DEVICE_NAME_SIZE];
	int mNumDevices;
	SDL_AudioDeviceID mDeviceId;

	bool initAudio(const char *deviceName);
	void deinitAudio();

protected:
	static void audioCallback(void* userdata, unsigned char* stream, int len);
	IPlayer& getPlayer();
	ISynth& getSynth();

public:
	Mixer(IPlayer& player, ISynth& synth);
	~Mixer();
	bool isThreadRunning() const;
	bool runThread(const char *deviceName);
	void stopThread();
	int& getSamples();
	int getSampleRate() const;

	void buildDeviceList();
	const int getNumDevices() const;
	const char* getDevice(int index) const;
	const char* getCurrentDeviceName() const;
	SDL_AudioDeviceID getCurrentDeviceID() const;
};
