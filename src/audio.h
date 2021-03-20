typedef struct
{
  ma_result result;
  ma_decoder decoder;
  ma_device_config deviceConfig;
  ma_device device;
} audio_data_t;

audio_data_t audio_data;


void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder_read_pcm_frames(&audio_data.decoder, pOutput, frameCount);

    (void)pInput;
}

ma_event g_stopEvent;

void play_audio(const char* filename)
{
  char file_path[255];
  sprintf(file_path, "%s%s", main_dir, filename);

  audio_data.result = ma_decoder_init_file(file_path, NULL, &audio_data.decoder);
  if (audio_data.result != MA_SUCCESS) {
    printf("bad file\n");
    exit(0);
  }
}


void init_audio()
{
  play_audio("empty.wav");

  audio_data.deviceConfig = ma_device_config_init(ma_device_type_playback);
  audio_data.deviceConfig.playback.format   = audio_data.decoder.outputFormat;
  audio_data.deviceConfig.playback.channels = audio_data.decoder.outputChannels;
  audio_data.deviceConfig.sampleRate        = audio_data.decoder.outputSampleRate;
  audio_data.deviceConfig.dataCallback      = data_callback;
  audio_data.deviceConfig.pUserData         = NULL;

  if (ma_device_init(NULL, &audio_data.deviceConfig, &audio_data.device) != MA_SUCCESS) {
    printf("Failed to open playback device.\n");
    ma_decoder_uninit(&audio_data.decoder);
    exit(0);
  }

  if (ma_device_start(&audio_data.device) != MA_SUCCESS) {
    printf("Failed to start playback device.\n");
    ma_device_uninit(&audio_data.device);
    ma_decoder_uninit(&audio_data.decoder);
    exit(0);
  }
}
