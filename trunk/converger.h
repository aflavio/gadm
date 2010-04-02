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
// ==========================================================================================
// A general purpose framework to test for convergence in an interative process.
//
// Copyright 2009-2010 Mayank Lahiri
// mlahiri@gmail.com
// Released under the terms of the GNU GPL.
// ==========================================================================================
#ifndef CONVERGER_H_INCLUDED
#define CONVERGER_H_INCLUDED

class Converger {
    public:
        Converger(int min, int max, double thresh, int iters);
        ~Converger();
        void next_iteration();
        bool converged();

        // value registers a percent change value for the
        // current trial
        //
        // many values can be registered for a single trial --
        // only the largest percent change will be maintained/considered
        void value(double delta);
        int at_trial()      { return trial; };
        double convtest()   { return sum/conv_iters; };
        double convvalue()  { return values[trial%conv_iters];};

    private:
        int conv_min;
        int conv_max;
        double conv_thresh;
        int conv_iters;
        int trial;
        double *values;
        double sum;
};


#endif // CONVERGER_H_INCLUDED
