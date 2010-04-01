/***************************************************************************
 *   Copyright (C) 2009 by Mayank Lahiri   *
 *   mlahiri@gmail.com   *
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

class INTERACTION {
    public:
        unsigned from, to, time;
        int perturb;
        int operator<(const INTERACTION &o) const {
            return ((int)time+perturb != (int)o.time+o.perturb) ? (((int)time)+perturb < ((int)o.time)+o.perturb) :
                   (  from!=o.from ? from<o.from : to<o.to );
        };
        bool operator==(const INTERACTION &o) const{ return from==o.from && to==o.to && time==o.time && perturb==o.perturb; };
        INTERACTION() { from = 0; to = 0; time = 0; perturb = 0; };
};

class GA_NODE {
    public:
        GA_NODE(unsigned hdf_len);
        GA_NODE(GA_NODE &);
        GA_NODE &operator=(const GA_NODE&);
        ~GA_NODE();

        char *b;
        double obj;
        int improvements;
        int contacts;
        unsigned len;
};

class RESULT_BY_ITER {
    public:
        double mean;
        double mean_sq;
        double max;
        double max_sq;

        RESULT_BY_ITER()    {
            mean = 0;
            mean_sq = 0;
            max = 0;
            max_sq = 0;
        }
};

class RESULT_BY_NODE {
    public:
        double final_obj;
        double final_obj_sq;
        double improve;
        double improve_sq;
        double contacts;
        double contacts_sq;

	RESULT_BY_NODE() {
            final_obj = 0;
            final_obj_sq = 0;
            improve = 0;
            improve_sq = 0;
            contacts = 0;
            contacts_sq = 0;
	}
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
		GA_hdf(unsigned num_inds, int len = -1);         // -1 means to choose a random length in [128,512]
		~GA_hdf();
		void interact(unsigned v1, unsigned v2, bool directed);
		void interact(unsigned v1, unsigned v2, GA_NODE *&child);
		void interact(set<pair<int,int> > &edges, bool directed);
		void hdf_generate(int len = -1);            // -1 means to choose a random length in [128,512]
        void randomize();
        void initator_set(int num);
        int count_nonzero();

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
