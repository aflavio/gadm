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
#include "ga.h"

using namespace std;

// global program state, in main.cpp
extern STATE state;
extern vector<TIMESTEP> dynet;


//
// runs an independent cascade experiment
//
// IN:  num = initiator set size
//      rng = reference to MTRNG object
//
// OUT: avg = (return value) = average population proportion activated
//      stdev = variance in above
//
double diffusion_gadm(int num, double &stdev)   {
    int iter;
    double sum = 0, sqsum = 0;

    for(iter = 0; iter < MAXITERS; iter++)  {
        // create GA object
        GA_hdf ga(state.nv);
        ga.initator_set(num);
        if(num != ga.count_nonzero())   {
            iter--;
            continue;
        }

        // process the data
        for(unsigned T = 0; T < dynet.size(); T++)  {
            TIMESTEP &ts = dynet[T];
            ga.interact(ts.edges, state.directed);
        }
        int act = ga.count_nonzero();

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

            printf("ITER %d act %d avg %.3f var %.3f\n", iter, act, run_avg, run_var);
            fflush(stdout);

            if( fabs((run_avg-prev_avg)/prev_avg) < 0.005 &&
                fabs((run_var-prev_var)/prev_var) < 0.005)  {
                    iter++;
                    break;
                }
        }
        sum += act;
        sqsum += act*act;
    }

    return 0;
}
