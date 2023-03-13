#include "GstEncVideoFromImages.h"

using namespace std;
using namespace app;

GstEncVideoFromImages::GstEncVideoFromImages(const int w, const int h, const int fpsNum, const int fpsDen, const std::string &mp4FilePath)
	: m_imgW(w)
	, m_imgH(h)
	, m_fps(fpsNum, fpsDen)
	, m_yuvImgSzInBytes(w*h * 3 / 2)
	, m_timeDurationNanoSec( gst_util_uint64_scale_int(1, GST_SECOND /*1sec = 1e+9 nano sec*/, fpsNum) )
	, m_mp4FilePath(mp4FilePath)
	, m_isInit( false )
{
	init();
}

GstEncVideoFromImages::~GstEncVideoFromImages()
{
	if (m_isInit) {
		endEnc();
	}
}

void GstEncVideoFromImages::init()
{
	if (m_isInit) {
		return;
	}

	GstApp *app = &m_gstApp;
	GstStateChangeReturn state_ret;
	gst_init(NULL, NULL); //Initialize Gstreamer
	app->timestamp = 0; //Set timestamp to 0

						//Create pipeline, and pipeline elements
	app->pipeline = (GstPipeline*)gst_pipeline_new("gstH264EncPipeline");
	app->src = (GstAppSrc*)gst_element_factory_make("appsrc", "gstH264EncSrc");
	app->filter1 = gst_element_factory_make("capsfilter", "gstH264EncFilter1");
	//app->encoder = gst_element_factory_make("omxh264enc", "gstH264EncOmx");
	app->encoder = gst_element_factory_make("x264", "gstH264EncOmx");
	app->filter2 = gst_element_factory_make("capsfilter", "gstH264EncFilter2");
	app->parser = gst_element_factory_make("h264parse", "gstH264EncParser");
	app->qtmux = gst_element_factory_make("qtmux", "gstH264EncMux");
	app->sink = gst_element_factory_make("filesink", "gstH264EncFileSink");

	if (!app->pipeline ||
		!app->src || !app->filter1 ||
		!app->encoder || !app->filter2 ||
		!app->parser || !app->qtmux ||
		!app->sink) {
		appAssert(0, "GstEncVideoFromImages(): Error creating pipeline elements!");
	}

	//Attach elements to pipeline
	gst_bin_add_many(
		GST_BIN(app->pipeline),
		(GstElement*)app->src,
		app->filter1,
		app->encoder,
		app->filter2,
		app->parser,
		app->qtmux,
		app->sink,
		NULL);

	//Set pipeline element attributes
	g_object_set(app->src, "format", GST_FORMAT_TIME, NULL);
	GstCaps *filtercaps1 = gst_caps_new_simple("video/x-raw",
		"format", G_TYPE_STRING, "I420",
		"width", G_TYPE_INT, m_imgW,
		"height", G_TYPE_INT, m_imgH,
		"framerate", GST_TYPE_FRACTION, m_fps.num, m_fps.den,
		NULL);
	g_object_set(G_OBJECT(app->filter1), "caps", filtercaps1, NULL);
	GstCaps *filtercaps2 = gst_caps_new_simple("video/x-h264",
		"stream-format", G_TYPE_STRING, "byte-stream",
		NULL);
	g_object_set(G_OBJECT(app->filter2), "caps", filtercaps2, NULL);
	g_object_set(G_OBJECT(app->sink), "location", m_mp4FilePath.c_str(), NULL);

	//Link elements together
	g_assert(gst_element_link_many(
		(GstElement*)app->src,
		app->filter1,
		app->encoder,
		app->filter2,
		app->parser,
		app->qtmux,
		app->sink,
		NULL));

	//Play the pipeline
	state_ret = gst_element_set_state((GstElement*)app->pipeline, GST_STATE_PLAYING);
	appAssert(state_ret == GST_STATE_CHANGE_ASYNC, " GstEncVideoFromImages::init(): cannot set state to paly()!" );
	m_isInit = true;
}


int GstEncVideoFromImages::addFrm(const HostYuvFrm *yuv)
{
	if ( !m_isInit ) {
		return -1;
	}
#if 0
	yuv.dump(".", "yuvToEnc");
#endif

	//allocate cpu memomry for current frm
	uint8_t* yuvImgBuf = (uint8_t*)malloc(m_yuvImgSzInBytes);
	if (yuvImgBuf == NULL) {
		appAssert(false, "GstEncVideoFromImages::addFrm(): can not alocated memory!");
	}

   //copy image from gpu to cpu
	uint64_t fn;
	yuv->hdCopyTo(yuvImgBuf, m_yuvImgSzInBytes, fn);

	GstBuffer * pushedBuffer = gst_buffer_new_wrapped((gpointer)yuvImgBuf, (gsize)m_yuvImgSzInBytes);

	//Set frame timestamp
	GST_BUFFER_PTS(pushedBuffer) = m_gstApp.timestamp;
	GST_BUFFER_DTS(pushedBuffer) = m_gstApp.timestamp;
	GST_BUFFER_DURATION(pushedBuffer) = m_timeDurationNanoSec;
	m_gstApp.timestamp += GST_BUFFER_DURATION(pushedBuffer);

	GstFlowReturn ret = gst_app_src_push_buffer(m_gstApp.src, pushedBuffer); //Push data into pipeline
	if (ret != GST_FLOW_OK) {
		dumpLog("GstEncVideoFromImages::addFrm(): fn=%d, sth wrong!", yuv->fn_);
		return -1;
	}

	if ( yuv->fn_ % 500 == 0) {
		dumpLog("GstEncVideoFromImages::addFrm(): fn=%d", yuv->fn_);
	}
	//note: the memoery of <yuvImgBuf> will be freeed inside gst
	return 0;
}

int GstEncVideoFromImages::endEnc()
{
	gst_app_src_end_of_stream(GST_APP_SRC(m_gstApp.src));
	APP_SLEEP_MS(50);
	dumpLog("GstEncVideoFromImages::endEnc(): well done!");
	m_isInit = false;
	return 0;
}
