#pragma once

#include <memory>
#include <vector>

class FFTHelper {
private:
    // static vector<complex<double>> rfft_helper(vector<double> data_in);
    // static vector<double> compute_rfft_and_get_magnitudes(vector<double> data_in);

public:
    static std::shared_ptr<std::vector<float>> ComputeFFTMagnitude(std::vector<float>& input);

    // Disallow creating an instance of this class
    FFTHelper() = delete;
};
