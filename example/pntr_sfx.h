#include "sfx_gen.h"


typedef struct {
  char riff_header[4];  // Contains "RIFF"
  int32_t wav_size;     // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
  char wave_header[4];  // Contains "WAVE"

  // Format Header
  char fmt_header[4];      // Contains "fmt " (includes trailing space)
  int32_t fmt_chunk_size;  // Should be 16 for PCM
  int16_t audio_format;    // Should be 1 for PCM. 3 for IEEE Float
  int16_t num_channels;
  int32_t sample_rate;
  int32_t byte_rate;         // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
  int16_t sample_alignment;  // num_channels * Bytes Per Sample
  int16_t bit_depth;         // Number of bits per sample

  // Data
  char data_header[4];  // Contains "data"
  int32_t data_bytes;   // Number of bytes in data. Number of samples * num_channels * sample byte size
                        // uint8_t bytes[]; // Remainder of wave file is bytes
} RIFF_header;

// hack to put app in global, so random can be used
pntr_app* _pntr_load_sfx_app;


// This function provided by the user returns an integer between
// 0 (inclusive) and range (exclusive).
int sfx_random(int range) {
  return pntr_app_random(_pntr_load_sfx_app, 0, range);
}

// generate a wav for SFX params and load it a pntr_sound of WAV
pntr_sound* pntr_load_sfx(SfxParams* params) {
  SfxSynth* synth = sfx_allocSynth(SFX_U8, 44100, 10);
  int sampleCount = sfx_generateWave(synth, params);

  RIFF_header wav_header = {
    .riff_header = "RIFF",
    .wave_header = "WAVE",
    .fmt_header = "fmt ",
    .data_header = "data",
    .fmt_chunk_size = 16,
    .audio_format = 1,
    .num_channels = 1,
    .sample_rate = 44100,
    .byte_rate = 44100,
    .sample_alignment = 1,
    .bit_depth = 8,
    .wav_size = sampleCount,
    .data_bytes = sampleCount
  };

  unsigned char* w = malloc(sampleCount + sizeof(wav_header));
  memcpy(w, &wav_header, sizeof(wav_header));
  memcpy((void*)((int64_t)w + sizeof(wav_header)), synth->samples.u8, sampleCount);

  pntr_sound* s = pntr_load_sound_from_memory(PNTR_APP_SOUND_TYPE_WAV, w, sampleCount + sizeof(wav_header));
  if (synth != NULL) {
    free(synth);
  }
  free(w);
  return s;
}