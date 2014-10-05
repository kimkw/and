package com.example.finalone1;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

public class DisplayImage extends Activity{
	
	private ImageView photo;
	private byte[]	data;
	Bitmap bm;
	Bitmap rotatedBitmap;
	int xpoint[] = new int[4];
	int ypoint[] = new int[4];
	private int count = 0;
	private boolean touchcnt = false;
	LayoutInflater controlInflater = null;
	Button findfeature, canny, getpoint, warp, rotate;
	
	int d_width;
	int d_height;
	int b_width;
	int b_height;
	boolean mode = false;
	
	Mat tmp, gtmp, inter, warpinter;
	Bitmap warpingresult;
	
	private BaseLoaderCallback mOpenCVCallBack = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status){
			switch( status){
				case LoaderCallbackInterface.SUCCESS:
				{
					Log.i("FINAL","Opencv loaded successfully");
				} break;
				default:{
					super.onManagerConnected(status);
				}break;
			}
		}	
	};
	
	public void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		setContentView(R.layout.image_layer);
		if(!OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_9, this, mOpenCVCallBack)){
			Log.e("FINAL", "Cannot connect to OpenCV Manager");
		}
		DisplayMetrics metrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(metrics); 
		d_width = metrics.widthPixels;
		d_height = metrics.heightPixels;
		
		photo = (ImageView)findViewById(R.id.image);
		Intent i = getIntent();
		data = i.getByteArrayExtra("pictureData");
		String picturePath = i.getStringExtra("pictureData2");
		Matrix matrix = new Matrix();
		if(data != null){
			bm = BitmapFactory.decodeByteArray(data, 0, data.length);
			matrix.postRotate(90);
		}else{
			bm = BitmapFactory.decodeFile(picturePath);
			if(bm.getWidth() < bm.getHeight())
				matrix.postRotate(0);
			else
				matrix.postRotate(90);
		}
		
		rotatedBitmap = Bitmap.createBitmap(bm,0,0,bm.getWidth(),bm.getHeight(),matrix, true);
		bm.recycle();
		Drawable drawable = new BitmapDrawable(getResources(),rotatedBitmap);
		photo.setBackgroundDrawable(drawable);
		b_width = rotatedBitmap.getWidth();
		b_height = rotatedBitmap.getHeight();
		
		
		
		controlInflater = LayoutInflater.from(getBaseContext());
		View viewControl = controlInflater.inflate(R.layout.control, null);
		LayoutParams layoutParamsControl = new LayoutParams(
				LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT);
		this.addContentView(viewControl, layoutParamsControl);
		
		
		findfeature = (Button)findViewById(R.id.findfeature);
		findfeature.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				if(count == 4){
					DoImage();
				}else{
					Toast toast = Toast.makeText(getApplicationContext(), (4-count)+" dot more", Toast.LENGTH_LONG);
					toast.show();
				}
			}
		});
		
		canny = (Button)findViewById(R.id.canny);
		canny.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				if( !mode){
					Bitmap result = Bitmap.createBitmap(tmp.cols(), tmp.rows(), Bitmap.Config.ARGB_8888);
					Utils.matToBitmap(tmp, result);
					Drawable drawable = new BitmapDrawable(getResources(),result);
					photo.setBackgroundDrawable(drawable);
					mode = true;
				}
				else{
					Bitmap result = Bitmap.createBitmap(tmp.cols(), tmp.rows(), Bitmap.Config.ARGB_8888);
					Utils.matToBitmap(inter, result);
					Drawable drawable = new BitmapDrawable(getResources(),result);
					photo.setBackgroundDrawable(drawable);
					mode = false;
				}
				
			}
		});
		
		getpoint = (Button)findViewById(R.id.getpoint);
		getpoint.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				if( !touchcnt){
					touchcnt = true;
					count = 0;
					Toast toast = Toast.makeText(getApplicationContext(), "select four point", Toast.LENGTH_SHORT);
					toast.show();
				}
			}
		});
		
		warp = (Button)findViewById(R.id.warp);
		warp.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				warpinter = new Mat(rotatedBitmap.getWidth(), rotatedBitmap.getHeight(), CvType.CV_8UC4);
				Utils.bitmapToMat(rotatedBitmap, warpinter);
				NativeJava.warp(warpinter.getNativeObjAddr());
				warpingresult = Bitmap.createBitmap(warpinter.cols(), warpinter.rows(),Bitmap.Config.ARGB_8888);
				Utils.matToBitmap(warpinter, warpingresult);
				Drawable drawable = new BitmapDrawable(getResources(),warpingresult);
				photo.setBackgroundDrawable(drawable);
				
			}
		});
		
		rotate = (Button)findViewById(R.id.rotate);
		rotate.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				Matrix matrix = new Matrix();
				matrix.postRotate(180);
				Bitmap rotated =Bitmap.createBitmap(warpingresult,0,0,warpingresult.getWidth(),
						warpingresult.getHeight(),matrix, true); 
				Drawable drawable = new BitmapDrawable(getResources(),rotated);
				photo.setBackgroundDrawable(drawable);
			}
		});
		
		
	}
	
	@Override
	public boolean onTouchEvent(MotionEvent event){
		int action = event.getAction();
		if(action == MotionEvent.ACTION_DOWN){
			double x = (int)event.getRawX();
			double y = (int)event.getRawY();
			if(touchcnt){
				double temp1 =  x / (double) d_width;
				double temp2 = temp1 * (double) b_width;

				double temp3 = (double) y / (double) d_height;
				double temp4 = temp3 * (double) b_height;

				xpoint[count] = (int)temp2;
				ypoint[count] = (int)temp4;
				count++;
				
				Toast toast = Toast.makeText(getApplicationContext(), "count: "+count+" xpoint: "+xpoint[count-1]+" ypoint: "+ypoint[count-1] , Toast.LENGTH_SHORT);
				toast.show();
				
				if( count == 4) touchcnt = false;
			}
		}
		return true;
	}
	
	private void DoImage(){
	
		tmp = new Mat(rotatedBitmap.getWidth(), rotatedBitmap.getHeight(), CvType.CV_8UC4);
		gtmp = new Mat(rotatedBitmap.getWidth(), rotatedBitmap.getHeight(), CvType.CV_8UC1);
		inter = new Mat();
		Utils.bitmapToMat(rotatedBitmap, tmp);
		Imgproc.cvtColor(tmp, gtmp, Imgproc.COLOR_RGB2GRAY);
		double thresh = Imgproc.threshold(gtmp, inter, -1, 255, Imgproc.THRESH_BINARY+Imgproc.THRESH_OTSU);
		Imgproc.Canny(inter, inter, thresh*0.4, thresh);
		Imgproc.dilate(inter, inter, Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(2,2)));
		NativeJava.findfeature(tmp.getNativeObjAddr(), gtmp.getNativeObjAddr(), inter.getNativeObjAddr(), xpoint, ypoint);
		
		Log.i("KKW", "1111");
		Bitmap result = Bitmap.createBitmap(tmp.cols(), tmp.rows(), Bitmap.Config.ARGB_8888);
		Log.i("KKW", "2222");
		Utils.matToBitmap(tmp, result);
		Drawable drawable = new BitmapDrawable(getResources(),result);
		photo.setBackgroundDrawable(drawable);
		
//		return result;
	}
}
