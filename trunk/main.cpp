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
#include <stdlib.h>
#include <cstring>
#include "utils.h"
#include "gadm.h"
using namespace std;

//
// GLOBALS
//
STATE state;
vector<TIMESTEP> dynet;

int main(int argc, char *argv[])    {
    // process command line
    if(argc < 4)    {
        printf("Usage: gadm <input file> <U=undirected,D=directed> <output file>\n");
        return 0;
    }
    state.directed = (argv[2][0]=='D'||argv[2][0]=='d');
    if(!(state.fpData = fopen(argv[1], "r")))   {
        fprintf(stderr, "Unable to open '%s'\n", argv[1]);
        return -1;
    }
    strncpy(state.fpName, argv[1], 255);
    if(!(state.fpOut = fopen(argv[3], "w+")))   {
        fprintf(stderr, "Unable to create '%s'\n", argv[3]);
        return -2;
    }

    // read in the whole .pair file of interactions
    TIMESTEP ts;
    unsigned total_edges = 0;
    while(read_pair_timestep(ts))   {
        dynet.push_back(ts);
        total_edges += ts.edges.size();
    }
    state.print();
    fclose(state.fpData);

    // perform the information accumulation experiment
    exp_gadm_info();

    // clean up
    fclose(state.fpOut);
    return 0;
}
