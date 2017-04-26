#pragma once

struct IPlayer;
struct ISynth;
struct SDL_Thread;
struct SDL_AudioCVT;
struct Sample16;

class Mixer
{
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
	
	bool initAudio();
	void deinitAudio();
	
protected:
	void queueAudio();
	//static int queueThread(void *userdata);
	static void audioCallback(void* userdata, unsigned char* stream, int len);
	IPlayer& getPlayer();
	ISynth& getSynth();

public:
	Mixer(IPlayer& player, ISynth& synth);
	~Mixer();
	bool isThreadRunning() const;
	bool runThread();
	void stopThread();
	void runQueueThread();
	int& getSamples();
	int getSampleRate() const;
};
