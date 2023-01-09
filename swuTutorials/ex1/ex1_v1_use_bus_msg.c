#include <gst/gst.h>
int ex1_v1_use_bus_msg(int argc, char* argv[])
{
	gboolean suc, terminate;
	GstElement* pipeline;
	GstElement *videoSrc, *videoSink;
	GstBus* bus;
	GstMessage* msg;

	/* 
	step 1: Initialize GStreamer
	we can also use: gst_init(NULL, NULL), if no argc and argv params
	*/
	gst_init(&argc, &argv);

	/* step 2: create pipelien and elements*/
	pipeline = gst_pipeline_new("encode-pipeline");
	videoSrc = gst_element_factory_make("videotestsrc", "videoSrc");
	videoSink = gst_element_factory_make("autovideosink", "sink");
	if (!pipeline || !videoSrc || !videoSink)
	{
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	/* step 3: add elements into pipeline and link*/
	gst_bin_add_many(GST_BIN(pipeline), videoSrc, videoSink, NULL);
	suc = gst_element_link_many(videoSrc, videoSink, NULL);
	if (!suc)
	{
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	/* step 4: Start playing */
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* step 5: get bus and mornitor its messages*/
	terminate = FALSE;
	bus = gst_element_get_bus(pipeline);
	do {
		msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
			GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
		if (msg == NULL) continue;

		/*step 5.1: process messages*/
		switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR:
			g_printerr("An error messaged received!");
			terminate = TRUE;
			break;
		case GST_MESSAGE_EOS:
			terminate = TRUE;
			break;
		case GST_MESSAGE_STATE_CHANGED:
			break;
		default:
			g_printerr("Unexpected message received.\n");
			break;
		}
		gst_message_unref(msg);
	} while (!terminate);

	/* step 5: Free resources if  terminate is TRUE */
	gst_message_unref(msg);
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);

	return 0;
}

