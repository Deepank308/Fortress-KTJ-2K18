#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include"opencv2/core/core.hpp" 
#include<iostream>
#include<math.h>
#include<bits/stdc++.h>
#include<stdlib.h>
#include<time.h>
#include<fstream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ctime>
#include<string>
using namespace std;
using namespace cv;

struct resized_arena_cord      // Find the co-ordinates 
{
    int tl_cord_x;
    int tl_cord_y;
    int br_cord_x;
    int br_cord_y;
};

 Point u;
 vector<vector<int> > given_BGR;
 vector<int> rank_template;
 vector<int> rank_arena;
 vector<Rect> rect_arena;
 vector<int> arena_rank_index;
 Mat a_color=imread("IMG_3.jpg",1);
 Mat a_arena=imread("arena_obj1.png",1);
 vector<Point> pnt;
 vector<Point> pnt_set_final;
 Mat b,a;
 Mat path_final;
 float scale;
 int arduino=-1;
 int no_of_templates;

Point makeValid(int x,int y)
{
    if(x>=a_arena.cols)
        x=a_arena.cols-1;
    if(y>=a_arena.rows)
        y=a_arena.rows-1;
    if(x<0) x=0;
    if(y<0) y=0;
    return Point(x,y);
}

void assign_BGR()
{
    cout<<"Assigning the BGR values"<<endl;
    vector<int> colors;
    colors.push_back(255);
    colors.push_back(255);
    colors.push_back(0);
    colors.push_back(1);
    given_BGR.push_back(colors);
    colors.erase (colors.begin(),colors.begin()+4);
    colors.push_back(0);
    colors.push_back(0);
    colors.push_back(255);
    colors.push_back(2);
    given_BGR.push_back(colors);
    colors.erase (colors.begin(),colors.begin()+4);
    colors.push_back(0);
    colors.push_back(255);
    colors.push_back(0);
    colors.push_back(3);
    given_BGR.push_back(colors);
    colors.erase (colors.begin(),colors.begin()+4);
    colors.push_back(255);
    colors.push_back(0);
    colors.push_back(0);
    colors.push_back(4);
    given_BGR.push_back(colors);
    colors.erase (colors.begin(),colors.begin()+4);
    colors.push_back(255);
    colors.push_back(0);
    colors.push_back(255);
    colors.push_back(5);
    given_BGR.push_back(colors);
    colors.erase (colors.begin(),colors.begin()+4);
}

Mat arena_rotate(Mat arena,int theta)
{
    //cout<<"Rotating the arena"<<endl;
    Mat M=getRotationMatrix2D(Point(arena.cols/2,arena.rows/2),theta,0.5);
    Mat dst;
    warpAffine(arena,dst,M, Size(arena.cols,arena.rows));
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    threshold(dst,dst,200, 255, THRESH_BINARY);
    Mat element = getStructuringElement( MORPH_RECT,Size( 2*1 + 1, 2*1+1 ),Point( 1, 1 ) ); 
    int i=2;
    while(i--)
        erode(dst,dst,element);
    i=5;
    while(i--)
        dilate(dst,dst,element);
    findContours(dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    Mat drawing = Mat::zeros(dst.size(), CV_8UC3 );
    if(contours.size()==0 ) return dst;
    vector<Rect> rect(contours.size());
    vector<vector<Point> > shape;
    shape.resize(contours.size());
    u.x=0;
    u.y=0;
    i=0;
    drawContours(drawing,contours,i,Scalar(0,0,255),2,8,hierarchy,0,Point());
    rect[i]=boundingRect(Mat(contours[i]));
    rectangle(drawing,rect[i].tl()-u,rect[i].br()+u,Scalar(255,0,0), 0, 8, 0 );
    Rect roi = Rect((rect[i].tl()-u).x,(rect[i].tl()-u).y,(rect[i].br()+u).x-(rect[i].tl()-u).x,(rect[i].br()+u).y-(rect[i].tl()-u).y);
    return dst(roi);
    
}

int template_match(Mat arena,Mat templ)
{
    cout<<"Matching templates "<<endl;
    double nz_min=1;
   for(int theta=0;theta<360;theta+=1)
    {
        Mat rotate=arena_rotate(arena,theta);
      	 rotate=arena.clone();
        double fx=(double)templ.cols/rotate.cols;
        double fy=(double)templ.rows/rotate.rows;
        Mat arena_resize(templ.rows,templ.cols,CV_8UC1,Scalar(0));
        resize(rotate, arena_resize,Size(0,0),fx,fy,INTER_LINEAR);
        Mat diff;
        compare(arena_resize,templ,diff,CMP_NE);
        double nz = (double)cv::countNonZero(diff)/(arena_resize.rows*arena_resize.cols);
       // cout<<"nz ="<<nz<<endl;
        if(nz==(double)0.0) return 0;
        if(nz<nz_min) 
            nz_min=nz;
    }
    if(nz_min<.25)
    { 
        cout<<"percentage "<<nz_min<<endl;
        return 1;
    }
    return 0;
}

void arena_rank_assign(vector<Mat> subregions_arena,vector<Mat> subregions_template)
{
cout<<subregions_template.size();
cout<<subregions_arena.size();
   cout<<"assigning arena ranks"<<endl;

    for(int i=0;i<subregions_arena.size();i++)
    {   
        if(rank_arena[i]==0)
        {
            for(int j=0;j<subregions_template.size();j++)
            {
                if(!rank_template[j]) continue;
                if(template_match(subregions_arena[i],subregions_template[j]))
                {
                    //if(rank_template[j]>100)continue;
                    rank_arena[i]=rank_template[j];
                    cout<<rank_template[j]<<endl;
                    arena_rank_index[rank_template[j]-1]=i;
                    cout<<"i ="<<i<<"j ="<<j<<endl;
                    char s=(char)j;
                    //imshow(s+"arena",subregions_arena[i]);
                    namedWindow(s+"template",WINDOW_NORMAL);
                    imshow(s+"template",subregions_template[j]);
                    rank_template[j]=0;break;
                }   
            }
        }
    }
 
}

void color_detect(Rect rect_i,int i_temp)
 {
    cout<<"detecting colours "<<endl;
    int pos_j=((rect_i.tl()-u).x+(rect_i.br()+u).x)/2;
    for(int i=(rect_i.tl()-u).y;i<=(rect_i.br()+u).y;i++)
    {
        int B=a_color.at<Vec3b>(i,pos_j)[0];
        int G=a_color.at<Vec3b>(i,pos_j)[1];
        int R=a_color.at<Vec3b>(i,pos_j)[2];
        //if(B>240 && G>240 && R>240) continue;
        for(int k=0;k<5;k++)
            if(abs(B-given_BGR[k][0])<=5&& abs(G-given_BGR[k][1])<=5 && abs(R-given_BGR[k][2])<=5)
                {
                    rank_template.push_back(k+1);
                    no_of_templates++;
                    cout<<"k"<<k+1<<endl;
                    return;
                }
    }
 }

 vector<Mat> template_process()
 {
    int i=3;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat a=imread("IMG_3.jpg",0);
    threshold(a,a,240, 255, THRESH_BINARY_INV);
    findContours(a, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    Mat drawing = Mat::zeros(a.size(), CV_8UC3 );
    vector<Rect> rect(contours.size());
    u.x=0;
    u.y=0;
    vector<Mat> subregions;
        for( int i = 0; i<contours.size(); i++ )
        {
            if(contourArea(contours[i])>200&&contourArea(contours[i])<50000)
            {
            drawContours( drawing, contours, i,Scalar(0,0,255), 2, 8, hierarchy, 0, Point() );
            rect[i]=boundingRect(Mat(contours[i]));
            color_detect(rect[i],i);
            rectangle(drawing,rect[i].tl()-u,rect[i].br()+u,Scalar(255,0,0), 0, 8, 0 );
            Rect roi = Rect((rect[i].tl()-u).x,(rect[i].tl()-u).y,(rect[i].br()+u).x-(rect[i].tl()-u).x,(rect[i].br()+u).y-(rect[i].tl()-u).y);
            subregions.push_back( a(roi));
           // string s=(char)i;
            //cout<<" s "<<s<<endl;
            imshow("template",a(roi));
            waitKey(10000);
	//imwrite(s+"t.png",a(roi));        
}
	}
    return subregions;
 }

 Mat initial_processing(Mat a,int i,int j)
 {
    Mat element = getStructuringElement( MORPH_RECT,Size( 2*1 + 1, 2*1+1 ),Point( 1, 1 ) );
    while(i--)
        erode(a,a,element);
    while(j--)
        dilate(a,a,element);
    return a;
 }

vector<Mat> arena_process()
{
    cout<<"Processing the arena"<<endl;
    int i=5;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat am;
    cvtColor( a_arena,am,CV_BGR2GRAY);
    threshold(am,am,150, 255, THRESH_BINARY);
    imshow("A",am);
    Mat a=initial_processing(am,5,8);
    //imshow("cool",a);
    Mat am_copy=a.clone();
    //imshow("A",a);
    findContours(a, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    for(int i=1;i<contours.size();i++)
        if(contourArea(contours[i-1])>8*contourArea(contours[i]))
        {
            Rect bound=boundingRect(Mat(contours[i-1]));
            bound=Rect(bound.tl().x,bound.tl().y,bound.br().x-bound.tl().x,bound.br().y-bound.tl().y);
            am_copy=am_copy(bound);
            a_arena=a_arena(bound);
            a=a(bound);
            break;
        }
    findContours(a, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) ); 
    Mat drawing = Mat::zeros(a.size(), CV_8UC3 );
    vector<Rect> rect(contours.size());   
    vector<Mat> subregions;
    namedWindow("drawing",WINDOW_NORMAL);
        for( int i = 0; i<contours.size(); i++ )
        {
            drawContours( drawing, contours, i,Scalar(0,0,255), 2, 8, hierarchy, 0, Point() );
            if(hierarchy[i][2]!=-1)
                if(i!=contours.size()-1)
                    if(contourArea(contours[i+1])>200)
                        continue;
            rect[i]=boundingRect(Mat(contours[i]));
            rectangle(drawing,rect[i].tl(),rect[i].br(),Scalar(255,0,0), 0, 8, 0 );
            Rect roi = Rect((rect[i].tl()).x,(rect[i].tl()).y,(rect[i].br()).x-(rect[i].tl()).x,(rect[i].br()).y-(rect[i].tl()).y);
            subregions.push_back( a(roi));
            rank_arena.push_back(0);
            arena_rank_index.push_back(-1);
            rect_arena.push_back(rect[i]);
            rectangle(am_copy,rect[i].tl(),rect[i].br(),Scalar(255),CV_FILLED);	
            char s= (char)i;
           // imwrite("arena.png",a(roi));
            imshow("drawing",drawing);
            waitKey(10000);
//imshow("drawing arena"+to_string(i),am_copy(roi));
        }
cout<<subregions.size()<<endl;
    namedWindow("Cropped image",WINDOW_NORMAL);    
    imshow("Cropped image",am_copy);           
    return subregions;
}

void rect_arena_resize()
{
    Point u;
    cout<<"Resizing arena for obstacle removal"<<endl;
    //cout<<"Getting bot diagonal \n";
    int dia=20;
    //cin>>dia;
    //cout<<"dia"<<dia/scale<<endl;
    u.x=(dia*1.0)/scale;

    u.y=u.x;
    cout<<"bot dimensions "<<u.x<<" "<<u.y<<endl;
    cout<<"dimension "<<a_arena.rows<<" "<<a_arena.cols<<endl;
    for(int i=0;i<rect_arena.size();i++)
    {
        resized_arena_cord crd;
        crd.tl_cord_x=rect_arena[i].tl().x-u.x;
        crd.tl_cord_y=rect_arena[i].tl().y-u.y;
        crd.br_cord_x=rect_arena[i].br().x+u.x;
        crd.br_cord_y=rect_arena[i].br().y+u.y;
        Point crd_tl=makeValid(crd.tl_cord_x,crd.tl_cord_y); 
        Point crd_br=makeValid(crd.br_cord_x,crd.br_cord_y);
        rectangle(a_arena,crd_tl,crd_br,Scalar(255,255,255),CV_FILLED);
        rect_arena[i]=Rect(crd_tl.x,crd_tl.y,crd_br.x-crd_tl.x,crd_br.y-crd_tl.y);
    }
    Mat a_arena_gray;
    cvtColor(a_arena,a_arena_gray,CV_BGR2GRAY);
    threshold(a_arena_gray,a_arena_gray,150,255,THRESH_BINARY);
    imwrite("rounded.png",a_arena_gray);
}

void pathController(Point initial)
{
    cout<<"Generating paths"<<endl;
    pnt_set_final.push_back(initial);
    cout<<"ari"<<arena_rank_index.size()<<endl;
    for(int i=0;i<arena_rank_index.size();i++)
    {
        cout<<"ari v "<<arena_rank_index[i]<<" "<<i<<endl;

        if(arena_rank_index[i]==-1) continue;
        Point center=Point((rect_arena[arena_rank_index[i]].tl().x+rect_arena[arena_rank_index[i]].br().x)/2,(rect_arena[arena_rank_index[i]].tl().y+rect_arena[arena_rank_index[i]].br().y)/2);
        pnt_set_final.push_back(center);
    }
    pnt_set_final.push_back(initial);
}

int angle_func(Point current,Point target)
{
    float path_angle; 
    if (target.x-current.x==0)
          if (target.y>current.y)//finding initial path angle
            path_angle = 3.14159 / 2;
          else
            path_angle = -3.14159 / 2;
        else
        {
            path_angle = -atan((target.y - current.y)*1.0 / (target.x - current.x));//finding the path angle
        }

        if (target.x - current.x >= 0)
        {
            if (path_angle<0)
                path_angle += 2 * 3.14159;
        }
        else
            path_angle += 3.14159;//as atan returns -ve value

        path_angle *= 180 / 3.14159;
        return (int)path_angle;
}
/*
void commandGenerator(char *speed, char *angle)
{
    cout<<"generating commands"<<endl;
    int ang_bot;
    int ang_dst;
        FILE  *ofs1,*ofs2;
    ofs1=fopen("Command_Set.txt","w");
    ofs2=fopen("/dev/ttyACM0","w");
    cout<<"file opened "<<endl;
    //FILE *fin=fopen("Command_Set.txt","w"); 
    //cout<<"Enter distance travelled for 500ms"<<endl;
    int dist;
    dist=atoi(speed);
    cout<<"dist "<<dist<<endl;
    //cout<<"Enter no for right angle turn"<<endl;

    int delay_ang;
    delay_ang=atoi(angle);
    cout<<"angle  "<<delay_ang<<endl;
    cout<<pnt_set_final.size()<<endl;
    for(int i=0;i<pnt_set_final.size()-1;i++)
    {
        if(i==0) ang_bot=90;
        else  
            ang_bot=angle_func(pnt_set_final[i-1],pnt_set_final[i]);
        ang_dst=angle_func(pnt_set_final[i],pnt_set_final[i+1]);
        int distance=(int)(sqrt(pow(pnt_set_final[i].x-pnt_set_final[i+1].x,2)+pow(pnt_set_final[i].y-pnt_set_final[i+1].y,2))*scale);
        int distance_time=(int)(500*((float)distance/dist));
        int ang_time=(int)((ang_dst-ang_bot)*(float)delay_ang/90.0);
        if(ang_time>0) 
            {
                string s1="A"+to_string(abs(ang_time))+"W"+to_string(distance_time)+"L"+to_string((i+1)%6);
                int h=0;
                 while (h<s1.size())
                {
                    fprintf(ofs1,"%c",s1[h]);
                    fprintf(ofs2,"%c",s1[h]);
                    h++;
                    sleep(1);
                }   
            }
            //fprintf(fin,"%s",'A'+to_string(ang_time)+'W'+to_string(distance_time));//put Aang_timeWdistance_time
        else
            {
                int h=0;
              string s1="D"+to_string(abs(ang_time))+"W"+to_string(distance_time)+"L"+to_string((i+1)%6);
                 while (h<s1.size())
                {
                    fprintf(ofs1,"%c",s1[h]);
                    fprintf(ofs2,"%c",s1[h]);
                    h++;
                    sleep(1);
                }
            }
           // fprintf(fin,"%s",'D'+to_string(ang_time)+'W'+to_string(distance_time));//put Dang_timeWdistance_time
    }
    fclose(ofs1);
    fclose(ofs2);   // fclose(fin);
}
*/


int main(int argc,char *argv[])
{
    assign_BGR();
    //init_arduino(argv[3]);
     scale=sqrt(280*280+230*230)/sqrt(pow(a_arena.cols,2)+pow(a_arena.rows,2));
    cout<<"scale "<<scale<<endl;
    cout<<"build"<<endl;
    vector<Mat> subregions_arena=arena_process();
    vector<Mat> subregions_template=template_process();
    cout<<"rank template "<<endl;
    cout<<no_of_templates;
    for(int i=0;i<abs(rank_template.size()-no_of_templates);i++)
        rank_template.pop_back();
    for(int i=0;i<rank_template.size();i++)
        cout<<rank_template[i]<<" ";
    cout<<endl;
    arena_rank_assign(subregions_arena,subregions_template);
    rect_arena_resize();
    cout<<"Image Processed "<<endl;
    cout<<"sub regions length "<<subregions_arena.size()<<endl;

    fstream ifs;
    ifs.open("Start_Point.txt",ios::in);    
    Point upper,lower;
    ifs>>upper.x;ifs>>upper.y;
    ifs>>lower.x;ifs>>lower.y;
    cout<<"center cordinates "<<(upper.x+lower.x)/2<<" "<<(upper.y+lower.y)/2<<endl;
    pathController(Point((upper.x+lower.x)/2,(upper.y+lower.y)/2));
    Mat path_show_final;
    cvtColor(a_arena,path_show_final,CV_BGR2GRAY);
    threshold(path_show_final,path_show_final,150,255,THRESH_BINARY);
    
    for(int i=1;i<pnt_set_final.size();i++)
    {
        line(path_show_final,pnt_set_final[i-1],pnt_set_final[i],Scalar(100));
    }
    namedWindow("Path showed",WINDOW_NORMAL);
    resizeWindow("Path showed",600, 600);
    imshow("Path showed",path_show_final);
    //commandGenerator(argv[1],argv[2]);
    //send_command();
    waitKey(5000);
}



