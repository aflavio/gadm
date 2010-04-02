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
#include "converger.h"
#include <math.h>


Converger::Converger(int min, int max, double thresh, int iters)   {
    conv_min = iters>min?iters:min;
    conv_max = iters>max?iters:max;
    conv_thresh = thresh;
    conv_iters = iters;
    trial  = 0;
    sum    = 0;
    values = new double[iters];
    for(int i = 0; i < iters; i++)
        values[i] = 0;
}
Converger::~Converger() {
    delete[] values;
}

void Converger::value(double delta) {
    delta   = fabs(delta);
    int pos = trial % conv_iters;
    sum -= values[pos];
    if(delta > values[pos])
        values[pos] = delta;
    sum += values[pos];
}

void Converger::next_iteration()    {
    trial++;
    int pos = trial % conv_iters;
    sum -= values[pos];
    values[trial%conv_iters]=0;
}

bool Converger::converged() {
    if(trial < conv_min)
        return false;
    if(trial >= conv_max)
        return true;

    return sum/(double)conv_iters < conv_thresh;
}
