#include <stdio.h>
#include "gadm.h"

using namespace std;

// global program state, in main.cpp
extern STATE state;
extern vector<TIMESTEP> dynet;


//
// the independent cascade experiments
//
void exp_indcascade()   {
    MTRNG rng;
    double stdev;

    for(double p = 0.01; p <= 0.65; p *= 2) {
        diffusion_indcascade(0.01, p, stdev, rng);
        diffusion_indcascade(0.05, p, stdev, rng);
        diffusion_indcascade(0.1, p, stdev, rng);
        diffusion_indcascade(0.2, p, stdev, rng);
        diffusion_indcascade(0.3, p, stdev, rng);
        diffusion_indcascade(0.4, p, stdev, rng);
        diffusion_indcascade(0.5, p, stdev, rng);
    }
}
