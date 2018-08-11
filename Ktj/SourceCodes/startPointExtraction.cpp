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

Mat frame;
int H=-1,S=-1,V=-1;

void Callbackfunc(int Event, int y, int x, int flags, void* userdata)
{
	if (Event == EVENT_LBUTTONDOWN)
	{
		cout<<" Y= "<<x<<" X= "<<y<<"\n";
		printf("H=%d\n", frame.at<Vec3b>(x,y)[0]);
		H=frame.at<Vec3b>(x, y)[0];
		printf("S=%d\n", frame.at<Vec3b>(x, y)[1]);
		S=frame.at<Vec3b>(x, y)[1];
		printf("V=%d\n\n",frame.at<Vec3b>(x, y)[2]);
		V=frame.at<Vec3b>(x, y)[2];
		

	}
}

int main()
{
	VideoCapture cap(1); 
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    for(int i=0;i<10;i++)
    	cap>>frame;

//	frame=imread("arena2.jpg",1);
    char ans='N';
    Mat frameBGR=frame.clone();
    cvtColor(frame, frame, CV_BGR2HSV);
    ofstream ofs;
    ofs.open("Start_Point.txt");
    for(int k=0;k<2;k++)
    {
    	Mat dest(frame.rows, frame.cols, CV_8UC1, Scalar(0));
    	int hl1,sl1,vl1,hh1,sh1,vh1;
    	cout<<"click on the region "<<k<<endl;

    	namedWindow("original", WINDOW_NORMAL);
    	do
    	{
			H=-1;S=-1;V=-1;
			imshow("original", frameBGR);
			waitKey(50);
			setMouseCallback("original", Callbackfunc, NULL);
			//waitKey(50);
			if(H!=-1)
			{
				hl1=H-30;sl1=S-30;vl1=V-30;hh1=H+30;sh1=S+30;vh1=V+30;
				if(hl1<0) hl1=0;
				if(sl1<0) sl1=0;
				if(vl1<0) vl1=0;
				if(hh1>179) hh1=179;
				if(sh1>255) sh1=255;
				if(vh1>255) vh1=255;
				printf("(%d,%d,%d) to (%d,%d,%d)",hl1,sl1,vl1,hh1,sh1,vh1);
				cout<<"\nAre you accepting the  limits(Y/y) "<<endl;
				cin>>ans;
				if(!(ans=='y'|| ans=='Y'))
				{
					H=-1;S=-1;V=-1;
					cout<<"Click again  \n";
				}
			}
    	}while((H==-1 )||!(ans=='y'|| ans=='Y'));
		inRange(frame,  Scalar(hl1, sl1, vl1), Scalar(hh1,sh1,vh1), dest);
		Mat element = getStructuringElement( MORPH_RECT,Size( 2*1 + 1, 2*1+1 ),Point( 1, 1 ) );
		for(int i=0;i<3;i++) erode(dest,dest,element);
		for(int i=0;i<5;i++) dilate(dest,dest,element);
		vector<vector<Point> > contours;
    	vector<Vec4i> hierarchy;
		findContours(dest, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
		Mat drawing = Mat::zeros(frame.size(), CV_8UC3 );
    	vector<Rect> rect(contours.size());
    	Point center;
    	int max_area=0;
    	cout<<contours.size()<<" size"<<endl;
    	for( int i = 0; i<contours.size(); i++ )
    	{
        	drawContours( drawing, contours, i,Scalar(0,0,255), 2, 8, hierarchy, 0, Point() );
        	/*if(hierarchy[i][2]!=-1)
            	//if(i!=contours.size()-1)
                	if(contourArea(contours[i+1])<200)
                    	continue;*/
            if(contourArea(contours[i])<contourArea(contours[max_area]) && contourArea(contours[i])>1000) max_area=i;     
    	}
    	Rect rct=boundingRect(Mat(contours[max_area]));
        center.x=(rct.tl().x+rct.br().x)/2;
        center.y=(rct.tl().y+rct.br().y)/2;
    	dest.at<uchar>(center.y,center.x)=0;
		cout<<" centre "<<center.x<<" "<<center.y<<endl;
		rectangle(frame,rct.tl(),rct.br(),Scalar(0,0,0),CV_FILLED);
    	ofs<<to_string(center.x)+" "+to_string(center.y)<<" ";
    	imshow("Start"+to_string(k),dest);
	}
    ofs.close();
    cvtColor(frame,frame,CV_HSV2BGR);

    namedWindow("refined",WINDOW_NORMAL);
    imshow("refined",frame);
    imwrite("rnd1_arena.png",frame);
	waitKey(0);
}
