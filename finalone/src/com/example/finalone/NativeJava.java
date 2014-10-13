package com.example.finalone1;

public class NativeJava {
	static{
		System.loadLibrary("imageprocessing");
	}
	public static native void findfeature(long matAddrBgr, long matAddrGr, long matAddrcanny, int[] xpoint, int[] ypoint);
	public static native void warp(long matAddrBgr);
	//public static native void tracking(long matAddrBgr, long matAddrGr, long matAddrcanny, boolean mode);
	//public static native void tracking(int width, int height, byte[] frameData1, byte[] frameData2, int[] pixels, boolean mode);
	public static native void tracking(int width, int height, byte[] frameData, int[] pixels, boolean mode);
	//public static native void findcorner(long matAddGr);
}
