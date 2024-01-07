# gstTutorial
by Shunguang Wu (shunguang.wu@gmail.com)
03/12/20

## Folder structure
dataset/ -- the sample images, videos used in the tutorial code \
doc/ -- document folder inluding ppts, and images used in the book \
gstOfficialTutorials/ -- the gst official tutorial examples \
ref/ -- references used in this book \
src/ --source code of each chapters \

## Contents
### ch1. Introduction
#### 1.1 Gstreamer brief
1.1.1 Media signal processing modeling
1.1.2 elements, pipeline, and playbins
1.1.3 how does it work?
#### 1.2 Set up playground
1.2.1 for windows
1.2.2 for Linux	 
#### 1.3 Command line tools
1.3.1 gst-launch-1.0
1.3.2 gst-inspect-1.0
1.3.3 gst-discovery-1.0
#### 1.4 miscellaneous preparations
1.4.1 set environment variables
1.4.3 brief of libUtil
1.4.4 run and test libUtil
#### 1.5 Overiew and chapter dpendency graph of this book
#### 1.6 Summary

### ch2 Build an "Hello World" application
#### 2.1 motivation
#### 2.2 Create elements,build a pipeline and run it
#### 2.2 Different ways to run a pipline
#### 2.3 Get and set parameters of an element
### 2.4 Wrap into a cpp class
### 2.5 Debug
### 2.6 Summary

## ch3 Approaches to deliver videos from "point A" to "Point B"
### 3.1 Catagories of deliver video from "point A" to "point B"
### 3.2 breif of video
3.2.1 what is image codecs
3.2.2 what is video codecs (https://en.wikipedia.org/wiki/Video_compression_picture_types)
3.2.3 some important parameters
### 3.3 extract images from a video and pack images into a video 
3.3.1 decode a video into individual images
3.3.2 encode a vidoe from individual images  
### 3.4 example of tranforming a video file format
3.4.1 AVI to MP4
3.4.2 MP4 to AVI
### 3.5 streaming video
3.5.1. TCP and UDP basics
  A.1 TCP protocal
  A.2 UDP protical (broad cast and echo modes)
3.5.2 rtspsrc and rtspsink
3.5.3 udpsrc and udpsink
### 3.6 Summary

## ch4 metadata 
### 4.1 gst metadata catogories 
### 4.2 add author and subtitle on video 
### 4.3 pass metadata between filters 
### 4.4 add frame levele metadata when encoding video 
### 4.5 extract frame level metadata when decoding video

## ch5 Audio

## ch6 mux and demux

## ch7 Final project: implementation of ehancement security by AI in industry security cameras
### 7.1 capture video from variety sources 
7.1.1 from camera 
7.1.2 frome files 
7.1.3 from web
### 7.2 frame level proceesing and manipulation
7.2.1 stabilization
7.2.2 object detection
7.2.3 downsampling	
### 7.3 sink video to different destinations
7.3.1 to screen
7.3.2 to HDMI port
7.3.3 to file
7.3.4 to ip port
-----------end ------------
