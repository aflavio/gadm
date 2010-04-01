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
#include <stdio.h>
#include "gadm.h"
#include "ga.h"

using namespace std;

// global program state, in main.cpp
extern STATE state;
extern vector<TIMESTEP> dynet;


//
// the GADM information accumulation experiments
//
void exp_gadm_info()   {
    GA_hdf ga(state.nv);
    ga.randomize();

//    for(unsigned x = 0; x < ga.num_nodes(); x++)
//        printf("INITIAL %d %f\n", x, ga.node(x)->obj);

    // process the data
    map<int,int> contacts;
    map<int,set<int> > indegree;
    map<int,set<int> > outdegree;
    map<int,int> first_seen;

    for(unsigned T = 0; T < dynet.size(); T++)  {
        TIMESTEP &ts = dynet[T];
        ga.interact(ts.edges, state.directed);

        for(set<pair<int,int> >::iterator itr=ts.edges.begin(); itr!=ts.edges.end();itr++)  {
            int v1 = itr->first;
            int v2 = itr->second;
            indegree[v2].insert(v1);
            outdegree[v1].insert(v2);
            contacts[v2]++;
            if(!state.directed)
                contacts[v1]++;
            if(first_seen.find(v1)==first_seen.end())
                first_seen[v1] = T;
            if(first_seen.find(v2)==first_seen.end())
                first_seen[v2] = T;
        }
    }

    for(unsigned x = 0; x < ga.num_nodes(); x++) {
        unsigned id = indegree[x].size();
        unsigned od = outdegree[x].size();
        int cont = contacts[x];
        int fs   = first_seen[x];
        printf("FINAL %d %f %d %d %d %d\n", x, ga.node(x)->obj, id, od, cont, fs);
    }

}
