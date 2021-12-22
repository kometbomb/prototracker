#include "Debug.h"
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
#include <cstring>
#include <algorithm>

#ifndef TUNING
#define TUNING 440.0
#endif

#ifndef SAMPLERATE
#define SAMPLERATE 44100
#endif

Mixer::Mixer(IPlayer& player, ISynth& synth)
	: mPlayer(player), mSynth(synth), mSampleRate(0),  mAudioOpened(false), mBuffer(NULL),
	mDeviceId(0)
{
#ifdef ENABLE_AUDIO_QUEUE
	mThread = NULL;
#endif
	mConvert = static_cast<SDL_AudioCVT*>(SDL_malloc(sizeof(SDL_AudioCVT)));
	buildDeviceList();
}


Mixer::~Mixer()
{
	deinitAudio();

	free(mConvert);
}


bool Mixer::runThread(const char *deviceName)
{
	debug("Opening audio device %s", deviceName);

	if (!initAudio(deviceName))
	{
		debug("Audio init failed");
		return false;
	}

#ifdef ENABLE_AUDIO_QUEUE
	mThreadRunning = true;
	mThread = SDL_CreateThread(queueThread, "QueueThread", this);

	if (!mThread)
	{
		debug("Thread was not created");
		return false;
	}
#endif

	SDL_PauseAudioDevice(mDeviceId, 0);

	return true;
}


void Mixer::stopThread()
{
	deinitAudio();
}


bool Mixer::initAudio(const char *deviceName)
{
	if (deviceName == NULL)
	{
		// Just use the first device that we can find
		buildDeviceList();
		if (getNumDevices() == 0)
			return false;

		return initAudio(getDevice(0));
	}
	SDL_AudioSpec want, have;

	SDL_memset(&want, 0, sizeof(want));

	want.freq = SAMPLERATE;
	want.format = AUDIO_S16SYS;
	want.channels = 2;
	want.samples = 1024;
	want.userdata = this;
#ifdef ENABLE_AUDIO_QUEUE
	want.callback = NULL;
#else
	want.callback = static_cast<SDL_AudioCallback>(audioCallback);
#endif

	if (mAudioOpened)
		SDL_CloseAudio();

	mAudioOpened = false;
	mDeviceId = SDL_OpenAudioDevice(deviceName, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);

	if (mDeviceId <= 0)
	{
		debug("Failed to open audio: %s", SDL_GetError());
		return false;
	}

	strncpy(mCurrentDevice, deviceName, MAX_DEVICE_NAME_SIZE);

	mAudioOpened = true;

	mSamples = 0;
	mSampleRate = have.freq;

	// We will not convert the sample rate, the synth simply runs at a different rate if
	// we don't get SAMPLERATE from the driver.

	SDL_BuildAudioCVT(mConvert, want.format, want.channels, mSampleRate, have.format, have.channels, mSampleRate);

#ifdef ENABLE_AUDIO_QUEUE
	mBufferSize = queueLengthTargetMs * mSampleRate / 1000;
#else
	mBufferSize = have.samples;
#endif

	debug("sampleRate = %d buffer = %d", mSampleRate, mBufferSize);

	if (mBuffer != NULL)
		delete[] mBuffer;

	mBuffer = new Sample16[mBufferSize * mConvert->len_mult];
	mSynth.setSampleRate(mSampleRate);

	return true;
}


void Mixer::deinitAudio()
{
#ifdef ENABLE_AUDIO_QUEUE
	mThreadRunning = false;

	if (mThread != NULL)
	{
		SDL_WaitThread(mThread, NULL);
    mThread = NULL;
	}
#endif

	if (mAudioOpened)
		SDL_CloseAudioDevice(mDeviceId);

	mAudioOpened = false;

	if (mBuffer != NULL)
		delete[] mBuffer;

	mBuffer = NULL;
	mDeviceId = 0;
}


#ifdef ENABLE_AUDIO_QUEUE
int Mixer::queueThread(void *data)
{
	Mixer& mixer = *reinterpret_cast<Mixer*>(data);
	return mixer.queueThreadInner();
}


int Mixer::queueThreadInner()
{
	debug("Running audio thread (deviceId = %d) %d", mDeviceId, mBufferSize);
	Uint64 lastUpdate = SDL_GetPerformanceCounter();
	Uint64 perfFreq = SDL_GetPerformanceFrequency();
	int minSliceLength = queueGranularityMs * mSampleRate / 1000;

	while (isThreadRunning())
	{
		// Buffer either the minimum buffer length
		// or if we are running low on buffer the target length

		int requestedSamples;

		if (queueLengthLowLimitMs * mSampleRate / 1000 > static_cast<int>(SDL_GetQueuedAudioSize(mDeviceId) / sizeof(Sample16)))
			requestedSamples = queueLengthTargetMs * mSampleRate / 1000;
		else
			requestedSamples = (SDL_GetPerformanceCounter() - lastUpdate) * mSampleRate / perfFreq;

		int samples = std::min(mBufferSize, requestedSamples);

		if (samples >= minSliceLength) {
			lastUpdate = SDL_GetPerformanceCounter();
			int bufferLength = samples * sizeof(Sample16);

			audioCallback(reinterpret_cast<void*>(this), reinterpret_cast<unsigned char*>(mBuffer), bufferLength);

			if (SDL_QueueAudio(mDeviceId, mBuffer, mConvert->len_cvt) != 0)
			{
				debug("SDL_QueueAudio: %s", SDL_GetError());
			}
		}
		else
		{
			SDL_Delay(1);
		}
	}

	debug("Audio thread exiting");

	return 0;
}
#endif


void Mixer::audioCallback(void* userdata, unsigned char* stream, int len)
{
	Uint64 startTicks = SDL_GetPerformanceCounter();
	Mixer& mixer = *static_cast<Mixer*>(userdata);
	IPlayer& player = mixer.getPlayer();
	Sample16 *data = mixer.mBuffer;
#ifdef ENABLE_AUDIO_QUEUE
	int length = len / sizeof(Sample16);
#else
	int length = mixer.mBufferSize;
#endif
	int chunk = mixer.getSampleRate() / player.getPlayerState().songRate;
	float hzConversion = static_cast<float>(TUNING / 2) / (float)mixer.getSampleRate(); // 1.0 = 440 Hz
	int samples = std::min(length, (chunk - mixer.getSamples() % chunk) % chunk);

	// debug("chunk = %d (%d buffer %d rate %d)", chunk, mixer.getSampleRate(), length, player.getPlayerState().songRate);

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

			oscillator.setVolume(trackState.trackState.volume * trackState.macroState.volume / ITrackState::maxVolume);
			oscillator.setEnabled(trackState.enabled);
		}

		chunk = mixer.getSampleRate() / player.getPlayerState().songRate;

		player.unlock();

		int toBeWritten = std::min(length - i, chunk);

		ISynth& synth = mixer.getSynth();
		synth.render(data + i, toBeWritten);
		synth.update(toBeWritten);
		mixer.getSamples() += toBeWritten;

		player.lock();
		player.getPlayerState().setUpdated(PlayerState::OscillatorProbe);
		player.unlock();
	}

	mixer.mConvert->len = length * sizeof(Sample16);
	mixer.mConvert->buf = reinterpret_cast<Uint8*>(mixer.mBuffer);
	SDL_ConvertAudio(mixer.mConvert);

	// Audio queue uses the mBuffer member so no need to copy if that was passed as
	// stream

	if (static_cast<void*>(stream) != static_cast<void*>(mixer.mBuffer))
	{
		memcpy(stream, mixer.mBuffer, len);
	}

	player.lock();
	player.getPlayerState().cpuUse = 100 * static_cast<float>(SDL_GetPerformanceCounter() - startTicks) / SDL_GetPerformanceFrequency() / (static_cast<float>(length) / mixer.getSampleRate());
	player.unlock();
}


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

#ifdef ENABLE_AUDIO_QUEUE
bool Mixer::isThreadRunning() const
{
	return mThreadRunning;
}
#endif

void Mixer::buildDeviceList()
{
	mNumDevices = SDL_GetNumAudioDevices(0);
	for (int index = 0 ; index < mNumDevices && index < MAX_DEVICES ; ++index)
	{
		strncpy(mDeviceList[index], SDL_GetAudioDeviceName(index, 0), MAX_DEVICE_NAME_SIZE);
	}
}


const int Mixer::getNumDevices() const
{
	return mNumDevices;
}


const char* Mixer::getDevice(int index) const
{
	return mDeviceList[index];
}


SDL_AudioDeviceID Mixer::getCurrentDeviceID() const
{
	return mDeviceId;
}


const char* Mixer::getCurrentDeviceName() const
{
	if (mDeviceId <= 0)
		return NULL;

	return mCurrentDevice;
}
