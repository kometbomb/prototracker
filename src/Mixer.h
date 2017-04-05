#pragma once

struct Player;
struct Synth;
struct SDL_Thread;
struct SDL_AudioCVT;
struct Sample16;

class Mixer
{
	Player& mPlayer;
	Synth& mSynth;
	
	int mSampleRate;
	int mSamples;
	bool mThreadRunning;
	bool mAudioOpened;
	SDL_Thread *mThread;
	SDL_AudioCVT *mConvert;
	Sample16 *mBuffer;
	int mBufferSize;
	
	void initAudio();
	void deinitAudio();
	
protected:
	void queueAudio();
	//static int queueThread(void *userdata);
	static void audioCallback(void* userdata, unsigned char* stream, int len);
	Player& getPlayer();
	Synth& getSynth();

public:
	Mixer(Player& player, Synth& synth);
	~Mixer();
	bool isThreadRunning() const;
	void runThread();
	void runQueueThread();
	int& getSamples();
	int getSampleRate() const;
};
