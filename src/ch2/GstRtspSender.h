//----------------------------------------------------------
//todo: seperatte it into 
 // GstStreamer {}
 // GstStreamerUdp  : public GstStreamer{}
 // GstStreamerRtsp : public GstStreamer{}
//----------------------------------------------------------
#ifndef __GST_RTSP_SENDER_H__
#define __GST_RTSP_SENDER_H__

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#include "appDefs.h"
#include "appUtils.h"
#include "AppStructs.h"
#include "HostYuvFrm.h"
#include "HostYuvFrmQ.h"
#include "GstRtspSenderCfg.h"

namespace app {
	class GstRtspSender {
	public:
		GstRtspSender( const GstRtspSenderCfgPtr cfg );
		GstRtspSender(const GstRtspSender &s) = delete;
		GstRtspSender(GstRtspSender &&s) = delete;
		GstRtspSender& operator=(const GstRtspSender &s) = delete;
		GstRtspSender& operator=( GstRtspSender &&s) = delete;
		~GstRtspSender();

		void startStreamingThread();
		bool endStreamingThread();

		//typically addNewFrm() is called by another thread 
		void addNewFrm(const HostYuvFrm *frm);

		std::string toString( const std::string &msg="" ) const {
			std::string s = msg + "streamInfo: ip=" + ipConvertNum2Str(m_cfg->clientIp) + "\n" + m_videoInfo.toString("m_videoInfo=");
			return s;
		}

	protected:
		void  init();
		void	main_loop();
		static void	cb_need_data(GstElement * appsrc, guint unused, gpointer user_data);

	public:
		std::atomic<bool> 	m_isOpened{ false };
		std::atomic<bool>   m_mainLoopEnd{ false };

	protected:
		GstRtspSenderCfgPtr	m_cfg{ nullptr };        //cfg 
		AppVideoStreamInfo	m_videoInfo{};  //video info used to define RTSP stream

		HostYuvFrmQPtr	m_hostYuvQ{ nullptr };    //YUV frm Que in host, multi-threads access
		HostYuvFrmPtr		m_hostYuvFrm{ nullptr };	//YUV frm read(hd copy) from <m_hostYuvQ>, it will be RTSP -UDP to client, 

		//-------------------------
		std::shared_ptr<std::thread>	m_gstThread{ nullptr };
		std::string			m_appsrcName{ "" };
		GMainLoop*			m_loop{ nullptr };

		GstClockTime   m_timestamp{ 0 };
		guint64				 m_timeDurationNanoSec{ 0 };
		int					   m_eosRequestCnt{ 0 };

		std::atomic<int32_t> m_clientCount{ 0 };
		std::atomic<bool>	   m_requestToClose{ false };       //request to close

		const int				m_nFrmFreqToLog{ 100 };
	};
	typedef std::shared_ptr<GstRtspSender>		GstRtspSenderPtr;
}
#endif // CLIENTGUI_H
