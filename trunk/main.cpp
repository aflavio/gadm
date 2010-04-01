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
#include "utils.h"
#include "gadm.h"
using namespace std;

//
// GLOBALS
//
STATE state;
vector<TIMESTEP> dynet;

int main(int argc, char *argv[])    {
    if(argc < 4)    {
        printf("Usage: gadm <input file> <U=undirected,D=directed> <operation>\n");
        printf("  operation:\n");
        printf("     1 = independent cascade experiment  (compare with #2)\n");
        printf("     2 = GADM expected spread experiment (compare with #1)\n");
        printf("     3 = GADM information accumulation experiment\n");
        return 0;
    }

    int op = atoi(argv[3]);
    state.quantize = QUANTIZE_NONE;
    state.directed = (argv[2][0]=='D'||argv[2][0]=='d');
    if(!(state.fpData = fopen(argv[1], "r")))   {
        fprintf(stderr, "Unable to open '%s'\n", argv[1]);
        return -1;
    }

    // read in the whole .pair file
    TIMESTEP ts;
    unsigned total_edges = 0;
    while(read_pair_timestep(ts))   {
        dynet.push_back(ts);
        total_edges += ts.edges.size();
    }

    printf("===========================================\n");
    printf("File       : %s\n", argv[1]);
    printf("Directed   : %s\n", state.directed?"yes":"no");
    printf("First epoch: %u\n", state.first_epoch);
    printf("Nodes      : %d\n", state.nv);
    printf("Edges(tot) : %d\n", total_edges);
    printf("Timestamps : %d\n", (int)dynet.size());
    printf("Mempeak    : %d MB\n", getMemoryPeak()/1024);
    printf("===========================================\n");

    // perform the requested operation
    switch(op)  {
        case 1:
            // independent cascade
            exp_indcascade();
            break;

        case 2:
            // GADM
            exp_gadm();
            break;

        case 3:
            exp_gadm_info();
            break;
    }

    return 0;
}
