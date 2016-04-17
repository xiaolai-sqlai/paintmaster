#include <stdio.h>
#include <iostream>
#include <string>  
#include <cv.h>  
#include <highgui.h>

#define file "1.png"
using namespace std;
using namespace cv;

void GaussKernel1D(double* filter, int r, double sigma)
{
	int length = 2*r+1;
	double sum = 0;
	for (int i = 0; i < length; i++)
	{
		filter[i] = exp( (double)( -(i-r)*(i-r) )/( 2.0*sigma*sigma ) );
		sum += filter[i];
	}
	for (int i = 0; i< length; i++)
	{
		filter[i] = filter[i] / sum;
	}
}
Mat GaussFilterFast(Mat src, int radius, double sigma)
{
	int rows = src.rows;
	int cols = src.cols;
	Mat temp = Mat::zeros(rows, cols, CV_8UC3);
	Mat out = Mat::zeros(rows, cols, CV_8UC3);
	double kernel[1000] = {0};
	GaussKernel1D(kernel, radius, sigma);
	int rem;
	double K;
	double tempR = 0.0, tempG = 0.0, tempB = 0.0;
	//double tempGray = 0.0;
    for (int j = 0; j < cols; j++)
    {
        for (int i = 0; i < rows; i++)
		{
			tempR = 0.0, tempG = 0.0, tempB = 0.0;
			//tempGray = 0.0;
			for (int k = -radius; k < radius; k++)
			{
				rem = (abs(i+k)%rows);
				K = kernel[k+radius];
				tempR += src.at<Vec3b>(rem,j)(0)*K;
				tempG += src.at<Vec3b>(rem,j)(1)*K;
				tempB += src.at<Vec3b>(rem,j)(2)*K;
				//tempGray += src.at<uchar>(rem,j)*K;				
			}
			temp.at<Vec3b>(i,j)(0) = (uchar)tempR;
			temp.at<Vec3b>(i,j)(1) = (uchar)tempG;
			temp.at<Vec3b>(i,j)(2) = (uchar)tempB;
			//temp.at<uchar>(i,j) = (uchar)tempGray;
		}
	}
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			tempR = 0.0; tempG = 0.0; tempB = 0.0;
			//tempGray = 0.0;
			for (int k = -radius; k <= radius; k++)
			{
				rem = (abs(j+k)%cols);
				K = kernel[k+radius];
				tempR += src.at<Vec3b>(i,rem)(0)*K;
				tempG += src.at<Vec3b>(i,rem)(1)*K;
				tempB += src.at<Vec3b>(i,rem)(2)*K;
				//tempGray += temp.at<uchar>(i,rem)*K;
				//cout<<K<<endl;
			}
			out.at<Vec3b>(i,j)(0) = (uchar)tempR;
			out.at<Vec3b>(i,j)(1) = (uchar)tempG;
			out.at<Vec3b>(i,j)(2) = (uchar)tempB;
			//out.at<uchar>(i,j) = (uchar)tempGray;
		}
	}
	return out;
}
Mat sketch(Mat src, int edgeIntensity)
{ 
	int rows = src.rows;
	int cols = src.cols;
	if (cols > rows)
	{
		rows = (float)rows/(float)cols*512;
		cols = 512;
	}
	else
	{
		cols = (float)cols/(float)rows*512;
		rows = 512;
	}
	Mat img1 = Mat::zeros(rows, cols, CV_8UC3);	
	IplImage Img = src;
	IplImage Img1 = img1;

	cvResize(&Img, &Img1, CV_INTER_LINEAR);
	
	Mat img(&Img1);
	Mat gaussImg = GaussFilterFast(img, edgeIntensity, edgeIntensity/3);
	Mat out = Mat::zeros(rows, cols, CV_8UC1);
	int gray, grayImg, grayGauss;
	for (int j = 0; j < cols; j++)
	{
		for (int i = 0; i < rows; i++)
		{
			grayImg = (img.at<Vec3b>(i,j)(0) + img.at<Vec3b>(i,j)(1) + img.at<Vec3b>(i,j)(2)) / 3;
			grayGauss = 255 - (gaussImg.at<Vec3b>(i,j)(0) + gaussImg.at<Vec3b>(i,j)(1) + gaussImg.at<Vec3b>(i,j)(2)) / 3;
			gray = grayImg + (grayImg * grayGauss) / (256 - grayGauss);
			if(gray >= 255)		gray = 255;
			else if (gray <= 0)	gray = 0;
			out.at<uchar>(i,j) = (uchar)gray;
		}
	}
	return out;
}
int ApplySketch(int edgeInten, const char* imagename, char* path, int save_n, char* outputname)
{
	Mat img = imread(imagename);
	if(img.empty())
	{
		fprintf(stderr, "Can not load image %s\n", imagename);
		return -1;
	}

	Mat out = sketch(img, edgeInten);
	char name[50];
	sprintf(name, "./%s/%s_%d.jpg", path, outputname, save_n);
	//imshow(name, out);
	imwrite(name,out);
	//waitKey();
	return 0;
}
int main(int argc, char *argv[])
{
	int edgeInten[8] = {4, 8, 10, 15, 30, 50, 80, 100};
	//int edgeInten = 40;
	const char* imagename = argv[1];
	//Mat img = imread(imagename);
	//if(img.empty())
	//{
	//	fprintf(stderr, "Can not load image %s\n", imagename);
	//	return -1;
	//}
	for (int i = 0; i<8; i++)
		ApplySketch(edgeInten[i], imagename, argv[2], i, argv[3]);
	//imshow("out", out);
	//imwrite("out.jpg",out);
	//waitKey();
	return 0;
}
