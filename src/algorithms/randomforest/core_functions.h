#ifndef ALGORITHMS_RANDOMFOREST_CORE_FUNCTIONS_H
#define ALGORITHMS_RANDOMFOREST_CORE_FUNCTIONS_H

#include <vector>
#include <iostream>
#include <hpx/hpx.hpp>
#include <opencv2/opencv.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <hpx/include/thread_executors.hpp>
#include "data_structures.cpp"

namespace algo {

    namespace randomforest {

        std::size_t locidx(std::size_t i, std::size_t np, std::size_t nl);

        void read_data_from_csv(std::string filename,
                                cv::Mat data,
                                cv::Mat classes,
                                std::uint64_t num_samples,
                                std::uint64_t num_attributes);

        algo::randomforest::prediction_result_struct
            train_and_predict(algo::randomforest::test_data_struct test_data);

        HPX_DEFINE_PLAIN_ACTION(train_and_predict, train_and_predict_action);
    }
}

#endif //ALGORITHMS_RANDOMFOREST_CORE_FUNCTIONS_CPP