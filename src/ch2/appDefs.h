/*
 *----------------------------------------------------------------------------------
 * appDefs.h - the defines of data types
 *
 * date:		4/21/2017
 * author:		shunguang.wu@jhuapl.edu
 *
 *
 *----------------------------------------------------------------------------------
 */

#ifndef __APP_DEFS_H__
#define __APP_DEFS_H__

#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <assert.h>

#include <cstddef>  
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <deque>
#include <locale>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <thread>

//open cv
//definitions of CV_VERSION_MAJOR, CV_VERSION_MINOR, CV_VERSION_REVISION
#include <opencv2/core/version.hpp>

#if CV_VERSION_MAJOR < 4
#include <opencv/cv.h>
#endif
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
//#include <opencv2/core/utility.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/imgcodecs.hpp>

#define float32		float
#define float64		double

//--------- my owen defs ----------------
#define TS_SINCE_EPOCH_MILLISEC (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())

#define APP_EPS_64F		(1e-15)
#define APP_EPS_32F 	(1e-7)

#define APP_REALMIN_32F (1e-38f)
#define APP_REALMAX_32F (1e+38f)
#define APP_REALMIN_64F (1e-308)
#define APP_REALMAX_64F (1e+308)

#define APP_MAX_UINT16		(0xFFFF)
#define APP_MAX_uint32_t	(0xFFFFFFFF)
#define APP_MAX_UINT64		(0xFFFFFFFFFFFFFFFF)
#define APP_SEQ_END				(0xFFFFFFFF)
#define APP_NAN_uint32_t	(0xFFFFFFFF)

#define APP_ROUND(x)	( (int) floor( x + 0.500 ) )
#define APP_NAN			( sqrt(-1.0) )
#define APP_ISNAN(x)	( x != x )

#define APP_MAX(a,b)	( (a) > (b) ? (a) : (b) )
#define APP_MIN(a,b)	( (a) > (b) ? (b) : (a) )
#define APP_INT_RAND_IN_RANGE(i1,i2) ( (i1) + rand() % ((i2) + 1 - (i1)) )

#define APP_SLEEP_MS( ms )   (std::this_thread::sleep_for(std::chrono::milliseconds(ms)))
#endif
