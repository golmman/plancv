#include "smooth_angle.h"





// Indentifies peaks in hough vote vector.
void peaks_find(const vector<int>& votes, vector<double>& peaks, double threshold) {
	int n = (int)votes.size();

	assert(n >= 5);

	double thr = threshold * votes[0];
	if (thr > votes[n-2] && thr > votes[n-1] && thr > votes[1] && thr > votes[2]) {
		peaks.push_back(0.0);
	}

	thr = threshold * votes[1];
	if (thr > votes[n-1] && thr > votes[0] && thr > votes[2] && thr > votes[3]) {
		peaks.push_back(CV_PI * 1 / n);
	}

	for (int k = 2; k < n - 2; ++k) {
		thr = threshold * votes[k];
		if (thr > votes[k-2] && thr > votes[k-1] && thr > votes[k+1] && thr > votes[k+2]) {
			peaks.push_back(CV_PI * k / n);
		}
	}

	thr = threshold * votes[n-2];
	if (thr > votes[n-4] && thr > votes[n-3] && thr > votes[n-1] && thr > votes[0]) {
		peaks.push_back(CV_PI * (n-2) / n);
	}

	thr = threshold * votes[n-1];
	if (thr > votes[n-3] && thr > votes[n-2] && thr > votes[0] && thr > votes[1]) {
		peaks.push_back(CV_PI * (n-1) / n);
	}
}


// Computes a partition of [0, pi[ based on the given weights. Pi = 0
void partition_weighted(const vector<double>& weights, vector<double>& partition) {
	vector<double> pa;
	partition_weighted_normed(weights, pa);

	// edit partition such that is a partition of [0, pi[ afterwards

	// skip negative values in the first step
	for (int k = 0; k < pa.size(); ++k) {
		if (pa[k] >= 0) {
			partition.push_back(CV_PI * pa[k]);
		}
	}

	// now add the negative values to the back
	for (int k = 0; k < pa.size(); ++k) {
		if (pa[k] < 0) {
			partition.push_back(CV_PI * (pa[k] + 1.0));
		} else {
			break; // break on first nonnegative value
		}
	}

	assert(partition.size() == pa.size());
}


// create an equidistant partition of [0, pi[
void partition_equidistant(double theta, vector<double>& partition) {
	int numangle = cvRound(CV_PI / theta);

	for (int k = 0; k < numangle; ++k) {
		partition.push_back(theta * k);
	}
}


// Normalise votes such that their sum is 1 afterwards.
// Peaks are smoothed by the given function. 
// The function should be concave with asymptotics o(n) (https://en.wikipedia.org/wiki/Big_O_notation#Little-o_notation).
void votes_normalise(const vector<int>& votes, vector<double>& weights, double (*peak_smoother)(int)) {
	uint64 n = votes.size();
	uint64 sum = 0;

	vector<int> votes2;

	// reduce peaks and smooth distribution
	for (int k = 0; k < n; ++k) {
		votes2.push_back((int)peak_smoother(votes[k]));
	}

	// calculate total votes
	for (int k = 0; k < n; ++k) {
		sum += votes2[k];
	}

	for (int k = 0; k < n; ++k) {
		weights.push_back((double)votes2[k] / sum);
	}
}

// creates a historgram based on normalised angle votes and writes it to an image
void histogram_create(const vector<double>& weights) {
	Mat img = image_create((int)weights.size(), (int)weights.size());
	double max_weight = 0.0;

	for (int k = 0; k < img.cols; ++k) {
		if (max_weight < weights[k]) {
			max_weight = weights[k];
		}
	}

	for (int k = 0; k < img.cols; ++k) {
		line(img, 
			Point(k, img.rows-1), 
			Point(k, (int)((double)img.rows-1 - weights[k] / max_weight * img.rows)), 
			Scalar(0, 128, 255));
	}

	image_write(img, "debug_histogram.png");
}



// Returns partition points based on the given weighted equidistant partition points.
// The equidistant partition points are assumed to be 0, 1/n, 2/n, ..., (n-1)/n
// where n = weights.size() and the sum of the weights has to be 1.
// 1 is excluded since the applications of this function identify 0 as 1 and vice versa (actually 0 = pi).
// Sampling points which cannot "decide" between two peaks are simply omitted so
// the number of returned points is always equal or lower than the number of weights.
// Negative values x have to be interpreted as x+1 < 1.
// E.g. the weights [0.75, 0.0, 0.0, 0.25] map to the sampling points [-0.083, 0.0, 0.083, 0.75].
void partition_weighted_normed(const vector<double>& weights, vector<double>& partition) {
	const int n = (int)weights.size();
	double y;
	double F = 0.0;
	int i = 0;

	for (int j = 1; j < 2*n; j += 2) {
		y = (double)j/(2*n);

		while (F < y) {
			assert(i < n);
			F += weights[i];
			++i;
		}

		if (F == y) {
			//cout << "ALAAAARM: " << F << " " << y << endl;
			//out.push_back((double)i/n - 1.0/(2 * n));
			continue;
		}

		// now F(x) >= y but we have to find the smallest x which satifies the inequality
		--i;
		F -= weights[i];

		assert(i < n);
		assert(weights[i] != 0.0);

		partition.push_back((y - F + weights[i] * i) / (weights[i] * n) - 1.0/(2.0 * n));
	}

}
