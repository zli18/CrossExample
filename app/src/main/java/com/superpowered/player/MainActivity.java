package com.superpowered.player;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.media.AudioManager;
import android.content.Context;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

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

        copyResources(R.raw.blues_loop_mp3);

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
                convert("blues_loop_mp3.mp3");
            }

            @Override
            public void onFailure(Exception error) {
                // FFmpeg is not supported by device
            }
        });


    }

    public void copyResources(int resId){
        Log.i("Test", "Setup::copyResources");
        InputStream in = this.getResources().openRawResource(resId);
        String filename = this.getResources().getResourceEntryName(resId);
        if(filename.endsWith("mp3")){
            filename += ".mp3";
        }else
        {
            filename += ".wav";
        }

        File f = new File(filename);

        if(!f.exists()){
            try {
                OutputStream out = new FileOutputStream(new File(this.getFilesDir(), filename));
                byte[] buffer = new byte[1024];
                int len;
                while((len = in.read(buffer, 0, buffer.length)) != -1){
                    out.write(buffer, 0, len);
                }
                in.close();
                out.close();
            } catch (FileNotFoundException e) {
                Log.i("Test", "Setup::copyResources - "+e.getMessage());
            } catch (IOException e) {
                Log.i("Test", "Setup::copyResources - "+e.getMessage());
            }
        }
    }

    private void convert(final String filename) {
        File mp3File = new File(this.getFilesDir(), filename);
        IConvertCallback callback = new IConvertCallback() {
            @Override
            public void onSuccess(File convertedFile) {
                // So fast? Love it!
                //File file = new File(MainActivity.this.getFilesDir(), filename.replace(".mp3", ".wav"));
                final int fileLengthBytes = (int) convertedFile.length();

                SuperpoweredPlayer(
                        Integer.parseInt(samplerateString),
                        Integer.parseInt(buffersizeString),
                        convertedFile.getPath(),
                        0,
                        fileLengthBytes);
            }

            @Override
            public void onFailure(Exception error) {
                // Oops! Something went wrong
            }
        };
        AndroidAudioConverter.with(this)
                // Your current audio_wav file
                .setFile(mp3File)
                // Your desired audio_wav format
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
