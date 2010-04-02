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
// GADM.h -- (1) class and global function declarations
//           (2) constants that define various experimental parameters (tweakable)
//
#ifndef GADM_H_INCLUDED
#define GADM_H_INCLUDED

#include<set>
#include<utility>
#include<vector>
#include<map>
#include<string>
#include "mt19937ar.h"
#include "utils.h"
#include "tokenizer.h"
using namespace std;

//***************************************************************************
// GLOBAL PARAMETERS
//
//
// G_MAX_INPUT_LINE is the maximum length in bytes of a single line in the input file
#define G_MAX_INPUT_LINE        (4096)
// G_MAX_TRIALS maximum number of random trials to give up if ANO estimates have not converged
#define G_MAX_TRIALS            (1000)


//***************************************************************************
// CLASS DEFINITIONS
//
//
// STATE is a convenience structure for storing the program's
// global state
//
enum {QUANTIZE_NONE, QUANTIZE_HOUR, QUANTIZE_DAY};
class STATE {
public:
    bool directed;
    FILE *fpData;
    FILE *fpOut;
    int quantize;
    char fpName[256];
    int nv;                             // number of vertices in current graph
    int ne;                             // number of edges in current graph
    int nt;                             // number of timesteps in current graph
    int first_epoch;                    // first epoch timestep

    // dynamic graph measures and such
    map<string, int> vertex_to_vid;
    vector<string>   vid_to_vertex;

    //
    // default state when the program is started
    //
    void defaultState()  {
        directed = false;
        fpData = NULL;
        fpName[0] = 0;
        clearGraphData();
    };
    void clearGraphData()   {
        nv = 0;
        ne = 0;
        nt = 0;
        first_epoch = -1;
        vertex_to_vid.clear();
        vid_to_vertex.clear();
    }
    void print(FILE *fp=stdout)    {
        fprintf(fp, "# File            : %s\n", fpName);
        fprintf(fp, "# Directed        : %s\n", directed?"yes":"no");
        fprintf(fp, "# Num.verts       : %d\n", nv);
        fprintf(fp, "# Num.edges(total): %d\n", ne);
        fprintf(fp, "# Num.timesteps   : %d\n", nt);
        fprintf(fp, "# First epoch     : %d\n", first_epoch);
    }
    STATE() {
        defaultState();
    };
};

//
// TIMESTEP stores a set of vertices and edges that were observed at a
// particular timestep
//
typedef struct {
    set<pair<int,int> > edges;
    set<int> vertices;
    int epoch;
} TIMESTEP;




//***************************************************************************
// FUNCTION DEFINITIONS
//
//
// input file reader
bool read_pair_timestep(TIMESTEP &ts);

// the main experiment function
void exp_gadm_info();


#endif // GADM_H_INCLUDED
