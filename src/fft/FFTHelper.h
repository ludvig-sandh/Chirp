#ifndef FFTHELPER_H
#define FFTHELPER_H

#include <bits/stdc++.h>
#include <complex>
#include <cmath>
#include <math.h>

#include "pocketfft_hdronly.h"

using namespace std;
using namespace pocketfft;

class FFTHelper {
    private:
    static vector<complex<double>> rfft_helper(vector<double> data_in);

    public:
    static vector<double> compute_rfft_and_get_magnitudes(vector<double> data_in);

    // Disallow creating an instance of this class
    FFTHelper() = delete;
};

#endif
