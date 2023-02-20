#ifndef __GST_NV_JPEG_ENC_H__
#define __GST_NV_JPEG_ENC_H__

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#include "libUtil/util.h"
#include "libDc/HostYuvFrm.h"
#include "libDc/CudaYuvFrm.h"
#include "libDc/CudaYuvFrmQ.h"
#include "libDc/CudaRgbFrm.h"
#include "RunUtil.h"
#include <atomic>

namespace ngv {
	class GstNvJpegEnc {
	public:
		GstNvJpegEnc( int w, int h, int yuvFrmQSz=10);
		GstNvJpegEnc(const GstNvJpegEnc &s) = delete;
		GstNvJpegEnc(GstNvJpegEnc &&s) = delete;
		GstNvJpegEnc& operator=(const GstNvJpegEnc &s) = delete;
		GstNvJpegEnc& operator=( GstNvJpegEnc &&s) = delete;
		~GstNvJpegEnc();
		
		void setFrmFreqToLog(int n) { 
			boost::mutex::scoped_lock lock(m_mtxGstLocal);
			m_nFrmFreqToLog = n;
		}

		void startGstEncThread();
		bool endGstEncThread();

		//typically addNewFrm() is called by another thread 
		void addNewFrm(const CudaRgbFrm *frm) {
			m_cudaYuvQ4Enc->wrtNext( frm );
		}


	protected:
		void init( int w, int h, int yuvFrmQSz );
		void	main_loop();
		static void	cb_need_data(GstElement * appsrc, guint unused, gpointer user_data);

	public:
		std::shared_ptr<boost::thread>	m_gstThread;
		GMainLoop			*m_loop;
		std::atomic<bool> 	m_isOpened;
		std::atomic<bool>   m_mainLoopEnd;
		int					m_nFrmFreqToLog;
		std::atomic<bool>	m_requestToClose;       //request to close

		CudaYuvFrmQPtr		m_cudaYuvQ4Enc;    //YUV frm Que in dev, multi-threads access
		HostYuvFrmPtr		m_hostYuvFrm;	   //YUV frm read(hd copy) from <m_cudaYuvQ4Enc>, it will be encoded
	protected:
		boost::mutex		m_mtxGstLocal;
	};
	typedef std::shared_ptr<GstNvJpegEnc>		GstNvJpegEncPtr;
}
#endif
