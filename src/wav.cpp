
#include <fstream>
#include <iostream>
typedef struct WAV_HEADER {
  /* RIFF Chunk Descriptor */
  uint8_t RIFF[4] = {'R', 'I', 'F', 'F'}; // RIFF Header Magic header
  uint32_t ChunkSize;                     // RIFF Chunk Size
  uint8_t WAVE[4] = {'W', 'A', 'V', 'E'}; // WAVE Header
  /* "fmt" sub-chunk */
  uint8_t fmt[4] = {'f', 'm', 't', ' '}; // FMT header
  uint32_t Subchunk1Size = 16;           // Size of the fmt chunk
  uint16_t AudioFormat = 1; // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM
                            // Mu-Law, 258=IBM A-Law, 259=ADPCM
  uint16_t NumOfChan = 2;   // Number of channels 1=Mono 2=Sterio
  uint32_t SamplesPerSec = 32000;   // Sampling Frequency in Hz
  uint32_t bytesPerSec = 32000 * 2; // bytes per second
  uint16_t blockAlign = 4;          // 2=16-bit mono, 4=16-bit stereo
  uint16_t bitsPerSample = 16;      // Number of bits per sample
  /* "data" sub-chunk */
  uint8_t Subchunk2ID[4] = {'d', 'a', 't', 'a'}; // "data"  string
  uint32_t Subchunk2Size;                        // Sampled data length
} wav_hdr;

struct WavFile {
    WavFile(std::string name, uint32_t size): 
        out {name, std::ios::binary},
        fsize {size} {}
    
    void set_sample_rate(uint32_t sr) {
        header.SamplesPerSec = sr;
        header.bytesPerSec = sr * 2;
    }

    void init() {
        header.ChunkSize = fsize + sizeof(wav_hdr) - 8;
        header.Subchunk2Size = fsize + sizeof(wav_hdr) - 44;
        out.write(reinterpret_cast<const char *>(&header), sizeof(header));
    }
    void write(double d) {
        write((int16_t) (d * (1 << 15)));
    }
    void write(int16_t d) {
        out.write(reinterpret_cast<char *>(&d), sizeof(int16_t));
    }
    wav_hdr header {};
    std::ofstream out;
    uint32_t fsize;

};
