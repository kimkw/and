#include <jni.h>
//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
//#include "C:\Users\user\Desktop\final\android-ndk-r9d-windows-x86_64\OpenCV-2.4.9-android-sdk\OpenCV-2.4.9-android-sdk\sdk\native\jni\include\opencv2\imgproc\imgproc_c.h"
//#include "C:\Users\user\Desktop\final\android-ndk-r9d-windows-x86_64\OpenCV-2.4.9-android-sdk\OpenCV-2.4.9-android-sdk\sdk\native\jni\include\opencv2\highgui\highgui.hpp"

#include <vector>
#include <math.h>


#define PI 3.1415926
#define C_MAX_COUNT 300
#define LOG_TAG "KKW"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using namespace std;
using namespace cv;

int thresh = 50;
int cornertresh = 20;
double linetresh = 5;
Point touchpt[4];
vector<int> nearindex;
vector<Point> crosspoint;

typedef struct _LINE_COMP{
	double a;
	double b;
	double c;
}LINE_COMP;
typedef struct _LENGTH{
	double len;
	int index;
}LENGTH;


double distance(double x1, double y1, double x2, double y2){
	return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}
int* cutcorner(vector<Point2f>& input, int* x, int* y){
	vector<Point2f>::iterator it;
	for( it = input.begin(); it != input.end() ; it++ ){
		//if( )
	}
}

void sort_touchpt(){

	int index[4];
	int max = 0;
	double Max_len = DBL_MIN;
	double dist[4];
	for( int i = 0 ; i < 4 ; i++){
		dist[i] = distance(touchpt[0].x, touchpt[0].y, touchpt[i].x, touchpt[i].y);
		if( dist[i] > Max_len){
			max = i;
			Max_len = dist[i];
		}
	}
	Point temp[4];
	if( max == 1){
		temp[0].x = touchpt[0].x;
		temp[0].y = touchpt[0].y;
		temp[1].x = touchpt[2].x;
		temp[1].y = touchpt[2].y;
		temp[2].x = touchpt[1].x;
		temp[2].y = touchpt[1].y;
		temp[3].x = touchpt[3].x;
		temp[3].y = touchpt[3].y;
	}else if( max == 2){
		temp[0].x = touchpt[0].x;
		temp[0].y = touchpt[0].y;
		temp[1].x = touchpt[1].x;
		temp[1].y = touchpt[1].y;
		temp[2].x = touchpt[2].x;
		temp[2].y = touchpt[2].y;
		temp[3].x = touchpt[3].x;
		temp[3].y = touchpt[3].y;
	}else{
		temp[0].x = touchpt[0].x;
		temp[0].y = touchpt[0].y;
		temp[1].x = touchpt[1].x;
		temp[1].y = touchpt[1].y;
		temp[2].x = touchpt[3].x;
		temp[2].y = touchpt[3].y;
		temp[3].x = touchpt[2].x;
		temp[3].y = touchpt[2].y;
	}
	for( int i = 0 ; i < 4 ; i++){
		touchpt[i].x = temp[i].x;
		touchpt[i].y = temp[i].y;
	}
}
void makeline(double x1, double y1, double x2, double y2, LINE_COMP *result){

	result->a = y2-y1;
	result->b = x1-x2;
	result->c = (x2-x1)*y1 - (y2-y1)*x1;
}
double getLinepointdist(LINE_COMP line, double x, double y){

	double top;
	double bottom;

	top = abs(line.a * x + line.b * y + line.c );
	bottom = sqrt(line.a*line.a + line.b*line.b);

	return top/bottom;
}
int getlinecross(LINE_COMP l1, LINE_COMP l2, Point *cross){
	double inv = (l1.a * l2.b - l2.a * l1.b);
	if(inv = 0) return 0;
	double x = (l2.b * l1.c) - (l1.b * l2.c);
	double y = -(l2.a * l1.c)+ (l1.a * l2.c);
	x = x/inv;
	y = y/inv;
	cross->x =x;
	cross->y =y;

	//LOGI("y : %f", inv);

	return 1;

}
void find_line_point(vector<Vec4i>& lines){
	for( int i = 0 ; i < lines.size() ;  i++ ){
		LINE_COMP temp[2];
		makeline(lines[i][0], lines[i][1], lines[i][2],lines[i][3],&temp[0]);
		for( int j =0 ; j < lines.size() ; j++){
			if( i == j ) continue;
			makeline(lines[j][0], lines[j][1], lines[j][2],lines[j][3],&temp[1]);
			Point tt;
			LOGI("x : %f %f %f", temp[0].a, temp[0].b, temp[0].c);
			int k = getlinecross(temp[0], temp[1], &tt);
			if( k = 1) crosspoint.push_back(tt);
		}
	}
}

/*
void find_line_point(vector<Vec4i>& lines){

	LINE_COMP linetemp[4];
	for( int i = 0 ; i < 3 ; i++)
		makeline(touchpt[i].x,touchpt[i].y, touchpt[i+1].x, touchpt[i+1].y,&linetemp[i]);
	makeline(touchpt[3].x, touchpt[3].y, touchpt[0].x, touchpt[0].y, &linetemp[3]);

	vector<LENGTH> lencomp;
	double len[4];
	for( int i = 0 ; i < 4 ; i++)
		len[i] = DBL_MAX;
	for( int i = 0 ; i < lines.size() ; i++ ){
		double temp = DBL_MAX;
		double ltemp = DBL_MAX;
		int index;
		for( int j = 0 ; j < 4 ; j++){
			double len1, len2;
			len1 = getLinepointdist(linetemp[j], lines[i][0], lines[i][1]);
			len2 = getLinepointdist(linetemp[j], lines[i][2], lines[i][3]);
			if (len1 < len2) {
				if (len2 < ltemp) {
					ltemp = len2;
					index = j;
				}
			} else {
				if (len1 < ltemp) {
					ltemp = len1;
					index = j;
				}
			}
		}
		LENGTH tt;
		tt.index = index;
		tt.len = ltemp;
		lencomp.push_back(tt);
		if(len[index] > ltemp) len[index] = ltemp;
	}
	for( int i = 0 ; i < lencomp.size() ; i++){
		if( lencomp[i].len < len[lencomp[i].index] + linetresh){
			nearindex.push_back(i);
		}
	}
	/*
	 * for (int i = 0; i < lines.size(); i++) {
		double len = DBL_MAX;
		for (int j = 0; j < 4; j++) {
			double len1, len2;
			len1 = getLinepointdist(linetemp[j], lines[i][0], lines[i][1]);
			len2 = getLinepointdist(linetemp[j], lines[i][2], lines[i][3]);
			if (len1 > len2) {
				if (len2 < len)
					len = len2;
			} else {
				if (len1 < len)
					len = len1;
			}
		}

		if (len < linetresh)
			nearindex.push_back(i);
	}
	for( int i = 0 ; i < 4 ; i++){
		double min = DBL_MAX;
		for( int j = 0 ; j < lines.size() ; i++){
			double len1, len2;
			int index;
			len1 = getLinepointdist(linetemp[i],lines[j][0], lines[j][1]);
			len2 = getLinepointdist(linetemp[i], lines[j][0],lines[j][1]);
			if(len1 > len2){
				if( len2 < min){

					min = len2;
				}
			}else{
				if( len1 < min) min = len1;
			}
		}
		minlength[i] = min;
	}



}*/

extern "C"{
JNIEXPORT void JNICALL Java_com_example_finalone_NativeJava_findfeature(JNIEnv *env,jobject, jlong addrBgra ,jlong addrGray, jlong addrCanny, jintArray xpoint, jintArray ypoint){

	jint* _xpoint = env->GetIntArrayElements(xpoint,0);
	jint* _ypoint = env->GetIntArrayElements(ypoint,0);

	Mat& mGr = *(Mat*)addrGray;
	Mat& mCn = *(Mat*)addrCanny;
	Mat& mBgra = *(Mat*)addrBgra;

	vector<Vec4i> lines;
	double deltaRho = 1; //1화소
	double deltaTheta = PI/180 ; // 각도 1
	int minVote = 5; // 선을 인정하는데 받는 최소 투표수
	double minLength = 20; // 선에대한 최소 길이
	double maxGap = 0.;	// 선에따른 최대 허용간격

	for( int i = 0 ; i < 4; i++){
		touchpt[i].x = _xpoint[i];
		touchpt[i].y = _ypoint[i];
	}
	sort_touchpt();		// sort four-point

	// find line
	lines.clear();
	nearindex.clear();
	HoughLinesP(mCn, lines, deltaRho, deltaTheta, minVote, minLength, maxGap);
	find_line_point(lines);

	Point temp;

	for( int i = 0 ; i < lines.size() ; i++){
		temp.x = lines[i][0];
		temp.y = lines[i][1];
		circle(mBgra, temp, 2,Scalar(255,0,0,255),4);
		temp.x = lines[i][2];
		temp.y = lines[i][3];
		circle(mBgra, temp, 2,Scalar(255,0,0,255),4);
	}

	//draw near line
	/*
	for( int i = 0 ; i < nearindex.size() ; i++){
			temp.x = lines[nearindex[i]][0];
			temp.y = lines[nearindex[i]][1];
			circle(mBgra, temp, 2,Scalar(255,255,0,255),4);
			temp.x = lines[nearindex[i]][2];
			temp.y = lines[nearindex[i]][3];
			circle(mBgra, temp, 2,Scalar(255,255,0,255),4);
		}
*/
	// find corner
	vector<Point2f> point;
	point.clear();
	int max_count = C_MAX_COUNT;
	goodFeaturesToTrack(mGr, point, max_count, 0.01,5,Mat(),3,0,0.04);


	// draw cross line

	for( int i = 0 ; i < crosspoint.size() ; i++){
		temp.x = crosspoint[i].x;
		temp.y = crosspoint[i].y;
		//if(temp.x < 0 || temp.y < 0 || temp.x >mBgra.size().width || temp.y > mBgra.size().height ) continue;
		circle(mBgra,temp, 2, Scalar(255,255,0,255),4);
	}
	//print line dot

	// print corner dot
	for( int i = 0 ; i < point.size() ; i++){
		temp.x = point[i].x;
		temp.y = point[i].y;
		circle(mBgra,temp, 2, Scalar(0,255,0,255),4);
	}

	// touch dot
/*	for( int i = 0 ; i < 4; i++){
		temp.x = _xpoint[i];
		temp.y = _ypoint[i];
		circle(mBgra,temp, 2, Scalar(0,0,255,255),4);
	}
*/
	temp.x = touchpt[0].x;
	temp.y = touchpt[0].y;
	circle(mBgra,temp, 2, Scalar(0,0,255,255),4);

	temp.x = touchpt[1].x;
	temp.y = touchpt[1].y;
	circle(mBgra,temp, 2, Scalar(0,255,255,255),4);
	temp.x = touchpt[2].x;
	temp.y = touchpt[2].y;
	circle(mBgra,temp, 2, Scalar(255,255,0,255),4);
	temp.x = touchpt[3].x;
	temp.y = touchpt[3].y;
	circle(mBgra,temp, 2, Scalar(255,0,255,255),4);
}
}
