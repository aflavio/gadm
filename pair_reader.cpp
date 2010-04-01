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
#include <time.h>
#include <map>
#include <assert.h>
#include <set>
#include <errno.h>
#include "gadm.h"
#include "tokenizer.h"
#include "state.h"
using namespace std;

// global program state, in main.cpp
extern STATE state;


//
// acceps a textual vertex label and returns the
// corressponding vertex number
//
int map_vertex(char *vertex)    {
    string vlabel(vertex);
    map<string,int>::iterator itr = state.vertex_to_vid.find(vlabel);
    if(itr == state.vertex_to_vid.end())    {
        state.vid_to_vertex.push_back(vlabel);
        return (state.vertex_to_vid[vlabel] = state.nv++);
    }
    return itr->second;
}

//
// quantizes a timestep using the global quantization type (hour, day, etc)
//
int quantize(int timestep)    {
    if(state.first_epoch == -1)
        state.first_epoch = timestep;
    timestep -= state.first_epoch;

    if(state.quantize == QUANTIZE_NONE)
        return timestep;
    time_t x = timestep;
    struct tm thetime;
    localtime_r(&x, &thetime);

    // quantize time
    switch(state.quantize)  {
        case QUANTIZE_HOUR:
            // set minutes and seconds to 0
            thetime.tm_min = 0;
            thetime.tm_sec = 0;
            break;

        case QUANTIZE_DAY:
            // set hour, minutes, seconds to 0
            thetime.tm_min = 0;
            thetime.tm_sec = 0;
            thetime.tm_hour = 0;
            break;
    }

    // convert back to epoch seconds
    timestep = mktime(&thetime);
    return timestep;
}

//
// reads the next timestep from the opened file,
// using the current quantization settings in the global
// state object
//
bool read_pair_timestep(TIMESTEP &ts)   {
    static Tokenizer tok;
    static char buf[4096] = "";
    if(!state.fpData)
        return false;
    ts.edges.clear();
    ts.vertices.clear();

    // do we have a left-over line from the last timestep, or do we need
    // to read another line from the file?
    if(!buf[0])
        if(!fgets(buf, 4096, state.fpData)) {
            // no lines left in file -- we're done reading the whole file
            buf[0] = 0;
            return false;
        }

    // read a new timestep
    ts.epoch = -1;
    do {
        tok.tokenize(buf);
        if(tok.num_tokens() >= 1)   {
            // get the timestep label
            char *endptr;
            errno = 0;
            int tsnum = strtod(tok.token(0), &endptr);
            if(errno || endptr == tok.token(0))   {
                // encountered a non-numeric timestamp
                fprintf(stderr, "Error: encountered a non-numeric timestep label '%s'\n", tok.token(0));
                return false;
            }
            int quant = quantize(tsnum);
            if(ts.epoch == -1)
                ts.epoch = quant;

            if(ts.epoch != quant)
                break;

            // add this observation to the timestep
            if(tok.num_tokens() >= 2)       {
                // have at least one vertex
                int vid1 = map_vertex(tok.token(1));
                ts.vertices.insert(vid1);
                if(tok.num_tokens() >= 3)   {
                    // have an edge
                    int vid2 = map_vertex(tok.token(2));

                    // skip self-loops
                    if(vid1 != vid2)    {
                        ts.vertices.insert(vid2);

                        // make sure vid1 < vid2, so if the graph is undirected then
                        // we never insert a duplicate edge with the endpoints flipped
                        if(!state.directed && vid2 < vid1)  {
                            int t = vid1;
                            vid1 = vid2;
                            vid2 = t;
                        }
                        ts.edges.insert(make_pair<int,int>(vid1,vid2));
                    }
                }
            }
        }

        // read the next line
        if(!fgets(buf, 4096, state.fpData)) {
            buf[0] = 0;
            break;
        }
    } while(1);

    assert(ts.edges.size() <= ts.vertices.size()*(ts.vertices.size()-1)/(state.directed?1:2) );

    return true;
}
