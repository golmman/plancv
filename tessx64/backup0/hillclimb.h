#ifndef _HILLCLIMB_H_
#define _HILLCLIMB_H_


double test_hill(double* in);
void hillclimb(double (*hill)(double*), double* in, double* step, int dim, int maxfails = 10, double stepfactor = 0.5);

#endif