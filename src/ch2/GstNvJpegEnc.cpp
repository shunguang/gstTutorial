#include "GstNvJpegEnc.h"
#include "libCfg/CfgBase.h"
using namespace std;
using namespace ngv;

GstNvJpegEnc::GstNvJpegEnc( int w, int h, int yuvFrmQSz)
	: m_gstThread(0)
	, m_loop(0)
	, m_isOpened(false)
	, m_mainLoopEnd(false)
	, m_nFrmFreqToLog(5000)
	, m_requestToClose(false)
	, m_mtxGstLocal()
    , m_cudaYuvQ4Enc(0)
    , m_hostYuvFrm(0)
{

	init( w, h, yuvFrmQSz );
}

GstNvJpegEnc :: ~GstNvJpegEnc()
{
	endGstEncThread();
}


void GstNvJpegEnc::startGstEncThread()
{
	dumpLog("GstNvJpegEnc::startGstEncThread(): AA" );
	m_gstThread.reset(new boost::thread(&GstNvJpegEnc::main_loop, this));
	m_isOpened = true;
}

bool GstNvJpegEnc::endGstEncThread()
{
	if (!m_isOpened.load()) {
		return true;
	}

	dumpLog("GstNvJpegEnc::endGstEncThread(): AA- m_mainLoopEnd=%d", m_mainLoopEnd.load() );

	m_requestToClose = true;
	g_main_loop_quit(m_loop);

    dumpLog("GstNvJpegEnc::endGstEncThread(): BB- m_mainLoopEnd=%d", m_mainLoopEnd.load() );

	int cnt = 0;
	while (1) {
		cnt++;
		BOOST_SLEEP(10);
		if( m_mainLoopEnd.load() ){
			break;
		}

		if (cnt > 1000) {
			dumpLog("GstNvJpegEnc::endStreamThread(): cannot close current stream");
			//todo: how to handel this?
			return false;
		}
	}
	m_gstThread->join();
	m_isOpened = false;

	dumpLog("GstNvJpegEnc::endStreamThread(): CC-m_mainLoopEnd=%d", m_mainLoopEnd.load() );
	return true;
}


void GstNvJpegEnc::main_loop()
{
    string appsrcName = "appsrcNvJpegEnc";
	const int w = m_hostYuvFrm->sz_.w;
	const int h = m_hostYuvFrm->sz_.h;
    const int sz = 3*(w*h/2);
	GError *err = 0;
	m_mainLoopEnd = false;

	gst_init(NULL, NULL);
	
    m_loop = g_main_loop_new(NULL, FALSE);


    //gst-launch-1.0 videotestsrc num-buffers=1 ! 'video/x-raw, width=(int)8000, height=(int)6000, format=(string)I420' ! nvjpegenc ! filesink location=test.jpg -e
    //GST_FORMAT_DEFAULT (1)
    std::string  pipelineStr = "appsrc name=" + appsrcName + " format=1 size=" + std::to_string(sz);
	pipelineStr += " ! video/x-raw, format=(string)I420, width=(int)" + std::to_string(w) + ", height=(int)" + std::to_string(h) + 
        " ! nvjpegenc quality=95" +
		" ! multifilesink location=ngn%06d.jpg";

    GstElement *pipeline = gst_parse_launch( (const gchar*)pipelineStr.c_str(), &err);

    GstElement *appsrc = gst_bin_get_by_name_recurse_up(GST_BIN(pipeline),  (const gchar *)appsrcName.c_str());

    g_signal_connect(appsrc, "need-data", G_CALLBACK(cb_need_data), (gpointer)this);

    /* play */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* start serving */
	dumpLog("GstNvJpegEnc::main_loop(): pipeline=" + pipelineStr );

	g_main_loop_run(m_loop);
	
	dumpLog("GstNvJpegEnc::main_loop(): exited for" );

	if (G_IS_OBJECT(m_loop)) {
		dumpLog("GstNvJpegEnc::main_loop(): unref m_loop...");
		g_object_unref(m_loop);
		m_loop = nullptr;
	}
	m_mainLoopEnd = true;
	dumpLog( "GstNvJpegEnc::main_loop(): D-cleanup done!" );
}


void GstNvJpegEnc::cb_need_data(GstElement *appsrc, guint unused, gpointer user_data)
{
	//dumpLog("GstNvJpegEnc::cb_need_data(): called!");

	GstFlowReturn ret;
	GstNvJpegEnc *pThis = reinterpret_cast<GstNvJpegEnc*>(user_data);


	//read next img from <m_cudaYuvQ4Enc> to <frm_host>
	HostYuvFrmPtr &frm_host = pThis->m_hostYuvFrm;
	bool hasNewFrm = pThis->m_cudaYuvQ4Enc->readNext(frm_host.get(), true);
	if (!hasNewFrm) {
        return;
	}

	//frm_host->wrtFrmNumOnImg(); 

	//soft copy from <frm_host> to <buffer>
	GstBuffer *buffer = gst_buffer_new_wrapped((guchar *)frm_host->buf_, gsize(frm_host->sz_));

	g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
	if (ret != GST_FLOW_OK) {
		//todo: something wrong, stop pushing
		dumpLog("GstNvJpegEnc::cb_need_data(): sth is wrong!  ret=%d, name=%s", (int)ret, gst_flow_get_name(ret));
	}

}


void GstNvJpegEnc::init( int w, int h, int yuvFrmQSz)
{
	m_hostYuvFrm.reset(new HostYuvFrm( w, h) );
	m_cudaYuvQ4Enc.reset(new CudaYuvFrmQ(w, h, yuvFrmQSz, "CudaYuvQ4JpegEnc" );
	m_requestToClose = false;
}

