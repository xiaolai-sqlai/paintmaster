#include <stdio.h>
#include <iostream>
#include <highgui.h>
#include <cv.h>

using namespace std;
using namespace cv;
#define NUMIM 66
#define NUM_FRAME 1000

struct image2video
{
public:
	void image_to_video();
	image2video(int freW,int freH)
		:i(1),img(0),writer(0),isColor(1),fps(30),frameW(800),frameH(600),num_kkk(45),kkk(0)
	{
		if(freW!=0 || freH != 0)
		{frameH = freH;
		frameW = freW;}
		//cout<<kkk<<endl;
	}

private:
	int i;
	IplImage *img;
	char image_name[100];
	CvVideoWriter *writer;
	int isColor;
	int fps; // 30or 25
	int frameW; // 744 for firewire cameras
	int frameH; // 480 for firewire cameras
	int num_kkk;
	int kkk;

};


void image2video::image_to_video()
{
	writer=cvCreateVideoWriter("signature.avi",CV_FOURCC('X','V','I','D'),fps,cvSize(frameW,frameH),isColor);
	printf("\tvideo height : %d\n\tvideo width : %d\n\tfps : %d\n", frameH, frameW, fps);
	while(i<NUM_FRAME)
	{
		//cout<<NUM_FRAME<<endl;
		sprintf(image_name, "./signature/0result%d.jpg", i);
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
	//cvDestroyWindow("mainWin");
	//system("pause");
}


int main(int argc, char *argv[])
{
	char houzui[16] = "creater:";
	char *strss = argv[1];
	int r=60,g=60,b=60;
	cout<<strlen(strss)<<endl;
	char *kongge = new char[strlen(strss)];
	for (int i=0;i<strlen(strss);i++)
	{
		if (i=strlen(strss)-1)
		{
			kongge[i] = '\0';
		}else
			kongge[i] = '\t';
	}
	for(int i=0;i<NUMIM;i++)
	{
		char imname[256];
		sprintf(imname,"image%d.jpg", i+1);
		IplImage* pImg = cvLoadImage(imname);
		int ImH = pImg->height;
		int ImW = pImg->width;
	

		if (!pImg) 
		{ 
			cout<<"pImg load error..."<<endl; 
			system("pause"); 
			exit(-1); 
		} 


		CvFont font; 
		cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX, 1, 1, 1, 2, 8); 
	  if(r>255) r=255;
    if(g>255) g=255;
	  if(b>255) b=255;
	 
		char qianming1[256];
		sprintf(qianming1, "%s", strss);
		cvPutText(pImg,qianming1, cvPoint(ImH/3-ImH/6,ImW/3-ImW/6), &font, CV_RGB(220,220,220));
 
		char qianming[256];
	   sprintf(qianming,"%s", argv[1]);
	   cvPutText(pImg,qianming, cvPoint(ImH/3,ImW/3), &font, CV_RGB(r,g,b));    

		r=r+3;
		g=g+3;
		b=b+3;
		char saveName[256];
		sprintf(saveName,"./signature/0result%d.jpg", i+1);
		cvSaveImage(saveName, pImg);   
  
		cvReleaseImage(&pImg); 

	}

	image2video I2V(800,600);
	I2V.image_to_video();

	return 0;
}
