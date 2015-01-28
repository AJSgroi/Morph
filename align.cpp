#include <cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define w 100

//add function headers
void FilledCircle(Mat img, Point center, Scalar color);
void DrawNumber(Mat img, Point center, int number, Scalar color);

int main (int argc, char** argv){

	//checking that the number of arguments is correct
	if (argc != 7) {
		cerr << "Incorrect usage of ./align"<<endl;
		cerr << endl;
		cerr << "Usage : ./align source_image source_featureFile target_image target_featureFile output_ImageName output_featureFile" << endl;
		cerr << endl;
		cerr << "source_image :		RGB input face image to be transformed" <<endl;
		cerr << "source_featureFile : 	text file where each line is an x,y pair separated by a comma"<<endl;
		cerr << "target_image : 		RGB input face image to transform to" << endl;
		cerr << "target_featureFile :	test file where each line is an x,y pair separated by a comma"<<endl;	
		cerr << "output_ImageName :	desire output image name with extension" << endl;
		cerr << "output_featureFile :	desire output point file name with extension" << endl;
		cerr << endl;
		exit(-1);
	}
	//getting the source image by input name
	Mat src1 = imread(argv[1], IMREAD_COLOR);
	Mat src;
	src1.copyTo(src);

	//getting the target image by input name
	Mat target = imread(argv[3], IMREAD_COLOR);

	//opening file of source points
	vector<int> vec_src;
	string token;
	ifstream srcfile(argv[2]);
	while(getline(srcfile, token, ',')){
		istringstream line(token);
		while(line >> token){
			int t;
			istringstream( token ) >> t;
			vec_src.push_back(t);
		}
	}
	
	//initialize values for plotting points
	//int count = 1;
	//Scalar color = Scalar(0,255,0);
	
	//create vector of points to hold the source values
	vector<Point> srcPoints;
	for(vector<int>::size_type i = 0; i != vec_src.size(); i=i+2){

		srcPoints.push_back(Point(vec_src[i]/2, vec_src[i+1]/2));
	//	Point tmp = Point(vec_avg[i], vec_avg[i+1]);
	//	FilledCircle(src, tmp, color);
	}
	
	
	vector<int> vec_tgt;
	string token2;
	ifstream tgtfile(argv[4]);
	while(getline(tgtfile, token2, ',')){
		istringstream line2(token2);
		while(line2 >> token2){
			int t;
			istringstream( token2 ) >> t;
			vec_tgt.push_back(t);
		}
	}
	
	//reinitialize values for plotting points, but with new color
	//count = 1;
	//color = Scalar(0,0,255);
	
	//create vector of points to hold the destination values 
	vector<Point> tgtPoints;
	for(vector<String>::size_type i = 0; i < vec_tgt.size(); i=i+2){

		tgtPoints.push_back(Point(vec_tgt[i]/2, vec_tgt[i+1]/2));
	//	Point tmp = Point(vec[i], vec[i+1]);
	//	FilledCircle(target, tmp, color);
	}
	

	//resize the source image and transform the points
	if(src1.rows != target.rows || src1.cols != target.cols){
			
		//Resizing the images to match in dimension;
		resize(src1, src, target.size(), 0, 0, INTER_LINEAR);
		
		//determine resize parameters
		double a = src1.rows;
		double b = target.rows;

		double scale_x = b/a;
		
		a = src1.cols;
		b = target.cols;

		double scale_y = b/a;
	
		//Transforming the feature points based on resize parameters
		for(int i = 0; i < srcPoints.size(); i++){
			
			Point tmp = srcPoints.at(i);
			tmp.x = tmp.x*scale_y;
			tmp.y = tmp.y*scale_x;
			srcPoints.at(i) = tmp;		

		/*	Scalar color2 = Scalar(0,255,0);
			FilledCircle(src, tmp, color2);
		
			Point tmp2 = tgtPoints.at(i);
			Scalar color3 = Scalar(0,0,255);
			FilledCircle(src, tmp2, color3);
		*/
		}
	}

	//Now we want to align based on eye points

	//use eye centers and nose centers
	vector<Point2f> affine_point_src;
	vector<Point2f> affine_point_dst;
	
	affine_point_src.push_back(srcPoints.at(49));		//tip of the nose
	affine_point_src.push_back(srcPoints.at(124));		//left outer eye
	affine_point_src.push_back(srcPoints.at(144));		//right outer eye
	affine_point_dst.push_back(tgtPoints.at(49));
	affine_point_dst.push_back(tgtPoints.at(124));
	affine_point_dst.push_back(tgtPoints.at(144));
	

	//create destination matrix/image
	Mat rot_src = Mat :: zeros(src.rows, src.cols, src.type());
	Mat affine1 = getAffineTransform(affine_point_src, affine_point_dst);
	warpAffine(src, rot_src, affine1, rot_src.size());

	//convert transformation matrix
	affine1.convertTo(affine1, CV_32FC1,1,0);	


	//write the endpoints to a file
	ofstream outFile;
	string fileName = argv[6];
	outFile.open(fileName.c_str());

	//reshape all the source points based on transformation matrix
	for(int i = 0; i < srcPoints.size(); i++){

		Point2f tmp = srcPoints.at(i);
		vector<Point3f> vec;
		vec.push_back(Point3f(tmp.x,tmp.y,1));
		Mat srcMat = Mat(vec).reshape(1).t();
		Mat dstMat = affine1*srcMat;
		tmp = Point2f(dstMat.at<float>(0,0), dstMat.at<float>(1,0));
	
		srcPoints.at(i) = tmp; 			

		outFile << tmp.x << "," << tmp.y << endl;

		/*
		FilledCircle(rot_src, tmp, Scalar(0,255,0));

		Point2f tmp2 = tgtPoints.at(i);
		FilledCircle(rot_src, tmp2, Scalar(0,0,255));
		*/
	}
	
	//close output file used for writing endpoints
	outFile.close();
	
	//copy adjusted to source
	rot_src.copyTo(src);

	//write new image
	string outfile_img = argv[5];
	imwrite(outfile_img, src);

	return 0;
}

//drawing a fixed size filled circle to use as an image marker
void FilledCircle(Mat img, Point center,Scalar color){

	int thickness = -1;
	int lineType = 8;

	circle(img, center, w/32, color , thickness, lineType);


}

//place a given number at the input location
void DrawNumber(Mat img, Point center, int number, Scalar color){

	string text;
	ostringstream convert;
	convert << number;
	text = convert.str();
	int lineType = 8;
	putText( img, text, center, CV_FONT_HERSHEY_COMPLEX, 2, color, 2, lineType);

}
