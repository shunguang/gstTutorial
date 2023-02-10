#include "Ch1CppExample.h"
Ch1CppExample::Ch1CppExample(const Ch1CppExampleCfg& cfg)
	: _cfg(cfg)
{
}

Ch1CppExample::~Ch1CppExample()
{
}

void Ch1CppExample::start()
{
	if (-1==mainLoop()) {
		return;
	}
}

void Ch1CppExample::stop() {
	g_main_loop_quit(_loop);
	while (1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		if (!g_main_loop_is_running(_loop)) {
			break;
		}
	}
	//_appThread->join();
	g_print("Ch1CppExample::stop(): successfully exit!");
}

int Ch1CppExample::mainLoop()
{
	GstStateChangeReturn ret;
	//GstMessage *msg = NULL;

	/*
	step 1: Initialize GStreamer
	we can also use gst_init(NULL, NULL), if no argc and argv params
	*/
	gst_init(NULL, NULL);

	if (!mainLoopHelperCreateElements()) {
		return -1;
	}

	if (!mainLoopHelperLinkElements()) {
		return -1;
	}


	/* step 4: Start playing */
	ret = gst_element_set_state(_pipeline, GST_STATE_PLAYING);

	/* step 5: use g_main_loop_run() */
	g_main_loop_run(_loop);

	g_print("Ch1CppExample::mainLoop(): quit!");
	gst_element_set_state(_pipeline, GST_STATE_NULL);
	gst_object_unref(_pipeline);
	return 0;
}

gboolean Ch1CppExample::mainLoopHelperCreateElements()
{
	_pipeline = gst_pipeline_new("ex1Pipeline");
	_videoSrc = gst_element_factory_make("videotestsrc", "videoSrc");
	_textOverlay = gst_element_factory_make("textoverlay", "textOverlay");
	_videoSink = gst_element_factory_make("autovideosink", "sink");
	_loop = g_main_loop_new(NULL, FALSE);
	if (!_pipeline || !_videoSrc || !_textOverlay || !_videoSink || !_loop)
	{
		g_printerr("Not all elements could be created, _pipeline=%p,_videoSrc=%p,_textOverlay=%p,_videoSink=%p,_loop=%p\n",
			_pipeline, _videoSrc, _textOverlay, _videoSink, _loop);
		return FALSE;
	}

	g_object_set(_textOverlay,
		"text",       _cfg.text.c_str(),
		"deltax",			_cfg.dx,
		"deltay",			_cfg.dy,
		"font-desc",  _cfg.fontdesc.c_str(),
		NULL);
	return TRUE;
}

//add elements into pipeline and link
gboolean Ch1CppExample::mainLoopHelperLinkElements()
{
	gboolean suc;
	gst_bin_add_many(GST_BIN(_pipeline), _videoSrc, _textOverlay, _videoSink, NULL);
	suc = gst_element_link_many(_videoSrc, _textOverlay, _videoSink, NULL);
	if (!suc)
	{
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(_pipeline);
		return FALSE;
	}

	_bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));

	gst_bus_add_signal_watch(_bus);
	g_signal_connect(G_OBJECT(_bus), "message", G_CALLBACK(this->cbMsgHandle), this);

	return TRUE;
}

gboolean Ch1CppExample::cbMsgHandle(GstBus* bus, GstMessage* message, gpointer user_data)
{
	Ch1CppExample* pThis = (Ch1CppExample*)user_data;

	//homework: how many types of messages does gst have?
	switch (GST_MESSAGE_TYPE(message)) {
	case GST_MESSAGE_ERROR:
	{
		GError* err = NULL;
		gchar* name, * debug = NULL;

		name = gst_object_get_path_string(message->src);
		gst_message_parse_error(message, &err, &debug);
		g_printerr("ERROR: from element %s: %s\n", name, err->message);
		if (debug != NULL)
			g_printerr("Additional debug info:\n%s\n", debug);

		g_error_free(err);
		g_free(debug);
		g_free(name);
		pThis->stop();
		break;
	}
	case GST_MESSAGE_WARNING:
	{
		g_print("received a warning msg\n");
		//todo:
		break;
	}
	case GST_MESSAGE_EOS:
	{
		g_print("received a EOS msg\n");
		pThis->stop();
		break;
	}
	default:
		break;
	}
	return TRUE;
}


