package com.example.finalone;

public class NativeJava {
	static{
		System.loadLibrary("imageprocessing");
	}
	public static native void findfeature(long matAddrGr, long matAddrcanny, int[] xpoint, int[] ypoint);
	//public static native void findcorner(long matAddGr);
}
