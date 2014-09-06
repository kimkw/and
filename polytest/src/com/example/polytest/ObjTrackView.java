package com.example.polytest;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;

import android.content.Context;
import android.graphics.Bitmap;

class ObjTrackView extends SampleViewBase {

	private int mFrameSize;
	private Bitmap mBitmap;
	private int[] mRGBA;

    public ObjTrackView(Context context) {
        super(context);
    }

	@Override
	protected void onPreviewStared(int previewWidtd, int previewHeight) {
		mFrameSize = previewWidtd * previewHeight;
		mRGBA = new int[mFrameSize];
		mBitmap = Bitmap.createBitmap(previewWidtd, previewHeight, Bitmap.Config.ARGB_8888);
	}

	@Override
	protected void onPreviewStopped() {
		if(mBitmap != null) {
			mBitmap.recycle();
			mBitmap = null;
		}
		mRGBA = null;
		
		
	}

    @Override
    protected Bitmap processFrame(byte[] data) {
        int[] rgba = mRGBA;

        ObTrack(getFrameWidth(), getFrameHeight(), data, rgba, MainActivity.bShowTresholded);

        Bitmap bmp = mBitmap; 
        bmp.setPixels(rgba, 0/* offset */, getFrameWidth() /* stride */, 0, 0, getFrameWidth(), getFrameHeight());
        return bmp;
    }

    public native void ObTrack(int width, int height, byte yuv[], int[] rgba, boolean debug);

    static {
    	System.loadLibrary("test");
    }
}
