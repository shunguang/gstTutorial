int test_GstMyAppMeta();
int test_download_mp3_raw_frames(int argc, char* argv[]);
int test_encode_raw_audio_frames_to_mp3(int argc, char* argv[]);
int test_enc_mux_img_audio_to_ts_file(int argc, char* argv[]);

int main(int argc, char* argv[])
{
  //test_GstMyAppMeta();
  //test_download_mp3_raw_frames( argc, argv);
  //test_encode_raw_audio_frames_to_mp3(argc, argv);
  test_enc_mux_img_audio_to_ts_file(argc, argv);
  return 0;
}
