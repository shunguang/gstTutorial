#include "GstUdpSender.h"
using namespace std;
using namespace app;

GstUdpSender::GstUdpSender(const GstUdpSenderCfgPtr cfg)
	: m_cfg(cfg)
{
	init();
}

GstUdpSender :: ~GstUdpSender()
{
	dumpLog("GstUdpSender :: ~GstUdpSender()--AAA--%s!", m_appsrcName.c_str());
	endStreamingThread();
	dumpLog("GstUdpSender :: ~GstUdpSender()--BBB--%s!", m_appsrcName.c_str());
}


void GstUdpSender::startStreamingThread()
{
	dumpLog(m_videoInfo.toString("GstUdpSender::startStreamingThread(): m_videoInfo="));
	m_gstThread.reset(new std::thread(&GstUdpSender::main_loop, this));
	m_isOpened = true;
}

bool GstUdpSender::endStreamingThread()		//return;

{
	if (!m_isOpened.load()) {
		return true;
	}

	dumpLog("GstUdpSender::endStreamingThread(): AA- m_mainLoopEnd=%d for %s", m_mainLoopEnd.load(), m_appsrcName.c_str());

	m_requestToClose = true;
	APP_SLEEP_MS(50);            //todo: need to investigate, i am not sure: 
															//pt is that give sometime let cb_need_data() not be called when <this> pointer is deleted.

	g_main_loop_quit(m_loop);

	dumpLog("GstUdpSender::endStreamThread(): BB- m_mainLoopEnd=%d for %s", m_mainLoopEnd.load(), m_appsrcName.c_str());

	int cnt = 0;
	while (1) {
		cnt++;
		APP_SLEEP_MS(10);
		if (m_mainLoopEnd.load()) {
			break;
		}
		if (cnt > 1000) {
			dumpLog("GstUdpSender::endStreamThread(): cannot close current stream: %s", m_appsrcName.c_str());
			return false;
		}
	}
	m_gstThread->join();
	m_isOpened = false;
	m_clientCount = 0;          //force as zero
	dumpLog("GstUdpSender::endStreamThread(): CC-m_mainLoopEnd=%d, serverPort=%d for %s", m_mainLoopEnd.load(), m_cfg->rtspPort, m_appsrcName.c_str());
	return true;
}


void GstUdpSender::main_loop()
{
	dumpLog(m_videoInfo.toString("GstUdpSender::main_loop()--AA: m_videoInfo="));

	GError* err = 0;
	m_mainLoopEnd = false;

	gst_init(NULL, NULL);
	m_loop = g_main_loop_new(NULL, FALSE);

	//min-latency=33333333 (nano sec) (1/30 sec)
	//GST_FORMAT_TIME(3);
	const int w = m_videoInfo.w;
	const int h = m_videoInfo.h;
	const uint64_t minLatency = (m_videoInfo.frame_rate.den * 1000000000) / m_videoInfo.frame_rate.num;
	const uint64_t maxLatency = 1000000000;   //1sec
	const int bFrms = m_videoInfo.frame_rate.num / m_videoInfo.frame_rate.den;
	const std::string clientIp = ipConvertNum2Str(m_cfg->clientIp);  //"127.0.0.1";
	const int br_Kbps = m_videoInfo.bitrate_Kbps;
	const int br_bps = 1000 * m_videoInfo.bitrate_Kbps;
	const int peak_br_bps = (int)(1.2 * br_bps);
	const string liveStr = (m_videoInfo.isLive) ? "true" : "false";
	const string syncStr = (m_videoInfo.isLive) ? "false" : "true";

	string h264EncPrm = "control-rate=2 profile=1 preset-level=0 qp-range=45,51:45,51:45,51 vbv-size=10 iframeinterval=5 EnableTwopassCBR=1 EnableStringentBitrate=1 bit-packetization=1 slice-header-spacing=256";
	int rtpPacketSzInBytes = 1024;
	std::string  pipelineStr;
	std::string  encEle = " x264 "; //" omxh264enc ";

	if (m_cfg->vidEncType == APP_VID_H264){
		//
			//client side:
			// linux (tested on tx2)
			// $export DISPLAY=:0
			// $gst-launch-1.0 udpsrc port=5000 ! 'application/x-rtp,encoding-name=H264,payload=96' ! rtph264depay ! h264parse ! omxh264dec ! nveglglessink
			//
			//windows (test on win7)
			// gst-launch-1.0 udpsrc port=5000 ! application/x-rtp,encoding-name=H264,payload=96 ! rtph264depay ! h264parse ! queue ! avdec_h264 ! autovideosink sync=false -e
			//

			//Edwin's andriod side
		//      nativeInit("udpsrc port=${viewModel.videoPort} ! application/x-rtp, encoding-name=H264, payload=96 ! rtpjitterbuffer drop-on-latency=false latency=1 ! " +
			//              "rtph264depay ! h264parse ! decodebin ! autovideosink sync=false")


	 //   std::string  pipelineStr = "appsrc name=" + m_appsrcName + " block=fasle format=3 is-live=true min-latency=" + std::to_string(minLatency);
		pipelineStr = "appsrc name=" + m_appsrcName + " do-timestamp=true format=3 is-live=" + liveStr;
		pipelineStr += " ! video/x-raw,format=(string)I420,width=(int)" + std::to_string(w) + ",height=(int)" + std::to_string(h) +
			" , framerate=(fraction)" + std::to_string(m_videoInfo.frame_rate.num) + "/" + std::to_string(m_videoInfo.frame_rate.den) +
			" ! identity check-imperfect-timestamp=true" +
			" ! nvvidconv" +
			" ! " + encEle + h264EncPrm + " bitrate=" + std::to_string(br_bps) + " peak-bitrate=" + std::to_string(peak_br_bps) +
			" ! video/x-h264,stream-format=(string)byte-stream" +
			" ! h264parse" +
			" ! rtph264pay pt=96 mtu=" + std::to_string(rtpPacketSzInBytes) +
			" ! udpsink host=" + clientIp + " port=5000 sync=" + syncStr + " async=false";
	}
	else if (m_cfg->vidEncType == APP_VID_H265) {
		//
		// h265 client: (tested)
		// $ export DISPLAY=:0
		// $ gst-launch-1.0 udpsrc port=5000 ! 'application/x-rtp,encoding-name=H265,payload=96' ! queue ! rtph265depay ! h265parse ! omxh265dec ! nveglglessink
		//	
		pipelineStr = "appsrc name=" + m_appsrcName + " block=false format=3 is-live=true min-latency=" + std::to_string(minLatency);
		pipelineStr += " ! video/x-raw,format=(string)I420,width=(int)" + std::to_string(w) + ",height=(int)" + std::to_string(h) +
			" , framerate=" + std::to_string(m_videoInfo.frame_rate.num) + "/" + std::to_string(m_videoInfo.frame_rate.den) +
			" ! identity check-imperfect-timestamp=true" +
			" ! nvvidconv" +
			" ! omxh265enc control-rate=2 bitrate=" + std::to_string(br_Kbps) + " tune=zerolatency" +  //  b-frames=10" +
			" ! video/x-h265,stream-format=(string)byte-stream ! h265parse ! rtph265pay" +
			" ! udpsink host=" + clientIp + " port=5000";
	}
	else {
		appAssert(0, "todo!");
	}

	GstElement* pipeline = gst_parse_launch((const gchar*)pipelineStr.c_str(), &err);
	if (nullptr == pipeline) {
		appAssert(0, "cannot build pipeline!");
	}

	GstElement* appsrc = gst_bin_get_by_name_recurse_up(GST_BIN(pipeline), (const gchar*)m_appsrcName.c_str());
	if (nullptr == appsrc) {
		appAssert(0, "cannot get appsrc!");
	}

	dumpLog(m_videoInfo.toString("GstUdpSender::main_loop()--BB: m_videoInfo="));

	this->m_clientCount = 1;
	this->m_timestamp = 0;
	g_signal_connect(appsrc, "need-data", G_CALLBACK(cb_need_data), (gpointer)this);

	/* play */
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* start serving */
	dumpLog("GstUdpSender::main_loop(): pipeline=" + pipelineStr);
	dumpLog("GstUdpSender::main_loop(): RTS-UDP -- clientIp:%s, rtspPort=%d", ipConvertNum2Str(m_cfg->clientIp).c_str(), m_cfg->rtspPort);
	g_main_loop_run(m_loop);

	dumpLog("GstUdpSender::main_loop(): exited for %s", m_appsrcName.c_str());


	if (G_IS_OBJECT(m_loop)) {
		dumpLog("unref m_loop...");
		g_object_unref(m_loop);
		m_loop = nullptr;
	}
	m_mainLoopEnd = true;
	dumpLog("D-cleanup done!");
}

void GstUdpSender::cb_need_data(GstElement* appsrc, guint unused, gpointer user_data)
{
	//dumpLog("GstUdpSender::cb_need_data(): called!");

	GstFlowReturn ret;
	GstUdpSender* pThis = reinterpret_cast<GstUdpSender*>(user_data);

	if (pThis->m_clientCount.load() < 1) {
		dumpLog("GstUdpSender::cb_need_data() -- no client connected!");
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
		dumpLog("GstUdpSender::cb_need_data() -- close stream");
		if (pThis->m_eosRequestCnt < 1) {
			gst_app_src_end_of_stream(GST_APP_SRC(appsrc));
		}
		pThis->m_eosRequestCnt += 1;
		dumpLog("GstUdpSender::cb_need_data(): m_eosRequestCnt=%d", pThis->m_eosRequestCnt);
		return;
	}


	//read next img from <pThis->m_hostYuvQ> to <pThis->m_hostYuvFrm> -- hard copy
	bool hasNewFrm = pThis->m_hostYuvQ->readNext(pThis->m_hostYuvFrm.get());
	//soft copy from <pThis->m_hostYuvFrm> to <buffer>
	GstBuffer* buffer = gst_buffer_new_wrapped((guchar*)pThis->m_hostYuvFrm->buf_, gsize(pThis->m_hostYuvFrm->sz_));

	/* increment the timestamp every 1/MY_FPS second */
	GST_BUFFER_PTS(buffer) = pThis->m_timestamp;
	GST_BUFFER_DURATION(buffer) = pThis->m_timeDurationNanoSec;
	pThis->m_timestamp += GST_BUFFER_DURATION(buffer);

	g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
	if (ret != GST_FLOW_OK) {
		//todo: something wrong, stop pushing
		dumpLog("GstUdpSender::cb_need_data(): sth is wrong!  ret=%d, name=%s", (int)ret, gst_flow_get_name(ret));
	}

	//dumpLog("GstUdpSender::cb_need_data(): hasNewFrm=%d, duration=%llu, ts=%llu", hasNewFrm, pThis->m_timeDurationNanoSec, pThis->m_timestamp);

}

void GstUdpSender::init()
{
	std::string ipStr = ipConvertNum2Str(m_cfg->clientIp);
	//RTSP image size
	m_videoInfo.w = m_cfg->imgSz.w;
	m_videoInfo.h = m_cfg->imgSz.h;

	m_videoInfo.bitrate_Kbps = m_cfg->bitrate_Kbps;

	m_videoInfo.frame_rate.num = (int)round(m_cfg->fps);
	m_videoInfo.frame_rate.den = 1;
	m_videoInfo.pix_fmt = AV_PIX_FMT_YUV420P;
	m_videoInfo.isLive = true;

	m_appsrcName = "appSrc_" + ipStr;
	m_hostYuvFrm.reset(new HostYuvFrm(m_videoInfo.w, m_videoInfo.h));

	m_hostYuvQ.reset(new HostYuvFrmQ(m_videoInfo.w, m_videoInfo.h, m_cfg->yuvImgCircularQueSize));

	m_timeDurationNanoSec = gst_util_uint64_scale_int(1, GST_SECOND /*1sec = 1e+9 nano sec*/, m_videoInfo.frame_rate.num /*25*/);
	//alocate memeory for <frm>

	m_requestToClose = false;
	m_eosRequestCnt = 0;
	m_clientCount = 0;
	dumpLog(m_videoInfo.toString("GstUdpSender::init(): m_videoInfo="));
	dumpLog("GstUdpSender::inti(): m_timeDurationNanoSec=%d, GST_SECOND=%d", m_timeDurationNanoSec, GST_SECOND);
}


void GstUdpSender::addNewFrm(const HostYuvFrm* frm)
{
	m_hostYuvQ->wrtNext(frm);
}
