package com.example.finalone1;

import java.io.IOException;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.SurfaceHolder;
import android.widget.ImageView;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;

public class CameraPreview implements SurfaceHolder.Callback, Camera.PreviewCallback{

	SurfaceHolder mHolder;
	Camera mCamera;
	private boolean bproc = false;
	byte[] FrameData1 = null;
	byte[] FrameData2 = null;
	private int[] pixels = null;
	private int PreviewSizeWidth;
	private int PreviewSizeHeight;
	private int imageFormat;
	private Bitmap bitmap = null;
	private ImageView MyCameraPreview = null;
	
	boolean startTracking = false;
	
	Handler mHandler = new Handler(Looper.getMainLooper());
	
	public CameraPreview(int PreviewlayoutWidth, int PreviewlayoutHeight, ImageView CameraPreview){
		PreviewSizeWidth = PreviewlayoutWidth;
		PreviewSizeHeight = PreviewlayoutHeight;
		MyCameraPreview = CameraPreview;
		bitmap = Bitmap.createBitmap(PreviewlayoutWidth, PreviewlayoutHeight, Bitmap.Config.ARGB_8888);
		pixels = new int[PreviewlayoutWidth * PreviewlayoutHeight];
	}
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		mCamera = Camera.open();
		try
		{
			mCamera.setPreviewDisplay(holder);
			mCamera.setPreviewCallback(this);
		}catch (IOException e){
			mCamera.release();
			mCamera = null;
		}
	}	
	
	public void onPause(){
		mCamera.stopPreview();
		mCamera.release();
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		Parameters para;
		para = mCamera.getParameters();
		para.setPreviewSize(PreviewSizeWidth, PreviewSizeHeight);
		para.setPictureSize(PreviewSizeWidth, PreviewSizeHeight);
		Log.i("kkk", "w" +PreviewSizeWidth+"height"+PreviewSizeHeight);
		para.setPictureFormat(PixelFormat.JPEG);
		imageFormat = para.getPreviewFormat();
		mCamera.setParameters(para);
		mCamera.startPreview();
	}
	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		mCamera.setPreviewCallback(null);
		mCamera.stopPreview();
		mCamera.release();
		mCamera = null;
		
	}

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		// TODO Auto-generated method stub
		if( imageFormat == ImageFormat.NV21){
			if( !bproc)
			{
				FrameData2 = data;
				mHandler.post(Doimage);
			}
		}
	}
	
	private Runnable Doimage = new Runnable(){
		public void run()
		{
			bproc = true;
			NativeJava.tracking(PreviewSizeWidth,PreviewSizeHeight,FrameData1, FrameData2, pixels, startTracking);
			bitmap.setPixels(pixels, 0, PreviewSizeWidth, 0, 0, PreviewSizeWidth, PreviewSizeHeight);
			MyCameraPreview.setImageBitmap(bitmap);
			if(startTracking)System.arraycopy(FrameData2, 0, FrameData1, 0, FrameData2.length);
			bproc = false;
		}
	};

}
