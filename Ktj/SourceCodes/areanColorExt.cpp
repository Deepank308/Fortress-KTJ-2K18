#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include"opencv2/core/core.hpp" 
#include<iostream>
#include<math.h>
#include<bits/stdc++.h>
#include<stdlib.h>
#include<time.h>
#include<fstream>
using namespace std;
using namespace cv;

Mat arena_feed=imread("rnd1_arena.png",1);
int H=-1;int S=-1;int V=-1;

void Callbackfunc(int Event, int y, int x, int flags, void* userdata)
{
	if (Event == EVENT_LBUTTONDOWN)
	{
		cout<<" Y= "<<x<<" X= "<<y<<"\n";
		printf("H=%d\n", arena_feed.at<Vec3b>(x,y)[0]);
		H=arena_feed.at<Vec3b>(x, y)[0];
		printf("S=%d\n", arena_feed.at<Vec3b>(x, y)[1]);
		S=arena_feed.at<Vec3b>(x, y)[1];
		printf("V=%d\n\n",arena_feed.at<Vec3b>(x, y)[2]);
		V=arena_feed.at<Vec3b>(x, y)[2];
	}
}

    	
int main()
{
	Mat arena_feedBGR=arena_feed.clone();
	cvtColor(arena_feed,arena_feed,CV_BGR2HSV);
	Mat dest(arena_feed.rows, arena_feed.cols, CV_8UC1, Scalar(0));
    int hl1,sl1,vl1,hh1,sh1,vh1;
    cout<<"click on the region "<<endl;
   	namedWindow("original", WINDOW_NORMAL);
   	char ans='N';
   	do
   	{
		H=-1;S=-1;V=-1;
		imshow("original", arena_feedBGR);
		waitKey(50);
		setMouseCallback("original", Callbackfunc, NULL);
		if(H!=-1)
		{
			hl1=H-30;sl1=S-40;vl1=V-20;hh1=H+30;sh1=S+40;vh1=V+20;
			if(hl1<0) hl1=0;
			if(sl1<0) sl1=0;
			if(vl1<0) vl1=0;
			if(hh1>179) hh1=179;
			if(sh1>255) sh1=255;
			if(vh1>255) vh1=255;
			printf("(%d,%d,%d) to (%d,%d,%d)",hl1,sl1,vl1,hh1,sh1,vh1);
			cout<<"\nAre you accepting the limits(Y/y) "<<endl;
			cin>>ans;
			if(!(ans=='y'|| ans=='Y'))
			{
				H=-1;S=-1;V=-1;
				cout<<"Click again \n";
			}
		}
   	}while((H==-1 )||!(ans=='y'|| ans=='Y'));
	inRange(arena_feed,Scalar(hl1, sl1, vl1), Scalar(hh1,sh1,vh1), dest);
	Mat element = getStructuringElement( MORPH_RECT,Size( 2*1 + 1, 2*1+1 ),Point( 1, 1 ) );
	
	namedWindow("arena Non",WINDOW_NORMAL);	
	imshow("arena Non",dest);

	for(int i=0;i<3;i++) erode(dest,dest,element);
	for(int i=0;i<5;i++) dilate(dest,dest,element);
	
	namedWindow("arena",WINDOW_NORMAL);	
	imshow("arena",dest);
	imwrite("arena_obj1.png",dest);
	waitKey(0);
}
