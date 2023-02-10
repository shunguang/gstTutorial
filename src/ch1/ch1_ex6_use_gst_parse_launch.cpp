//introduction
// https://gstreamer.freedesktop.org/documentation/x264/index.html?gi-language=c
// example:
// gst-launch-1.0 -v videotestsrc pattern=snow ! video/x-raw,width=1280,height=720 ! autovideosink
// gst-launch-1.0 -v videotestsrc num-buffers = 1000 ! x264enc qp-min=18 ! avimux ! filesink location = videotestsrc.avi

#include <gst/gst.h>
static gboolean cb_msg_handler(GstBus* bus, GstMessage* message, gpointer user_data);
static GMainLoop* loop = NULL;
int ch1_ex6_use_gst_parse_launch(int argc, char* argv[])
{
  char launchStr[1024];
  GError* error = NULL;
  GstPipeline* pipeline = NULL;
  GstElement* textOverlay = NULL;
  GstBus* bus = NULL;

	/*
  step 1: Initialize GStreamer
  we can also use gst_init(NULL, NULL), if no argc and argv params
  */
  gst_init(&argc, &argv);
  loop = g_main_loop_new(NULL, FALSE);

  /* step 2: set launch string and creat pipeline */
#ifdef _WIN32
  strcpy_s(launchStr, 1024, "videotestsrc name=videoSrc ! textoverlay name=textOverlay ! autovideosink name=sink");
#else
  strcpy(launchStr, "videotestsrc name=videoSrc ! textoverlay name=textOverlay ! autovideosink name=sink");
#endif
  g_print("Using launch string: %s\n", launchStr);
  pipeline = (GstPipeline*)gst_parse_launch(launchStr, &error);
  if (pipeline == NULL) {
    g_print("Failed to parse launch: %s\n", error->message);
    return -1;
  }
  if (error) g_error_free(error);

  /* step 3: get elements from pipeline by name via gst_bin_get_by_name() */
  textOverlay = gst_bin_get_by_name(GST_BIN(pipeline), "textOverlay");
  g_object_set(textOverlay,
    "text", "Front Door",
    "deltax", 10,
    "deltay", 10,
    "font-desc",  "Sans, 48",
    NULL);

  /* step 4: get bus, set signals to watch, and connect signal and callbacks */
  bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
  gst_bus_add_signal_watch(bus);
  g_signal_connect(G_OBJECT(bus), "message", G_CALLBACK(cb_msg_handler), NULL);

  /* step 5: Start playing and run main_loop */
  gst_element_set_state( (GstElement *)pipeline, GST_STATE_PLAYING);
  g_main_loop_run(loop);

  /* step 6: Free resources when <loop> exit */
  gst_element_set_state((GstElement*)pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
  return 0;
}

static gboolean cb_msg_handler(GstBus* bus, GstMessage* message, gpointer user_data)
{
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
    g_main_loop_quit(loop);
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
    g_main_loop_quit(loop);
    break;
  }
  default:
    break;
  }
  return TRUE;
}