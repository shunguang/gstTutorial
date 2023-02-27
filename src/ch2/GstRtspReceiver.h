/*
*------------------------------------------------------------------------
*GstRtspReceiver.h
*
* This code was developed by Shunguang Wu in his spare time. No government
* or any client funds were used.
*
*
* THE SOFTWARE IS PROVIDED AS-IS AND WITHOUT WARRANTY OF ANY KIND,
* EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
* WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
*
* IN NO EVENT SHALL THE AUTHOR OR DISTRIBUTOR BE LIABLE FOR
* ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
* OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
* WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
* LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
* OF THIS SOFTWARE.
*
* Permission to use, copy, modify, distribute, and sell this software and
* its documentation for any purpose is prohibited unless it is granted under
* the author's written notice.
*
* Copyright(c) 2020 by Shunguang Wu, All Right Reserved
*-------------------------------------------------------------------------
*/

//--------------------------------------------------------------------------------------
// Objets:
//  1. decode H264 stream via hardward devices
//  2. save the roiginal stream video as mp4 files.
// 
// To keep the unified archeture unchange, for gst-rtsp mainloop kind of caprure, we do:
// 1. frms received by new_frm_cb() are saved in a local queue (redundency, unneccassry)
// 2. procNextTask() moves the avaiable frame into 	m_camDc->m_frmInfoQ;
// by swu on 2/22/2020
//--------------------------------------------------------------------------------------
#ifndef __CAP_SAVE_RTSP_H264_H__
#define __CAP_SAVE_RTSP_H264_H__

#include <cstdlib>
#include <gst/gst.h>
#include <gst/gstinfo.h>
#include <gst/app/gstappsink.h>
//#include <glib-unix.h>

#include "appUtils.h"
#include "HostYuvFrm.h"
#include "HostYuvFrmQ.h"
#include "GstRtspReceiverCfg.h"
namespace app {
	class GstRtspReceiver
	{
	public:
		GstRtspReceiver(const GstRtspReceiverCfgPtr cfg);
		~GstRtspReceiver();

		bool startRcvThread();
		bool endRcvThread();

		bool getYuvFrm(HostYuvFrm* dst) {
			return m_hostYuvFrmQ->readNext(dst);
		}

	protected:
		bool init();
		void startMainLoopThread();
		void endMainLoopThread();

	protected:
		static void eos_cb(GstAppSink * appsink, gpointer user_data);
		static GstFlowReturn new_sample_cb(GstAppSink *appsink, gpointer user_data);

		int	decAndSaveLoop();
		std::string createLaunchStr();

	protected:
		GstRtspReceiverCfgPtr  m_cfg{ nullptr };
		std::shared_ptr<std::thread>	m_gstThread{ nullptr };
		GMainLoop*			m_main_loop{ nullptr };
		GstPipeline*		m_gst_pipeline{ nullptr };
		bool						m_mainLoopExited{false};
		bool						m_mainLoopThreadRunning{false};

		//local 
		HostYuvFrmQPtr	m_hostYuvFrmQ{ nullptr };    //YUV frm Que in host, multi-threads access
		HostYuvFrmPtr		m_hostYuvFrm{ nullptr };			  //YUV frm read(hd copy) from <m_hostYuvQ>, it will be RTSP -UDP to client, 

		int			m_frmInterval_ms{ 33 };
		uint64	m_frmNum{ 0 };
		bool		m_isOpened{ false };
	};
	typedef std::shared_ptr<GstRtspReceiver> GstRtspReceiverPtr;
}
#endif 
