#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/audio/audio.h>
#include <fstream>
#include <iostream>
#include <string>

static void print_audio_info(GstSample* sample) {
  // Get the caps from the sample
  GstCaps* caps = gst_sample_get_caps(sample);
  if (!caps) {
    std::cerr << "Failed to get caps from sample." << std::endl;
    return;
  }

  // Create a GstAudioInfo structure
  GstAudioInfo info;
  if (gst_audio_info_from_caps(&info, caps)) {
    // Print audio format details
    std::cout << "Sample Rate: " << GST_AUDIO_INFO_RATE(&info) << std::endl;
    std::cout << "Channels: " << GST_AUDIO_INFO_CHANNELS(&info) << std::endl;
    std::cout << "Format: " << gst_audio_format_to_string(GST_AUDIO_INFO_FORMAT(&info)) << std::endl;

    // Determine the data type
    GstAudioFormat format = GST_AUDIO_INFO_FORMAT(&info);
    switch (format) {
    case GST_AUDIO_FORMAT_S16LE:
      std::cout << "Data type: Signed 16-bit little-endian integers" << std::endl;
      break;
    case GST_AUDIO_FORMAT_S32LE:
      std::cout << "Data type: Signed 32-bit little-endian integers" << std::endl;
      break;
    case GST_AUDIO_FORMAT_F32LE:
      std::cout << "Data type: 32-bit little-endian floating point" << std::endl;
      break;
    default:
      std::cout << "Data type: Other format" << std::endl;
      break;
    }
  }
  else {
    std::cerr << "Failed to get audio info from caps." << std::endl;
  }
}

static GstFlowReturn new_sample(GstAppSink* appsink, gpointer user_data) {
  GstSample* sample;
  GstBuffer* buffer;
  GstMapInfo map;
  static int frame_count = 0;

  sample = gst_app_sink_pull_sample(appsink);
  buffer = gst_sample_get_buffer(sample);

  gst_buffer_map(buffer, &map, GST_MAP_READ);

#if 1
  char filename[256];
  std::snprintf(filename, 256, "frm_%05d.bin", frame_count);
  std::ofstream file(filename, std::ios::binary);
  file.write((char*)map.data, map.size);
  file.close();
#else
  print_audio_info(sample);
#endif

  gst_buffer_unmap(buffer, &map);
  gst_sample_unref(sample);
  frame_count++;
  return GST_FLOW_OK;
}

static void pad_added_handler(GstElement* src, GstPad* new_pad, gpointer data) {
  GstPad* sink_pad = gst_element_get_static_pad((GstElement*)data, "sink");
  if (gst_pad_is_linked(sink_pad)) {
    gst_object_unref(sink_pad);
    return;
  }
  gst_pad_link(new_pad, sink_pad);
  gst_object_unref(sink_pad);
}

int test_download_mp3_raw_frames(int argc, char* argv[]) {
  gst_init(&argc, &argv);

  if (argc != 2) {
    g_printerr("Usage: %s <mp3 file>\n", argv[0]);
    return -1;
  }

  GstElement* pipeline, * source, * decoder, * sink;
  GstBus* bus;
  GstMessage* msg;
  GstStateChangeReturn ret;

  pipeline = gst_pipeline_new("mp3-decoder");
  source = gst_element_factory_make("filesrc", "source");
  decoder = gst_element_factory_make("decodebin", "decoder");
  sink = gst_element_factory_make("appsink", "sink");

  if (!pipeline || !source || !decoder || !sink) {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

  g_object_set(source, "location", argv[1], NULL);
  g_object_set(sink, "emit-signals", TRUE, "sync", FALSE, NULL);
  g_signal_connect(sink, "new-sample", G_CALLBACK(new_sample), NULL);
  g_signal_connect(decoder, "pad-added", G_CALLBACK(pad_added_handler), sink);

  gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);
  gst_element_link(source, decoder);

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
