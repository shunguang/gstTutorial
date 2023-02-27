#ifndef __GST_RTSP_RECEIVER_CFG_H__
#define __GST_RTSP_RECEIVER_CFG_H__

#include "appDefs.h"
#include "appUtils.h"
#include "AppStructs.h"

namespace app {

	class GstRtspReceiverCfg {
	public:
		GstRtspReceiverCfg()
		{
		}

		GstRtspReceiverCfg(const GstRtspReceiverCfg&x) = default;
		GstRtspReceiverCfg& operator=(const GstRtspReceiverCfg&x) = default;

		std::string toString() const {
			std::ostringstream oss;   // stream used for the conversion
			oss << "isUdp=" << isUdp
				<< "rtspUsrName=" << rtspUsrName
				<< "rtspUsrPassword = " << rtspUsrPassword
				<< "rtspUrl=" << rtspUrl

				<< "udpPort=" << udpPort
				<< "recDir=" << recDir
				<< "mp4LocationAndPrefix=" << mp4LocationAndPrefix
				<< "fps=" << fps
				<< "imgSz=(" << imgSz.w << "," << imgSz.h << ")"
				<< "yuvImgCircularQueSize=" << yuvImgCircularQueSize;

			return oss.str();
		}

	public:
		bool isUdp{ true };
		std::string rtspUsrName{ "admin" };
		std::string rtspUsrPassword{ "admin" };
		std::string rtspUrl{ "rtsp://127.0.0.1/5000" };

		uint16_t udpPort{ 5000 };
		std::string recDir{ "./" };
		std::string mp4LocationAndPrefix{ "./myRecFolder/tmp" };

		float     fps{ 30.0f };           //video frms/sec
		ImgSize		imgSz{1920,1080};
		int       yuvImgCircularQueSize{ 10 };
	};
	typedef std::shared_ptr<GstRtspReceiverCfg>		GstRtspReceiverCfgPtr;
}
#endif
