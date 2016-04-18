#include<iostream>
#include <highgui.h>
#include <cv.h>
#include <stdlib.h>
#include <cstdlib>

using namespace std;
using namespace cv;

extern "C"
{
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
}


#define NUM_FRAME 500 //只处理前300帧，根据视频帧数可修改


void Image_to_video(int width,int height)
{
	int i = 1;
	IplImage* img = 0;
	char image_name[100];
	printf("------------- image to video ... ----------------\n");
	//初始化视频编写器，参数根据实际视频文件修改
	CvVideoWriter *writer = 0;
	int isColor = 1;
	int fps = 30; // or 25
	int frameW = width; 
	int frameH = height; 
	writer = cvCreateVideoWriter("sumiao.avi", CV_FOURCC('X', 'V', 'I', 'D'), fps, cvSize(frameW, frameH), isColor);
	printf("\tvideo height : %d\n\tvideo width : %d\n\tfps : %d\n", frameH, frameW, fps);
	//创建窗口
	cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE);
	while (i<NUM_FRAME)
	{
		sprintf(image_name, "%s%d%s", "image", i, ".jpg");
		cout << image_name << endl;


		img = cvLoadImage(image_name);
		if (!img)
		{
			printf("Could not load image file...\n");
			break;
		}
		cvShowImage("mainWin", img);
		char key = cvWaitKey(30);
		cvWriteFrame(writer, img);
		i++;
	}
	cvReleaseVideoWriter(&writer);
	cvDestroyWindow("mainWin");
	system("pause");
}




int main()
{
	string  sourceImPath = "E:\\C++Workspace\\素材文件\\素描\\原图.jpg";
	string  targetImPath = "E:\\C++Workspace\\素材文件\\素描\\素描8.png";
	string  pencilPath = "E:\\C++Workspace\\素材文件\\素描\\pencil.png";

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
	//imshow("素描线条",edgeIm);
	//waitKey(0);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// 找到轮廓
	//contours参数为检测的轮廓数组，每一个轮廓用一个point类型的vector表示
	//hiararchy参数和轮廓个数相同，每个轮廓contours[ i ]对应4个hierarchy元素hierarchy[ i ][ 0 ] ~hierarchy[ i ][ 3 ]，
	//分别表示后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号，如果没有对应项，该值设置为负数。
	//CV_RETR_TREE：建立一个等级树结构的轮廓
	findContours(edgeIm, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// 多边形逼近轮廓 + 获取矩形和圆形边界框
	vector<vector<Point> > contours_poly(contours.size());     //近似后的轮廓点集
	vector<Rect> boundRect(contours.size());                   //包围点集的最小矩形vector
															   //vector<Point2f>center(contours.size());                  //包围点集的最小圆形vector
															   //vector<float>radius(contours.size());                    //包围点集的最小圆形半径vector

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	}


	// 画多边形轮廓和包围的矩形框 
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
	cout << "轮廓数： " << contours.size() << endl;
	cout << "总像素数" << pix_sum << endl;
	int page;
	page = 320 / 7;
	cout << "页数" << page << endl;


	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(255, 255, 255);
		if (boundRect[i].width >= c / 20|| boundRect[i].height >= r / 20)
		{
			//rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
			drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
			cout << boundRect[i].width << "<-宽度，高度->" << boundRect[i].height << endl;

			for (int k = 0; k < contours_poly[i].capacity(); k++)
			{
				pix_num++;
				Mat submask;
				Mat subpencilIm;
				Mat submask1;
				Mat subpencilIm1;
				Mat targetIm1;
				//targetIm1 = imread("E:\\C++Workspace\\素材文件\\素描\\素描1.png");
				char path_name[100];
				sprintf(path_name, "%s%d%s", "E:\\C++Workspace\\素材文件\\素描\\素描", (int)(pix_num / page)+1, ".png");
				cout << "层数" << (int)(pix_num / page)+1<< "轮廓的像素数" << pix_num << endl;
				targetIm1 = imread(path_name);
				//imshow("底层",targetIm1);
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
					sprintf(image_name, "%s%d%s", "image", ++num_pix, ".jpg");//保存的图片名
					imwrite(image_name, mixIm);   //保存一帧图片
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
						//imshow("1类", targetIm1);
						//waitKey;
						cout << "1类" << endl;
						char image_name[200];
						sprintf(image_name, "%s%d%s", "image", ++num_pix, ".jpg");//保存的图片名
						imwrite(image_name, targetIm1);   //保存一帧图片
						continue;
					}
					else
					{
						subpencilIm.copyTo(targetIm1(cv::Rect(contours_poly[i][k].x, contours_poly[i][k].y, subpencilIm.cols, subpencilIm.rows)), submask);
						//imshow("2类", targetIm1);
						//waitKey;
						cout << "2类" << endl;
						char image_name[200];
						sprintf(image_name, "%s%d%s", "image", ++num_pix, ".jpg");//保存的图片名
						imwrite(image_name, targetIm1);   //保存一帧图片
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
						//imshow("3类", targetIm1);
						//waitKey;
						cout << "3类" << endl;
						char image_name[200];
						sprintf(image_name, "%s%d%s", "image", ++num_pix, ".jpg");//保存的图片名
						imwrite(image_name, targetIm1);   //保存一帧图片
						continue;
					}
					else
					{
						subpencilIm.copyTo(targetIm1(cv::Rect(contours_poly[i][k].x, contours_poly[i][k].y, subpencilIm.cols, subpencilIm.rows)), submask);
						//imshow("4类", targetIm1);
						//waitKey;
						cout << "4类" << endl;
						char image_name[200];
						sprintf(image_name, "%s%d%s", "image", ++num_pix, ".jpg");//保存的图片名
						imwrite(image_name, targetIm1);   //保存一帧图片
						continue;
					}
				}

				else
				{
					pencilIm.copyTo(targetIm1(cv::Rect(contours_poly[i][k].x, contours_poly[i][k].y, pencilIm.cols, pencilIm.rows)), mask);
					//imshow("5类", targetIm1);
					//waitKey;
					cout << "5类" << endl;
					char image_name[200];
					sprintf(image_name, "%s%d%s", "image", ++num_pix, ".jpg");//保存的图片名
					imwrite(image_name, targetIm1);   //保存一帧图片
					continue;
				}
			}
		}
	}
	//imshow("轮廓线条", drawing);
	//waitKey(0);

    breakLoop:char cmdstr[256];
	char mydir[256]="E:\\C++Workspace\\sumiao\\sumiao";
	sprintf(cmdstr, "cd /d \"%s\" & ffmpeg.exe %s", mydir, "-i image%d.jpg -r 30 out.mp4");
	system(cmdstr);
	//Image_to_video(originalIm.cols, originalIm.rows);

	//system("pause");
	return 0;

}