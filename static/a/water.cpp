#include <stdio.h>
#include <iostream>
#include <string>  
#include <cv.h>  
#include <highgui.h>


using namespace std;
using namespace cv;
#define NUM_FRAME 1000
#define NUMIM 70

struct image2video
{
public:
	void image_to_video();
	image2video(int freW,int freH)
	:i(0),img(0),writer(0),isColor(1),fps(30),frameW(800),frameH(600),num_kkk(45),kkk(0)
	{
		frameH = freH;
		frameW = freW;
		cout<<kkk<<endl;
	}

private:
	int i;
	IplImage * img;
	char image_name[100];
	CvVideoWriter *writer;
	int isColor;
	int fps; // 30or 25
	int frameW; // 744 for firewire cameras
	int frameH; // 480 for firewire cameras
	int num_kkk;//ÑÓ³ÙÖ¡Êý
	int kkk;//ÑÓ³Ù¼ÆÊý

};

void image2video::image_to_video()
{
	writer=cvCreateVideoWriter("out.avi",CV_FOURCC('X','V','I','D'),fps,cvSize(frameW,frameH),isColor);
	printf("\tvideo height : %d\n\tvideo width : %d\n\tfps : %d\n", frameH, frameW, fps);
	while(i<NUM_FRAME)
	{
		//cout<<NUM_FRAME<<endl;
		sprintf(image_name, "./waterres/0results%d.jpg", i);
		img = cvLoadImage(image_name);
		if(!img)
		{
			printf("Could not load image file...\n");
			break;
		}
		//cvShowImage("mainWin", img);
		char key = cvWaitKey(20);
		cvWriteFrame(writer, img);
		i++;
	}
	cvReleaseVideoWriter(&writer);
}

int main(int argc, char *argv[])
{

	char targetImpath[100];
	sprintf(targetImpath, "./input/%s.jpg", argv[1]);
	string shuidiImpath = "./mask.jpg";

	Mat targetIm = imread(targetImpath,CV_LOAD_IMAGE_COLOR);
	Mat shuidiIm1 = imread(shuidiImpath,CV_LOAD_IMAGE_COLOR);
	int rowIm = targetIm.rows;
	int colIm = targetIm.cols;

	//cout<<shuidiIm1.cols<<endl;
	//cout<<colIm<<endl;
	Mat shuidiIm;
	if (shuidiIm1.rows != rowIm || shuidiIm1.cols != colIm)
	{
		Size dsize = Size(colIm,rowIm);
		resize(shuidiIm1,shuidiIm,dsize);
	}else
	shuidiIm = shuidiIm1.clone();
	//cout<<shuidiIm.cols<<endl;
	cout<<colIm<<endl;
	Mat imageROI;
	imageROI = targetIm(cv::Rect(0,0,shuidiIm.cols,shuidiIm.rows));
	addWeighted(imageROI, 0.6, shuidiIm, 0.4, 0, imageROI);

	//cout<<targetIm.cols<<' '<<targetIm.rows


	for (int num_pic=0;num_pic<NUMIM;num_pic++)
	{  
		Mat imageROI1;
		Mat targetIm1 = targetIm.clone();
		char liushuiImpath[200];
		sprintf(liushuiImpath,"./waterimg/image%d.jpg",num_pic);
		Mat liushuiIm = imread(liushuiImpath,CV_LOAD_IMAGE_COLOR);

		Mat liushuiIm1;
		if (liushuiIm.rows != rowIm || liushuiIm.cols != colIm)
		{
			Size dsize = Size(colIm,rowIm);
			resize(liushuiIm,liushuiIm1,dsize);
		}else
		liushuiIm1 = liushuiIm.clone();
		cout<<liushuiIm1.rows<<endl;
		imageROI1 = targetIm1(cv::Rect(0,0,liushuiIm1.cols,liushuiIm1.rows));
		addWeighted(imageROI1, 1, liushuiIm1, 0.7, 0, imageROI1);


		char image_name[200];
		sprintf(image_name, "./waterres/0results%d.jpg", num_pic);
		cout<<image_name<<endl;
		imwrite(image_name, targetIm1);

	}
	image2video i2v(colIm,rowIm);
	i2v.image_to_video();

	return 0;
}