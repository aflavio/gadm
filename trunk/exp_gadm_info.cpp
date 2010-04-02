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
//
// exp_gadm_info.cpp -- Performs the information diffusion experiment described
//                      in [Lahiri and Cebrian, AAAI-2010]. Produces output
//                      suitable for input into the free R statistical analysis
//                      package using the R command 'read.table(..., header=T)'
//
#include <assert.h>
#include <stdio.h>
#include "gadm.h"
#include "ga.h"
#include "converger.h"

using namespace std;

// global program state, in main.cpp
extern STATE state;
extern vector<TIMESTEP> dynet;


//
// the GADM information accumulation experiments
//
// the function below encompasses the entire GADM loop
//
void exp_gadm_info()   {
    // some vectors to store initial and final ANOs
    vector<double> ano_initial;
    vector<double> ano_final;
    for(int i = 0; i < state.nv; i++) {
        ano_initial.push_back(0);
        ano_final.push_back(0);
    }

    // maps to store some general node properties
    map<int,int> contacts;
    map<int,set<int> > indegree;
    map<int,set<int> > outdegree;
    map<int,int> first_seen;

    // start the random trials
    Converger conv(3, G_MAX_TRIALS, 0.05, 5);
    for(int trial = 1; trial <= G_MAX_TRIALS; trial++)  {
        GA_hdf ga(state.nv);
        ga.randomize();

        // record initial fitness of all nodes
        double max = ga.pop_max();
        for(unsigned x = 0; x < ga.num_nodes(); x++)    {
            double old_val = ano_initial[x]/trial;
            double this_round = max<1e-20 ? 1.0 : ga.node(x)->obj/max;
            double pcnt = (((this_round+ano_initial[x])/trial) - old_val)/old_val;
            if(old_val > 0)
                conv.value(pcnt);
            ano_initial[x] += this_round;
        }

        // process the data
        for(unsigned T = 0; T < dynet.size(); T++)  {
            TIMESTEP &ts = dynet[T];
            ga.interact(ts.edges, state.directed);

            if(trial == 1)
                for(set<pair<int,int> >::iterator itr=ts.edges.begin(); itr!=ts.edges.end();itr++)  {
                    int v1 = itr->first;
                    int v2 = itr->second;
                    indegree[v2].insert(v1);
                    outdegree[v1].insert(v2);
                    contacts[v2]++;
                    if(!state.directed) {
                        contacts[v1]++;
                        indegree[v1].insert(v2);
                        outdegree[v2].insert(v1);
                    }
                    if(first_seen.find(v1)==first_seen.end())
                        first_seen[v1] = T;
                    if(first_seen.find(v2)==first_seen.end())
                        first_seen[v2] = T;
                }
        }

        // record final fitness of all nodes
        assert(ga.pop_max() >= max);
        max = ga.pop_max();
        for(unsigned x = 0; x < ga.num_nodes(); x++)    {
            double old_val = ano_final[x]/trial;
            double this_round = max<1e-20 ? 1.0 : ga.node(x)->obj/max;
            double pcnt = (((this_round+ano_final[x])/trial) - old_val)/old_val;
            if(old_val > 0)
                conv.value(pcnt);
            ano_final[x] += this_round;
        }

        // output data to result file
        rewind(state.fpOut);

        // output header
        state.print(state.fpOut);
        fprintf(state.fpOut, "# Trials: %d\n", trial);
        fprintf(state.fpOut, "id ano_initial ano_final indegree outdegree contacts firstseen label\n");

        // output all data
        for(unsigned x = 0; x < ga.num_nodes(); x++)
            fprintf(state.fpOut, "%d %.4f %.4f %u %u %d %d \"%s\"\n",
                    x, ano_initial[x]/trial, ano_final[x]/trial, (unsigned)indegree[x].size(),
                    (unsigned)outdegree[x].size(), contacts[x], first_seen[x], state.vid_to_vertex[x].c_str());
        fflush(state.fpOut);

        // check convergence
        printf("TRIAL %d: finished, conv test %f conv val %f\n", trial, conv.convtest(), conv.convvalue());
        if(conv.converged())
            break;
        conv.next_iteration();

    }
}
