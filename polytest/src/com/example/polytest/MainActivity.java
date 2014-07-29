package com.example.polytest;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import android.support.v7.app.ActionBarActivity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;

public class MainActivity extends ActionBarActivity implements CvCameraViewListener2{

	private static final String TAG = "OPENCVTEST";
	private CameraBridgeViewBase mOpenCvCameraView;
	
	private Mat mRgba;
	private Mat mIntermediateMat;
	private Mat mGray;
	
	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			switch (status){
			case LoaderCallbackInterface.SUCCESS:
			{
				Log.i(TAG,"Opencv loaded successfully");
				mOpenCvCameraView.enableView();
			}break;
			default:
			{
				super.onManagerConnected(status);
			}break;
			}
		};
		
	};
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		Log.i(TAG, "called onCreate");
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setContentView(R.layout.activity_main);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE); 
		
		mOpenCvCameraView = (CameraBridgeViewBase)findViewById(R.id.surfaceview);
		mOpenCvCameraView.setCvCameraViewListener(this);
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
	public void onCameraViewStarted(int width, int height) {
		// TODO Auto-generated method stub
		mRgba = new Mat(height,width,CvType.CV_8UC4);
		mIntermediateMat = new Mat(height,width,CvType.CV_8UC4);
		mGray = new Mat(height, width, CvType.CV_8UC1);
	}

	@Override
    public void onPause()
    {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_3, this, mLoaderCallback);
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }
    
	@Override
	public void onCameraViewStopped() {
		// TODO Auto-generated method stub
		mRgba.release();
		mIntermediateMat.release();
		mGray.release();
	}

	@Override
	public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
		// TODO Auto-generated method stub
		mRgba = inputFrame.rgba();
		mGray = inputFrame.gray();
		Native.findSquares(mGray.getNativeObjAddr(), mRgba.getNativeObjAddr());
		return mRgba;
	}
	
}
