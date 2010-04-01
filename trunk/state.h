#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED
#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include <set>

using namespace std;
enum { QUANTIZE_NONE, QUANTIZE_DAY, QUANTIZE_HOUR };

//
// a convenience structure for storing the program's
// global state
//
class STATE {
public:
    bool directed;
    FILE *fpData;
    int quantize;
    char fpName[256];
    int nv;                             // number of vertices in current graph
    int ne;                             // number of edges in current graph
    int nue;                            // number of unique edges in current graph
    int nt;                             // number of timesteps in current graph
    int min_sup;                        // edge support threshold
    int first_epoch;                    // first epoch timestep

    // dynamic graph measures and such
    map<string, int> vertex_to_vid;
    vector<string>   vid_to_vertex;
    map<pair<int,int>, int> edge_count;
    map<int,int> edge_support_histogram;
    map<double,int> edge_support_cdf;

    //
    // default state when the program is started
    //
    void defaultState()  {
        directed = false;
        fpData = NULL;
        quantize = QUANTIZE_NONE;
        fpName[0] = 0;
        clearGraphData();
    };
    void clearGraphData()   {
        nv = 0;
        nue = 0;
        ne = 0;
        nt = 0;
        min_sup = 0;
        first_epoch = -1;

        vertex_to_vid.clear();
        vid_to_vertex.clear();
        edge_count.clear();
        edge_support_cdf.clear();
        edge_support_histogram.clear();
    }
    void print()    {
        printf("File            : %s\n", fpName);
        printf("Directed        : %s\n", directed?"yes":"no");
        printf("Quantize        : %s\n", quantize==QUANTIZE_NONE?"none":(quantize==QUANTIZE_DAY?"day":"hour"));
        printf("Min.support     : %d\n", min_sup);
        printf("Num.verts       : %d\n", nv);
        printf("Num.edges(total): %d\n", ne);
        printf("Num.edges(uniq.): %d\n", nue);
        printf("Num.timesteps   : %d\n", nt);
        printf("First epoch     : %d\n", first_epoch);
    }
    STATE() {
        defaultState();
    };
};


#endif // STATE_H_INCLUDED
