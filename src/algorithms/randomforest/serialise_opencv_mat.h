#ifndef ALGO_RANDOMFOREST_SERIALISE_OPENCV_MAT_CPP
#define ALGO_RANDOMFOREST_SERIALISE_OPENCV_MAT_CPP

#include <hpx/runtime/serialization/serialize.hpp>
#include <opencv2/opencv.hpp>

namespace hpx
{
    namespace serialization
    {
        void serialize(hpx::serialization::output_archive& ar, cv::Mat& m, int version);

        void serialize(hpx::serialization::input_archive& ar, cv::Mat& m, int version);

    }
}

#endif //ALGO_RANDOMFOREST_SERIALISE_OPENCV_MAT_CPP