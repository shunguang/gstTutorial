#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

static gboolean read_and_push_data(GstElement* appsrc) {
  static int frame_count = 0;
  std::ostringstream filename;
  filename << "frame_" << frame_count++ << ".bin";
  std::ifstream file(filename.str(), std::ios::binary);

  if (!file) {
    // End of files
    gst_app_src_end_of_stream(GST_APP_SRC(appsrc));
    return FALSE;
  }

  file.seekg(0, std::ios::end);
  std::streampos size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  file.read(buffer.data(), size);
  file.close();

  g_print("frame_count=%d, size=%lu, buffer.size()=%lu\n", frame_count, static_cast<unsigned long long>(size), buffer.size());

  GstBuffer* gst_buffer = gst_buffer_new_allocate(NULL, buffer.size(), NULL);
  GstMapInfo map;
  gst_buffer_map(gst_buffer, &map, GST_MAP_WRITE);
  memcpy(map.data, buffer.data(), buffer.size());
  gst_buffer_unmap(gst_buffer, &map);

  GST_BUFFER_PTS(gst_buffer) = gst_util_uint64_scale(frame_count * GST_SECOND, 1, 10);
  GST_BUFFER_DURATION(gst_buffer) = gst_util_uint64_scale(GST_SECOND, 1, 10);

  GstFlowReturn ret;
  g_signal_emit_by_name(appsrc, "push-buffer", gst_buffer, &ret);
  gst_buffer_unref(gst_buffer);

  if (ret != GST_FLOW_OK) {
    return FALSE;
  }

  return TRUE;
}

int test_encode_raw_audio_frames_to_mp3(int argc, char* argv[]) {
  gst_init(&argc, &argv);

  if (argc != 2) {
    g_printerr("Usage: %s <output mp3 file>\n", argv[0]);
    return -1;
  }

  GstElement* pipeline, * appsrc, * audioconvert, * lame, * filesink;
  GstBus* bus;
  GstMessage* msg;
  GstStateChangeReturn ret;

  pipeline = gst_pipeline_new("mp3-encoder");
  appsrc = gst_element_factory_make("appsrc", "appsrc");
  audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
  lame = gst_element_factory_make("lamemp3enc", "lamemp3enc");
  filesink = gst_element_factory_make("filesink", "filesink");

  if (!pipeline || !appsrc || !audioconvert || !lame || !filesink) {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

  // Set appsrc properties
  g_object_set(G_OBJECT(appsrc), "format", GST_FORMAT_TIME, NULL);

  GstCaps* caps = gst_caps_new_simple("audio/x-raw",
    "format", G_TYPE_STRING, "S16LE",
    "layout", G_TYPE_STRING, "interleaved",
    "channels", G_TYPE_INT, 2,
    "rate", G_TYPE_INT, 44100,
    NULL);
  gst_app_src_set_caps(GST_APP_SRC(appsrc), caps);
  gst_caps_unref(caps);

  g_object_set(G_OBJECT(filesink), "location", argv[1], NULL);

  gst_bin_add_many(GST_BIN(pipeline), appsrc, audioconvert, lame, filesink, NULL);
  gst_element_link_many(appsrc, audioconvert, lame, filesink, NULL);

  g_signal_connect(appsrc, "need-data", G_CALLBACK(read_and_push_data), NULL);

  ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the playing state.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  bus = gst_element_get_bus(pipeline);
  msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
    (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

  if (msg != NULL) {
    GError* err;
    gchar* debug_info;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error(msg, &err, &debug_info);
      g_printerr("Error received from element %s: %s\n",
        GST_OBJECT_NAME(msg->src), err->message);
      g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
      g_clear_error(&err);
      g_free(debug_info);
      break;
    case GST_MESSAGE_EOS:
      g_print("End-Of-Stream reached.\n");
      break;
    default:
      g_printerr("Unexpected message received.\n");
      break;
    }
    gst_message_unref(msg);
  }

  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
  return 0;
}


