#ifndef _APP_STRUCTS_H_
#define _APP_STRUCTS_H_

#include "appDefs.h"
#include "appUtils.h"

namespace app {
	enum AVPixelFormat {
		AV_PIX_FMT_NONE = 0,
		AV_PIX_FMT_YUV420P
	};

	enum AppVideoEncType {
		APP_VID_H264 = 0,
		APP_VID_H265
	};

 
	struct AVRational {
		AVRational(int num_=30, int den_=1) : num(num_), den(den_) {}
		int num{ 30 };
		int den{ 1 };
	};


	struct ImgSize {
		ImgSize(int w_ = 0, int h_ = 0) : w(w_), h(h_) {}

		int w{ 0 };
		int h{ 0 };
	};

	struct AppVideoStreamInfo {
		AppVideoStreamInfo() : time_base({ 0,0 }), start_time(0), duration(0), bitrate_Kbps(0),
			frame_rate({ 0,0 }), w(0), h(0), pix_fmt(app::AV_PIX_FMT_NONE), frmSzInBytes(0), totFrms(0), isLive(true) {}
		AVRational time_base;
		uint64_t     start_time;
		uint64_t     duration;
		uint64_t     bitrate_Kbps;

		AVRational  	frame_rate;
		int 			    w;
		int 			    h;
		AVPixelFormat	pix_fmt;
		uint64_t		  frmSzInBytes;
		uint64_t		  totFrms;
		bool			    isLive;

		//todo: condering different pix_fmt
		uint32_t getVideoRawFrmSizeInBytes() const {
			return 3 * (w * h / 2);
		}
		float64 getVideoDurationSec() const {
			float64 dt = time_base.num * ((float64)duration / (float64)time_base.den);
			return dt;
		}
		//return frms/sec
		float64 getVideoFrmRate() const {
			return (float64)frame_rate.num / (float64)frame_rate.den;
		}

		uint64_t get_total_video_frms() {
			float64 fr = (float64)frame_rate.num / (float64)frame_rate.den;
			float64 t = (float64)duration / time_base.den * time_base.num;
			uint64_t x = ceil(t * fr);
			return x;
		}

		float64 getVideoTimeInSec(const uint64_t frmNum) {
			float64 t = frmNum * ((float64)frame_rate.den / (float64)frame_rate.num);
			return t;
		}

		uint64_t getVideoFrmNumFromTime(const float32 t) {
			uint64_t fn = (t * frame_rate.num) / frame_rate.den;
			return fn;
		}


		std::string toString(const std::string& msg) const {
			char buf[1024];
			snprintf(buf, 1024, "time_base[num=%d, den=%d],start_time=%llu, duration=%llu, frame_rate[num=%d,den=%d],w=%d,h=%d,pix_fmt=%d",
				time_base.num, time_base.den, start_time, duration, frame_rate.num, frame_rate.den, w, h, pix_fmt);
			return msg + std::string(buf);
		}
	};
}
#endif
