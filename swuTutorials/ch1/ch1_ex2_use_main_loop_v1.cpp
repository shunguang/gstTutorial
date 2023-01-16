#include <gst/gst.h>
//pros: only use g_main_loop_run(), very simple code
//cons: will run forever and cannot gracefully exit
int ch1_ex2_use_main_loop_v1(int argc, char *argv[])
{
  gboolean suc=FALSE;
  GstElement* pipeline=NULL;
  GstElement* videoSrc=NULL, *videoSink = NULL;
  //GstBus *bus = NULL;
  //GstMessage *msg = NULL;
  GMainLoop* loop=NULL;

  /* 
  step 1: Initialize GStreamer 
  we can also use gst_init(NULL, NULL), if no argc and argv params
  */
  gst_init (&argc, &argv);

  /* step 2: create pipeline and elements*/
  pipeline = gst_pipeline_new("ex1Pipeline");
  videoSrc = gst_element_factory_make("videotestsrc", "videoSrc");
  videoSink = gst_element_factory_make("autovideosink", "sink");
  loop = g_main_loop_new(NULL, FALSE);
  if (!pipeline || !videoSrc || !videoSink || !loop)
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
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* step 5: use g_main_loop_run() */
  g_main_loop_run(loop);

  /* step 6: Free resources when g_loop exit */
  //gst_message_unref (msg);
  //gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
}

