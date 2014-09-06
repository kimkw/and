package com.example.finalone;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.ShutterCallback;
import android.os.Bundle;
import android.provider.MediaStore.Images;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity implements SurfaceHolder.Callback{

	private LayoutInflater mInflater = null;
	Camera mCamera;
	byte[] tempdata;
	boolean mPreviewRunning = false;
	private SurfaceHolder mSurfaceHolder;
	private SurfaceView mSurfaceView;
	Button takepicture;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		getWindow().setFormat(PixelFormat.TRANSLUCENT);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setContentView(R.layout.activity_main);
		
		mSurfaceView = (SurfaceView)findViewById(R.id.surface);
		mSurfaceHolder = mSurfaceView.getHolder();
		mSurfaceHolder.addCallback(this);
		
		mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		
		mInflater = LayoutInflater.from(this);
		View overView = mInflater.inflate(R.layout.cameraoverlay, null);
		this.addContentView(overView, new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
		
		takepicture = (Button)findViewById(R.id.button);
		takepicture.setOnClickListener(new OnClickListener(){
			public void onClick(View view)
			{
				mCamera.takePicture(mShutterCallback, mPictureCallback, mjpeg);
			}
		});
	}
	ShutterCallback mShutterCallback = new ShutterCallback()
    {
        @Override
        public void onShutter() {
            // TODO Auto-generated method stub
        }
    };

    PictureCallback mPictureCallback = new PictureCallback()
    {
        @Override
        public void onPictureTaken(byte[] data, Camera c) {
            // TODO Auto-generated method stub
        }    
    };
    PictureCallback mjpeg = new PictureCallback()
    {
        public void onPictureTaken(byte[] data, Camera c)
        {
            if(data != null)
            {
                tempdata=data;
                
                Intent showPicture = new Intent(MainActivity.this, DisplayImage.class);
                showPicture.putExtra("pictureData", data);
                startActivity(showPicture);
                //saveImage();
                
            }
        }
    };

	void saveImage() {
		Bitmap bm = BitmapFactory.decodeByteArray(tempdata, 0, tempdata.length);
		String url = Images.Media.insertImage(getContentResolver(), bm, null,
				null);

		bm.recycle();
		Bundle bundle = new Bundle();
		if (url != null) {
			bundle.putString("url", url);
			Intent mIntent = new Intent();
			mIntent.putExtras(bundle);
			setResult(RESULT_OK, mIntent);
		} else {
			Toast.makeText(this, "Picture can not be saved", Toast.LENGTH_SHORT)
					.show();
		}

		finish();
	}

    
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
		// TODO Auto-generated method stub
		try{
			if(mPreviewRunning){
				mCamera.stopPreview();
				mPreviewRunning = false;
			}
			Camera.Parameters p = mCamera.getParameters();
			p.setPreviewSize(w, h);
			
			mCamera.setParameters(p);
			mCamera.setPreviewDisplay(holder);
			mCamera.startPreview();
			mPreviewRunning = true;
		}catch(Exception e){
			
		}
	}

	@Override
	public void surfaceCreated(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
		mCamera = Camera.open();
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
		mCamera.stopPreview();
		mPreviewRunning = false;
		mCamera.release();
		mCamera = null;
	}
}
