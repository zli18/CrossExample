#include "SuperpoweredPlayer.h"
#include <SuperpoweredSimple.h>
#include <SuperpoweredCPU.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

static void playerEventCallback(void *clientData,
                                SuperpoweredAdvancedAudioPlayerEvent event,
                                void *__unused value) {

    //SuperpoweredAdvancedAudioPlayer *player = *((SuperpoweredAdvancedAudioPlayer **) clientData);
    if (event == SuperpoweredAdvancedAudioPlayerEvent_LoadSuccess) {
        //SuperpoweredAdvancedAudioPlayer *player = *((SuperpoweredAdvancedAudioPlayer **)clientData);
        //player->setBpm(126.0f);
        //player->setFirstBeatMs(353);
        //player->setPosition(player->firstBeatMs, false, false);
        //player->looping = false;
    }

    if (event == SuperpoweredAdvancedAudioPlayerEvent_EOF) {


        //bool *stopLooping = (bool *) value;
        //*stopLooping = false;
//
//        while(player->loopCount > 0) {
//            player->seek(0);
//            player->play(false);
//            player->loopCount--;
//        }

    } else if (event == SuperpoweredAdvancedAudioPlayerEvent_LoopEnd) {

        Info *info = (Info *) clientData;
        (info->count)--;
        if (info->count == 0) {
            bool *stopLooping = (bool *) value;
            *stopLooping = false;
        }

        //player->exitLoop(false);
    }
}


static bool audioProcessing(void *clientdata, short int *audioIO, int numberOfSamples,
                            int __unused samplerate) {
    return ((SuperpoweredPlayer *) clientdata)->process(audioIO, (unsigned int) numberOfSamples);
}

SuperpoweredPlayer::SuperpoweredPlayer(unsigned int samplerate, unsigned int buffersize,
                                       const char *path, int fileAoffset, int fileAlength)
        : activeFx(0), crossValue(0.0f), volume(1.0f * headroom) {
    stereoBuffer = (float *) memalign(16, (buffersize + 16) * sizeof(float) * 2);

    Info *info = new Info();
    info->count = 3;

    audioPlayer = new SuperpoweredAdvancedAudioPlayer(info,
                                                      playerEventCallback, samplerate, 0);
    audioPlayer->open(path, fileAoffset, fileAlength);
    //audioPlayer->loop(0, 3000, true, 255, false);
    //audioPlayer->loopCount = 3;
    audioPlayer->looping = true;

    audioSystem = new SuperpoweredAndroidAudioIO(samplerate, buffersize, false, true,
                                                 audioProcessing,
                                                 this, -1, SL_ANDROID_STREAM_MEDIA, buffersize * 2);
}

SuperpoweredPlayer::~SuperpoweredPlayer() {
    delete audioSystem;
    delete audioPlayer;
    free(stereoBuffer);
}

void SuperpoweredPlayer::onPlayPause(bool play) {
    if (!play) {
        audioPlayer->pause();

    } else {
        //bool masterIsA = (crossValue <= 0.5f);
        //audioPlayer->seek(0);
        //audioPlayer->looping = false;
        //audioPlayer->loop(0, audioPlayer->durationMs * 3, true, 255, false);
        audioPlayer->play(false);


    };
    SuperpoweredCPU::setSustainedPerformanceMode(play); // <-- Important to prevent audio dropouts.
}

bool SuperpoweredPlayer::process(short int *output, unsigned int numberOfSamples) {

    bool silence = !audioPlayer->process(stereoBuffer, false, numberOfSamples);

    // The stereoBuffer is ready now, let's put the finished audio into the requested buffers.
    if (!silence)
        SuperpoweredFloatToShortInt(stereoBuffer, output, numberOfSamples);
    return !silence;
}

static SuperpoweredPlayer *example = NULL;

extern "C" JNIEXPORT void
Java_com_superpowered_player_MainActivity_SuperpoweredPlayer(JNIEnv *javaEnvironment,
                                                             jobject __unused obj, jint samplerate,
                                                             jint buffersize, jstring apkPath,
                                                             jint fileAoffset, jint fileAlength,
                                                             jint fileBoffset, jint fileBlength) {
    const char *path = javaEnvironment->GetStringUTFChars(apkPath, JNI_FALSE);
    example = new SuperpoweredPlayer((unsigned int) samplerate, (unsigned int) buffersize, path,
                                     fileAoffset, fileAlength);
    javaEnvironment->ReleaseStringUTFChars(apkPath, path);
}

extern "C" JNIEXPORT void
Java_com_superpowered_player_MainActivity_onPlayPause(JNIEnv *__unused javaEnvironment,
                                                      jobject __unused obj, jboolean play) {
    example->onPlayPause(play);
}