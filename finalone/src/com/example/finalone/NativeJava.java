package com.example.finalone1;

public class NativeJava {
	static{
		System.loadLibrary("imageprocessing");
	}
	public static native void findfeature(long matAddrBgr, long matAddrGr, long matAddrcanny, int[] xpoint, int[] ypoint);
	public static native void warp(long matAddrBgr);
	//public static native void findcorner(long matAddGr);
}
