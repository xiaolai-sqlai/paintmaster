#include <stdio.h>
#include <iostream>
#include <highgui.h>
#include <cv.h>
#include <string>

using namespace std;
using namespace cv;


int main()
{
	char sourceImPath[50];
	char targetImPath[50];
  	sprintf(sourceImPath, "input/%s.jpg", argv[1]);
	sprintf(targetImPath, "sketchout/%s_7.jpg", argv[1]);
	string pencilPath = "pencil.png";

	Mat originalIm = imread(sourceImPath);
	Mat backgroundIm = Mat(originalIm.size(), CV_8UC3, Scalar(255, 255, 255));
	//Mat sourceIm = imread(sourceImPath, CV_LOAD_IMAGE_COLOR);
	Mat targetIm = imread(targetImPath);
	Mat pencilIm = imread(pencilPath);
	Mat mask = imread(pencilPath, 0);
	int r = originalIm.rows;
	int c = originalIm.cols;
	int num_pix = 0;
	cout << r << ' ' << c << endl;
	Mat edgeIm;
	Canny(targetIm, edgeIm, 50, 100, 3);

	cv::vector<cv::vector<cv::Point> > contours(1000);
	cv::vector<cv::Vec4i> hierarchy;

	findContours(edgeIm, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));



	cv::vector<cv::vector<cv::Point> > contours_poly(contours.size());
	cv::vector<cv::Rect> boundRect(contours.size());                   

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}

	Mat drawing = Mat::zeros(edgeIm.size(), CV_8UC3);
	int pix_num = 0;
	int pix_sum=0;
	for (int i = 0; i < contours.size();i++)
	{
		if (boundRect[i].width >= c / 20 || boundRect[i].height >= r / 20)
		{
			for (int k = 0; k < contours_poly[i].capacity(); k++)
			{
				pix_sum++;
			}
		}
	}
	int page;
	page = 320 / 7;
	cout << "page" << page << endl;


	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(255, 255, 255);
		if (boundRect[i].width >= c / 20|| boundRect[i].height >= r / 20)
		{
			//rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
			drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
			cout << boundRect[i].width << boundRect[i].height << endl;

			for (int k = 0; k < contours_poly[i].capacity(); k++)
			{
				pix_num++;
				Mat submask;
				Mat subpencilIm;
				Mat submask1;
				Mat subpencilIm1;
				Mat targetIm1;
				char path_name[100];
				sprintf(path_name, "./sketchout/%s_%d.jpg", argv[1], (int)(pix_num / page));
				targetIm1 = imread(path_name);
				//waitKey(0);	

				cout << "contours_poly[" << i << "][" << k << "]" << contours_poly[i][k] << endl;
				cout << contours_poly[i][k].x << " " << contours_poly[i][k].y << endl;

				if (pix_num == 350) 
				{ 
					goto breakLoop; 
				}

				if (pix_num>=320)
				{
					Mat mixIm;
					addWeighted(targetIm1, 0.7, originalIm, 0.3, 0, mixIm);
					char image_name[200];
					sprintf(image_name, "sumiaoout/image%d.jpg", ++num_pix);
					imwrite(image_name, mixIm);
					continue;
				}

				if (contours_poly[i][k].x + pencilIm.cols >= targetIm.cols)
				{
					subpencilIm = pencilIm.colRange(0, originalIm.cols - contours_poly[i][k].x);
					submask = mask.colRange(0, originalIm.cols - contours_poly[i][k].x);
					if (contours_poly[i][k].y + pencilIm.rows >= targetIm.rows)
					{
						subpencilIm1 = subpencilIm.rowRange(0, originalIm.rows - contours_poly[i][k].y);
						submask1 = submask.rowRange(0, originalIm.rows - contours_poly[i][k].y);
						subpencilIm1.copyTo(targetIm1(cv::Rect(contours_poly[i][k].x, contours_poly[i][k].y, subpencilIm1.cols, subpencilIm1.rows)), submask1);
						char image_name[200];
						sprintf(image_name, "sumiaoout/image%d.jpg", ++num_pix);
						imwrite(image_name, targetIm1);
						continue;
					}
					else
					{
						subpencilIm.copyTo(targetIm1(cv::Rect(contours_poly[i][k].x, contours_poly[i][k].y, subpencilIm.cols, subpencilIm.rows)), submask);
						char image_name[200];
						sprintf(image_name, "sumiaoout/image%d.jpg", ++num_pix);
						imwrite(image_name, targetIm1);
						continue;
					}
				}

				else if (contours_poly[i][k].y + pencilIm.rows >= targetIm.rows)
				{
					subpencilIm = pencilIm.rowRange(0, originalIm.rows - contours_poly[i][k].y);
					submask = mask.rowRange(0, originalIm.rows - contours_poly[i][k].y);
					if (contours_poly[i][k].x + pencilIm.cols >= targetIm.cols)
					{
						subpencilIm1 = subpencilIm.colRange(0, originalIm.cols - contours_poly[i][k].x);
						submask1 = submask.colRange(0, originalIm.cols - contours_poly[i][k].x);
						subpencilIm1.copyTo(targetIm1(cv::Rect(contours_poly[i][k].x, contours_poly[i][k].y, subpencilIm1.cols, subpencilIm1.rows)), submask1);
						char image_name[200];
						sprintf(image_name, "sumiaoout/image%d.jpg", ++num_pix);
						imwrite(image_name, targetIm1);
						continue;
					}
					else
					{
						subpencilIm.copyTo(targetIm1(cv::Rect(contours_poly[i][k].x, contours_poly[i][k].y, subpencilIm.cols, subpencilIm.rows)), submask);
						char image_name[200];
						sprintf(image_name, "sumiaoout/image%d.jpg", ++num_pix);
						imwrite(image_name, targetIm1);
						continue;
					}
				}

				else
				{
					pencilIm.copyTo(targetIm1(cv::Rect(contours_poly[i][k].x, contours_poly[i][k].y, pencilIm.cols, pencilIm.rows)), mask);
					char image_name[200];
					sprintf(image_name, "sumiaoout/image%d.jpg", ++num_pix);
					imwrite(image_name, targetIm1);
					continue;
				}
			}
		}
	}

 breakLoop:return 0;
    
	

}