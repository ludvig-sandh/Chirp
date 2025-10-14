#pragma once

#include <memory>
#include <vector>

constexpr double MIN_DB = -100.0; // floor value for normalization
constexpr double MAX_DB = 0.0;    // top of range

class FFTHelper {
public:
    static std::shared_ptr<std::vector<float>> ComputeFFTMagnitudeDB(const std::vector<float>& window);

    // Disallow creating an instance of this class
    FFTHelper() = delete;

private:
    static size_t next_pow2(size_t v);
    // static vector<complex<double>> rfft_helper(vector<double> data_in);
    // static vector<double> compute_rfft_and_get_magnitudes(vector<double> data_in);
};
