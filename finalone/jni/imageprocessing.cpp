#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <time.h>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
//#include "C:\Users\user\Desktop\final\android-ndk-r9d-windows-x86_64\OpenCV-2.4.9-android-sdk\OpenCV-2.4.9-android-sdk\sdk\native\jni\include\opencv2\imgproc\imgproc_c.h"
//#include "C:\Users\user\Desktop\final\android-ndk-r9d-windows-x86_64\OpenCV-2.4.9-android-sdk\OpenCV-2.4.9-android-sdk\sdk\native\jni\include\opencv2\highgui\highgui.hpp"

#include <vector>
#include <math.h>
#include <algorithm>

#define PI 3.1415926
#define C_MAX_COUNT 300
#define LOG_TAG "KKW"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using namespace std;
using namespace cv;

int thresh = 50;
int cornertresh = 20;
double linetresh = 30;
double thetatresh = 10;
Point touchpt[4];
vector<int> nearindex;
vector<Point> crosspoint;


int maxX, maxY;
int minX, minY;

typedef struct _LINE_COMP{
	double a;
	double b;
	double c;
}LINE_COMP;
typedef struct _LENGTH{
	double len;
	int index;
}LENGTH;

vector<int> l1;
vector<int> l2;
vector<int> l3;
vector<int> l4;


double distance(double x1, double y1, double x2, double y2){
	return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

void sort_touchpt(){

	int index[4];
	int max = 0;
	double Max_len = DBL_MIN;
	double dist[4];
	maxX = INT_MIN;
	maxY = INT_MIN;
	minX = INT_MAX;
	minY = INT_MAX;

	for( int i = 0 ; i < 4 ; i++){
		dist[i] = distance(touchpt[0].x, touchpt[0].y, touchpt[i].x, touchpt[i].y);
		if( dist[i] > Max_len){
			max = i;
			Max_len = dist[i];
		}
		if( maxX < touchpt[i].x) maxX = touchpt[i].x;
		if( maxY < touchpt[i].y) maxY = touchpt[i].y;
		if( minX > touchpt[i].x) minX = touchpt[i].x;
		if( minY > touchpt[i].y) minY = touchpt[i].y;
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
int getlinecross(LINE_COMP line1, LINE_COMP line2, Point *cross){
	double x,y;
	double tmp1,tmp2;
	tmp1 = (line2.c * line1.b) - (line1.c * line2.b);
	tmp2 = (line1.a * line2.b) - (line2.a * line1.b);
	x = tmp1/ tmp2;
	tmp1 = (line1.a * line2.c) - (line2.a * line1.c);
	tmp2 = (line2.a * line1.b) - (line1.a * line2.b);
	y = tmp1/ tmp2;
	cross->x = x;
	cross->y = y;

	LOGI("%f, %f", x,y);
}
double getTheta(double x1, double y1, double x2, double y2){
	double result = 0;
	double dx = 0;
	double dy = 0;
	if( x1 > x2 ){
		dx = x1 - x2;
		dy = y1 - y2;
		result = atan2(dy,dx) * 180 / PI;
	}else{
		dx = x2 - x1;
		dy = y2 - y1;
		result = atan2(dy,dx) * 180 / PI;
	}

	return result;
}/*
void removeoutlier(vector<Vec4i>& lines) {
	double l1_avg;
	double l2_avg;
	double l3_avg;
	double l4_avg;

	LINE_COMP line1_t;
	vector<LENGTH> length1;


	for (int i = 0; i < l1.size(); i++) {
		double tmp = 0;
		makeline(lines[l1[i]][0], lines[l1[i]][1], lines[l1[i]][2],
				lines[l1[i]][3], &line1_t);
		double len, len1, len2;
		len = 0;
		for (int j = 0; j < l1.size(); j++) {
			len1 = getLinepointdist(line1_t, lines[l1[j]][0], lines[l1[j]][1]);
			len2 = getLinepointdist(line1_t, lines[l1[j]][2], lines[l1[j]][3]);
			if (len1 < len2)
				len += len1;
			else
				len += len2;
		}
		LENGTH temp;
		temp.index = i;
		temp.len = len;
		length1.push_back(temp);
	}
	int min_idx;
	double min_len = DBL_MAX;
	for (int i = 0; i < length1.size(); i++) {
		if (min_len > length1[i].len) {
			min_idx = length1[i].index;
			min_len = length1[i].len;
		}
	}
	makeline(lines[min_idx][0], lines[min_idx][1], lines[min_idx][2],
			lines[min_idx][3], &line1_t);
	for (int i = 0; i < l1.size(); i++) {
		double len1, len2;
		len1 = getLinepointdist(line1_t, lines[l1[i]][0], lines[l1[i]][1]);
		len2 = getLinepointdist(line1_t, lines[l1[i]][2], lines[l1[i]][3]);
		if (len1 < len2) {
			if (len1 > linetresh) {
				l1.erase(l1.begin() + i);
			}
		} else {
			if (len2 > linetresh) {
				l1.erase(l1.begin() + i);
			}
		}
	}
	LOGI("l1 end");
	LINE_COMP line2_t;
	vector<LENGTH> length2;
	for (int i = 0; i < l2.size(); i++) {
		double tmp = 0;
		makeline(lines[l2[i]][0], lines[l2[i]][1], lines[l2[i]][2],
				lines[l2[i]][3], &line2_t);
		double len, len1, len2;
		len = 0;
		for (int j = 0; j < l2.size(); j++) {
			len1 = getLinepointdist(line2_t, lines[l2[j]][0], lines[l2[j]][1]);
			len2 = getLinepointdist(line2_t, lines[l2[j]][2], lines[l2[j]][3]);
			if (len1 < len2)
				len += len1;
			else
				len += len2;
		}
		LENGTH temp;
		temp.index = i;
		temp.len = len;
		length2.push_back(temp);
	}
	min_idx = 0;
	min_len = DBL_MAX;
	for (int i = 0; i < length2.size(); i++) {
		if (min_len > length2[i].len) {
			min_idx = length2[i].index;
			min_len = length2[i].len;
		}
	}
	makeline(lines[min_idx][0], lines[min_idx][1], lines[min_idx][2],
			lines[min_idx][3], &line2_t);
	for (int i = 0; i < l2.size(); i++) {
		double len1, len2;
		len1 = getLinepointdist(line2_t, lines[l2[i]][0], lines[l2[i]][1]);
		len2 = getLinepointdist(line2_t, lines[l2[i]][2], lines[l2[i]][3]);
		if (len1 < len2) {
			if (len1 > linetresh) {
				l2.erase(l2.begin() + i);
			}
		} else {
			if (len2 > linetresh) {
				l2.erase(l2.begin() + i);
			}
		}
	}

	LOGI("l2 end");

	LINE_COMP line3_t;
	vector < LENGTH > length3;
	for (int i = 0; i < l3.size(); i++) {
		double tmp = 0;
		makeline(lines[l3[i]][0], lines[l3[i]][1], lines[l3[i]][2],
				lines[l3[i]][3], &line3_t);
		double len, len1, len2;
		len = 0;
		for (int j = 0; j < l3.size(); j++) {
			len1 = getLinepointdist(line3_t, lines[l3[j]][0], lines[l3[j]][1]);
			len2 = getLinepointdist(line3_t, lines[l3[j]][2], lines[l3[j]][3]);
			if (len1 < len2)
				len += len1;
			else
				len += len2;
		}
		LENGTH temp;
		temp.index = i;
		temp.len = len;
		length3.push_back(temp);
	}
	min_len = DBL_MAX;
	for (int i = 0; i < length3.size(); i++) {
		if (min_len > length3[i].len) {
			min_idx = length3[i].index;
			min_len = length3[i].len;
		}
	}
	makeline(lines[min_idx][0], lines[min_idx][1], lines[min_idx][2],
			lines[min_idx][3], &line3_t);
	for (int i = 0; i < l3.size(); i++) {
		double len1, len2;
		len1 = getLinepointdist(line3_t, lines[l3[i]][0], lines[l3[i]][1]);
		len2 = getLinepointdist(line3_t, lines[l3[i]][2], lines[l3[i]][3]);
		if (len1 < len2) {
			if (len1 > linetresh) {
				l3.erase(l3.begin() + i);
			}
		} else {
			if (len2 > linetresh) {
				l3.erase(l3.begin() + i);
			}
		}
	}

	LOGI("l3 end");

	LINE_COMP line4_t;
	vector < LENGTH > length4;
	for (int i = 0; i < l4.size(); i++) {
		double tmp = 0;
		makeline(lines[l4[i]][0], lines[l4[i]][1], lines[l4[i]][2],
				lines[l4[i]][3], &line4_t);
		double len, len1, len2;
		len = 0;
		for (int j = 0; j < l4.size(); j++) {
			len1 = getLinepointdist(line4_t, lines[l4[j]][0], lines[l4[j]][1]);
			len2 = getLinepointdist(line4_t, lines[l4[j]][2], lines[l4[j]][3]);
			if (len1 < len2)
				len += len1;
			else
				len += len2;
		}
		LENGTH temp;
		temp.index = i;
		temp.len = len;
		length4.push_back(temp);
	}
	min_len = DBL_MAX;
	for (int i = 0; i < length4.size(); i++) {
		if (min_len > length4[i].len) {
			min_idx = length4[i].index;
			min_len = length4[i].len;
		}
	}
	makeline(lines[min_idx][0], lines[min_idx][1], lines[min_idx][2],
			lines[min_idx][3], &line4_t);
	for (int i = 0; i < l4.size(); i++) {
		double len1, len2;
		len1 = getLinepointdist(line4_t, lines[l4[i]][0], lines[l4[i]][1]);
		len2 = getLinepointdist(line4_t, lines[l4[i]][2], lines[l4[i]][3]);
		if (len1 < len2) {
			if (len1 > linetresh) {
				l4.erase(l4.begin() + i);
			}
		} else {
			if (len2 > linetresh) {
				l4.erase(l4.begin() + i);
			}
		}
	}

	LOGI("l4 end");
}*/
void find_line_point(vector<Vec4i>& lines){

	l1.clear();
	l2.clear();
	l3.clear();
	l4.clear();
	double touchangle[4];
	for(int i = 0 ; i < 3 ; i++){
		touchangle[i] = getTheta(touchpt[i].x, touchpt[i].y, touchpt[i+1].x, touchpt[i+1].y );
	}
	touchangle[3] = getTheta(touchpt[0].x, touchpt[0].y, touchpt[3].x, touchpt[3].y);


	LINE_COMP linetemp[4];
	for( int i = 0 ; i < 3 ; i++)
		makeline(touchpt[i].x,touchpt[i].y, touchpt[i+1].x, touchpt[i+1].y,&linetemp[i]);
	makeline(touchpt[3].x, touchpt[3].y, touchpt[0].x, touchpt[0].y, &linetemp[3]);



	for( int i = 0 ; i < lines.size() ; i++){
		if( (lines[i][0] > minX) && (lines[i][0] < maxX) &&
		 (lines[i][1] > minY) && (lines[i][1] < maxY) &&
		 (lines[i][2] > minX) && (lines[i][2] < maxX) &&
		 (lines[i][3] > minY) && (lines[i][3] < maxY) ){
			double thetatemp = getTheta(lines[i][0],lines[i][1], lines[i][2], lines[i][3]);
			int temp[4];
			int cnt = 0;
			for( int j = 0 ; j < 4 ; j++){
				if(abs( thetatemp - touchangle[j]) < thetatresh){
					temp[cnt] = j;
					cnt++;
				}
			}
			if( cnt == 0  ) continue;
			// choice one
			int cnt_02 = 0;
			int cnt_13 = 0;
			for( int j = 0 ; j < cnt ; j++){
				if( temp[j] == 0 || temp[j] == 2)
					cnt_02++;
				else
					cnt_13++;
			}
			double len1, len2;
			double tt;
			int idx;
			if( cnt == 4 ) LOGI("BUG");
			if (cnt_02 > cnt_13) {
				len1 = getLinepointdist(linetemp[0], lines[i][0],
						lines[i][1]);
				tt = getLinepointdist(linetemp[0], lines[i][2],
						lines[i][3]);
				if (len1 > tt)
					len1 = tt;
				len2 = getLinepointdist(linetemp[2], lines[i][0],
						lines[i][1]);
				tt = getLinepointdist(linetemp[2], lines[i][2],
						lines[i][3]);
				if (len2 > tt)
					len2 = tt;

				idx = 0;
				if (len1 > len2)
					idx = 2;
			}else{
				len1 = getLinepointdist(linetemp[1], lines[i][0],
						lines[i][1]);
				tt = getLinepointdist(linetemp[1], lines[i][2],
						lines[i][3]);
				if (len1 > tt)
					len1 = tt;
				len2 = getLinepointdist(linetemp[3], lines[i][0],
						lines[i][1]);
				tt = getLinepointdist(linetemp[3], lines[i][2],
						lines[i][3]);
				if (len2 > tt)
					len2 = tt;

				idx = 1;
				if (len1 > len2)
					idx = 3;
			}
			switch( idx){
			case 0:
				l1.push_back(i);
				break;
			case 1:
				l2.push_back(i);
				break;
			case 2:
				l3.push_back(i);
				break;
			case 3:
				l4.push_back(i);
				break;
			}
		}
	}
}
void line2dot(Point *dot, vector<Vec4i>& lines, vector<int>& index){
	int cnt = 0;
	for(int i = 0 ; i < index.size() ; i++){
		dot[cnt].x = lines[index[i]][0];
		dot[cnt].y = lines[index[i]][1];
		cnt++;
		dot[cnt].x = lines[index[i]][2];
		dot[cnt].y = lines[index[i]][3];
		cnt++;
	}
}
bool find_in_samples(Point *samples, int no_samples, Point *data){
	for( int i = 0 ; i < no_samples; i++){
		if( samples[i].x == data->x && samples[i].y == data->y){
			return true;
		}
	}
	return false;
}
void get_samples(Point *samples, int no_samples, Point *data, int no_data){
	srand(time(NULL));
	for( int i = 0 ; i < no_samples ;){
		int j = rand()%no_data;
		if(!find_in_samples(samples, i, &data[j])){
			samples[i] = data[j];
			i++;
		}
	}
}
int compute_model_parameter(Point samples[], int no_samples, LINE_COMP &model){
	// using PCA
	double sx = 0 , sy = 0;
	double sxx = 0, syy = 0;
	double sxy = 0, sw = 0;

	for( int i = 0 ; i < no_samples ; i++){
		double x = samples[i].x;
		double y = samples[i].y;

		sx += x;
		sy += y;
		sxx += x*x;
		sxy += x*y;
		syy += y*y;
		sw += 1;
	}
	// variance;

	double vxx = (sxx - sx*sx/sw)/sw;
	double vxy = (sxy - sx*sy/sw)/sw;
	double vyy = (syy - sy*sy/sw)/sw;

	//axis
	double theta = atan2(2*vxy, vxx-vyy)/2;

	double mx = cos(theta);
	double my = sin(theta);
	double sx1 = sx/sw;
	double sy1 = sy/sw;
	// line equation : sin(theta)*(x-sx1) = cos(theta)*(y-sy1)

	model.a = my;
	model.b = -mx;
	model.c = my * (-sx1) + mx * sy1;

	return 1;


}
double model_verification(Point *inliers, int *no_inliers, LINE_COMP estimated_model, Point *data, int no_data, double distance_threshold){
	*no_inliers = 0;
	double cost = 0;
	for( int i = 0 ; i < no_data ; i++){
		double distance = getLinepointdist(estimated_model,data[i].x, data[i].y );
		if( distance < distance_threshold){
			cost += 1.;
			inliers[*no_inliers] = data[i];
			++(*no_inliers);
		}
	}
	return cost;
}
double ransac_line_fitting(Point *data, int no_data, LINE_COMP &model, double distance_threshold ){
	const int no_samples = 2;
	if( no_data < no_samples){
		return 0;
	}

	Point *samples = new Point[no_samples];
	int no_inliers = 0;
	Point *inliers = new Point[no_data];

	LINE_COMP estimated_model;
	double max_cost  = 0;

	int max_iteration = (int)(1+log(1.-0.99)/log(1.-pow(0.5, no_samples)));
	for( int i = 0 ; i < max_iteration ; i++){
		// hypothesis
		get_samples(samples, no_samples, data, no_data);
		// predict parameter
		compute_model_parameter(samples, no_samples, estimated_model);

		// verification
		double cost = model_verification(inliers, &no_inliers, estimated_model, data, no_data, distance_threshold);
		if( max_cost < cost){
			max_cost = cost;
			compute_model_parameter(inliers, no_inliers, model);
		}
	}
	delete [] samples;
	delete [] inliers;

	return max_cost;
}

extern "C"{
JNIEXPORT void JNICALL Java_com_example_finalone1_NativeJava_findfeature(JNIEnv *env,jobject, jlong addrBgra ,jlong addrGray, jlong addrCanny, jintArray xpoint, jintArray ypoint){

	jint* _xpoint = env->GetIntArrayElements(xpoint,0);
	jint* _ypoint = env->GetIntArrayElements(ypoint,0);

	Mat& mGr = *(Mat*)addrGray;
	Mat& mCn = *(Mat*)addrCanny;
	Mat& mBgra = *(Mat*)addrBgra;

	vector<Vec4i> lines;
	double deltaRho = 1; //1화소
	double deltaTheta = PI/180 ; // 각도 1
	int minVote = 5; // 선을 인정하는데 받는 최소 투표수
	double minLength = 30; // 선에대한 최소 길이
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


	LOGI("l1 : %d", l1.size());
	LOGI("l2 : %d", l2.size());
	LOGI("l3 : %d", l3.size());
	LOGI("l4 : %d", l4.size());

	for( int i = 0; i < l1.size(); i++) {
		temp.x = lines[l1[i]][0];
		temp.y = lines[l1[i]][1];
		circle(mBgra, temp, 2,Scalar(255,255,0,255),20);
		temp.x = lines[l1[i]][2];
		temp.y = lines[l1[i]][3];
		circle(mBgra, temp, 2,Scalar(255,255,0,255),20);
	}
	for( int i = 0; i < l2.size(); i++) {
		temp.x = lines[l2[i]][0];
		temp.y = lines[l2[i]][1];
		circle(mBgra, temp, 2,Scalar(255,0,255,255),20);
		temp.x = lines[l2[i]][2];
		temp.y = lines[l2[i]][3];
		circle(mBgra, temp, 2,Scalar(255,0,255,255),20);
	}
	for( int i = 0; i < l3.size(); i++) {
		temp.x = lines[l3[i]][0];
		temp.y = lines[l3[i]][1];
		circle(mBgra, temp, 2,Scalar(0,255,255,255),20);
		temp.x = lines[l3[i]][2];
		temp.y = lines[l3[i]][3];
		circle(mBgra, temp, 2,Scalar(0,255,255,255),20);
	}
	for( int i = 0; i < l4.size(); i++) {
		temp.x = lines[l4[i]][0];
		temp.y = lines[l4[i]][1];
		circle(mBgra, temp, 2,Scalar(255,255,255,255),20);
		temp.x = lines[l4[i]][2];
		temp.y = lines[l4[i]][3];
		circle(mBgra, temp, 2,Scalar(255,255,255,255),20);
	}

	Point *dot1 = new Point[l1.size()*2];
	line2dot(dot1, lines, l1);
	LINE_COMP line1;
	ransac_line_fitting(dot1, l1.size()*2, line1, 50 );
	delete [] dot1;

	Point *dot2 = new Point[l2.size()*2];
	line2dot(dot2, lines, l2);
	LINE_COMP line2;
	ransac_line_fitting(dot2, l2.size()*2, line2, 50 );
	delete [] dot2;

	Point *dot3 = new Point[l3.size()*2];
	line2dot(dot3, lines, l3);
	LINE_COMP line3;
	ransac_line_fitting(dot3, l3.size()*2, line3, 50 );
	delete [] dot3;

	Point *dot4 = new Point[l4.size()*2];
	line2dot(dot4, lines, l4);
	LINE_COMP line4;
	ransac_line_fitting(dot4, l4.size()*2, line4, 50 );
	delete [] dot4;



	Point crspt[4];
	getlinecross(line1, line2, &crspt[0]);
	getlinecross(line2, line3, &crspt[1]);
	getlinecross(line3, line4, &crspt[2]);
	getlinecross(line4, line1, &crspt[3]);
	for( int i = 0 ; i < 4 ; i++){
		circle(mBgra, crspt[i], 2, Scalar(255,0,0,255),30 );
		//LOGI("%d, %d", crspt[i].x, crspt[i].y);
	}

	/*removeoutlier(lines);

	for( int i = 0; i < l1.size(); i++) {
		temp.x = lines[l1[i]][0];
		temp.y = lines[l1[i]][1];
		circle(mBgra, temp, 2,Scalar(0,0,255,255),10);
		temp.x = lines[l1[i]][2];
		temp.y = lines[l1[i]][3];
		circle(mBgra, temp, 2,Scalar(0,0,255,255),10);
	}
	for( int i = 0; i < l2.size(); i++) {
		temp.x = lines[l2[i]][0];
		temp.y = lines[l2[i]][1];
		circle(mBgra, temp, 2,Scalar(0,0,255,255),10);
		temp.x = lines[l2[i]][2];
		temp.y = lines[l2[i]][3];
		circle(mBgra, temp, 2,Scalar(0,0,255,255),10);
	}
	for( int i = 0; i < l3.size(); i++) {
		temp.x = lines[l3[i]][0];
		temp.y = lines[l3[i]][1];
		circle(mBgra, temp, 2,Scalar(0,0,255,255),10);
		temp.x = lines[l3[i]][2];
		temp.y = lines[l3[i]][3];
		circle(mBgra, temp, 2,Scalar(0,0,255,255),10);
	}
	for( int i = 0; i < l4.size(); i++) {
		temp.x = lines[l4[i]][0];
		temp.y = lines[l4[i]][1];
		circle(mBgra, temp, 2,Scalar(0,0,255,255),10);
		temp.x = lines[l4[i]][2];
		temp.y = lines[l4[i]][3];
		circle(mBgra, temp, 2,Scalar(0,0,255,255),10);
	}

*/

	for( int i = 0; i < lines.size(); i++) {
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
