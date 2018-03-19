#ifndef Header_SuperpoweredExample
#define Header_SuperpoweredExample

#include <math.h>
#include <pthread.h>

#include "SuperpoweredPlayer.h"
#include <SuperpoweredAdvancedAudioPlayer.h>
#include <SuperpoweredFilter.h>
#include <SuperpoweredRoll.h>
#include <SuperpoweredFlanger.h>
#include <AndroidIO/SuperpoweredAndroidAudioIO.h>

#define HEADROOM_DECIBEL 3.0f
static const float headroom = powf(10.0f, -HEADROOM_DECIBEL * 0.025f);

class SuperpoweredPlayer {
public:

	SuperpoweredPlayer(unsigned int samplerate, unsigned int buffersize, const char *path, int fileAoffset, int fileAlength);
	~SuperpoweredPlayer();

	bool process(short int *output, unsigned int numberOfSamples);
	void onPlayPause(bool play);

private:
    SuperpoweredAndroidAudioIO *audioSystem;
    SuperpoweredAdvancedAudioPlayer *audioPlayer;

    float *stereoBuffer;
    unsigned char activeFx;
    float crossValue, volume;

public:
	int count=3;

};


#endif
