#include <math.h>
#include <iostream>

#include "hillclimb.h"


using namespace std;


double test_hill(double* in) {
	return exp(-in[0]*in[0] - in[1]*in[1]);
}



void hillclimb(double (*hill)(double*), double* in, double* step, int dim, int maxfails, double stepfactor) {


	double current = 0.0;
	double best = hill(in);
	bool improved = false;
	int* dir = new int[dim];
	double old_in = 0.0;
	int fails = 0;

	while (fails < maxfails) {

		improved = false;

		for (int d = -1; d <= 1; ++d) {
			for (int k = 0; k < dim; ++k) {
				old_in = in[k];
				in[k] += step[k] * d;

				current = hill(in);

				if (best < current) {
					best = current;
					improved = true;
					goto superbreak;
				} else {
					in[k] = old_in;
				}
			}
		}

		superbreak:

		if (!improved) {
			fails += 1;
			for (int k = 0; k < dim; ++k) {
				step[k] *= stepfactor;
			}

			cout << "fail" << endl;
		} else {
			cout << "height = " << best << " at ";
			for (int k = 0; k < dim; ++k) {
				cout << in[k] << " ";
			}
			cout << endl;
		}

	}


	delete[] dir;
}