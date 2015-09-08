#!/usr/bin/env python2.7
#-*- conding:utf-8 -*-
import cv
#import sys
#from opencv.cv import *
#from opencv.highgui import *
__name__='a'
__name__='__main__'
	      
if __name__ == '__main__':
    image = cv.LoadImage ("test1.jpg")
    cv.NamedWindow ("mywin")
    cv.ShowImage ("mywin", image)
    cv.WaitKey (1000)
    image1 = cv.LoadImage ("test.jpg")
    cv.ShowImage ("mywin", image1)
    cv.WaitKey (100)
