#include <stdio.h>
#include <iostream>
#include <highgui.h>
#include <cv.h>
#include <string>


using namespace std;
using namespace cv;



int main(int argc, char *argv[])
{
	char targetImPath[50];
	sprintf(targetImPath, "sketchout/%s_7.jpg", argv[1]);
	string pencilPath = "pencil.png";

	Mat originalIm = imread(targetImPath);
	Mat backgroundIm = Mat(originalIm.size(), CV_8UC3, Scalar(255, 255, 255));

	Mat targetIm = imread(targetImPath);
	Mat pencilIm = imread(pencilPath);
	Mat mask = imread(pencilPath);
	int r = originalIm.rows;
	int c = originalIm.cols;
	int num_pix = 0;
	cout << r << ' ' << c << endl;
	Mat edgeIm;
	Canny(targetIm, edgeIm, 50, 100, 3);


	vector<vector<Point> > contours;
	cv::vector<cv::Vec4i> hierarchy;

	findContours(edgeIm, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);


	cv::vector<cv::vector<cv::Point> > contours_poly(contours.size());

	cv::vector<cv::Rect> boundRect(contours.size());

	for (int i = 0; i<contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}


	Mat drawing = Mat::zeros(edgeIm.size(), CV_8UC3);
	int pix_num = 0;
	int pix_sum=0;
	int threshold;

	for (int p = 1; p <= 50; p++)
	{
		for (int i = 0; i < contours.size(); i++)
		{
			if (boundRect[i].width >= c / p || boundRect[i].height >= r / p)
			{
				for (int k = 0; k < contours_poly[i].capacity(); k++)
				{
					pix_sum++;
				}
			}
		}

		if (pix_sum >= 350)
		{
			threshold = p;
			break;
		}
	}

	int page;
	page = 350 / 7;
	cout << "页数" << page << endl;


	for (int i = 0; i<contours.size(); i++)
	{
		Scalar color = Scalar(255, 255, 255);
		if (boundRect[i].width >= c / threshold || boundRect[i].height >= r / threshold)
		{
			cout << boundRect[i].width << "<-宽度，高度->" << boundRect[i].height << endl;

			for (int k = 0; k < contours_poly[i].capacity(); k++)
			{
				pix_num++;
				Mat submask;
				Mat subpencilIm;
				Mat submask1;
				Mat subpencilIm1;
				Mat targetIm1;

				char path_name[100];
				sprintf(path_name, "%s%d%s", "E:\\C++Workspace\\素材文件\\素描\\1\\out", (int)(pix_num / page), ".png");
				cout << "层数" << (int)(pix_num / page)+1<< "轮廓的像素数" << pix_num << endl;
				targetIm1 = imread(path_name);


				cout << "contours_poly[" << i << "][" << k << "]" << contours_poly[i][k] << endl;
				cout << contours_poly[i][k].x << " " << contours_poly[i][k].y << endl;

				if (pix_num==pix_sum||pix_num == 350) 
				{ 
					goto breakLoop; 
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