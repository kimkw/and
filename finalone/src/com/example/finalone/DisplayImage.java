package com.example.finalone;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.widget.ImageView;

public class DisplayImage extends Activity{
	
	private ImageView photo;
	private byte[]	data;
	
	public void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		setContentView(R.layout.image_layer);
		
		photo = (ImageView)findViewById(R.id.image);
		
		Intent i = getIntent();
		data = i.getByteArrayExtra("pictureData");
		Bitmap bm = BitmapFactory.decodeByteArray(data, 0, data.length);
		photo.setImageBitmap(bm);
	}
}
