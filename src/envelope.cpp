#include <math.h>
#include <iostream>
// https://www.youtube.com/watch?v=Ti_1H3TdkZY

extern const int sample_rate;

enum EnvState {
    env_state_attack,
    env_state_decay,
    env_state_sustain,
    env_state_release,
    env_state_done
};
struct Envelope {
    Envelope() {}
    Envelope(double atk, double dcy, double rlse):
        attack_rate {atk},
        decay_rate {dcy}, 
        release_rate {rlse} {
            calc_multiplier(1, attack_rate);
        }

    void calc_multiplier(double tar, double time_seconds) {
        double overshoot = 1.01;
        target = tar;
        auto dist = target - output;
        asymtote = output + dist * overshoot;
        exponenet = output - asymtote;
        double tau = -1 * time_seconds / log(1.-1./overshoot);
        multiplier = pow(exp(-1./tau), 1./sample_rate);
    }

    void trigger_release() {
        if (state == env_state_attack or state == env_state_decay or state == env_state_sustain) {
            state = env_state_release;
            calc_multiplier(0, release_rate);
        }
    }

    void update_state() {
        switch (state) {
        case env_state_attack:
            if (output >= 1) {
                calc_multiplier(sustain_level, decay_rate);
                state = env_state_decay;
            }
            break;
        case env_state_decay:
            if (output <= sustain_level) {
                calc_multiplier(sustain_level, 1);
                state = env_state_sustain;
            }
            break;
        case env_state_release:
            if (output <= 0) {
                state = env_state_done;
            }
            break;
        
        default:
            break;
        }
    }

    double process() {
        output = asymtote + exponenet;
        exponenet *= multiplier;
        update_state();
        if (state == env_state_done) {
            output = 0;
            multiplier = 0;
            exponenet = 0;
            asymtote = 0;
        }
        return output;
    }

    double sustain_level {.5};
    double attack_rate;
    double decay_rate;
    double release_rate;
    double target {};
    double multiplier {};
    double output {};
    double exponenet {};
    double asymtote {};
    EnvState state {env_state_attack};
};