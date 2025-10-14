#include "FFTHelper.hpp"

#include "pocketfft_hdronly.h"

#include <cassert>
#include <complex>
#include <cmath>
#include <iostream>

// vector<complex<double>> FFTHelper::rfft_helper(vector<double> data_in) {
//     /*
//     shape_t shape_in;               // dimensions of the input shape
//     stride_t stride_in;             // must have the size of each element. Must have size() equal to shape_in.size()
//     stride_t stride_out;            // must have the size of each element. Must have size() equal to shape_in.size()
//     shape_t axes;                   // 0 to shape.size()-1 inclusive
//     bool forward;                   // FORWARD or BACKWARD
//     float* data_in;                 // input data (reals)
//     complex<float>* data_out;       // output data (FFT(input))
//     float fct;                      // scaling factor

//     r2c(const shape_t & shape_in,
//         const stride_t & stride_in, const stride_t & stride_out, const shape_t & axes,
//         bool forward, const T * data_in, complex<T> *data_out, T fct,
//         size_t nthreads = 1)
//     */

//     unsigned int N = data_in.size();
//     shape_t shape_in{N};
//     stride_t stride_in{sizeof(double)};
//     stride_t stride_out{sizeof(complex<double>)};
//     shape_t axes{0};
//     bool forward{ BACKWARD };     
//     vector<complex<double>> data_out(N);
//     double fct{ 1.0f };   

//     r2c(
//         shape_in, 
//         stride_in, 
//         stride_out, 
//         axes,
//         forward, 
//         data_in.data(), 
//         data_out.data(), 
//         fct
//     );

//     data_out.resize(N / 2 + 1);
//     return data_out;
// }

// vector<double> FFTHelper::compute_rfft_and_get_magnitudes(vector<double> data_in) {
//     vector<complex<double>> data_out = FFTHelper::rfft_helper(data_in);
//     vector<double> magnitudes;
//     for (size_t i = 0; i < data_out.size(); i++) {
//         magnitudes.push_back(abs(data_out[i]));
//     }
//     return magnitudes;
// }

std::shared_ptr<std::vector<float>> FFTHelper::ComputeFFTMagnitude(std::vector<float>& input) {
    const size_t N = input.size();
    assert(N > 0 && "Input vector must not be empty");

    // Convert input to double (pocketfft expects double)
    std::vector<double> input_double(input.begin(), input.end());

    // Prepare low-level parameters
    pocketfft::shape_t shape_in{N};
    pocketfft::stride_t stride_in{sizeof(double)};
    pocketfft::stride_t stride_out{sizeof(std::complex<double>)};
    pocketfft::shape_t axes{0};
    bool forward{true};
    double fct{1.0};

    // Output buffer (complex)
    std::vector<std::complex<double>> fftOut(N);

    // Perform real-to-complex FFT
    pocketfft::r2c(shape_in, stride_in, stride_out, axes, forward,
        input_double.data(), fftOut.data(), fct);

    // Resize to N/2 + 1 (real FFT symmetry)
    fftOut.resize(N / 2 + 1);

    // Convert complex output to magnitude as float
    auto result = std::make_shared<std::vector<float>>(fftOut.size());
    for (size_t i = 0; i < fftOut.size(); ++i) {
        (*result)[i] = static_cast<float>(std::abs(fftOut[i]));
    }

    return result;
}