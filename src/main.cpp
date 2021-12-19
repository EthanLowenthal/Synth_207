#include <iostream>
#include <stdlib.h>
#include "../MidiFile/MidiFile.h"
#include <bitset>
#include <stdio.h>
#include "wav.cpp"
#include "song.cpp"

const int sample_rate = 32000;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "ERROR: You must include path to midi file: ./main [path_to_file]" << std::endl;
        return 1;
    }
    std::string filename {argv[1]};
    Song song {};

    smf::MidiFile midifile;
    midifile.read(filename);
    if (!midifile.status()) {
        std::cerr << "Error reading MIDI file " << filename << std::endl;
        exit(1);
    }
    
    midifile.linkNotePairs();
    midifile.doTimeAnalysis();
    int track_index = 1;
    for (track_index=0; track_index<midifile.getTrackCount(); track_index++ ) {
        Track t {};
        song.tracks.push_back(t);
        for (int i=0; i<midifile[track_index].size(); i++) {
            auto& event = midifile[track_index][i];
            if (event.isNoteOn()) {
                if (!event.isLinked()) continue;
                auto& off_event = *event.getLinkedEvent();
                song.add_note(event.getKeyNumber(), off_event.getDurationInSeconds(), midifile.getTimeInSeconds(event.tick), track_index);
            } else if (event.isTimbre()) {
                song.tracks.back().instrument = event.at(1);
            } else if (event.isController()) {
                switch (event[1])
                {
                case 7:
                    song.tracks.back().volume = event[0] / 255.0;
                    break;
                case 10:
                    song.tracks.back().pan = event.at(2)/64.-1 ;
                    break;
                default:
                    break;
                }
            }
        }
    }

    

    WavFile file {"out.wav", 4*song.get_last_sample()};

    // Track t {};
    // t.instrument = 0;
    // t.volume = 1;
    // song.tracks.push_back(t);
    // song.add_note(69, 1, 0, 0);
    // WavFile file {"test.wav", 6*sample_rate};
    file.set_sample_rate(sample_rate);
    file.init();
    while (!song.done) {
        auto sample = song.get_next_sample();
        file.write(sample.left);
        file.write(sample.right);
    }

    std::cout << "done" << std::endl;

    return 0;
}
