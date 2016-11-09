#ifndef ALGO_RANDOMFOREST_HELPERFUNCTIONS_CPP
#define ALGO_RANDOMFOREST_HELPERFUNCTIONS_CPP

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/ml/ml.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace algo{

    namespace randomforest{

        void read_data_from_csv(std::string filename, cv::Mat data, cv::Mat classes, std::uint64_t num_samples, std::uint64_t num_attributes);

        std::size_t locidx(std::size_t i, std::size_t np, std::size_t nl);

    }
}

#endif //ALGO_RANDOMFOREST_HELPERFUNCTIONS_CPP