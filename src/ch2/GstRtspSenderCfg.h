#ifndef __GST_RTSP_SENDER_CFG_H__
#define __GST_RTSP_SENDER_CFG_H__

#include "appDefs.h"
#include "appUtils.h"
#include "AppStructs.h"

namespace app {

	class GstRtspSenderCfg {
	public:
		GstRtspSenderCfg( const std::string &serverIp_ = "127.0.0.1", const uint16_t serverPort_ = 4000,
			const std::string &clientIp_ = "127.0.0.1", 	const uint16_t clientPort_ = 5000 )
			: serverIp(ipConvertStr2Num( serverIp_))
			, serverPort(serverPort_)
			, clientIp(ipConvertStr2Num(clientIp_))
			, clientPort(clientPort_)
		{
		}

		GstRtspSenderCfg(const GstRtspSenderCfg&x) = default;
		GstRtspSenderCfg& operator=(const GstRtspSenderCfg&x) = default;

		std::string toString() const {
			std::ostringstream oss;   // stream used for the conversion
			oss << "serverIp=" << ipConvertNum2Str(serverIp)
				<< "clientIp=" << ipConvertNum2Str(clientIp)
				<< "serverPort=" << serverPort
				<< "fps=" << fps
				<< "imgSz=(" << imgSz.w << "," << imgSz.h << ")"
				<< "bitrate_Kbps=" << bitrate_Kbps
				<< "yuvImgCircularQueSize=" << yuvImgCircularQueSize;
			return oss.str();
		}

	public:
		bool			isUdp{ true };
		uint32_t  serverIp{ 0 };
		uint16_t  serverPort{ 0 };  

		uint32_t  clientIp{ 0 };
		uint16_t  clientPort{ 0 };

		float			fps{ 30.0f };           //video frms/sec
		ImgSize		imgSz{1920,1080};
		int				bitrate_Kbps{80000};
		int       yuvImgCircularQueSize{ 10 };
	};
	typedef std::shared_ptr<GstRtspSenderCfg>		GstRtspSenderCfgPtr;

}
#endif
