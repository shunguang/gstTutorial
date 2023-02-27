#include "GstRtspSender.h"
using namespace std;
using namespace app;

GstRtspSender::GstRtspSender(const GstRtspSenderCfgPtr cfg)
	: m_cfg(cfg)
{
	init();
}

GstRtspSender :: ~GstRtspSender()
{
	dumpLog("GstRtspSender :: ~GstRtspSender()--AAA--%s!", m_appsrcName.c_str());
	endStreamingThread();
	dumpLog("GstRtspSender :: ~GstRtspSender()--BBB--%s!", m_appsrcName.c_str());
}


void GstRtspSender::startStreamingThread()
{
	dumpLog(m_videoInfo.toString("GstRtspSender::startStreamingThread(): m_videoInfo="));
	m_gstThread.reset(new std::thread(&GstRtspSender::main_loop, this));
	m_isOpened = true;
}

bool GstRtspSender::endStreamingThread()		//return;

{
	if (!m_isOpened.load()) {
		return true;
	}

	dumpLog("GstRtspSender::endStreamingThread(): AA- m_mainLoopEnd=%d for %s", m_mainLoopEnd.load(), m_appsrcName.c_str());

	m_requestToClose = true;
	APP_SLEEP_MS(50);            //todo: need to investigate, i am not sure: 
															//pt is that give sometime let cb_need_data() not be called when <this> pointer is deleted.

	g_main_loop_quit(m_loop);

	dumpLog("GstRtspSender::endStreamThread(): BB- m_mainLoopEnd=%d for %s", m_mainLoopEnd.load(), m_appsrcName.c_str());

	int cnt = 0;
	while (1) {
		cnt++;
		APP_SLEEP_MS(10);
		if (m_mainLoopEnd.load()) {
			break;
		}
		if (cnt > 1000) {
			dumpLog("GstRtspSender::endStreamThread(): cannot close current stream: %s", m_appsrcName.c_str());
			return false;
		}
	}
	m_gstThread->join();
	m_isOpened = false;
	m_clientCount = 0;          //force as zero
	dumpLog("GstRtspSender::endStreamThread(): CC-m_mainLoopEnd=%d, serverPort=%d for %s", m_mainLoopEnd.load(), m_cfg->rtspPort, m_appsrcName.c_str());
	return true;
}


void GstRtspSender::main_loop()
{
	dumpLog(m_videoInfo.toString("GstRtspSender::main_loop()--AA: m_videoInfo="));

	GError* err = 0;
	m_mainLoopEnd = false;

	gst_init(NULL, NULL);
	m_loop = g_main_loop_new(NULL, FALSE);

	//min-latency=33333333 (nano sec) (1/30 sec)
	//GST_FORMAT_TIME(3);
	const std::string udpHostIp = ipConvertNum2Str(m_cfg->serverIp);  //"127.0.0.1";
	const int br_Kbps = m_videoInfo.bitrate_Kbps;
#if 1
	const std::string  encEle = " x264enc tune=zerolatency ";								//for windows 
#else
	const std::string  encEle = " nvvidconv | omxh264enc ";  //for jetson
#endif
	std::string  pipelineStr;

	//gst-launch-1.0 -v videotestsrc ! videoconvert ! x264enc tune=zerolatency bitrate=4000 ! rtph264pay ! udpsink host=localhost port=5000
	//gst-launch-1.0 -v videotestsrc ! video/x-raw,width=1920,height=1080,formate=(string)I420 ! videoconvert ! x264enc tune=zerolatency bitrate=4000 ! rtph264pay ! udpsink host=127.0.0.1 port=5000
	pipelineStr = "appsrc name=" + m_appsrcName + " do-timestamp=true format=3 " +
	  " ! video/x-raw,format=(string)I420,width=(int)" + std::to_string(m_videoInfo.w) + ",height=(int)" + std::to_string(m_videoInfo.h) +
		",framerate=(fraction)" + std::to_string(m_videoInfo.frame_rate.num) + "/" + std::to_string(m_videoInfo.frame_rate.den) +
		" ! videoconvert"
		" ! " + encEle + " bitrate=" + std::to_string(br_Kbps) +
		" ! h264parse" +
		" ! rtph264pay pt=96" +
		" ! udpsink host=" + udpHostIp + " port=5000";

	dumpLog("pipelineStr=%s", pipelineStr.c_str() );

	GstElement* pipeline = gst_parse_launch((const gchar*)pipelineStr.c_str(), &err);
	if (nullptr == pipeline) {
		appAssert(0, "cannot build pipeline!");
	}

	GstElement* appsrc = gst_bin_get_by_name_recurse_up(GST_BIN(pipeline), (const gchar*)m_appsrcName.c_str());
	if (nullptr == appsrc) {
		appAssert(0, "cannot get appsrc!");
	}

	dumpLog(m_videoInfo.toString("GstRtspSender::main_loop()--BB: m_videoInfo="));

	this->m_clientCount = 1;
	this->m_timestamp = 0;
	g_signal_connect(appsrc, "need-data", G_CALLBACK(cb_need_data), (gpointer)this);

	/* play */
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* start serving */
	dumpLog("GstRtspSender::main_loop(): pipeline=" + pipelineStr);
	dumpLog("GstRtspSender::main_loop(): RTS-UDP -- clientIp:%s, rtspPort=%d", ipConvertNum2Str(m_cfg->clientIp).c_str(), m_cfg->rtspPort);
	g_main_loop_run(m_loop);

	dumpLog("GstRtspSender::main_loop(): exited for %s", m_appsrcName.c_str());


	if (G_IS_OBJECT(m_loop)) {
		dumpLog("unref m_loop...");
		g_object_unref(m_loop);
		m_loop = nullptr;
	}
	m_mainLoopEnd = true;
	dumpLog("D-cleanup done!");
}

void GstRtspSender::cb_need_data(GstElement* appsrc, guint unused, gpointer user_data)
{
	//dumpLog("GstRtspSender::cb_need_data(): called!");

	GstFlowReturn ret;
	GstRtspSender* pThis = reinterpret_cast<GstRtspSender*>(user_data);

	if (pThis->m_clientCount.load() < 1) {
		dumpLog("GstRtspSender::cb_need_data() -- no client connected!");
		return;
	}

	if (pThis->m_requestToClose.load()) {
		//from: https://stackoverflow.com/questions/48073759/appsrc-is-stuck-in-preroll-even-when-sending-eos
		//when you want to stop perform following steps : 
		//[1] stop pushing the data to appsrc
		//[2] send eos to oggmux(where you should get eos on bus)   ----wus1: we do not have this
		//[3] send eos to appsrc 
		//The above will make sure everything is closed.� Prabhakar Lad Jan 3 '18 at 10:55
		//When the last byte is pushed into appsrc, you must call gst_app_src_end_of_stream() to make it send an EOS downstream.
		dumpLog("GstRtspSender::cb_need_data() -- close stream");
		if (pThis->m_eosRequestCnt < 1) {
			gst_app_src_end_of_stream(GST_APP_SRC(appsrc));
		}
		pThis->m_eosRequestCnt += 1;
		dumpLog("GstRtspSender::cb_need_data(): m_eosRequestCnt=%d", pThis->m_eosRequestCnt);
		return;
	}

	//read next img from <pThis->m_hostYuvQ> to <pThis->m_hostYuvFrm> -- hard copy
	bool hasNewFrm = pThis->m_hostYuvQ->readNext(pThis->m_hostYuvFrm.get());
	if (hasNewFrm) {
		//soft copy from <pThis->m_hostYuvFrm> to <buffer>
		GstBuffer* buffer = gst_buffer_new_allocate(NULL, pThis->m_hostYuvFrm->sz_, NULL);
		gst_buffer_fill(buffer, 0, pThis->m_hostYuvFrm->buf_, pThis->m_hostYuvFrm->sz_);

		/* increment the timestamp every 1/MY_FPS second */
		GST_BUFFER_PTS(buffer) = pThis->m_timestamp;
		GST_BUFFER_DTS(buffer) = pThis->m_timestamp;
		GST_BUFFER_DURATION(buffer) = pThis->m_timeDurationNanoSec;
		pThis->m_timestamp += GST_BUFFER_DURATION(buffer);

		g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
		if (ret != GST_FLOW_OK) {
			//todo: something wrong, stop pushing
			dumpLog("GstRtspSender::cb_need_data(): sth is wrong!  ret=%d, name=%s", (int)ret, gst_flow_get_name(ret));
		}

		//if (0 == pThis->m_hostYuvFrm->fn_ % 100) {
			g_print("fn=%lu\n", pThis->m_hostYuvFrm->fn_);
		//}
		gst_buffer_unref(buffer);
	}
	//dumpLog("GstRtspSender::cb_need_data(): hasNewFrm=%d, duration=%llu, ts=%llu", hasNewFrm, pThis->m_timeDurationNanoSec, pThis->m_timestamp);

}

void GstRtspSender::init()
{
	std::string ipStr = ipConvertNum2Str(m_cfg->clientIp);
	//RTSP image size
	m_videoInfo.w = m_cfg->imgSz.w;
	m_videoInfo.h = m_cfg->imgSz.h;

	m_videoInfo.bitrate_Kbps = m_cfg->bitrate_Kbps;

	m_videoInfo.frame_rate.num = (int)round(m_cfg->fps);  //todo: float --> (num, den)
	m_videoInfo.frame_rate.den = 1;
	m_videoInfo.pix_fmt = AV_PIX_FMT_YUV420P;
	m_videoInfo.isLive = false;

	m_appsrcName = "appSrc_" + ipStr;
	m_hostYuvFrm.reset(new HostYuvFrm(m_videoInfo.w, m_videoInfo.h));

	m_hostYuvQ.reset(new HostYuvFrmQ(m_videoInfo.w, m_videoInfo.h, m_cfg->yuvImgCircularQueSize));

	m_timeDurationNanoSec = gst_util_uint64_scale_int(1, GST_SECOND /*1sec = 1e+9 nano sec*/, m_videoInfo.frame_rate.num /*25*/);
	//alocate memeory for <frm>

	m_requestToClose = false;
	m_eosRequestCnt = 0;
	m_clientCount = 0;
	dumpLog(m_videoInfo.toString("GstRtspSender::init(): m_videoInfo="));
	dumpLog("GstRtspSender::inti(): m_timeDurationNanoSec=%d, GST_SECOND=%d", m_timeDurationNanoSec, GST_SECOND);
}


void GstRtspSender::addNewFrm(const HostYuvFrm* frm)
{
	m_hostYuvQ->wrtNext(frm);
}
