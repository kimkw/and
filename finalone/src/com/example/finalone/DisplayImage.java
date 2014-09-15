package com.example.finalone;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.os.Bundle;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;
import android.view.View.OnTouchListener;

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
	Button findfeature, save, getpoint, reset;
	
	int d_width;
	int d_height;
	int b_width;
	int b_height;
	
	
	public void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		setContentView(R.layout.image_layer);
		
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		photo = (ImageView)findViewById(R.id.image);
		Intent i = getIntent();
		data = i.getByteArrayExtra("pictureData");
		bm = BitmapFactory.decodeByteArray(data, 0, data.length);
		Matrix matrix = new Matrix();
		matrix.postRotate(-90);
		rotatedBitmap = Bitmap.createBitmap(bm,0,0,bm.getWidth(),bm.getHeight(),matrix, true);
		b_width = rotatedBitmap.getWidth();
		b_height = rotatedBitmap.getHeight();
		
		photo.setImageBitmap(rotatedBitmap);
		
		Display d = getWindowManager().getDefaultDisplay();
		d_width = d.getWidth();
		d_height = d.getHeight();
		
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
					Toast toast = Toast.makeText(getApplicationContext(), "select four point", Toast.LENGTH_LONG);
					toast.show();
				}else{
				//	DoImage();
				}
			}
		});
		
		save = (Button)findViewById(R.id.save);
		save.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				
			}
		});
		
		getpoint = (Button)findViewById(R.id.getpoint);
		getpoint.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				if( !touchcnt){
					touchcnt = true;
					count = 0;
				}
			}
		});
		
		reset = (Button)findViewById(R.id.reset);
		reset.setOnClickListener(new Button.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				
			}
		});
		
		
	}
	@Override
	public boolean onTouchEvent(MotionEvent event){
		int action = event.getAction();
		if(action == MotionEvent.ACTION_DOWN){
			int x = (int)event.getRawX();
			int y = (int)event.getRawY();
			if(touchcnt){
				xpoint[count] = (int)(x/d_width * b_width );
				ypoint[count] = (int)(y/d_height * b_height);
				count++;
				
				if( count == 4) touchcnt = false;
			}
		}
		return true;
	}
	private Bitmap DoImage(){
	
		Bitmap result = null;
		Mat tmp = new Mat( bm.getWidth(), bm.getHeight(), CvType.CV_8UC4);
		Mat gtmp = new Mat( bm.getWidth(), bm.getHeight(), CvType.CV_8UC4);
		Mat inter = new Mat();
		Utils.bitmapToMat(rotatedBitmap, tmp);
		Imgproc.cvtColor(tmp, gtmp, Imgproc.COLOR_RGB2GRAY);
		Imgproc.Canny(gtmp, inter, 50, 50);
		NativeJava.findfeature(gtmp.getNativeObjAddr(), inter.getNativeObjAddr(), xpoint, ypoint);
		Utils.matToBitmap(tmp, result);
		
		return result;
	}
}
