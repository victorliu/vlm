#ifndef CIRCLE_FIT_H_INCLUDED
#define CIRCLE_FIT_H_INCLUDED

int circle_fit(
	int npts, const double *pt,
	int *niters,
	double *tol,
	double *c, double *r
); /*
Fits a circle to a set of points, minimizing RMS distance from each
point to the nearest point on the circle.

Arguments:
	npts:   The number of points to fit.
	pt:     The points to fit. The first point is (pt[0], pt[1]),
	        then (pt[2], pt[3]), etc.
	niters: On input, the maxmimum number of iterations to perform.
	        On exit, the number of iterations performed.
	tol:    On input, the error tolerance for convergence.
	        On exit, the fit error; RMS distance from points to circle.
	c:      The returned center (c[0], c[1]).
	r:      The returned radius.

Returns:
	0 on convergence.
	n < 0 if the n-th parameter is invalid.
	n > 0 when convergence is not attained.
*/

#endif /* CIRCLE_FIT_H_INCLUDED */
