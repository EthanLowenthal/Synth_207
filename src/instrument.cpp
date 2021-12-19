#include <math.h>
#include <map>
#include "reverb.cpp"

extern const int sample_rate;

struct Instrument {
    double attack;
    double decay;
    double release;
    Instrument(double atk, double d, double r):
        attack{atk},
        decay{d},
        release{r} {}

    virtual double osc(double freq, double time, double amp) { return 0; };
};
double panio_wave(double freq, double time) {
    double out {};
    int r = 300./freq + 1;
    for (int n {1};n<=r;n+=2) {
        out += sin(n*freq*time)/n;
    }
    return out;
}
struct Panio:public Instrument {
    // using Instrument::Instrument;
    Panio(): Instrument(.07, .1, .3) {}
    double osc(double freq, double time, double amp) override {
        double sample {};

        for (int i {1}; i<=3; i++){
            sample += panio_wave(freq / i, time) * amp * i;
        }
        for (int i {1}; i<=5; i++){
            sample += panio_wave(freq * i, time) * amp / i;
        }
        // sample += sin(5 * time ) / 5;
        return sample;
    }
};
struct Flute:public Instrument {
    // using Instrument::Instrument;
    Flute(): Instrument(.4, .1, .3) {}
    double osc(double freq, double time, double amp) override {
        double sample {};
        auto lfo = (sin(time * 7.) * .7);

        // for (int i {1}; i<=3; i++){
        //     sample += sin(freq * time / i) * amp* i;
        // }
        sample += sin(lfo + freq * time) * amp / freq * 440;
        
        
        return reverb.process(sample);
    }
    Reverb reverb {.03, .131, 1.};
};
double saw(double x) {
    double i {};
    return 2. * modf(x, &i) -1.;
}
struct Strings:public Instrument {
    // using Instrument::Instrument;
    Strings(): Instrument(1.5, 5., .5) {}
    double osc(double freq, double time, double amp) override {
        double sample {};

        for (int i {1}; i<=3; i++){
            sample += sin(freq * time / i) * amp * i;
        }
        for (int i {1}; i<=5; i++){
            sample += sin(freq * time * i) * amp / i;
        }
        // sample *= amp;
        // // buffer[head] *= .1;
        // buffer[head] = sample;
        // sample += buffer[head+1];
        // head++;
        // head = head % (4000);
        return sample;
    }
    Reverb reverb {.2, .3, 1.};
};


typedef Instrument* InstrumentMaker();

template <class X> Instrument* make() {
  return new X;
}

std::map<int,InstrumentMaker*> instuments {
    {0,make<Panio>},
    {32,make<Panio>}, // bass
    {35,make<Panio>}, // bass
    {42,make<Strings>},
    {45,make<Strings>},
    {48,make<Strings>},
    {50,make<Strings>}, // SynthStrings
    {53,make<Strings>}, // voice
    {56,make<Panio>}, // trumpet
    {57,make<Panio>}, // trombone
    {58,make<Panio>}, // tuba
    {60,make<Panio>}, // french horn
    {62,make<Panio>}, // french horn
    {68,make<Flute>}, // oboe
    {70,make<Flute>}, // basson
    {71,make<Flute>}, // calrinet
    {72,make<Flute>},
    {73,make<Flute>},
    {80,make<Strings>}, // saw lead
    {81,make<Strings>},
    {87,make<Strings>},
    {89,make<Strings>}, // pad
};
