#include <cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
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
void DrawLine(Mat img, Point start, Point end);
void DrawNumber(Mat img, Point center, int number);

int main (int argc, char** argv){

	//checking that the number of arguments is correct
	if (argc != 5) {
		cerr << "Incorrect usage of ./visualize"<<endl;
		cerr << endl;
		cerr << "Usage : ./visualize source_image source_featureFile output_ImageName output_featureFile" << endl;
		cerr << endl;
		cerr << "source_image :		RGB input face image to be transformed" <<endl;
		cerr << "source_featureFile : 	text file where each line is an x,y pair separated by a comma"<<endl;
		cerr << "output_ImageName :	desired output image name with extension" << endl;
		cerr << "output_featureFile :	desired output point file name with extension" << endl;
		cerr << endl;
		exit(-1);
	}

	//read source image
	Mat src1 = imread(argv[1], IMREAD_COLOR);
	Mat src;
	src1.copyTo(src);	

	//read source image feature points
	vector<int> vec;
	string token;
	ifstream file(argv[2]);
	while(getline(file, token, ',')){
		istringstream line(token);
		while(line >> token){
			int t;
			istringstream( token ) >> t;
			vec.push_back(t);
		}
	}
	
	//initialize plotting values
	//int count = 1;
	Scalar color = Scalar(255,0,0);
	
	//create vector of points
	vector<Point2f> asm_points;

	//for(vector<String>::size_type i = 0; i != vec.size(); i=i+2){
	for(vector<String>::size_type i = 0; i != vec.size(); i=i+2){
	
		//marking all the stasm points on the image as a check
		float x = vec[i]/2;
		float y = vec[i+1]/2;
		//cout << x << " ," << y << endl;
		FilledCircle(src1, Point(x,y), color);
		//DrawNumber(src, Point(x,y), count);
		asm_points.push_back(Point2f(x,y));
		//count++;
	}


	//write the endpoints to a file
	ofstream outFile;
	string fileName2 = argv[4];
	outFile.open(fileName2.c_str());
	outFile << "192\n";
	

	//iterate through stasm points to draw a line between them all
	for(vector<Point3f>::size_type i = 0; i != asm_points.size(); i++){
		//set initial guess at start and end of line
		Point2f start = asm_points[i];
		Point2f end = asm_points[i+1];
		
		//exit for end of profile
		if(i == 40){
			continue;
		}
		//exit for end of nose
		else if(i == 57){
			continue;
		}
		//exit for end of outerlip
		else if(i == 85){
			end = asm_points[58];
		}
		//exit for end of innerlip
		else if(i == 113){
			end = asm_points[86];
		}
		//exit for end of right eye
		else if(i == 133){
			end = asm_points[114];
		}
		//exit for end of left eye
		else if(i == 153){
			end = asm_points[134];
		}
		//exit for end of right eyebrow
		else if(i == 173){
			end = asm_points[154];
		}
		//exit for end of left eyebrow
		else if(i == 193){
			end = asm_points[174];
		}

		//Draw line if allowed through
		//DrawLine(src, start, end);	
		
		//write endpoints to text file
		outFile << start.x << " " << start.y << " " << end.x << " " << end.y << "\n";
	}


	//write new image
	imwrite(argv[3], src1);
	
	//close output file used for writing endpoints
	outFile.close();

	return 0;
}

//drawing a fixed size filled circle to use as an image marker
void FilledCircle(Mat img, Point center,Scalar color){

	int thickness = -2;
	int lineType = 8;

	circle(img, center, w/32, color , thickness, lineType);


}

//drawing a line between two points
void DrawLine(Mat img, Point start, Point end){

	int thickness = 2;
	int lineType = 8;
	line(img , start, end, Scalar(0,0,255), thickness, lineType);

}

//place a given number at the input location
void DrawNumber(Mat img, Point center, int number){

	string text;
	ostringstream convert;
	convert << number;
	text = convert.str();
	int lineType = 8;
	putText( img, text, center, CV_FONT_HERSHEY_COMPLEX, 0.25, Scalar(0,0,255), 1, lineType);

}

