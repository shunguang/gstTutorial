# gstTutorial

------------------------------------------

by Shunguang Wu (shunguang.wu@gmail.com)

03/12/20

------------------------------------------

## Folder structure

 dataset/                      -- the sample images, videos used in the tutorial code
 
 doc/                          -- document folder inluding ppts, and images used in the book
 
 gstOfficialTutorials/         -- the gst official tutorial examples
 
 ref/                          -- references used in this book
 
 src/                          --source code of each chapters
  

## Contents
ch1. Introduction 

   1.1 Gstreamer brief
   
	   1.1.1 Media signal processing modeling
	   
	   1.1.2 elements, pipeline, and playbins
	   
	   1.1.3 how does it work?
	   
   1.2 Set up playground
   
       1.2.1 for windows
       
	   1.2.2 for Linux	 
	   
   1.3 Command line tools
   
       1.3.1 gst-launch-1.0
       
	   1.3.2 gst-inspect-1.0
	   
	   1.3.3 gst-discovery-1.0
   
   1.4 miscellaneous preparations
   
       1.4.1 set environment variables
       
	   1.4.3 brief of libUtil
	   
	   1.4.4 run and test libUtil
	   
   1.5 Overiew and chapter dpendency graph of this book 
   
   1.6 Summary	   
	         
   
ch2 Build an "Hello World" application

    2.1 motivation
    
    2.2 Create elements,build a pipeline and run it
    
    2.2 Different ways to run a pipline
    
    2.3 Get and set parameters of an element
    
    2.4 Wrap into a cpp class
    
	2.5 Debug
	
	2.6 Summary
	
ch3 Differ ways to deliver a video from "point A" to "Point B"

    3.1 Catagories of deliver video from "point A" to "point B"
    
	3.2 breif of video
	
      	A. what is image codecs
	
      	B. what is video codecs (https://en.wikipedia.org/wiki/Video_compression_picture_types)
	
		C. some important parameters
		
	3.3 extract images from a video and pack images into a video 
	
		A. decode a video into individual images
		
		B. encode a vidoe from individual images  
		
	3.4 example of tranforming a video file format
	
		A. AVI to MP4
		
		B. MP4 to AVI
		
	3.5 streaming video
	
		A. TCP and UDP basics
		
		   A.1 TCP protocal
		   
           A.2 UDP protical (broad cast and echo modes)
	   
  	    B. rtspsrc and rtspsink
	    
	    C. udpsrc and udpsink
	    
	3.6 Summary

ch4 metadata
    4.1 gst metadata catogories 
	4.2 add author and subtitle on video
	4.3 pass metadata between filters
	4.4 add frame levele metadata when encoding video
	4.5 extract frame level metadata when decoding video

ch5 Audio	

ch6 mux and demux

chx Final project: 
    implementation of ehancement security by AI in industry security cameras 
    5.1 capture video from variety sources
	    5.1.1 from camera
		5.1.2 frome files
		5.1.3 from web

	5.2 frame level proceesing and manipulation
	    5.2.1 stabilization
		5.2.2 object detection
		5.2.3 downsampling	
		
    5.3 sink video to different destinations
	    5.3.1 to screen
        5.3.2 to HDMI port
        5.3.3 to file
        5.3.4 to ip port
		
	5.4 an app of rtspsrc and rtspsink
	    5.4.1 rtssrc
        5.4.2 rtspsink
	
  todo ...
  
 
