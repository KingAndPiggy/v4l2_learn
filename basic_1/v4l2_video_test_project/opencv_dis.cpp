#include <cv.h>  
#include <highgui.h>  
#include "./apue.h"
 
using namespace cv;  

int pfdout, pfdin;
 
int char_to_num(char ch);

//tell parent
void TELL_PARENT(pid_t pid)
{
	if (write(pfdout, "c", 1) != 1)
		err_sys("write error to pfd2[1]");
}	
//wait parent
void WAIT_PARENT(void)
{
	char c;

	if (read(pfdin, &c, 1) != 1)
		err_sys("read error for pfd1[0]");

	if (c != 'p')
		err_sys("WAIT_PARENT: incorrect data");
}

int main(int argc, char* argv[])  
{  
    Mat image;  
	char *str;
			 
    /*if (argc != 2 || !image.data)   
    {  
        printf("No image data\n");  
        return -1;  
    }*/  
	if (argc != 3)
		err_sys("usage: test <pathname  string>");
 
	str = argv[2];
	pfdout = char_to_num(str[0]);
	pfdin  = char_to_num(str[1]);
    namedWindow("Display Image", CV_WINDOW_AUTOSIZE);  
	while(1) {
		WAIT_PARENT();
		image = imread(argv[1], 1);  
		TELL_PARENT(getppid());
		imshow("Display Image", image);  
		waitKey(50);  
	}
    return 0;  
} 
int char_to_num(char ch)
{
	return (ch - '0');
}
