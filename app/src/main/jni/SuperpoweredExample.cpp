#include "SuperpoweredExample.h"
#include <SuperpoweredSimple.h>
#include <SuperpoweredCPU.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

static void playerEventCallbackA(void *clientData, SuperpoweredAdvancedAudioPlayerEvent event, void * __unused value) {
    if (event == SuperpoweredAdvancedAudioPlayerEvent_LoadSuccess) {
    	SuperpoweredAdvancedAudioPlayer *playerA = *((SuperpoweredAdvancedAudioPlayer **)clientData);
        playerA->setBpm(126.0f);
        playerA->setFirstBeatMs(353);
        playerA->setPosition(playerA->firstBeatMs, false, false);
    };
}


static bool audioProcessing(void *clientdata, short int *audioIO, int numberOfSamples, int __unused samplerate) {
	return ((SuperpoweredExample *)clientdata)->process(audioIO, (unsigned int)numberOfSamples);
}

SuperpoweredExample::SuperpoweredExample(unsigned int samplerate, unsigned int buffersize,
                                         const char *path, int fileAoffset, int fileAlength)
        : activeFx(0), crossValue(0.0f), volume(1.0f * headroom) {
    stereoBuffer = (float *)memalign(16, (buffersize + 16) * sizeof(float) * 2);

    audioPlayer = new SuperpoweredAdvancedAudioPlayer(&audioPlayer , playerEventCallbackA, samplerate, 0);
    audioPlayer->open(path, fileAoffset, fileAlength);


    audioSystem = new SuperpoweredAndroidAudioIO(samplerate, buffersize, false, true, audioProcessing,
                                                 this, -1, SL_ANDROID_STREAM_MEDIA, buffersize * 2);
}

SuperpoweredExample::~SuperpoweredExample() {
    delete audioSystem;
    delete audioPlayer;
    free(stereoBuffer);
}

void SuperpoweredExample::onPlayPause(bool play) {
    if (!play) {
        audioPlayer->pause();

    } else {
        bool masterIsA = (crossValue <= 0.5f);
        audioPlayer->play(!masterIsA);

    };
    SuperpoweredCPU::setSustainedPerformanceMode(play); // <-- Important to prevent audio dropouts.
}


#define MINFREQ 60.0f
#define MAXFREQ 20000.0f

static inline float floatToFrequency(float value) {
    if (value > 0.97f) return MAXFREQ;
    if (value < 0.03f) return MINFREQ;
    value = powf(10.0f, (value + ((0.4f - fabsf(value - 0.4f)) * 0.3f)) * log10f(MAXFREQ - MINFREQ)) + MINFREQ;
    return value < MAXFREQ ? value : MAXFREQ;
}


bool SuperpoweredExample::process(short int *output, unsigned int numberOfSamples) {

    bool silence = !audioPlayer->process(stereoBuffer, false, numberOfSamples);

    // The stereoBuffer is ready now, let's put the finished audio into the requested buffers.
    if (!silence)
        SuperpoweredFloatToShortInt(stereoBuffer, output, numberOfSamples);
    return !silence;
}

static SuperpoweredExample *example = NULL;

extern "C" JNIEXPORT void Java_com_superpowered_player_MainActivity_SuperpoweredExample(JNIEnv *javaEnvironment, jobject __unused obj, jint samplerate, jint buffersize, jstring apkPath, jint fileAoffset, jint fileAlength, jint fileBoffset, jint fileBlength) {
    const char *path = javaEnvironment->GetStringUTFChars(apkPath, JNI_FALSE);
    example = new SuperpoweredExample((unsigned int)samplerate, (unsigned int)buffersize, path, fileAoffset, fileAlength);
    javaEnvironment->ReleaseStringUTFChars(apkPath, path);
}

extern "C" JNIEXPORT void Java_com_superpowered_player_MainActivity_onPlayPause(JNIEnv * __unused javaEnvironment, jobject __unused obj, jboolean play) {
	example->onPlayPause(play);
}