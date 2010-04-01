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
#include <assert.h>
#include <map>
#include <limits.h>
#include <vector>
#include <string.h>
#include <float.h>
#include "gadm.h"
#include "ga.h"
#include "mt19937ar.h"

using namespace std;

//
// Constructors, overloaded operators, and destructors
//
GA_NODE::GA_NODE(unsigned length)  {
    b   = new char[length];
    len = length;
    improvements = 0;
    contacts     = 0;
    obj          = DBL_MIN;
}
GA_NODE::GA_NODE(GA_NODE &o)  {
    improvements = o.improvements;
    contacts     = o.contacts;
    len          = o.len;
    b = new char[o.len];
    memcpy(b, o.b, len);
}
GA_NODE &GA_NODE::operator=(const GA_NODE&o)  {
    if(this != &o)  {
        if(len != o.len)    {
            delete[] b;
            b = new char[o.len];
            len = o.len;
        }
        improvements = o.improvements;
        contacts     = o.contacts;
        obj          = o.obj;
        memcpy(b, o.b, len);
    }
    return *this;
}
GA_NODE::~GA_NODE() {
    delete[] b;
}
SCHEMA::SCHEMA(unsigned len)    {
    b = new char[len];
    length = len;
}
SCHEMA::~SCHEMA()   {
    delete[] b;
}
GA_hdf::GA_hdf(unsigned num_inds, int len)	{
	// setup
	nv = num_inds;
	hdf_len = UINT_MAX;
	pop = new GA_NODE* [nv];
	for(unsigned i = 0; i < nv; i++)
        pop[i] = NULL;

	// generate the HDF fitness function
	hdf_generate(len);
}
GA_hdf::~GA_hdf()	{
    for(unsigned i = 0; i < nv; i++)
        if(pop[i])
            delete pop[i];
	delete[] pop;
	for(unsigned i = 0; i < hdf_schema.size(); i++)
        delete hdf_schema[i];
    hdf_schema.clear();
}


//
// Generate a new random HDF of a specified length
// If len == -1, then len is chosen randomly from [256,1024]
//
void GA_hdf::hdf_generate(int len)	{
    // choose a new HDF length and resize the bistrings in the old population
    if(len < 0)
        len = rng.genrand_real2() * 768 + 256;
    if((unsigned)len != hdf_len)  {
        hdf_len = (unsigned)len;
        for(unsigned i = 0; i < nv; i++)    {
            if(pop[i])
                delete pop[i];
            pop[i] = new GA_NODE(hdf_len);
        }
    }
//    printf("# Generating HDF of length %u: ", hdf_len);

    // delete all the old schema
    for(unsigned i = 0; i < hdf_schema.size(); i++)
        delete hdf_schema[i];
    hdf_schema.clear();

	// generate elementary schema, all positive valued
	unsigned num_elementary = rng.genrand_real2()*(hdf_len/10) + 1;
	for(unsigned i = 0; i < num_elementary; i++)	{
	    unsigned l = rng.genrand_real2() * (hdf_len/10) + 1;
	    SCHEMA *s = new SCHEMA(l);
		s->length = l;
		s->start  = rng.genrand_real1() * (hdf_len-s->length);
		s->val    = rng.genrand_real1() * 4 + 1;
		for(unsigned l = 0; l < s->length; l++)
			s->b[l] = rng.genrand_real2() * 3;  // 0, 1 and 2 for the wildcard
		hdf_schema.push_back(s);
	}
//	printf("%u elementary schema", (unsigned)hdf_schema.size());

	// generate higher-order schema for a fixed number of orders
	// for now, 1% randomly chosen pairs of mutually exclusive elementary schema
	// cause a penalty or benefit of 1 more than the sum of the elementary schema
	unsigned sch_pos = 0, sch_neg = 0;
	unsigned list_start = 0; unsigned list_end = num_elementary;
	for(unsigned order = 0; order < 1; order++)	{
		unsigned exclusive_pairs = 0;

		for(unsigned i1 = list_start; i1 < list_end; i1++)
			for(unsigned i2 = i1+1; i2 < list_end; i2++)
				if(hdf_schema[i1]->start >= hdf_schema[i2]->start+hdf_schema[i2]->length ||
				   hdf_schema[i2]->start >= hdf_schema[i1]->start+hdf_schema[i1]->length)
				   	exclusive_pairs++;

		double accept_prob = 0.01 * exclusive_pairs;
		for(unsigned i1 = list_start; i1 < list_end; i1++)
			for(unsigned i2 = i1+1; i2 < list_end; i2++)
				if( (hdf_schema[i1]->start >= hdf_schema[i2]->start+hdf_schema[i2]->length ||
				     hdf_schema[i2]->start >= hdf_schema[i1]->start+hdf_schema[i1]->length)   &&
				     rng.genrand_real1() < accept_prob )	{
				   // create order-2 schema
				   SCHEMA *first = hdf_schema[i1]->start < hdf_schema[i2]->start ? hdf_schema[i1] : hdf_schema[i2];
				   SCHEMA *second = hdf_schema[i1]->start < hdf_schema[i2]->start ? hdf_schema[i2] : hdf_schema[i1];
				   SCHEMA *s = new SCHEMA(second->start+second->length - first->start);
				   s->val = (first->val + second->val + 1) * (rng.genrand_real1()<0.5?-1:1);
				   if(s->val < 0)
                       sch_neg++;
                   else
                       sch_pos++;
				   s->start = first->start;
				   unsigned x;
				   for(x = 0; x < first->length; x++)
				   		s->b[x] = first->b[x];
				   for(; x < second->start-first->start; x++)
				   		s->b[x] = 2;
                   for(unsigned j = 0; j < second->length; j++)
				   		s->b[x++] = second->b[j];
				   assert(s->length == x);
				   hdf_schema.push_back(s);
				}

        list_start = list_end;
        list_end   = hdf_schema.size();
	}
//	printf(", %u total schema (%u positive, %u negative)\n", (unsigned)hdf_schema.size(), sch_pos+num_elementary, sch_neg);
}

//
// messes up all the bits in every node in the GA
//
void GA_hdf::randomize()	{
	obj_max   = DBL_MIN;
	obj_sum   = 0;
	obj_sumsq = 0;

    // zoink all them bits
	for(unsigned i = 0; i < nv; i++)	{
		for(unsigned l = 0; l < hdf_len; l++)
			pop[i]->b[l] = rng.genrand_real1() < 0.5 ? 0 : 1;
		pop[i]->obj = objective(pop[i]);
		obj_sum    += pop[i]->obj;
		obj_sumsq  += pop[i]->obj * pop[i]->obj;
		if(pop[i]->obj > obj_max)
			obj_max = pop[i]->obj;
	}
}

//
// gives null strings to all nodes, except "num" randomly chosen ones
// which get arbitrary schemas
//
void GA_hdf::initator_set(int num)  {
	obj_max   = DBL_MIN;
	obj_sum   = 0;
	obj_sumsq = 0;

	for(unsigned i = 0; i < nv; i++)    {
	    memset(pop[i]->b, 0, hdf_len);
	    pop[i]->obj = 0;
	}

	for(int i = 0; i < num; i++)   {
	    int x;
	    do {
	        x = rng.genrand_real2() * nv;
	    } while(pop[x]->obj > 1e-5);

	    // give this guy all the schemas
	    pop[x]->obj = 0;
	    for(unsigned u = 0; u < hdf_schema.size(); u++) {
	        const SCHEMA &s = *hdf_schema[u];
	        for(unsigned l = 0; l < s.length; l++)
                if(s.b[l] != 2)
                    pop[x]->b[l+s.start] = s.b[l];
            pop[x]->obj += s.val;
	    }
	    if(pop[x]->obj < 0)
            pop[x]->obj = 0;
        obj_max = pop[x]->obj;
	}
}


//
// HDF objective function
//
double GA_hdf::objective(GA_NODE *n)	{
	n->obj = 0;
	for(unsigned s = 0; s < hdf_schema.size(); s++)	{
		SCHEMA &sch = *hdf_schema[s];
		bool match = true;
		for(unsigned l = 0; l < sch.length; l++)
			if(sch.b[l]==2)
				continue;
			else
				if(sch.b[l] != n->b[sch.start+l])	{
					match = false;
					break;
				}

		if(match)
			n->obj += sch.val;
	}
	if(n->obj < 0)
		n->obj = 0;
	return n->obj;
}

//
// GA operators on a set of edges -- crossover only for now
// if "directed" is true, then v1 is not replaced under any circumstances
//
void GA_hdf::interact(set<pair<int,int> > &edges, bool directed) {
    map<int, GA_NODE*> tng;
    GA_NODE *child;

    for(set<pair<int,int> >::iterator itr = edges.begin(); itr != edges.end(); itr++)   {
        int v1 = itr->first;
        int v2 = itr->second;
        assert(v1 < (int)nv && v2 < (int)nv);
        interact(v1, v2, child);

        if(child->obj > pop[v2]->obj)    {
            if(tng.find(v2) == tng.end())
                tng[v2] = new GA_NODE(*child);
            else {
                if(tng[v2]->obj < child->obj)   {
                    delete tng[v2];
                    tng[v2] = new GA_NODE(*child);
                }
            }
        }

        if(!directed)
            if(child->obj > pop[v1]->obj)    {
                if(tng.find(v1) == tng.end())
                    tng[v1] = new GA_NODE(*child);
                else {
                    if(tng[v1]->obj < child->obj)   {
                        delete tng[v1];
                        tng[v1] = new GA_NODE(*child);
                    }
                }
            }

        delete child;
    }

    // copy next generation to current
    for(map<int,GA_NODE*>::iterator itr = tng.begin(); itr!=tng.end(); itr++)   {
        *pop[itr->first] = *(itr->second);
        delete itr->second;
    }

}

//
// returns the number of GA nodes with nonzero objective scores
//
int GA_hdf::count_nonzero() {
    int r = 0;

    for(unsigned x = 0; x < nv; x++)
        if(pop[x]->obj > 1e-5)
            r++;
    return r;
}

//
// GA operators on two nodes -- crossover only for now
// if "directed" is true, then v1 is not replaced under any circumstances
//
void GA_hdf::interact(unsigned v1, unsigned v2, bool directed)	{
	assert(v1 < nv && v2 < nv);
	GA_NODE *parent1    = pop[v1];
	GA_NODE *parent2    = pop[v2];
	GA_NODE *child1     = new GA_NODE(hdf_len);
	GA_NODE *child2     = new GA_NODE(hdf_len);
	GA_NODE *bestchild  = NULL;

	// one-point crossover for now:
	unsigned split   = rng.genrand_real2() * hdf_len;
	unsigned l;
    for(l = 0; l < split; l++)  {
		child1->b[l] = parent1->b[l];
		child2->b[l] = parent2->b[l];
    }
	for( ; l < hdf_len; l++)    {
		child1->b[l] = parent2->b[l];
		child2->b[l] = parent1->b[l];
	}

    // evaluate
	objective(child1);
	objective(child2);
	bestchild = child1->obj > child2->obj ? child1 : child2;

    // the second parent is always replaced by the best child if
    // the best child's fitness is greater
    if(bestchild->obj > parent2->obj)   {
        obj_sum      -= parent2->obj;
        obj_sumsq    -= parent2->obj * parent2->obj;
        *parent2 = *bestchild;
        obj_sum      += parent2->obj;
        obj_sumsq    += parent2->obj * parent2->obj;
        parent2->improvements++;
		parent2->contacts++;
    }

    // the first parent is replaced if the interaction is not directed
    if(!directed && bestchild->obj > parent1->obj)   {
        obj_sum      -= parent1->obj;
        obj_sumsq    -= parent1->obj * parent1->obj;
        *parent1 = *bestchild;
        obj_sum      += parent1->obj;
        obj_sumsq    += parent1->obj * parent1->obj;
        parent1->improvements++;
		parent1->contacts++;
	}

    // does the population maximum go up too?
	if(parent1->obj > obj_max)
		obj_max = parent1->obj;
	if(parent2->obj > obj_max)
		obj_max = parent2->obj;

    assert(obj_sum/nv <= obj_max);
	delete child1;
	delete child2;
}

//
// GA operators on two nodes -- crossover only for now
// if "directed" is true, then v1 is not replaced under any circumstances
//
// creates a new GA_NODE object for child, sets the "child" pointer to
// this new GA_NODE object (i.e., NEEDS TO BE DESTROYED)
//
void GA_hdf::interact(unsigned v1, unsigned v2, GA_NODE *&child)	{
	assert(v1 < nv && v2 < nv);
	GA_NODE *parent1    = pop[v1];
	GA_NODE *parent2    = pop[v2];
	if(rng.genrand_real2()<0.5) {
	    GA_NODE *x = parent1;
	    parent1 = parent2;
	    parent2 = x;
	}
	child = new GA_NODE(hdf_len);

	// one-point crossover for now:
	unsigned split   = rng.genrand_real2() * hdf_len;
	unsigned l;
    for(l = 0; l < split; l++)
		child->b[l] = parent1->b[l];
	for( ; l < hdf_len; l++)
		child->b[l] = parent2->b[l];

	objective(child);
}
