package com.example.polytest;

import java.io.IOException;
import java.util.List;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public abstract class SampleViewBase extends SurfaceView implements SurfaceHolder.Callback, Runnable {
    private static final String TAG = "Sample::SurfaceView";

    private Camera              mCamera;
    private SurfaceHolder       mHolder;
    private int                 mFrameWidth;
    private int                 mFrameHeight;
    private byte[]              mFrame;
    private boolean             mThreadRun;
    private byte[]              mBuffer;


    public SampleViewBase(Context context) {
        super(context);
        mHolder = getHolder();
        mHolder.addCallback(this);
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    public int getFrameWidth() {
        return mFrameWidth;
    }

    public int getFrameHeight() {
        return mFrameHeight;
    }

    public void setPreview() throws IOException {
        //if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB)
        //    mCamera.setPreviewTexture( new SurfaceTexture(10) );
        //else
        	mCamera.setPreviewDisplay(null);
	}

    public void surfaceChanged(SurfaceHolder _holder, int format, int width, int height) {
        Log.i(TAG, "surfaceCreated");
        if (mCamera != null) {
            Camera.Parameters params = mCamera.getParameters();
            List<Camera.Size> sizes = params.getSupportedPreviewSizes();
            mFrameWidth = width;
            mFrameHeight = height;

            // selecting optimal camera preview size
            {
                int  minDiff = Integer.MAX_VALUE;
                for (Camera.Size size : sizes) {
                    if (Math.abs(size.height - height) < minDiff) {
                        mFrameWidth = size.width;
                        mFrameHeight = size.height;
                        minDiff = Math.abs(size.height - height);
                    }
                }
            }

            params.setPreviewSize(getFrameWidth(), getFrameHeight());

            List<String> FocusModes = params.getSupportedFocusModes();
            if (FocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO))
            {
            	params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            }

            mCamera.setParameters(params);

            /* Now allocate the buffer */
            params = mCamera.getParameters();
            int size = params.getPreviewSize().width * params.getPreviewSize().height;
            size  = size * ImageFormat.getBitsPerPixel(params.getPreviewFormat()) / 8;
            mBuffer = new byte[size];
            /* The buffer where the current frame will be coppied */
            mFrame = new byte [size];
            mCamera.addCallbackBuffer(mBuffer);

			try {
				setPreview();
			} catch (IOException e) {
				Log.e(TAG, "mCamera.setPreviewDisplay/setPreviewTexture fails: " + e);
			}

            /* Notify that the preview is about to be started and deliver preview size */
            onPreviewStared(params.getPreviewSize().width, params.getPreviewSize().height);

            /* Now we can start a preview */
            mCamera.startPreview();
        }
    }

    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(TAG, "surfaceCreated");
        mCamera = Camera.open();

        mCamera.setPreviewCallbackWithBuffer(new PreviewCallback() {
            public void onPreviewFrame(byte[] data, Camera camera) {
                synchronized (SampleViewBase.this) {
                    System.arraycopy(data, 0, mFrame, 0, data.length);
                    SampleViewBase.this.notify(); 
                }
                camera.addCallbackBuffer(mBuffer);
            }
        });
                    
        (new Thread(this)).start();
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG, "surfaceDestroyed");
        mThreadRun = false;
        if (mCamera != null) {
            synchronized (this) {
                mCamera.stopPreview();
                mCamera.setPreviewCallback(null);
                mCamera.release();
                mCamera = null;
            }
        }
        onPreviewStopped();
    }

    protected abstract Bitmap processFrame(byte[] data);
    protected abstract void onPreviewStared(int previewWidtd, int previewHeight);
    protected abstract void onPreviewStopped();

    public void run() {
        mThreadRun = true;
        Log.i(TAG, "Starting processing thread");
        while (mThreadRun) {
            Bitmap bmp = null;

            synchronized (this) {
                try {
                    this.wait();
                    bmp = processFrame(mFrame);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            if (bmp != null) {
                Canvas canvas = mHolder.lockCanvas();
                if (canvas != null) {
                    canvas.drawBitmap(bmp, (canvas.getWidth() - getFrameWidth()) / 2, (canvas.getHeight() - getFrameHeight()) / 2, null);
                    mHolder.unlockCanvasAndPost(canvas);
                }
            }
        }
    }
}