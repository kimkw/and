package com.example.polytest;

public class Native {

	static{
		System.loadLibrary("test");
	}
	public static native void findSquares(long matAddrGr, long matAddrRgba);
}
