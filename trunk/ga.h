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
#ifndef __NETCOLLAB_H
#define __NETCOLLAB_H

#include <stdio.h>
#include <set>
#include <vector>
#include <math.h>
#include <float.h>

using namespace std;

class GA_NODE {
    public:
        GA_NODE(unsigned hdf_len);
        GA_NODE &operator=(const GA_NODE&);
        ~GA_NODE();

        char *b;
        double obj;
        int improvements;
        int contacts;
        unsigned len;
};

class SCHEMA {
    public:
        SCHEMA(unsigned hdf_len);
        ~SCHEMA();

        unsigned start;
        unsigned length;
        char *b;
        int val;
};

class GA_hdf	{
	public:
        // constructor and destructor
		GA_hdf(unsigned num_inds, int len = -1);         // -1 means to choose a random length in [256,1024] bits
		~GA_hdf();

		// GA operators
		void hdf_generate(int len = -1);            // -1 means to choose a random length in [256,1024] bits
        void randomize();
		void interact(unsigned v1, unsigned v2, bool directed);
		void interact(unsigned v1, unsigned v2, GA_NODE *child);
		void interact(const set<pair<int,int> > &edges, bool directed);
		GA_NODE *crossover_1pt(GA_NODE *p1, GA_NODE *p2);

        // simple accessor functions
		double pop_mean()	        { return obj_sum / nv; };
		double pop_stdev()	        { return sqrt((double)(obj_sumsq/nv) - pow((double)(obj_sum/nv), 2)); };
		double pop_max()            { return obj_max; };
		unsigned get_hdf_len()      { return hdf_len; };
		unsigned num_nodes()        { return nv; };
		GA_NODE *node(unsigned n)	{ return pop[n]; };

	protected:
		vector<SCHEMA*> hdf_schema;
		GA_NODE **pop;
		unsigned hdf_len;
        unsigned nv;
		double obj_sum;
		double obj_sumsq;
		double obj_max;
		MTRNG rng;

        double objective(GA_NODE *);
};

#endif
