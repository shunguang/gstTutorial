#include <gst/gst.h>

//
//before run, you should check the "videotestsrc" parameters by runing
//$ gst-inspect-1.0 videotestsrc
//in command line either in windows or linux
//
static gboolean cb_msg_handle(GstBus* bus, GstMessage* message, gpointer user_data);
static GMainLoop* loop = NULL;
int ch1_ex3_get_n_set_src_element_properties(int argc, char *argv[])
{
  gboolean suc = FALSE;
  GstElement* pipeline = NULL;
  GstElement* videoSrc = NULL, * videoSink = NULL;
  GstBus* bus = NULL;

  /*
  step 1: Initialize GStreamer
  we can also use gst_init(NULL, NULL), if no argc and argv params
  */
  gst_init(&argc, &argv);

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

  /* step 3: get properties of videotestsrc element*/
  GType mod;      //GstVideoTestSrcAnimationMode
  GType pattern; //GstVideoTestSrcPattern
  gboolean doTimestamp;
  gint numBuffers;
  //my_object would be the Gstreamer element
  g_object_get(videoSrc,
    "animation-mode", &mod,
    "pattern", &pattern,
    "do-timestamp", &doTimestamp,
    "num-buffers", &numBuffers,
    NULL);
  g_print("defaul: mod=%d, pattern=%d,doTimestamp=%d,numBuffers=%d\n", mod, pattern, doTimestamp, numBuffers);
 //
 //ouput: mod=0, pattern=0,doTimestamp=0,numBuffers=-1
 //(check to see if matches the inspect results)
 //

  /* step 4: set new properties of videotestsrc element*/
  mod = 1;             //"wall-time"
  pattern=1;           //"snow"
  doTimestamp = TRUE;  //"true"
  numBuffers = 1000;   //will only genrate 1000 frames.
  g_object_set(videoSrc,
    "animation-mode", mod,
    "pattern", pattern,
    "do-timestamp", doTimestamp,
    "num-buffers", numBuffers,
    NULL);
  g_print("new: mod=%d, pattern=%d,doTimestamp=%d,numBuffers=%d\n", mod, pattern, doTimestamp, numBuffers);

  /* step 5: add elements into pipeline and link*/
  gst_bin_add_many(GST_BIN(pipeline), videoSrc, videoSink, NULL);
  suc = gst_element_link_many(videoSrc, videoSink, NULL);
  if (!suc)
  {
    g_printerr("Elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
  gst_bus_add_signal_watch(bus);
  g_signal_connect(G_OBJECT(bus), "message", G_CALLBACK(cb_msg_handle), NULL);

  /* step 6: Start playing */
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  /* step 7: use g_main_loop_run() */
  g_main_loop_run(loop);

  /* step 8: Free resources when g_loop exit */
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
  return 0;
}

static gboolean cb_msg_handle(GstBus* bus, GstMessage* message, gpointer user_data)
{
  switch (GST_MESSAGE_TYPE(message)) {
  case GST_MESSAGE_ERROR: {
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
    g_main_loop_quit(loop);
    break;
  }
  case GST_MESSAGE_WARNING: {
    g_print("received a warning msg\n");
    //todo:
    break;
  }
  case GST_MESSAGE_EOS: {
    g_print("received a EOS msg\n");
    g_main_loop_quit(loop);
    break;
  }
  default:
    break;
  }
  return TRUE;
}