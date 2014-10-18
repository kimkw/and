#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <time.h>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2/video/tracking.hpp>
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
double linetresh = 100;
double thetatresh = 10;
double linediv = 30;
Point touchpt[4];
int monitor = 0;
bool monitorset = true;
Mat first;
bool firstset = false;

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

Point crspt[4];	// save cross point


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
}
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

//				idx = 0;
//				if (len1 > len2)
//					idx = 2;
//
				if( len1 < len2){
					idx = 0;
					if( len1 > linetresh) continue;
				}else{
					idx = 2;
					if( len2 > linetresh) continue;
				}

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


//				idx = 1;
//				if (len1 > len2)
//					idx = 3;
				if( len1 < len2){
					idx = 1;
					if( len1 > linetresh) continue;
				}else{
					idx = 3;
					if( len2 > linetresh) continue;
				}
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
void make_warp(Mat& src_img, Mat& dst_img, Point2f Point[],int size ){
	Point2f dest_point[4];
	dest_point[0].x = 0;
	dest_point[0].y = 0;
	dest_point[1].x = size;
	dest_point[1].y = 0;
	dest_point[2].x = size;
	dest_point[2].y = size;
	dest_point[3].x = 0;
	dest_point[3].y = size;

	Mat transform_matrix = getPerspectiveTransform(Point, dest_point);
	warpPerspective(src_img, dst_img, transform_matrix, Size(size, size));
}
/*
void makehist(Mat& image, int hist[], int histsize, int modelsize){
	memset(hist, 0, histsize);
	for( int i = 0 ; i < modelsize ; i++){
		for( int j = 0 ; j < modelsize ; j++){
			int index = image.at<uchar>(i, j) / (255/ histsize);
			if( index == histsize) index = index-1;
			hist[index]++;
		}
	}
//	for( int i = 0 ;i < size ; i++){
//		hist[i] = 0;
//		for( int j = 0 ; j < size ; j++){
//			hist[i] += image.at<uchar>(i, j);
//		}
//		hist[i] = hist[i]/size;
//	}
}
double cossim(int hist1[], int hist2[], int size) {
	int scalar = 0;
	double sum1;
	double sum2;
	for( int i = 0 ; i < size ; i++){
		scalar += hist1[i] * hist2[i];
		sum1 += hist1[i] * hist1[i];
		sum2 += hist2[i] * hist2[i];
	}
	sum1 = sqrt(sum1);
	sum2 = sqrt(sum2);

	double cos_theta = (double) scalar / (sum1*sum2);
	double result = 1- ( 2 * acos( cos_theta) / PI ) ;

	return result;
}*/
bool monitoring_change(Mat& image1, Mat& image2, vector<Point2f>& P1, vector<Point2f>& P2, int modelsize, int histsize, double thresh){

	Mat dst1;
	Mat dst2;
	Point2f Point1[4];
	Point2f Point2[4];

	for( int i = 0 ;i < 4 ; i++){
		Point1[i].x = P1[i].x;
		Point1[i].y = P1[i].y;
		Point2[i].x = P2[i].x;
		Point2[i].y = P2[i].y;
	}
	make_warp(image1, dst1, Point1, modelsize);
	make_warp(image2, dst2, Point2, modelsize);

	MatND hist1;
	MatND hist2;

	float range_0[] = {0,255};
	const float* ranges[] = { range_0 };
	int channels[] = {0};
	int hist_size[] = {256};
	hist_size[0] = histsize;
	calcHist(&dst1, 1, channels, Mat(), hist1, 1, hist_size, ranges, true, false);
	calcHist(&dst2, 1, channels, Mat(), hist2, 1, hist_size, ranges, true, false);

	normalize(hist1, hist1, 1, 0, NORM_L1);
	normalize(hist2, hist2, 1, 0, NORM_L1);

	double bhattacharyya = compareHist(hist1, hist2,CV_COMP_BHATTACHARYYA);
	double correl = compareHist(hist1, hist2, CV_COMP_CORREL);
	LOGI("%f", bhattacharyya);
	LOGI("%f", correl);
	//if( correl - bhattacharyya > thresh) return true;
	if( correl > thresh ) return true;
	else	return false;
	/*
	int *hist1 = new int[histsize];
	int *hist2 = new int[histsize];

	makehist( dst1, hist1, histsize, modelsize);
	makehist( dst2, hist2, histsize, modelsize);
	double t = cossim(hist1, hist2, histsize);
	LOGI("%f", t);
	if( t > thresh) return true;
	else	return false;
	*/

}
extern "C"{
JNIEXPORT void JNICALL Java_com_example_finalone1_NativeJava_findfeature(JNIEnv *env,jobject, jlong addrBgra ,jlong addrGray, jlong addrCanny, jintArray xpoint, jintArray ypoint){

	jint* _xpoint = env->GetIntArrayElements(xpoint,0);
	jint* _ypoint = env->GetIntArrayElements(ypoint,0);

	Mat& mGr = *(Mat*)addrGray;
	Mat& mCn = *(Mat*)addrCanny;
	Mat& mBgra = *(Mat*)addrBgra;

	first = mGr.clone();
	firstset = true;

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




	getlinecross(line1, line2, &crspt[0]);
	getlinecross(line2, line3, &crspt[1]);
	getlinecross(line3, line4, &crspt[2]);
	getlinecross(line4, line1, &crspt[3]);
	for( int i = 0 ; i < 4 ; i++){
		circle(mBgra, crspt[i], 2, Scalar(255,0,0,255),30 );
		//LOGI("%d, %d", crspt[i].x, crspt[i].y);
	}




	for( int i = 0; i < lines.size(); i++) {
		temp.x = lines[i][0];
		temp.y = lines[i][1];
		circle(mBgra, temp, 2,Scalar(255,0,0,255),4);
		temp.x = lines[i][2];
		temp.y = lines[i][3];
		circle(mBgra, temp, 2,Scalar(255,0,0,255),4);
	}


	// find corner
	vector<Point2f> point;
	point.clear();
	int max_count = C_MAX_COUNT;

	//print line dot
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

JNIEXPORT void JNICALL Java_com_example_finalone1_NativeJava_warp(JNIEnv *env,jobject, jlong addrBgra){
	Mat& mBgra = *(Mat*)addrBgra;

	int width = mBgra.size().width;
	int height = mBgra.size().height;

	Point2f source_point[4];
	Point2f dest_point[4];

	for(int i = 0 ; i < 4 ; i++){
		source_point[i].x = crspt[i].x;
		source_point[i].y = crspt[i].y;
	}
	double len1 = distance(crspt[0].x, crspt[0].y, crspt[1].x, crspt[1].y);
	double len2 = distance(crspt[1].x, crspt[1].y, crspt[2].x, crspt[2].y);


	if( len1 > len2) { // len1 is long
		dest_point[0].x = 0;
		dest_point[0].y = 0;
		dest_point[1].x = width;
		dest_point[1].y = 0;
		dest_point[2].x = width;
		dest_point[2].y = height;
		dest_point[3].x = 0;
		dest_point[3].y = height;


	}else{
		dest_point[0].x = 0;
		dest_point[0].y = height;
		dest_point[1].x = 0;
		dest_point[1].y = 0;
		dest_point[2].x = width;
		dest_point[2].y = 0;
		dest_point[3].x = width;
		dest_point[3].y = height;
	}

	Mat temp = mBgra.clone();
	Mat transform_matrix = getPerspectiveTransform(source_point,dest_point);
	warpPerspective(temp, mBgra, transform_matrix, Size(width, height));
}
JNIEXPORT int JNICALL Java_com_example_finalone1_NativeJava_tracking(JNIEnv *env, jobject obj, jint width, jint height, jbyteArray yuv, jintArray bgra, jboolean mode){

	jbyte* _yuv = env->GetByteArrayElements(yuv,0);
	jint* _bgra = env->GetIntArrayElements(bgra, 0);

	Mat mYuv(height+height/2 , width, CV_8UC1, (unsigned char *)_yuv);
	Mat mGr(height, width, CV_8UC1,(unsigned char *)_yuv);
	Mat mBgra(height, width, CV_8UC4, (unsigned char *)_bgra);

//	vector<Point2f> srcpt1(4);
//	vector<Point2f> srcpt2(4);
//	vector<uchar> status(4);
//	vector<float> error(4);

	vector<Point2f> srcpt1;
	vector<Point2f> srcpt2;
	vector<uchar> status;
	vector<float> error;
	cvtColor(mYuv, mBgra, CV_YUV420sp2BGR, 4);

	if(firstset) {
		if( mode) {
			for( int i = 0; i < 4; i++) {
				Point2f temp;
				temp.x = crspt[i].x;
				temp.y = crspt[i].y;
				srcpt1.push_back(temp);
				circle(mBgra, temp, 2,Scalar(0,255,255,255),20);
			}
			calcOpticalFlowPyrLK(
					first,
					mGr,
					srcpt1,
					srcpt2,
					status,
					error,
					Size(50,50),
					5
			);
			for( int i = 0; i < srcpt1.size(); i++) {
//				LOGI("%d  :  %d ", i, status[i]);
				Point p1, p2;
				p1.x = (int) srcpt1[i].x;
				p1.y = (int) srcpt1[i].y;
				p2.x = (int) srcpt2[i].x;
				p2.y = (int) srcpt2[i].y;
				line(mBgra, p1, p2, Scalar(0,0,255,255), 10);
//				crspt[i].x = srcpt2[i].x;
//				crspt[i].y = srcpt2[i].y;
			}
			for( int i = 0; i < 4; i++) {
				Point2f temp;
				temp.x = crspt[i].x;
				temp.y = crspt[i].y;
				circle(mBgra, temp, 2,Scalar(0,255,0,255),20);
			}
			if( monitor % 20  == 0){
				monitor = 0;
				if(monitoring_change(
					first,
					mGr,
					srcpt1,
					srcpt2,
					300,
					100,
					0.9	)){
					LOGI("ttttttttt");
					monitorset= true;
					// not change
				}else{
					LOGI("fffffffff");
					monitorset = false;
					// change
				}
			}
			monitor++;
			if(!monitorset ) {
				Point2f temp;
				temp.x = width/2;
				temp.y = height/2;
				circle(mBgra, temp, 2,Scalar(255,0,0,255),100);
			}
//			LOGI("555555");
//			first.release();
//			first = mGr.clone();
		}
	}
	env->ReleaseIntArrayElements(bgra, _bgra,0);
	env->ReleaseByteArrayElements(yuv,_yuv,0);
}
}
