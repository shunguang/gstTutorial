/*
*------------------------------------------------------------------------
*GstRtspReceiver.cpp
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
#include "GstRtspReceiver.h"
#define CAP_TO_HOST 1

using namespace std;
using namespace app;
GstRtspReceiver::GstRtspReceiver(const GstRtspReceiverCfgPtr cfg)
	: m_cfg( cfg )
{
	init();
	cout << "GstRtspReceiver::GstRtspReceiver(): called" << endl;
}

GstRtspReceiver::~GstRtspReceiver()
{
	endRcvThread();
}

bool GstRtspReceiver::startRcvThread()
{
	dumpLog( "GstRtspReceiver::startRcvThread(): ... ");
	startMainLoopThread();
	return true;
}

bool GstRtspReceiver::endRcvThread()
{
	if (m_isOpened) {
		endMainLoopThread();
		m_isOpened = false;
	}
	return true;
}

bool GstRtspReceiver::init()
{
	//init this camera
	const int &w = m_cfg->imgSz.w;
	const int &h = m_cfg->imgSz.h;

	//init currrent camera capture params
	m_frmInterval_ms = floor( 1000.0/m_cfg->fps );

	m_hostYuvFrm.reset(new HostYuvFrm(w, h));

	m_hostYuvFrmQ.reset( new HostYuvFrmQ(w, h, m_cfg->yuvImgCircularQueSize) );

	//dumpLog( "Yuv: (w=%d,h=%d)", m_hostYuvFrm->w_, m_hostYuvFrm->h_);

	//check if recoder folder exist
	if( !m_cfg->recDir.empty() ){
		if (!appFolderExists(m_cfg->recDir)) {
			appCreateDir(m_cfg->recDir);
		}
	}

	return true;
}

//-----------------------------------------------------------------------
//wrt decoded frm image into <m_hostYuvFrmQ>
// or
//<m_localYuvFrmQ_d> if choose  gpu memory
//-----------------------------------------------------------------------
GstFlowReturn GstRtspReceiver::new_sample_cb(GstAppSink *appsink, gpointer user_data)
{
	//dumpLog("GstRtspReceiver::new_sample_cb(): AAA.");
	//gateway to access this->xyz
	GstRtspReceiver *pThis = reinterpret_cast<GstRtspReceiver*>(user_data);

	GstSample *sample = NULL;
	g_signal_emit_by_name(appsink, "pull-sample", &sample, NULL);
	if (sample){
		GstBuffer *buffer = NULL;
		GstCaps   *caps = NULL;
		GstMapInfo map = { 0 };

		caps = gst_sample_get_caps(sample);
		if (!caps){
			dumpLog("GstRtspReceiver::new_sample_cb(): could not get snapshot format.");
		}
		gst_caps_get_structure(caps, 0);
		buffer = gst_sample_get_buffer(sample);
		gst_buffer_map(buffer, &map, GST_MAP_READ);

		appAssert(pThis->m_hostYuvFrm->sz_ == map.size, "GstRtspReceiver::new_sample_cb(): size does not match!");

		//local dump and debug 
		HostYuvFrm yuv(pThis->m_cfg->imgSz.w, pThis->m_cfg->imgSz.h, map.data, map.size, pThis->m_frmNum);  //soft copy
		//yuv.dump(".", "yuv_h");
		//dumpLog( "GstRtspReceiver::new_sample_cb(): mapSzie=%d, fn=%d", map.size, pThis->m_frmNum);
		pThis->m_hostYuvFrmQ->wrtNext( &yuv );

		if (pThis->m_frmNum % 100 == 0) {
			dumpLog("GstRtspReceiver::new_sample_cb(): fn=%llu, map.size = %lu, bufSz=%lu", pThis->m_frmNum, map.size, gst_buffer_get_size(buffer));
		}
		gst_buffer_unmap(buffer, &map);
		gst_sample_unref(sample);

		pThis->m_frmNum++;
	}
	else{
		dumpLog("GstRtspReceiver::new_sample_cb(): could not make snapshot");
	}

	return GST_FLOW_OK;
}

int GstRtspReceiver :: decAndSaveLoop() 
{
	dumpLog("GstRtspReceiver :: decAndSaveLoop(): init!" );
	m_mainLoopThreadRunning = true;
	m_mainLoopExited = false;

	gst_init( NULL, NULL);
	m_main_loop = g_main_loop_new(NULL, FALSE);

	string launch_string = createLaunchStr();
	dumpLog("GstRtspReceiver :: decAndSaveLoop(): Using launch string: %s", launch_string.c_str());

	GError *error = nullptr;
	m_gst_pipeline = (GstPipeline*)gst_parse_launch(launch_string.c_str(), &error);
	if (m_gst_pipeline == nullptr) {
		appExit("GstRtspReceiver :: decAndSaveLoop(): Failed to parse launch: %s", error->message);
	}
	if (error) g_error_free(error);

	GstElement *appSink = gst_bin_get_by_name(GST_BIN(m_gst_pipeline), "appYuvSink");
	dumpLog("GstRtspReceiver :: decAndSaveLoop(): appSink=0X%08x", appSink );

#if 1
	GstAppSinkCallbacks callbacks = { GstRtspReceiver::eos_cb, NULL, GstRtspReceiver::new_sample_cb };
	gst_app_sink_set_callbacks(GST_APP_SINK(appSink), &callbacks, this, NULL);
#else
	//this approach also works!
	g_signal_connect(appSink, "new_sample", G_CALLBACK(GstRtspReceiver::new_sample_cb), (gpointer)this );
	g_signal_connect(appSink, "eos",        G_CALLBACK(GstRtspReceiver::eos_cb),        (gpointer)this );
#endif

	gst_element_set_state((GstElement*)m_gst_pipeline, GST_STATE_PLAYING);

	APP_SLEEP_MS(10);
	dumpLog("GstRtspReceiver :: decAndSaveLoop(): %s -- dec and save start!",  m_cfg->rtspUrl.c_str() );

	g_main_loop_run(m_main_loop);

	//---- reach the next line when quit from g_main_loop_run()
	dumpLog("GstRtspReceiver :: decAndSaveLoop(): main_loop quited - A!" );

	gst_element_set_state((GstElement*)m_gst_pipeline, GST_STATE_NULL);
	dumpLog("GstRtspReceiver :: decAndSaveLoop(): main_loop quited -- B!" );

	gst_object_unref(appSink);	
	dumpLog("GstRtspReceiver :: decAndSaveLoop(): main_loop quited -- C!" );

	gst_object_unref(GST_OBJECT(m_gst_pipeline));
	dumpLog("GstRtspReceiver :: decAndSaveLoop(): main_loop quited -- D!" );
	
	g_main_loop_unref(m_main_loop);
	dumpLog("GstRtspReceiver :: decAndSaveLoop(): main_loop quited -- E!" );
	m_mainLoopExited = true;
	dumpLog("GstRtspReceiver :: decAndSaveLoop(): %s -- dec and save exit!", m_cfg->rtspUrl.c_str() );

	return 0;
}

std::string GstRtspReceiver::createLaunchStr()
{
	//rtph264depay-- Extracts H264 video from RTP packets
	//splitmuxsink:
	//max-size-bytes: max. amount of data per file (in bytes, 0=disable).
	//max-size-time:  max. amount of time per file (in ns, 0=disable).
	//max-files:      maximum number of files to keep on disk. Once the maximum is reached,old files start to be deleted to make room for new ones.
	//send-keyframe-requests: request a keyframe every max-size-time ns to try splitting at that point. Needs max-size-bytes to be 0 in order to be effective.
	//cout << "GstRtspReceiver::createLaunchStr(): " << endl <<  m_camCfg.toString() << endl;

	//receiver side for other plantforms:
  //linux (tested on tx2)
	// $export DISPLAY=:0
	// $gst-launch-1.0 udpsrc port=5000 ! 'application/x-rtp,encoding-name=H264,payload=96' ! rtph264depay ! h264parse ! omxh264dec ! nveglglessink
	//
	//windows (test on win7)
	// gst-launch-1.0 udpsrc port=5000 ! application/x-rtp,encoding-name=H264,payload=96 ! rtph264depay ! h264parse ! queue ! avdec_h264 ! autovideosink sync=false -e
	//
	//andriod side
	// "udpsrc port=${viewModel.videoPort} ! application/x-rtp, encoding-name=H264, payload=96 ! rtpjitterbuffer drop-on-latency=false latency=1 ! " +
	//              "rtph264depay ! h264parse ! decodebin ! autovideosink sync=false")

	//todo: read these two from <m_camCfg>
	ostringstream oss;
	if (m_cfg->isUdp) {
		oss << "udpsrc port=" << m_cfg->udpPort << " ! application/x-rtp, encoding-name=H264, payload = 96 ! ";
	}
	else {
		oss << "-e -v rtspsrc debug=1 user-id=" << m_cfg->rtspUsrName << " user-pw=" << m_cfg->rtspUsrPassword << " location=" << m_cfg->rtspUrl << " ! ";
	}
	std::string srcStr = oss.str();

	oss.str("");
	oss.clear();
	oss << "avdec_h264 ! ";                   //for windows
//			<< "omxh264dec ! nvvidconv ! ";     //for jeston decoder
	std::string decStr = oss.str();

	oss.str("");
	oss.clear();
	if (0 != m_cfg->mp4LocationAndPrefix.compare("NULL")) {
		oss << srcStr << "rtph264depay ! h264parse ! "   //Parses H.264 streams  
			<< decStr
			<< "video/x-raw, format=I420, width=" << m_cfg->imgSz.w << ", height=" << m_cfg->imgSz.h << " ! "
			<< "appsink name=appYuvSink";
	}
	else{
		oss << srcStr
			<< "tee name=tsplit ! "     								//split into two parts
			<< "queue ! rtph264depay ! h264parse ! "   	//Parses H.264 streams  
			<< decStr									          				//hd decoder
			<< "video/x-raw, format=I420, width=" << m_cfg->imgSz.w << ", height=" << m_cfg->imgSz.h << " ! "
			<< "appsink name=appYuvSink tsplit. ! "
			<< "queue ! rtph264depay ! h264parse ! "
			<< "splitmuxsink name=myMp4Sink location=" << m_cfg->mp4LocationAndPrefix << "%04d.mp4 max-size-time=7200000000000 max-size-bytes=0 max-files=100 send-keyframe-requests=TRUE";
	}
	return oss.str();
}

//end-of-stream callback
void GstRtspReceiver::eos_cb(GstAppSink* appsink, gpointer user_data)
{
	//gateway to access this->xyz
	GstRtspReceiver* pThis = reinterpret_cast<GstRtspReceiver*>(user_data);

	dumpLog("GstRtspReceiver::eos_cb(): app sink receive eos!");
	//force main loop quit and starover
	pThis->endMainLoopThread();
	APP_SLEEP_MS(100);
	pThis->startMainLoopThread();

	dumpLog("GstRtspReceiver::eos_cb(): MainLoopThread restarted!");
}

void GstRtspReceiver::startMainLoopThread()
{
	if (!m_isOpened) {
		m_gstThread.reset(new std::thread(&GstRtspReceiver::decAndSaveLoop, this));
		m_isOpened = true;
	}
	else {
		dumpLog("GstRtspReceiver::startMainLoopThread(): sthing is wrong, already started!");
	}
}


void GstRtspReceiver::endMainLoopThread()
{
	dumpLog("GstRtspReceiver::endMainLoopThread(): A1- m_mainLoopThreadRunning=%d", m_mainLoopThreadRunning);
	if (!m_mainLoopThreadRunning) {
		dumpLog("GstRtspReceiver::endMainLoopThread(): warning mainloop is not runnig  m_mainLoopThreadRunning=%d", m_mainLoopThreadRunning);
		return;
	}

	dumpLog("GstRtspReceiver::endMainLoopThread(): A2- m_mainLoopEnd=%d", m_mainLoopExited);
	g_main_loop_quit(m_main_loop);
	uint32_t cnt=0;
	while (1) {
		APP_SLEEP_MS(10);
		if (m_mainLoopExited) {
			break;
		}
		if( cnt++>1000 ){
			dumpLog("GstRtspStreamer::endMainLoopThread(): abnormal quit! --  m_mainLoopEnd=%d", m_mainLoopExited);
			break;
		}
	}
	dumpLog("GstRtspStreamer::endMainLoopThread(): BB-m_mainLoopExited=%d", m_mainLoopExited  );
	m_gstThread->join();
	m_mainLoopThreadRunning = false;
	dumpLog("GstRtspStreamer::endMainLoopThread(): CC-m_mainLoopThreadRunning=%d, %s", m_mainLoopThreadRunning, m_cfg->toString().c_str() );
}
