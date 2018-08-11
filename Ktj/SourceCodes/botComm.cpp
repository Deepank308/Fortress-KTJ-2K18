/*#include<stdio.h>
#include<unistd.h>
int main()
{
    char data='W';//[] = {'W','\n','\0'};  //Random data we want to send
    FILE *file;
    file = fopen("/dev/ttyACM1","w");  //Opening device file
    int i = 0;
    for(i = 0 ;data[i]!='\0' ; i++)
    {
        fprintf(file,"%c",data[i]); //Writing to the file
//      fprintf(file,"%c",','); //To separate digits
        sleep(1);
    }
	fprintf(file,"%c",data);
	
    fclose(file);
}*/
#include <iostream>
#include <stdio.h>
#include<fstream>
#include<string>
#include<unistd.h>
using namespace std;

int
main()
{
//  FILE *file;
	FILE *ofs;
  //Opening device file

  char getchar[]="W200A200W200A200W200Q \n";
int i=0;
ofs=fopen("/dev/ttyACM0","w");

  while (getchar[i]!='\0')
    {
//      file = fopen("Dee.txt", "w");
     // cout << ">>" << endl;
     // cin >> getchar;
     // ofs.write((char *)getchar,sizeof(getchar)); //Writing to the file
	fprintf(ofs,"%c",getchar[i]);
	i++;
	sleep(1);
}
      fclose(ofs);
    }

