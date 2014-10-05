package com.example.finalone1;

import java.nio.ByteBuffer;

import org.apache.http.util.ByteArrayBuffer;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;

public class MainActivity extends Activity{

	Button album, cam;
	int REQUEST_CODE_IMAGE = 2;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	
		getWindow().setFormat(PixelFormat.TRANSLUCENT);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setContentView(R.layout.activity_main);
		
		album = (Button)findViewById(R.id.album_choice);
		album.setOnClickListener(new Button.OnClickListener(){

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				Intent intent = new Intent(Intent.ACTION_PICK,android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
				startActivityForResult(intent, REQUEST_CODE_IMAGE);
			}
			
		});
		cam = (Button)findViewById(R.id.camera_choice);
		cam.setOnClickListener(new Button.OnClickListener(){

			@Override
			public void onClick(View v) {
				Intent intent = new Intent(MainActivity.this, CameraActivity.class);
				startActivity(intent);
				finish();	
			}
		});
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data){
		super.onActivityResult(requestCode, resultCode, data);
		if( requestCode == REQUEST_CODE_IMAGE && resultCode == RESULT_OK && null != data){
			
			Uri selectImageUri = data.getData();
			String[] filePath = {MediaStore.Images.Media.DATA};
			Cursor imageCursor = this.getContentResolver().query(selectImageUri, filePath, null, null, null);
			imageCursor.moveToFirst();
			
			int columnIndex = imageCursor.getColumnIndex(filePath[0]);
			String imagePath = imageCursor.getString(columnIndex);
			imageCursor.close();
			
//			Bitmap bitmap = BitmapFactory.decodeFile(imagePath);
//			int bytes = bitmap.getRowBytes() * bitmap.getHeight();
//			ByteBuffer byteBuffer = ByteBuffer.allocate(bytes);
//			
//			byte[] array = byteBuffer.array();
			
			Intent showPicture = new Intent(MainActivity.this,
					DisplayImage.class);
			showPicture.putExtra("pictureData2", imagePath);
			
			startActivity(showPicture);
		}
	}
}
