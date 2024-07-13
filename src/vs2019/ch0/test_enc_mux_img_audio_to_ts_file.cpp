#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <fstream>
#include <iostream>

#define IMG_W 1920
#define IMG_H 1080
#define VSZ 3110400  //W*H*3/2
#define ASZ 4608     //1152*2*2

#define VIDEO_FRM_SAMPLE_FREQ   30       //Hz
#define RAW_AUDIO_SAMPLING_FREQ 44100    //Hz
#define RAW_AUDIO_FRM_SAMPLES   1152   
#define VIDEO_LENGTH_NS         (240 * GST_SECOND)
GstFlowReturn on_video_feed(GstElement* appsrc, guint size, gpointer user_data)
{
  static int vid_count = 0;
  static GstClockTime m_timestampVid = 0;

  const GstClockTime m_timeDurationVid_nanoSec= gst_util_uint64_scale_int(1, GST_SECOND, VIDEO_FRM_SAMPLE_FREQ); 

  size = VSZ;
  if (vid_count > 99)
  {
    vid_count = 0;
  }


  char* buffer = new char[VSZ];
  char filename[256];
  std::snprintf(filename, 256, "C:/Users/wus1/Projects/swu-personal/gstTutorial/dataset/yuvFrms/yuv-%06d.bin", vid_count++);

  std::ifstream file(filename, std::ios::binary);
  file.read(buffer, VSZ);
  file.close();


  GstBuffer* gst_buffer = gst_buffer_new_allocate(NULL, size, NULL);
  gst_buffer_fill(gst_buffer, 0, buffer, size);
  delete[] buffer;


  GST_BUFFER_PTS(gst_buffer) = m_timestampVid;
  GST_BUFFER_DTS(gst_buffer) = m_timestampVid;
  GST_BUFFER_DURATION(gst_buffer) = m_timeDurationVid_nanoSec;
  m_timestampVid += GST_BUFFER_DURATION(gst_buffer);

  GstFlowReturn ret;
  g_signal_emit_by_name(appsrc, "push-buffer", gst_buffer, &ret);
  gst_buffer_unref(gst_buffer);

  printf("read file: %s, m_timestampVid=%lu, m_timeDurationVid_nanoSec=%lu\n", filename, m_timestampVid, m_timeDurationVid_nanoSec);

  if (m_timestampVid > VIDEO_LENGTH_NS)
  {
    g_signal_emit_by_name(appsrc, "end-of-stream", 0);
    return GST_FLOW_EOS;
  }

  return ret;
}

GstFlowReturn on_audio_feed(GstElement* appsrc, guint size, gpointer user_data) 
{
  static int aid_count = 0;
  static GstClockTime m_timestampAud = 0;
  const GstClockTime m_timeDurationAud_nanoSec =  gst_util_uint64_scale_int(RAW_AUDIO_FRM_SAMPLES, GST_SECOND, RAW_AUDIO_SAMPLING_FREQ);

 //---------------
  size = ASZ;
  if (aid_count > 9000)
  {
    aid_count = 0;
  }

  char* buffer = new char[ASZ];
  char filename[256];
  std::snprintf(filename, 256, "C:/Users/wus1/Projects/swu-personal/gstTutorial/dataset/audioFrms/frm_%05d.bin", aid_count++);

  std::ifstream file(filename, std::ios::binary);
  file.read(buffer, ASZ);
  file.close();
//------------------

  GstBuffer* gst_buffer = gst_buffer_new_allocate(NULL, size, NULL);
  gst_buffer_fill(gst_buffer, 0, buffer, size);
  delete[] buffer;

  GST_BUFFER_PTS(gst_buffer) = m_timestampAud;
  GST_BUFFER_DTS(gst_buffer) = m_timestampAud;
  GST_BUFFER_DURATION(gst_buffer) = m_timeDurationAud_nanoSec;
  m_timestampAud += GST_BUFFER_DURATION(gst_buffer);

  GstFlowReturn ret;
  g_signal_emit_by_name(appsrc, "push-buffer", gst_buffer, &ret);
  gst_buffer_unref(gst_buffer);

  printf("read file: %s, m_timestampAud=%lu, m_timeDurationAud_nanoSec=%lu\n", filename, m_timestampAud, m_timeDurationAud_nanoSec);

  if (m_timestampAud > VIDEO_LENGTH_NS)
  {
    g_signal_emit_by_name(appsrc, "end-of-stream", 0);
    return GST_FLOW_EOS;
  }

  return ret;
}

int test_enc_mux_img_audio_to_ts_file(int argc, char* argv[])
{
  gst_init(&argc, &argv);

  GstElement* pipeline, * video_appsrc, * audio_appsrc,  * video_convert, * audio_convert, * x264enc, * aacenc, * mpegtsmux, * filesink, * audio_resample;

  pipeline = gst_pipeline_new("pipeline");
  video_appsrc = gst_element_factory_make("appsrc", "video_appsrc");
  audio_appsrc = gst_element_factory_make("appsrc", "audio_appsrc");
  video_convert = gst_element_factory_make("videoconvert", "video_convert");
  //audio_convert = gst_element_factory_make("audioconvert", "audio_convert");    //we do not need the convert, since opusenc accepts S16LE also
  audio_resample = gst_element_factory_make("audioresample", "audio_resample");   //audio resample is important, since opusenc only accpet 48000Hz and our raw data is 44100Hz 

  x264enc = gst_element_factory_make("x264enc", "x264enc");
  aacenc = gst_element_factory_make("opusenc", "opusenc");
  mpegtsmux = gst_element_factory_make("mpegtsmux", "mpegtsmux");
  filesink = gst_element_factory_make("filesink", "filesink");


  if (!pipeline || !video_appsrc || !audio_appsrc || !video_convert || /* !audio_convert ||*/ !audio_resample || !x264enc || !aacenc || !mpegtsmux || !filesink) {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

  g_object_set(filesink, "location", "output.ts", NULL);

  GstCaps* video_caps = gst_caps_new_simple("video/x-raw",
    "format", G_TYPE_STRING, "I420",
    "width", G_TYPE_INT, IMG_W,
    "height", G_TYPE_INT, IMG_H,
    "framerate", GST_TYPE_FRACTION, VIDEO_FRM_SAMPLE_FREQ, 1,
    NULL);
  g_object_set(video_appsrc, "caps", video_caps, "format", GST_FORMAT_TIME, NULL);
  gst_caps_unref(video_caps);

  GstCaps* audio_caps = gst_caps_new_simple("audio/x-raw",
    "format", G_TYPE_STRING, "S16LE",
    "channels", G_TYPE_INT, 2,
    "rate", G_TYPE_INT, RAW_AUDIO_SAMPLING_FREQ,
    "layout", G_TYPE_STRING, "interleaved",
    NULL);
  g_object_set(audio_appsrc, "caps", audio_caps, "format", GST_FORMAT_TIME, NULL);
  gst_caps_unref(audio_caps);


  g_signal_connect(video_appsrc, "need-data", G_CALLBACK(on_video_feed),nullptr);
  g_signal_connect(audio_appsrc, "need-data", G_CALLBACK(on_audio_feed),nullptr);

  gst_bin_add_many(GST_BIN(pipeline), video_appsrc, audio_resample, video_convert, x264enc, audio_appsrc, /*audio_convert,*/ audio_resample, aacenc, mpegtsmux, filesink, NULL);

  if (!gst_element_link_many(video_appsrc, video_convert, x264enc, mpegtsmux, NULL)) {
    g_printerr("Video elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  if (!gst_element_link_many(audio_appsrc,/* audio_convert,*/ audio_resample, aacenc, mpegtsmux, NULL)) {
    g_printerr("Audio elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  if (!gst_element_link(mpegtsmux, filesink)) {
    g_printerr("Muxer and sink could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the playing state.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  GstBus* bus = gst_element_get_bus(pipeline);
  GstMessage* msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

  if (msg != NULL) {
    GError* err;
    gchar* debug_info;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error(msg, &err, &debug_info);
      g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
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
