#include "Mixer.h"
#include "SDL.h"
#include "IPlayer.h"
#include "Sample.h"
#include "ISynth.h"
#include "SequenceRow.h"
#include "ITrackState.h"
#include "IOscillator.h"
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

#ifndef TUNING
#define TUNING 440.0
#endif

#ifndef SAMPLERATE
#define SAMPLERATE 44100
#endif

Mixer::Mixer(IPlayer& player, ISynth& synth)
	: mPlayer(player), mSynth(synth), mSampleRate(0), mThread(NULL), mAudioOpened(false), mBuffer(NULL)
{
	mConvert = static_cast<SDL_AudioCVT*>(SDL_malloc(sizeof(SDL_AudioCVT)));
}


Mixer::~Mixer()
{
	deinitAudio();
	
	free(mConvert);
}


bool Mixer::runThread()
{
	if (!initAudio())
		return false;
	
	SDL_PauseAudio(0);
	
	return true;
}


void Mixer::stopThread()
{
	deinitAudio();
}


/*void Mixer::runQueueThread()
{
	SDL_Thread *mThread = SDL_CreateThread(queueThread, "", this);
}*/


bool Mixer::initAudio()
{
	SDL_AudioSpec want, have;

	SDL_memset(&want, 0, sizeof(want));
	
	want.freq = SAMPLERATE;
	want.format = AUDIO_S16SYS;
	want.channels = 2;
	want.samples = 1024;
	want.callback = static_cast<SDL_AudioCallback>(audioCallback); 
	want.userdata = this;
	
	if (mAudioOpened)
		SDL_CloseAudio();
	
	mAudioOpened = false;

	if (SDL_OpenAudio(&want, &have) < 0) 
	{
		/*fprintf(stderr, "Failed to open audio: %s\n", SDL_GetError());
		exit(1);*/
		return false;
	} 
	
	mAudioOpened = true;
	
	mSampleRate = have.freq;
	mSamples = 0;
	mBufferSize = have.samples;
	
	if (mBuffer != NULL)
		delete[] mBuffer;
		
	mBuffer = new Sample16[mBufferSize];
	
	SDL_BuildAudioCVT(mConvert, want.format, want.channels, have.freq, have.format, have.channels, have.freq);
	
	//printf("Got %d Hz format=%d (wanted %d Hz/%d) buffer = %d\n", have.freq, have.format, want.freq, want.format, want.samples);
	
	mSynth.setSampleRate(mSampleRate);
	
	return true;
}


void Mixer::deinitAudio()
{
	mThreadRunning = false;
	
	if (mThread != NULL)
	{
		SDL_WaitThread(mThread, NULL);
	}
	
	if (mAudioOpened)
		SDL_CloseAudio();
	
	mAudioOpened = false;
	
	if (mBuffer != NULL)
		delete[] mBuffer;
	
	mBuffer = NULL;
}


void Mixer::audioCallback(void* userdata, unsigned char* stream, int len)
{
	Mixer& mixer = *static_cast<Mixer*>(userdata);
	IPlayer& player = mixer.getPlayer();
	
	Sample16 *data = reinterpret_cast<Sample16*>(stream);
	int length = mixer.mBufferSize; //len / sizeof(Sample16);
	int chunk = mixer.getSampleRate() / player.getPlayerState().songRate; 
	float hzConversion = static_cast<float>(TUNING / 2) / (float)mixer.getSampleRate(); // 1.0 = 440 Hz
	int samples = std::min(length, (chunk - mixer.getSamples() % chunk) % chunk);
	
	/*
	 * Render "leftovers" before the next sequence tick
	 */
	
	if (samples > 0)
	{
		mixer.getSynth().render(data, samples);
		mixer.getSynth().update(samples);
		mixer.getSamples() += samples;
		
		player.lock();
		player.getPlayerState().setUpdated(PlayerState::OscillatorProbe);
		player.unlock();
	}
	
	/*
	 * Fill the rest of the buffer and handle ticks
	 */
	 	
	for (int i = samples ; i < length ; i += chunk)
	{
		player.lock();
		
		player.runTick();
		player.advanceTick();
		
		for (int track = 0 ; track < SequenceRow::maxTracks ; ++track)
		{
			ITrackState& trackState = player.getTrackState(track);
			IOscillator& oscillator = mixer.getSynth().getOscillator(track);
			
			if (trackState.triggered)
			{
				trackState.triggered = false;
				oscillator.triggerNote();
			}
				
			oscillator.handleTrackState(trackState);
			oscillator.setFrequency(trackState.trackState.frequency * trackState.macroState.frequency * hzConversion);
			
			if (trackState.enabled)
				oscillator.setVolume(trackState.trackState.volume * trackState.macroState.volume / ITrackState::maxVolume);
			else
				oscillator.setVolume(0);
		}
		
		chunk = mixer.getSampleRate() / player.getPlayerState().songRate;
			
		player.unlock();
		
		int toBeWritten = std::min(length - i, chunk);
		
		mixer.getSynth().render(data + i, toBeWritten);
		mixer.getSynth().update(toBeWritten);
		mixer.getSamples() += toBeWritten;
		
		player.lock();
		player.getPlayerState().setUpdated(PlayerState::OscillatorProbe);
		player.unlock();
	}
	
	mixer.mConvert->len = length * sizeof(Sample16);
	mixer.mConvert->buf = stream;
	SDL_ConvertAudio(mixer.mConvert);
}


/*void Mixer::queueAudio()
{
	Player& player = getPlayer();
	int chunk = getSampleRate() / 50; // 50 Hz
	Sample16 *data = new Sample16[chunk];
	float hzConversion = 440.0f / (float)getSampleRate(); // 1.0 = 440 Hz
	
	player.lock();
		
	player.runTick();
	player.advanceTick();
	
	for (int track = 0 ; track < SequenceRow::maxTracks ; ++track)
	{
		TrackState& trackState = player.getTrackState(track);
		Oscillator& oscillator = getSynth().getOscillator(track);
		oscillator.setFrequency(trackState.trackState.frequency * trackState.macroState.frequency * hzConversion);
		oscillator.setVolume(trackState.trackState.volume);
	}
		
	player.unlock();
	
	getSynth().render(data, chunk);
	getSynth().update(chunk);
	getSamples() += chunk;
	
	SDL_QueueAudio(1, data, sizeof(Sample16) * chunk);
	
	delete[]data;
}


int Mixer::queueThread(void *userdata)
{
	Mixer& mixer = *static_cast<Mixer*>(userdata);
	
	while(mixer.isThreadRunning())
	{
		mixer.queueAudio();
		SDL_Delay(20);
	}
	
	return 0;
}*/


int Mixer::getSampleRate() const
{
	return mSampleRate;
}


IPlayer& Mixer::getPlayer()
{
	return mPlayer;
}

ISynth& Mixer::getSynth()
{
	return mSynth;
}


int& Mixer::getSamples()
{
	return mSamples;
}


bool Mixer::isThreadRunning() const
{
	return mThreadRunning;
}
