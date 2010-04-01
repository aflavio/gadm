#include <stdio.h>
#include "gadm.h"

using namespace std;

// global program state, in main.cpp
extern STATE state;
extern vector<TIMESTEP> dynet;


//
// the GADM experiments
//
void exp_gadm()   {
    double stdev;

    diffusion_gadm(0.01*state.nv, stdev);
}
