// QPSOL.cc                                                -*- C++ -*-
/*

Copyright (C) 1996 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#if defined (__GNUG__)
#pragma implementation
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cfloat>
#include <cmath>

#ifndef QPSOL_MISSING

#include "QPSOL.h"
#include "f77-fcn.h"

extern "C"
{
  int F77_FCN (qpsol, QPSOL) (int&, int&, int&, int&, int&, int&,
			      int&, int&, double&, double*, double*,
			      double*, double*, double*, double*,
			      int (*)(const int&, const int&,
				      const int&, const int&, double*,
				      double*, double*),
			      int&, int&, int&, int*, double*, int&,
			      int&, double&, double*, int*, int&,
			      double*, int&);

  int F77_FCN (dgemv, DGEMV) (const char*, const int&, const int&,
			      const double&, const double*,
			      const int&, const double*, const int&,
			      const double&, double*, const int&,
			      long);
}

int
qphess (const int& n, const int& nrowh, const int&,
	const int& jthcol, double *hess, double *x, double *hx)
{
  if (jthcol > 0)
    {
      int hp = (jthcol - 1) * nrowh;
      for (int i = 0; i < n; i++)
	hx[i] = hess[hp+i];
    }
  else
    {
      F77_FCN (dgemv, DGEMV) ("N", n, n, 1.0, hess, n, x, 1, 0.0, hx,
			      1, 1L);
    }

  return 0;
}

ColumnVector
QPSOL::do_minimize (double& objf, int& inform, ColumnVector& lambda)
{
  int n = x.capacity ();
 
  int itmax = (iteration_limit () < 0) ? 50 * n : iteration_limit ();
  int msglvl = print_level ();
  int nclin = lc.size ();
  int nctotl = nclin + n;

  double bigbnd = infinite_bound ();

  double *pa = 0;
  Matrix clin;
  if (nclin > 0)
    {
      clin = lc.constraint_matrix ();
      pa = clin.fortran_vec ();
    }

  double *pbl = new double [nctotl];
  double *pbu = new double [nctotl];

  if (bnds.size () > 0)
    {
      for (int i = 0; i < n; i++)
	{
	  pbl[i] = bnds.lower_bound (i);
	  pbu[i] = bnds.upper_bound (i);
	}
    }
  else
    {
      for (int i = 0; i < n; i++)
	{
	  pbl[i] = -bigbnd;
	  pbu[i] = bigbnd;
	}
    }

  for (int i = 0; i < nclin; i++)
    {
      pbl[i+n] = lc.lower_bound (i);
      pbu[i+n] = lc.upper_bound (i);
    }

  double *pc = c.fortran_vec ();

  double *featol = new double [nctotl];
  double tmp = feasibility_tolerance ();
  for (int i = 0; i < nctotl; i++)
    featol[i] = tmp;

  double *ph = H.fortran_vec ();

  int cold = 1;
  int lp = 0;
  int orthog = 1;

  int *istate = new int [nctotl];

  double *px = x.fortran_vec ();

  int iter = 0;
  lambda.resize (nctotl);
  double *pclambda = lambda.fortran_vec ();

  int leniw = 2 * n;

  int lenw;
  int ncon = nclin > 1 ? nclin : 1;
  if (lp == 0 || nclin >= n)
    lenw = 2*n*(n + 2) + nclin + 2*ncon;
  else
    lenw = 2*ncon*(1 + ncon) + 4*n + nclin;

  int *iw = new int [leniw];
  double *w = new double [lenw];

  F77_FCN (qpsol, QPSOL) (itmax, msglvl, n, nclin, nctotl, ncon, n,
			  n, bigbnd, pa, pbl, pbu, pc, featol, ph,
			  qphess, cold, lp, orthog, istate, px,
			  inform, iter, objf, pclambda, iw, leniw, w,
			  lenw);

  delete [] pbl;
  delete [] pbu;
  delete [] featol;
  delete [] istate;
  delete [] iw;
  delete [] w;

  return x;
}

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
