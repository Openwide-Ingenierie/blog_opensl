package fr.openwide.blogsles;

import android.media.AudioManager;
import android.os.Build;
import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.Button;

//mainactivity>
public class MainActivity extends Activity implements OnTouchListener {

	private long audiocontext;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		int samplerate = 44100;
		int buffersize = 64;
		//à partir d'android 4.2, android est capable de nous indiquer ses
		//préfèrences audio
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1)
		{
			AudioManager audiomanager = (AudioManager)getSystemService(Context.AUDIO_SERVICE);
			samplerate =Integer.getInteger(audiomanager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE), 44100);
			buffersize = Integer.getInteger(audiomanager.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER), 64);
		}
		audiocontext = audioStart(samplerate, buffersize);

		setContentView(R.layout.activity_main);

		Button C4 = (Button)findViewById(R.id.C4Btn);
		C4.setOnTouchListener(this);
		Button D4 = (Button)findViewById(R.id.D4Btn);
		D4.setOnTouchListener(this);
		Button E4 = (Button)findViewById(R.id.E4Btn);
		E4.setOnTouchListener(this);
		Button F4 = (Button)findViewById(R.id.F4Btn);
		F4.setOnTouchListener(this);
		Button G4 = (Button)findViewById(R.id.G4Btn);
		G4.setOnTouchListener(this);
		Button A4 = (Button)findViewById(R.id.A4Btn);
		A4.setOnTouchListener(this);
		Button B4 = (Button)findViewById(R.id.B4Btn);
		B4.setOnTouchListener(this);
	}

	public boolean onTouch (View v, MotionEvent event)
	{
		if (event.getAction() == MotionEvent.ACTION_DOWN)
		{
			switch(v.getId())
			{
				case R.id.C4Btn:
					audioNoteOn(audiocontext, 60);
				break;

				case R.id.D4Btn:
					audioNoteOn(audiocontext, 62);
					break;

				case R.id.E4Btn:
					audioNoteOn(audiocontext, 64);
					break;

				case R.id.F4Btn:
					audioNoteOn(audiocontext, 65);
					break;

				case R.id.G4Btn:
					audioNoteOn(audiocontext, 67);
					break;

				case R.id.A4Btn:
					audioNoteOn(audiocontext, 69);
					break;

				case R.id.B4Btn:
					audioNoteOn(audiocontext, 71);
					break;
				default:
					break;
			}

		}
		else if (event.getAction() == MotionEvent.ACTION_UP)
		{
			audioNoteOff(audiocontext);
		}

		return true;
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	//// méthodes jni
	public static native long audioStart(int samplerate, int buffersize);
	public static native boolean audioStop(long ctx);
	public static native boolean audioNoteOn(long ctx, int tone);
	public static native boolean audioNoteOff(long ctx);

	static
	{
		System.loadLibrary("Blog_opensl");
	}
}
//mainactivity<