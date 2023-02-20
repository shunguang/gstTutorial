#ifndef __GST_UDP_SENDER_CFG_H__
#define __GST_UDP_SENDER_CFG_H__

#include "appDefs.h"
#include "appUtils.h"
#include "AppStructs.h"

namespace app {

	class GstUdpSenderCfg {
	public:
		GstUdpSenderCfg( const std::string &serverIp_ = "127.0.0.1",
			const std::string &clientIp_ = "127.0.0.1",
			const uint16_t rtspPort_ = 5000 )
			: serverIp(ipConvertStr2Num( serverIp_))
			, clientIp(ipConvertStr2Num( clientIp_))
			, rtspPort(rtspPort_)
		{
		}

		GstUdpSenderCfg(const GstUdpSenderCfg&x) = default;
		GstUdpSenderCfg& operator=(const GstUdpSenderCfg&x) = default;

		std::string toString() const {
			std::ostringstream oss;   // stream used for the conversion
			oss << "serverIp=" << ipConvertNum2Str(serverIp)
				<< "clientIp=" << ipConvertNum2Str(clientIp)
				<< "rtspPort=" << rtspPort
				<< "fps=" << (int)fps
				<< "imgSz=(" << imgSz.w << "," << imgSz.h << ")"
				<< "bitrate_Kbps=" << bitrate_Kbps
				<< "yuvImgCircularQueSize=" << yuvImgCircularQueSize;
			return oss.str();
		}

	public:
		uint32_t  serverIp{ 0 };
		uint16_t  rtspPort{ 0 };	//used for RTSP port, server sends it to client via msg or preconfig 
		          						    //to generate url: "rtsp://192.168.1.100:8554/live.sdp"; where "192.168.1.100" is server ip
		uint32_t  clientIp{ 0 };
		uint8_t   fps{ 30 };           //video frms/sec
		ImgSize		imgSz{1920,1080};
		int				bitrate_Kbps{4000};
		int       yuvImgCircularQueSize{ 10 };
		AppVideoEncType vidEncType{ app::APP_VID_H264 };
	};
	typedef std::shared_ptr<GstUdpSenderCfg>		GstUdpSenderCfgPtr;

}
#endif
