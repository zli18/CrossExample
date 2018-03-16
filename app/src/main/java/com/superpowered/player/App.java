package com.superpowered.player;

import android.app.Application;

import cafe.adriel.androidaudioconverter.AndroidAudioConverter;
import cafe.adriel.androidaudioconverter.callback.ILoadCallback;

/**
 * Created by Zhiyun Li on 3/15/18.
 * <p>
 * Novel Effect Inc.
 * zli@NovelEffect.com
 */


public class App extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        AndroidAudioConverter.load(this, new ILoadCallback() {
            @Override
            public void onSuccess() {
                // Great!
            }
            @Override
            public void onFailure(Exception error) {
                // FFmpeg is not supported by device
            }
        });
    }

}
