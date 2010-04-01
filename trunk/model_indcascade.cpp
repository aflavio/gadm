/***************************************************************************
 *   Copyright (C) 2009-2010 by Mayank Lahiri                              *
 *   mlahiri@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "gadm.h"

using namespace std;

// global program state, in main.cpp
extern STATE state;
extern vector<TIMESTEP> dynet;


//
// runs an independent cascade experiment
//
// IN:  num = initiator set size
//      p   = probability of infection
//      rng = reference to MTRNG object
//
// OUT: avg = (return value) = average population proportion activated
//      stdev = variance in above
//
double diffusion_indcascade(double init, double p, double &stdev, MTRNG &rng)   {
    double sum = 0, sqsum = 0;
    int num = init*state.nv;
    char *activated = new char[state.nv];
    unsigned iter;

    for(iter = 0; iter < MAXITERS; iter++) {
        // set initial activated
        memset(activated, 0, state.nv);
        for(int i = 0; i < num; i++)    {
            int x;
            do {
                x = rng.genrand_real2() * state.nv;
            } while(activated[x]);
            activated[x] = 1;
        }
        int act = num;

        // process the data
        for(unsigned T = 0; T < dynet.size(); T++)  {
            TIMESTEP &ts = dynet[T];
            set<int> new_activations;

            // compute new activations in this timestep
            for(set<pair<int,int> >::iterator edg = ts.edges.begin(); edg != ts.edges.end(); edg++)   {
                const pair<int,int> &edge = *edg;
                if(activated[edge.first] || (!state.directed && activated[edge.second])) {
                    if(activated[edge.first] && !activated[edge.second] && rng.genrand_real2() <= p)
                        new_activations.insert(edge.second);
                    if(!state.directed && activated[edge.second] && !activated[edge.first] && rng.genrand_real2() <= p)
                        new_activations.insert(edge.first);
                }
            }

            // finalize new activations
            for(set<int>::iterator itr = new_activations.begin(); itr != new_activations.end(); itr++)
                activated[*itr] = 1;
            act += new_activations.size();
        }

        // done with this iteration
        if(iter > 3)    {
            // check convergence
            double prev_avg = sum / (double)iter;
            double prev_var = sqrt((sqsum - pow(sum,2.0)/(double)iter) / (iter-1.0));
            double run_avg = (sum + act) / (iter+1.0);
            double run_var = sqrt((sqsum+act*act - pow(sum+act,2.0)/(iter+1.0))/(iter*1.0));
            run_avg /= state.nv;
            run_var /= state.nv;
            prev_avg /= state.nv;
            prev_var /= state.nv;

            if( fabs((run_avg-prev_avg)/prev_avg) < 0.001 &&
                fabs((run_var-prev_var)/prev_var) < 0.001)  {
                    iter++;
                    break;
                }
        }
        sum += act;
        sqsum += act*act;
    }

    stdev = sqrt((sqsum - pow(sum,2.0)/(double)iter) / (iter-1.0)) / state.nv;
    printf("IND_CASCADE: prob %.2f initial %.2f final %.3f +- %.3f iters %d\n", p, init, sum/(double)(state.nv*iter), stdev, iter);

    delete[] activated;
    return sum/(double)(iter*state.nv);
}
