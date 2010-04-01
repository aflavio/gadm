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
#ifndef GADM_H_INCLUDED
#define GADM_H_INCLUDED

#include<set>
#include<utility>
#include "mt19937ar.h"
#include "state.h"
#include "utils.h"
#include "tokenizer.h"

using namespace std;

#define MAXITERS        5000

typedef struct {
    set<pair<int,int> > edges;
    set<int> vertices;
    int epoch;
} TIMESTEP;

bool read_pair_timestep(TIMESTEP &ts);

// specific experiments
void exp_indcascade();
void exp_gadm();
void exp_gadm_info();

// diffusion models
double diffusion_indcascade(double num, double p, double &stdev, MTRNG &rng);
double diffusion_gadm(int num, double &stdev);


#endif // GADM_H_INCLUDED
