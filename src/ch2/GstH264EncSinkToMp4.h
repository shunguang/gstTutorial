#ifndef __GST_H264_ENC_SINK_TO_MP4_H__
#define __GST_H264_ENC_SINK_TO_MP4_H__
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#include "appDefs.h"
#include "appUtils.h"
#include "AppStructs.h"
#include "HostYuvFrm.h"

namespace app {

	struct GstApp {
		GstPipeline *pipeline;
		GstAppSrc  *src;
		GstElement *filter1;
		GstElement *encoder;
		GstElement *filter2;
		GstElement *parser;
		GstElement *qtmux;
		GstElement *sink;

		GstClockTime timestamp;
		guint sourceid;
	};

	static GstApp m_gstApp;

	class GstH264EncSinkToMp4 {
	public:
		GstH264EncSinkToMp4(const int w, const int h, const int fpsNum, const int fpsDen=1, const std::string &mp4FilePath="./ngv_h264.mp4");
		GstH264EncSinkToMp4(const GstH264EncSinkToMp4 &s) = delete;
		GstH264EncSinkToMp4(GstH264EncSinkToMp4 &&s) = delete;
		GstH264EncSinkToMp4& operator=(const GstH264EncSinkToMp4 &s) = delete;
		GstH264EncSinkToMp4& operator=(GstH264EncSinkToMp4 &&s) = delete;
		~GstH264EncSinkToMp4();
	
		void init();
		int addFrm( const HostYuvFrm *frm );
		int endEnc();
	protected:

	protected:
		const int					m_imgW;
		const int					m_imgH;
		const AVRational	m_fps;
		const uint32_t		m_yuvImgSzInBytes;
		const guint64			m_timeDurationNanoSec;

		std::string		m_mp4FilePath;

		bool				m_isInit;
	};

	typedef std::shared_ptr<GstH264EncSinkToMp4>		GstH264EncSinkToMp4Ptr;

}  //namespace ngv
#endif
