package com.example.finalone1;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.ShutterCallback;
import android.os.Bundle;
import android.provider.MediaStore.Images;
import android.util.DisplayMetrics;
import android.util.Log;
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
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.Toast;

public class CameraActivity extends Activity{

	private LayoutInflater mInflater = null;
	Camera mCamera;
	private CameraPreview camPreview;
	private ImageView MyCameraPreview = null;
	int width;
	int height;
	byte[] tempdata;
	boolean mPreviewRunning = false;
	Button takepicture, start_tracking;
	private FrameLayout mainLayout;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		getWindow().setFormat(PixelFormat.TRANSLUCENT);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setContentView(R.layout.camera_activity);
		
		DisplayMetrics metrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(metrics);
		
		
		width = metrics.widthPixels;
		height = metrics.heightPixels;
		
		MyCameraPreview = new ImageView(this);
		SurfaceView mSurfaceView = new SurfaceView(this);
		SurfaceHolder mSurfaceHolder = mSurfaceView.getHolder();
		camPreview = new CameraPreview(width, height, MyCameraPreview);
		
		mSurfaceHolder.addCallback(camPreview);
		mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		
		mainLayout = (FrameLayout)findViewById(R.id.frameLayout1);
		mainLayout.addView(mSurfaceView,new LayoutParams(width, height));
		mainLayout.addView(MyCameraPreview, new LayoutParams(width,height));
		/*
		mSurfaceView = (SurfaceView)findViewById(R.id.surface);
		mSurfaceHolder = mSurfaceView.getHolder();
		mSurfaceHolder.addCallback(this);
		mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		*/
		
		
		mInflater = LayoutInflater.from(this);
		View overView = mInflater.inflate(R.layout.cameraoverlay, null);
		this.addContentView(overView, new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
		
		takepicture = (Button)findViewById(R.id.button);
		takepicture.setOnClickListener(new OnClickListener(){
			public void onClick(View view)
			{
				camPreview.mCamera.takePicture(mShutterCallback, mPictureCallback, mjpeg);
			}
		});
		
		start_tracking = (Button)findViewById(R.id.tracking);
		start_tracking.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				if(!camPreview.startTracking) camPreview.startTracking= true;
				else camPreview.startTracking = false;
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
            	//camPreview.FrameData1 = new byte[]
            	Log.i("copy","11111111111111");
                tempdata=data;
//                camPreview.FrameData1 = new byte[data.length];
//                System.arraycopy(data, 0, camPreview.FrameData1,0,data.length);
                Log.i("copy","22222222222222");
                Intent showPicture = new Intent(CameraActivity.this, DisplayImage.class);
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



}
