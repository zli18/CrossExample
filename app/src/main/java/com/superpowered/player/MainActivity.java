package com.superpowered.player;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.media.AudioManager;
import android.content.Context;

import java.io.File;

import android.os.Build;
import android.widget.Button;
import android.view.View;

import cafe.adriel.androidaudioconverter.AndroidAudioConverter;
import cafe.adriel.androidaudioconverter.callback.IConvertCallback;
import cafe.adriel.androidaudioconverter.callback.ILoadCallback;
import cafe.adriel.androidaudioconverter.model.AudioFormat;

public class MainActivity extends AppCompatActivity {
    boolean playing = false;
    String samplerateString = null, buffersizeString = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (Build.VERSION.SDK_INT >= 17) {
            AudioManager audioManager = (AudioManager) this.getSystemService(Context.AUDIO_SERVICE);
            samplerateString = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
            buffersizeString = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        }
        if (samplerateString == null) samplerateString = "44100";
        if (buffersizeString == null) buffersizeString = "512";

        AndroidAudioConverter.load(this, new ILoadCallback() {
            @Override
            public void onSuccess() {
                convert("mx12-ifbirdwants-loop.mp3");
            }

            @Override
            public void onFailure(Exception error) {
                // FFmpeg is not supported by device
            }
        });


    }

    private void convert(String filename) {
        File mp3File = new File(this.getFilesDir(), filename);
        IConvertCallback callback = new IConvertCallback() {
            @Override
            public void onSuccess(File convertedFile) {
                // So fast? Love it!
                File file = new File(MainActivity.this.getFilesDir(), "count3wav.wav");
                final int fileLengthBytes = (int) file.length();

                SuperpoweredPlayer(
                        Integer.parseInt(samplerateString),
                        Integer.parseInt(buffersizeString),
                        file.getPath(),
                        0,
                        fileLengthBytes);
            }

            @Override
            public void onFailure(Exception error) {
                // Oops! Something went wrong
            }
        };
        AndroidAudioConverter.with(this)
                // Your current audio file
                .setFile(mp3File)
                // Your desired audio format
                .setFormat(AudioFormat.WAV)
                // An callback to know when conversion is finished
                .setCallback(callback)
                // Start conversion
                .convert();

    }

    public void SuperpoweredPlayer_PlayPause(View button) {  // Play/pause.
        playing = !playing;
        onPlayPause(playing);
        Button b = (Button) findViewById(R.id.playPause);
        if (b != null) b.setText(playing ? "Pause" : "Play");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private native void SuperpoweredPlayer(int samplerate, int buffersize, String apkPath, int fileAoffset, int fileAlength);

    private native void onPlayPause(boolean play);


    static {
        System.loadLibrary("SuperpoweredPlayer");
    }
}
