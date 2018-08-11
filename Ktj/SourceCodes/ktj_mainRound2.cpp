#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include"opencv2/core/core.hpp" 
#include<iostream>
#include<math.h>
#include<bits/stdc++.h>
#include<stdlib.h>
#include<time.h>
//#include<process.h>
using namespace std;
using namespace cv;

struct resized_arena_cord
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
 Mat a_color=imread("Templatearena.png",1);
 Mat a_arena=imread("test_case_3.png",1);
 vector<Point> pnt;
 Mat b,a;
 Mat path_final;

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
    vector<int> colors;
    colors.push_back(36);
    colors.push_back(28);
    colors.push_back(237);
    colors.push_back(1);
    given_BGR.push_back(colors);
    colors.erase (colors.begin(),colors.begin()+4);
    colors.push_back(76);
    colors.push_back(177);
    colors.push_back(34);
    colors.push_back(2);
    given_BGR.push_back(colors);
    colors.erase (colors.begin(),colors.begin()+4);
    colors.push_back(204);
    colors.push_back(72);
    colors.push_back(63);
    colors.push_back(3);
    given_BGR.push_back(colors);
    colors.erase (colors.begin(),colors.begin()+4);
    colors.push_back(0);
    colors.push_back(242);
    colors.push_back(255);
    colors.push_back(4);
    given_BGR.push_back(colors);
    colors.erase (colors.begin(),colors.begin()+4);
}

Mat arena_rotate(Mat arena,int theta)
{
    //cout<<"in arena rotate"<<endl;
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
    //cout<<"Out arena rotate"<<endl;
    return dst(roi);
    
}

int template_match(Mat arena,Mat templ)
{
    double nz_min=1;//cout<<"In template match"<<endl;
    //imshow("arena",arena);
    //cout<<arena.rows<<" "<<arena.cols<<" "<<templ.rows<<" "<<templ.cols;
    for(int theta=0;theta<360;theta+=1)
    {
        Mat rotate=arena_rotate(arena,theta);
        double fx=(double)templ.cols/rotate.cols;
        double fy=(double)templ.rows/rotate.rows;
        Mat arena_resize(templ.rows,templ.cols,CV_8UC1,Scalar(0));
        resize(rotate, arena_resize,Size(0,0),fx,fy,INTER_LINEAR);
        Mat diff;
        compare(arena_resize,templ,diff,CMP_NE);
        double nz = (double)cv::countNonZero(diff)/(arena_resize.rows*arena_resize.cols);
        if(nz==(double)0.0) continue;
        if(nz<nz_min) 
            //cout<<nz<<endl;
            nz_min=nz;
            //return 1;
    }
    if(nz_min<.15) 
    {
        //cout<<nz_min<<endl;
        //cout<<"Out template match"<<endl;
        return 1;
    }
    //cout<<"Out template match"<<endl;
    return 0;
}

void arena_rank_assign(vector<Mat> subregions_arena,vector<Mat> subregions_template)
{
    cout<<"assigning arena ranks"<<endl;
    for(int i=0;i<subregions_arena.size();i++)
    {   
        if(rank_arena[i]==0)
        {
            for(int j=0;j<4;j++)
            {
                if(!rank_template[j]) continue;
                if(template_match(subregions_arena[i],subregions_template[j]))
                {
                    rank_arena[i]=rank_template[j];
                    arena_rank_index[rank_template[j]-1]=i;
                    cout<<"i ="<<i<<"j ="<<j<<endl;
                   // imshow(to_string(i),subregions_arena[i]);
                    rank_template[j]=0;break;
                }   
            }
        }
    }
}

void color_detect(Rect rect_i,int i_temp)
 {
    int pos_j=((rect_i.tl()-u).x+(rect_i.br()+u).x)/2;
    for(int i=(rect_i.tl()-u).y;i<=(rect_i.br()+u).y;i++)
    {
        int B=a_color.at<Vec3b>(i,pos_j)[0];
        int G=a_color.at<Vec3b>(i,pos_j)[1];
        int R=a_color.at<Vec3b>(i,pos_j)[2];
        if(B>240 && G>240 && R>240) continue;
        for(int k=0;k<4;k++)
            if(B==given_BGR[k][0] && G==given_BGR[k][1] && R==given_BGR[k][2])
                {
                    rank_template.push_back(k+1);
                    return;
                }
    }
 }

vector<Mat> template_process()
 {
    int i=3;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat a=imread("Templatearena.png",0);
    threshold(a,a,240, 255, THRESH_BINARY_INV);
    findContours(a, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    Mat drawing = Mat::zeros(a.size(), CV_8UC3 );
    vector<Rect> rect(contours.size());
    u.x=0;
    u.y=0;
    vector<Mat> subregions;
        for( int i = 0; i<4; i++ )
        {
            drawContours( drawing, contours, i,Scalar(0,0,255), 2, 8, hierarchy, 0, Point() );
            rect[i]=boundingRect(Mat(contours[i]));
            color_detect(rect[i],i);
            rectangle(drawing,rect[i].tl()-u,rect[i].br()+u,Scalar(255,0,0), 0, 8, 0 );
            Rect roi = Rect((rect[i].tl()-u).x,(rect[i].tl()-u).y,(rect[i].br()+u).x-(rect[i].tl()-u).x,(rect[i].br()+u).y-(rect[i].tl()-u).y);
            subregions.push_back( a(roi));
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
    int i=5;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat am;
    cvtColor( a_arena,am,CV_BGR2GRAY);
    threshold(am,am,150, 255, THRESH_BINARY);
    Mat a=initial_processing(am,5,8);
    //imshow("cool",a);
    Mat am_copy=a.clone();
    //imshow("A",a);
    findContours(a, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    for(int i=1;i<contours.size();i++)
        if(contourArea(contours[i-1])>8*contourArea(contours[i]))
        {
            Rect bound=boundingRect(Mat(contours[i-1]));
            cout<<bound.tl().x+5<<" "<<bound.tl().y+5<<" "<<bound.br().x-bound.tl().x-10<<" "<<bound.br().y-bound.tl().y-10<<endl;
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
            //if(contourArea(contours[i])>25000)continue;
            //cout<<" area "<<contourArea(contours[i]);
            
            rect[i]=boundingRect(Mat(contours[i]));
            rectangle(drawing,rect[i].tl(),rect[i].br(),Scalar(255,0,0), 0, 8, 0 );
            Rect roi = Rect((rect[i].tl()).x,(rect[i].tl()).y,(rect[i].br()).x-(rect[i].tl()).x,(rect[i].br()).y-(rect[i].tl()).y);
            subregions.push_back( a(roi));
            rank_arena.push_back(0);
            arena_rank_index.push_back(-1);
            rect_arena.push_back(rect[i]);
            rectangle(am_copy,rect[i].tl(),rect[i].br(),Scalar(255),CV_FILLED);
           // imwrite(to_string(i)+"an2.png",a(roi));
            imshow("drawing",drawing);
        }
    namedWindow("Cropped image",WINDOW_NORMAL);    
    imshow("Cropped image",am_copy);           
    return subregions;
}

void rect_arena_resize()
{
    cout<<"Enter bot diagonal \n";
    int dia;
    cin>>u.x;
    u.y=u.x;
    cout<<u.x<<" "<<u.y<<endl;
    cout<<"dimension "<<a_arena.rows<<" "<<a_arena.cols<<endl;
    for(int i=0;i<rect_arena.size();i++)
    {
        //if(arena_rank_index[i]==-1) continue;
        //cout<<rect_arena[i].tl().x<<" "<<rect_arena[i].tl().y<<" "<<i<<endl;
        resized_arena_cord crd;
        crd.tl_cord_x=rect_arena[i].tl().x-u.x;
        crd.tl_cord_y=rect_arena[i].tl().y-u.y;
        crd.br_cord_x=rect_arena[i].br().x+u.x;
        crd.br_cord_y=rect_arena[i].br().y+u.y;
        Point crd_tl=makeValid(crd.tl_cord_x,crd.tl_cord_y); //&& isValid(crd.br_cord_x,crd.br_cord_y)
        Point crd_br=makeValid(crd.br_cord_x,crd.br_cord_y);
        rectangle(a_arena,crd_tl,crd_br,Scalar(255,255,255),CV_FILLED);
        rect_arena[i]=Rect(crd_tl.x,crd_tl.y,crd_br.x-crd_tl.x,crd_br.y-crd_tl.y);
        //rect_arena[arena_rank_index[i]].br().x=(rect_arena[arena_rank_index[i]].br()+u).x;
        //cout<<rect_arena[arena_rank_index[i]].tl().x<<" "<<rect_arena[arena_rank_index[i]].tl().y<<" "<<i<<endl;
    }
    Mat a_arena_gray;
    cvtColor(a_arena,a_arena_gray,CV_BGR2GRAY);
    threshold(a_arena_gray,a_arena_gray,150,255,THRESH_BINARY);
    //imshow("new arena ",a_arena_gray);
    imwrite("rounded.png",a_arena_gray);
}

int line_obs(int x,int y)
{
    Point u;int i=0;
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
    u.y=y;int i=0;
    for(i=0;i<pnt.size();i++)
    {
        //cout<<" dist="<<abs(u.x-pnt[i].x)+abs(u.y-pnt[i].y)<<" ";
        if((abs(u.x-pnt[i].x)+abs(u.y-pnt[i].y))<100 )//&& (abs(u.x-pnt[i].x)+abs(u.y-pnt[i].y))>50)
            return 1;
    }
    return 0;
}

int IsValid(int x,int y,int r,int c)
{
    if(x<5||y<5||x>=r-5||y>=c-5)
        return 0;
    if(a.at<uchar>(y,x)==255)
        return 0;
    if(!line_obs(x,y))
        return 0;
    return dist_thresh(x,y);
    //return 1;
}

int dist(Point p1,Point p2,Rect final_des)
{
    /*if(p2.x<p1.x && p2.y<p1.y)
        return (abs(p2.x-final_des.tl().x)+abs(p2.y-final_des.tl().y));
    if(p2.x>p1.x && p2.y>p1.y)
        return (abs(p2.x-final_des.br().x)+abs(p2.y-final_des.br().y));
    if(p2.x<p1.x && p2.y>p1.y)
        return (abs(p2.x-final_des.tl().x)+abs(p2.y-final_des.br().y));
    if(p2.x>p1.x && p2.y<p1.y)
        return (abs(p2.x-final_des.br().x)+abs(p2.y-final_des.tl().y));*/
    return (int)(abs(p2.x-p1.x)+abs(p2.y-p1.y));
}

Point srand_path(Point src, Rect final_des)
{
    time_t t;//Mat show;
    cout<<"in method "<<src.x<<" "<<src.y<<endl;
    cvtColor(a_arena,a,CV_BGR2GRAY);
    threshold(a,a,150,255,THRESH_BINARY);
    b=a.clone();
    rectangle(a,final_des.tl(),final_des.br(),Scalar(0),CV_FILLED);
    //cvtColor(a_arena,show,CV_BGR2GRAY);
    //rectangle(show,final_des.tl(),final_des.br(),Scalar(0),CV_FILLED);
    unsigned int seedval=(unsigned)time(&t);
    int difference;
    //cout<<a.cols<<" "<<a.rows<<endl<<endl;
    //cout<<endl<<final_des.tl().x<<" "<<final_des.tl().y<<endl;
    //cout<<endl<<final_des.br().x<<" "<<final_des.br().y<<endl;
        pnt.clear();
        pnt.push_back(Point(src.x,src.y));
        int count=0;
        int x=src.x,y=src.y,x_new,y_new;
        Point dst=Point((final_des.tl().x+final_des.br().x)/2,(final_des.tl().y+final_des.br().y)/2);
        do
        {
            count=0;
            srand(seedval);
            //cout<<"Aadi "<<endl;
            do
            {
                x_new=(int)(rand()%70)+x-10;
                y_new=(int)(rand()%70)+y-10;
                count++;
                //cout<<count<<"   ";
                if(count>50000)
                {
                    cout<<"Program terminated due to unexpected results \n";
                    return src;
                }
                 difference=dist(dst,Point(x_new,y_new),final_des)-dist(dst,Point(x,y),final_des);
            }while(!(IsValid(x_new,y_new,a.cols,a.rows) && difference<0));
            /*if((abs(x_new-pnt[pnt.size()-1].x)+abs(y_new-pnt[pnt.size()-1].y))<15)
            {
                pnt.pop_back();
                cout<<"pop"<<endl;
            }*/
            cout<<"x_new and y_new "<<x_new<<" "<<y_new<<endl;
            pnt.push_back(Point(x_new,y_new));
            //line(show,Point(x_new,y_new),Point(x,y),Scalar(255));
            //imshow("Show",show);
            x=x_new;y=y_new;
            //waitKey(1000);
            //b.at<uchar>(y_new,x_new)=255;
            
        }while(!(x_new>final_des.tl().x && x_new<final_des.br().x && y_new>final_des.tl().y && y_new<final_des.br().y));

            //cout<<pnt.size()<<endl<<endl;
            //imshow("Path",b);
            namedWindow("Path",WINDOW_NORMAL);
            resizeWindow("Path",600, 600);
            cout<<"size in method "<<pnt.size()<<endl;
            //
            line(b,Point(src.x,src.y),Point(pnt[0].x,pnt[0].y),Scalar(255));
            for(int i=1;i<pnt.size();i++)
            { 
                line(b,Point(pnt[i-1].x,pnt[i-1].y),Point(pnt[i].x,pnt[i].y),Scalar(255));
                //b.at<uchar>(pnt[i].y,pnt[i].x)=255;
                //cout<<pnt[i].x<<" "<<pnt[i].y<<endl;
                imshow("Path",b);
                waitKey(10);
            }
    //waitKey(0);
            cout<<"leaving method"<<pnt[pnt.size()-2]<<endl;
            return pnt[pnt.size()-2];
}

void pathController()
{
    Point src;
    Point src_accept(20,20);
    //cout<<"Image Processed "<<endl;
    char ans;
    cvtColor(a_arena,path_final,CV_BGR2GRAY);
    threshold(path_final,path_final,150,255,THRESH_BINARY);
    for(int i=0;i<arena_rank_index.size();i++)
    {
        if(arena_rank_index[i]==-1) continue;
        do
        {
            if(i==0)
                src = srand_path(Point(20,20),rect_arena[arena_rank_index[i]]);
            else
                src = srand_path(src_accept,rect_arena[arena_rank_index[i]]);
            if(!(src.x==src_accept.x && src_accept.y==src.y)) 
            {    
            cout<<"Is the path fucking good(Y/y)? for "<<arena_rank_index[i]<<endl;
            //imshow("Path",b);
            //waitKey(0);
            cin>>ans;
            cout<<"Your answer is "<<ans<<endl;
            destroyWindow("Path");
            if(ans=='Y' || ans=='y')
            {
                cout<<pnt.size()<<endl;
                src_accept.x=src.x;
                src_accept.y=src.y;
                for(int j=1;j<pnt.size();j++)
                    line(path_final,Point(pnt[j-1].x,pnt[j-1].y),Point(pnt[j].x,pnt[j].y),Scalar(255));
                //imshow("Till time path ",path_final);
            }
            cout<<src.x<<" "<<src.y<<endl;
            cout<<src_accept.x<<" "<<src_accept.y<<endl;
            }
            else ans='N';
        }while(!(ans=='Y' || ans=='y'));
    }
    namedWindow("Till time path ",WINDOW_NORMAL);
    resizeWindow("Till time path ",600, 600);
    imshow("Till time path ",path_final);
    waitKey(5000);
}

int main()
{
    assign_BGR();
    cout<<"build"<<endl;
    vector<Mat> subregions_arena=arena_process();
    vector<Mat> subregions_template=template_process();
    arena_rank_assign(subregions_arena,subregions_template);
    rect_arena_resize();
    cout<<"Image Processed "<<endl;
        cout<<"sub regions length "<<subregions_arena.size()<<endl;    

    //for(int i=0;i<arena_rank_index.size();i++)
      //  cout<<arena_rank_index[i]<<endl;
    pathController();
    /*Point src;
    char ans;
    cvtColor(a_arena,path_final,CV_BGR2GRAY);
    for(int i=0;i<arena_rank_index.size()-1;i++)
    {
        if(arena_rank_index[i]==-1) continue;
        do
        {
            if(i==0)
                src = srand_path(Point(0,0),rect_arena[arena_rank_index[i]]);
            else
                src = srand_path(src,rect_arena[arena_rank_index[i]]);
            cout<<"Is the path fucking good(Y/y)? "<<endl;
            imshow("Path",b);
            waitKey(0);
            cin>>ans;
            if(ans=='Y' || ans=='y')
            {
                for(int i=1;i<pnt.size();i++)
                    line(path_final,Point(pnt[i-1].x,pnt[i-1].y),Point(pnt[i].x,pnt[i].y),Scalar(255));
                imshow("Till time path ",path_final);
            }
        }while(!(ans=='Y' || ans=='y'));
    }
    imshow("Till time path ",path_final);*/
    waitKey(0);
}