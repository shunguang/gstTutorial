#ifndef __GST_ENC_VIDEO_FROM_IMAGES_H__
#define __GST_ENC_VIDEO_FROM_IMAGES_H__
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

	class GstEncVideoFromImages {
	public:
		GstEncVideoFromImages(const int w, const int h, const int fpsNum, const int fpsDen=1, const std::string &mp4FilePath="./ngv_h264.mp4");
		GstEncVideoFromImages(const GstEncVideoFromImages &s) = delete;
		GstEncVideoFromImages(GstEncVideoFromImages &&s) = delete;
		GstEncVideoFromImages& operator=(const GstEncVideoFromImages &s) = delete;
		GstEncVideoFromImages& operator=(GstEncVideoFromImages &&s) = delete;
		~GstEncVideoFromImages();
	
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

	typedef std::shared_ptr<GstEncVideoFromImages>		GstEncVideoFromImagesPtr;

}  //namespace ngv
#endif
