#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include"opencv2/core/core.hpp" 
#include<iostream>
#include<math.h>
#include<bits/stdc++.h>
#include<stdlib.h>
using namespace std;
using namespace cv;

int count=0;

vector<Point> pnt;
Mat a=imread("arena1.png",0);
int i=0;
int line_obs(int x,int y)
{
	Point u;
	u.x=x;
	u.y=y;
	int flag=1,z;
	i=pnt.size()-1;
	double tan_val;
	if(u.x==pnt[i].x)
	{
		if(pnt[i].y<u.y)
		{
			for(int j=pnt[i].y;j<=u.y;j++)
			{
				if(a.at<uchar>(j,u.x)==255)
					return 0;
			}
		}
		else
		{
			for(int j=u.y;j<=pnt[i].y;j++)
			{
				if(a.at<uchar>(j,u.x)==255)
					return 0;
			}
		}
		return 1;
	}
	else
	tan_val=(double)(u.y-pnt[i].y)/(u.x-pnt[i].x);
	if(u.x>pnt[i].x)
	{
		for(int j=pnt[i].x;j<=u.x;j++)
		{
			 z=(tan_val*(j-pnt[i].x)+pnt[i].y);
			if(a.at<uchar>(z,j))
					return 0;
		}
	}
	else
	{
		for(int j=pnt[i].x;j<=u.x;j--)
		{
			 z=(tan_val*(j-pnt[i].x)+pnt[i].y);
			if(a.at<uchar>(z,j))
				return 0;
		}
	}
	return 1;
}

int dist_thresh(int x,int y)
{
	Point u;
	u.x=x;
	u.y=y;
	for(i=0;i<pnt.size();i++)
		if((abs(u.x-pnt[i].x)+abs(u.y-pnt[i].y))<100 && (abs(u.x-pnt[i].x)+abs(u.y-pnt[i].y))>50)
			return 1;
	return 0;
}

int IsValid(int x,int y,int r,int c)
{
	if(x<0||y<0||x>=r||y>=c)
		return 0;
	if(a.at<uchar>(y,x)==255)
		return 0;
	if(!line_obs(x,y))
		return 0;
	return dist_thresh(x,y);
	return 1;
}

int main()
{
	Mat b;RNG rng;
	do
	{
		b=a.clone();
		pnt.clear();
		pnt.push_back(Point(0,0));
		int count=0;
		int x=0,y=0,x_new,y_new;
		do
		{
			count=0;
			do
			{
				x_new=(int)(rng.uniform((double)0, (double)1)*100)+x-10;
				y_new=(int)(rng.uniform((double)0, (double)1)*100)+y-10;
				count++;
				if(count>50)
				{
					cout<<"Program terminated due to unexpected results ";
					return 1;
				}
			}while(!IsValid(x_new,y_new,a.cols,a.rows));
			pnt.push_back(Point(x_new,y_new));
			x=x_new;y=y_new;
			b.at<uchar>(y_new,x_new)=255;
			cout<<x_new<<" "<<y_new<<endl;
		}while(!(x_new>780&&x_new<836&&y_new>580&&y_new<540));
	}while(pnt.size()>25);
	imshow("Path",b);
	waitKey(0);
}