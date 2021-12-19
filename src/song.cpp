#include <math.h>
#include <vector>
#include <set>
#include <iostream>
#include "instrument.cpp"
#include "envelope.cpp"
#include "../Filters/Filters.h"

const double HALF_STEP = pow(2., 1./12.); 
extern const int sample_rate;

struct Track {
    int instrument {};
    double volume {};
    double pan {};
};


struct Note {
    Note (int note, double duration_seconds, double start_seconds, Track* track): 
        start {(int) (start_seconds * sample_rate)},
        freq {440. * pow(HALF_STEP, note-69)}, 
        duration {(int) (duration_seconds * sample_rate)},
        track {track} {
            try{
            InstrumentMaker* inst = instuments.at(track->instrument);
            instrument = inst();
            envelope = {instrument->attack, instrument->decay, instrument->release};
            }catch (...) {std::cout << track->instrument;throw std::out_of_range {"asdf"};}
        }

    double get_sample(int time) {   
        counter++;
        if (counter >= duration) envelope.trigger_release();
        envelope.process();

        auto amplitude = envelope.output * track->volume;
        double samples_to_time = 2*M_PI/sample_rate;

        auto sample = instrument->osc(freq, time*samples_to_time, amplitude);
        // done = envelope.state == env_state_done;
        done = counter > (duration * 2);
        return sample;

        // auto lfo = (sin(time * samples_to_time * instrument->lfo_freq) * instrument->lfo_amp);
        // double sample {};

        // for (int i {1}; i<=3; i++){
        //     sample += sin(lfo+freq*time*samples_to_time / i) * amplitude * i;
        // }
        // for (int i {1}; i<=5; i++){
        //     sample += sin(lfo+freq*time*samples_to_time * i) * amplitude / i;
        // }
        // counter++;
        // return sample * track->volume;
    }
    bool operator<(const Note& other) const {
        return start < other.start;
    }
    bool done {false};
    unsigned int counter {0};
    int start;
    double freq;
    int duration;
    Envelope envelope {};
    Instrument* instrument;
    Track* track;
};
struct Sample {
    double right {0};
    double left {0};
};
struct Song {
    Song():
        low_pass_left {1./sample_rate, 2 * M_PI * 1000.},
        low_pass_right {1./sample_rate, 2 * M_PI * 1000.} {
    }
    void add_note(int note, double duration, double start, int track) {
        Note n {note, duration, start, &tracks[track]};
        notes.insert(n);
    }
    Sample filter(Sample value) {
        Sample ret {
            low_pass_left.update(value.left / 70),
            low_pass_right.update(value.right / 70)
        };
        return ret;
    }
    Sample get_next_sample() {
        Sample sample;
        if (!notes.empty()) {
            if (notes.begin()->start <= counter) {
                playing_notes.push_back(*notes.begin());
                notes.erase(notes.begin());
            }
        }
        for(std::vector<Note>::iterator iterator = playing_notes.begin(); iterator != playing_notes.end(); ) {
            if(!iterator->done) {
                auto value = iterator->get_sample(counter);
                sample.left += (1 - iterator->track->pan) * value;
                sample.right += (1 + iterator->track->pan) * value;
                ++iterator;
            } else {
                iterator = playing_notes.erase(iterator);
            }
        }
        counter++;
        done = notes.empty() and playing_notes.empty();
        return filter(sample);
    }
    uint32_t get_last_sample() {
        auto last_note = std::prev(notes.end());
        return last_note->start + last_note->duration;
    }
    bool done {false};
    int counter {0};
    std::multiset<Note> notes {};
    std::vector<Track> tracks {};
    std::vector<Note> playing_notes {};
    LowPassFilter3 low_pass_left;
    LowPassFilter3 low_pass_right;
};
