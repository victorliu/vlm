#include <stdlib.h>
#include <math.h>

/*****************************************************************************/
/*                                                                           */
/*  tricircumcenter()   Find the circumcenter of a triangle.                 */
/*                                                                           */
/*  The result is returned both in terms of x-y coordinates and xi-eta       */
/*  coordinates, relative to the triangle's point `a' (that is, `a' is       */
/*  the origin of both coordinate systems).  Hence, the x-y coordinates      */
/*  returned are NOT absolute; one must add the coordinates of `a' to        */
/*  find the absolute coordinates of the circumcircle.  However, this means  */
/*  that the result is frequently more accurate than would be possible if    */
/*  absolute coordinates were returned, due to limited floating-point        */
/*  precision.  In general, the circumradius can be computed much more       */
/*  accurately.                                                              */
/*                                                                           */
/*  The xi-eta coordinate system is defined in terms of the triangle.        */
/*  Point `a' is the origin of the coordinate system.  The edge `ab' extends */
/*  one unit along the xi axis.  The edge `ac' extends one unit along the    */
/*  eta axis.  These coordinate values are useful for linear interpolation.  */
/*                                                                           */
/*  If `xi' is NULL on input, the xi-eta coordinates will not be computed.   */
/*                                                                           */
/*  Code courtesy of Jonathan Shewchuk, placed in public domain.             */
/*                                                                           */
/*****************************************************************************/

static void tricircumcenter(const double *a, const double *b, const double *c, double *circumcenter, double *xi, double *eta)
{
  double xba, yba, xca, yca;
  double balength, calength;
  double denominator;
  double xcirca, ycirca;

  /* Use coordinates relative to point `a' of the triangle. */
  xba = b[0] - a[0];
  yba = b[1] - a[1];
  xca = c[0] - a[0];
  yca = c[1] - a[1];
  /* Squares of lengths of the edges incident to `a'. */
  balength = xba * xba + yba * yba;
  calength = xca * xca + yca * yca;

  /* Calculate the denominator of the formulae. */
#ifdef EXACT
  /* Use orient2d() from http://www.cs.cmu.edu/~quake/robust.html     */
  /*   to ensure a correctly signed (and reasonably accurate) result, */
  /*   avoiding any possibility of division by zero.                  */
  denominator = 0.5 / orient2d(b, c, a);
#else
  /* Take your chances with floating-point roundoff. */
  denominator = 0.5 / (xba * yca - yba * xca);
#endif

  /* Calculate offset (from `a') of circumcenter. */
  xcirca = (yca * balength - yba * calength) * denominator;  
  ycirca = (xba * calength - xca * balength) * denominator;  
  circumcenter[0] = xcirca;
  circumcenter[1] = ycirca;

  if (xi != (double *) NULL) {
    /* To interpolate a linear function at the circumcenter, define a     */
    /*   coordinate system with a xi-axis directed from `a' to `b' and    */
    /*   an eta-axis directed from `a' to `c'.  The values for xi and eta */
    /*   are computed by Cramer's Rule for solving systems of linear      */
    /*   equations.                                                       */
    *xi = (xcirca * yca - ycirca * xca) * (2.0 * denominator);
    *eta = (ycirca * xba - xcirca * yba) * (2.0 * denominator);
  }
}

// The following code is based on that found at:
// https://www.spaceroots.org/documents/circle/CircleFitter.java
//
// Copyright (c) 2005-2007, Luc Maisonobe
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with
// or without modification, are permitted provided that
// the following conditions are met:
// 
//    Redistributions of source code must retain the
//    above copyright notice, this list of conditions and
//    the following disclaimer. 
//    Redistributions in binary form must reproduce the
//    above copyright notice, this list of conditions and
//    the following disclaimer in the documentation
//    and/or other materials provided with the
//    distribution. 
//    Neither the names of spaceroots.org, spaceroots.com
//    nor the names of their contributors may be used to
//    endorse or promote products derived from this
//    software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

/*
Compute the error metric J = sum_i (|p_i - c| - r)^2 as well as its gradient
with respect to the center (dJ).
*/
double error_metric(
	int n, const double *pt, const double *c, const double *r,
	double *dJ
){
	int i;
	double J = 0;
	dJ[0] = 0; dJ[1] = 0;
	for(i = 0; i < n; ++i){
		double dx = pt[2*i+0] - c[0];
		double dy = pt[2*i+1] - c[1];
		double di = hypot(dx, dy);
		double dr = di - *r;
		double ratio = dr/di;
		J += dr * (di + *r);
		dJ[0] += dx * ratio;
		dJ[1] += dy * ratio;
	}
	dJ[0] *= 2;
	dJ[1] *= 2;
	return J;
}

/* Computes the Newton step */
double newton_step(
	int n, const double *pt,
	double *c, double *r,
	double u, double v
){
	int i;
	double sum1 = 0;
	double sum2 = 0;
	double sumFac = 0;
	double sumFac2R = 0;
	for(i = 0; i < n; ++i){
		double dx = c[0] - pt[2*i+0];
		double dy = c[1] - pt[2*i+1];
		double di = hypot(dx, dy);
		double coeff1 = (dx*u + dy*v) / di;
		double coeff2 = di - *r;
		sum1 += coeff1*coeff2;
		sum2 += coeff2 / di;
		sumFac += coeff1;
		sumFac2R += coeff1*coeff1/di;
	}
	return -sum1 / ((u*u+v*v) * sum2 - sumFac*sumFac/n + (*r)*sumFac2R);
}

int circle_fit(
	int n, const double *pt,
	int *niters,
	double *tol,
	double *c, double *r
){
	int i, j, k, m;
	const int max_iters = *niters;
	double Jprev, Uprev = 0, Vprev = 0;
	double dJ[2];
	double dJprev[2] = { 0, 0 };
	const double tol_limit = (*tol)*(*tol);
	
	const double tol_inner = 0.1;
	const double tol_outer = 1e-12;
	
	if(n < 3){ return -1; }
	
	// For all possible triplets, compute circumcenter
	c[0] = 0; c[1] = 0;
	for(i = 0; i < n-2; ++i){
		for(j = i+1; j < n-1; ++j){
			for(k = j+1; k < n; ++k){
				double cc[2];
				tricircumcenter(
					&pt[2*i+0], &pt[2*j+0], &pt[2*k+0],
					cc, NULL, NULL
				);
				c[0] += cc[0];
				c[1] += cc[1];
				++m;
			}
		}
	}
	// Compute average circumcenter and radius
	c[0] /= m;
	c[1] /= m;
	*r = 0;
	for(i = 0; i < n; ++i){
		*r += hypot(pt[2*i+0] - c[0], pt[2*i+1] - c[1]);
	}
	*r /= n;

	*tol = error_metric(n, pt, c, r, dJ);
	if(*tol < tol_limit){ return 0; }
	for(*niters = 0; *niters < max_iters; ){
		double Jinner;
		double u = -dJ[0];
		double v = -dJ[1];
		if(*niters > 0){ // Polak-Ribiere coefficient
			double beta = (
				dJ[0]*(dJ[0] - dJprev[0]) + dJ[1]*(dJ[1] - dJprev[1])
				) / (dJprev[0]*dJprev[0] + dJprev[1]*dJprev[1]);
			u += beta * Uprev;
			v += beta * Vprev;
		}
		dJprev[0] = dJ[0];
		dJprev[1] = dJ[1];
		Uprev = u;
		Vprev = v;
		
		// rough minimization along search direction
		do{
			double lambda;
			Jinner = *tol;
			lambda = newton_step(n, pt, c, r, u, v);
			c[0] += lambda * u;
			c[1] += lambda * v;
			
			*r = 0;
			for(i = 0; i < n; ++i){
				*r += hypot(pt[2*i+0] - c[0], pt[2*i+1] - c[1]);
			}
			*r /= n;
			*tol = error_metric(n, pt, c, r, dJ);
		}while(
			((++(*niters)) < max_iters) &&
			fabs((*tol)-Jinner) > (*tol)*tol_inner
		);
		if(*tol < tol_limit || fabs(*tol-Jinner) < tol_outer * (*tol)){
			*tol = sqrt(*tol);
			return 0;
		}
		Jprev = *tol;
	}
	*tol = sqrt(*tol);
	return 1;
}
