#include <vector>
#include <iostream>
// https://medium.com/the-seekers-project/coding-a-basic-reverb-algorithm-part-2-an-introduction-to-audio-programming-4db79dd4e325
extern const int sample_rate;

struct CombFilter {
    CombFilter(double decay_factor, double delay):
        decay_factor {decay_factor},
        buffer_size {int(delay * sample_rate)},
        buffer {(double*) calloc(buffer_size, sizeof(double))} {}
    ~CombFilter() {
        free(buffer);
    }
    double process(double sample) {
        // std::cout << head << '\t' << buffer_size << std::endl;
        auto ret = sample + buffer[(head+1)%buffer_size];
        buffer[head] += sample;
        buffer[head] *= decay_factor;
        head++;
        head %= buffer_size;

        return ret;
    }
    int head {};
    int buffer_size;
    double decay_factor;
    double* buffer;
};

struct AllPassFilter {
    AllPassFilter(double decay_factor, double delay):
        decay_factor {decay_factor},
        buffer_size {int(delay * sample_rate)},
        buffer {(double*) calloc(buffer_size, sizeof(double))} {}
    ~AllPassFilter() {
        free(buffer);
    }
    double process(double sample) {
        int head = sample_count % buffer_size;
        buffer[(head+1)%buffer_size] = sample;
        if (sample_count >= buffer_size) {
            sample += -decay_factor * buffer[head];
        }
        if (sample_count >= buffer_size+1) {
            sample += -decay_factor * buffer[(head>=20)?head-20:sample_count-20+head];
        }
        head++;
        return sample;
    }
    double decay_factor;
    int sample_count {};
    int buffer_size;
    double* buffer;
};

struct Reverb {
    Reverb(double comb_decay, double all_pass_decay, double delay):
        comb1 {comb_decay, delay}, 
        comb2 {comb_decay - 0.01173, delay - 0.1313}, 
        comb3 {comb_decay + 0.01931, delay - 0.2743}, 
        comb4 {comb_decay - 0.00797, delay - 0.31},
        all_pass1 {all_pass_decay, delay},  
        all_pass2 {all_pass_decay, delay} {}

    double process(double sample) {
        sample = comb1.process(sample) + comb2.process(sample) + comb3.process(sample) + comb4.process(sample);
        sample = all_pass1.process(sample);
        sample = all_pass2.process(sample);
        return sample;
    } 
    CombFilter comb1;
    CombFilter comb2;
    CombFilter comb3;
    CombFilter comb4;
    AllPassFilter all_pass1;
    AllPassFilter all_pass2;
};