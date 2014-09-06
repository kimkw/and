package com.example.polytest;



import android.support.v7.app.ActionBarActivity;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;

public class MainActivity extends Activity {

private static final String TAG = "ObjTrackActivity";
    
    private MenuItem mItemPreviewRGBA;
    private MenuItem mItemPreviewTresholded;
    
    public static boolean bShowTresholded = false;

    public MainActivity() {
		// TODO Auto-generated constructor stub
    	Log.i(TAG, "Instantiated new " + this.getClass());
	}
     

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(new ObjTrackView(this));
    }
    
    public boolean onCreateOptionsMenu(Menu menu){
    	Log.i(TAG, "onCreateOptionsMenu");
    	mItemPreviewRGBA = menu.add("Preview RGBA");
    	mItemPreviewTresholded = menu.add("Preview Thresholded");
    	return true;
    }
    
    public boolean onOptionsItemSelected(MenuItem item){
    	Log.i(TAG, "Menu Item selected " + item);
    	if (item == mItemPreviewRGBA)
    		bShowTresholded = false;
        else if (item == mItemPreviewTresholded)
        	bShowTresholded = true;
    	return true;
    }
	
	
}
