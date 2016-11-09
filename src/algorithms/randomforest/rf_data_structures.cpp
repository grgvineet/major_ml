#ifndef  ALGO_RANDOMFOREST_RF_DATASTRUCTURES_CPP
#define  ALGO_RANDOMFOREST_RF_DATASTRUCTURES_CPP

#include <hpx/hpx.hpp>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/ml/ml.hpp"
#include "helper_functions.h"

namespace algo{

    namespace randomforest{

        struct prediction_result_struct
        {
        public:
            cv::Mat classification_result;
            std::uint64_t num_rows;

            prediction_result_struct()
            {}

            prediction_result_struct(cv::Mat result, std::uint64_t num):
                    classification_result(result),
                    num_rows(num)
            {}

        private:
            // Serialization support: even if all of the code below runs on one
            // locality only, we need to provide an (empty) implementation for the
            // serialization as all arguments passed to actions have to support this.
            friend class hpx::serialization::access;

            template <typename Archive>
            void serialize(Archive& ar, const unsigned int version)
            {
                ar & classification_result & num_rows;
            }
        };

        struct test_data_struct
        {
        public:
            cv::Mat samples;
            std::uint64_t num_rows;

            test_data_struct()
            {}

            test_data_struct(cv::Mat data, std::uint64_t num):
                    samples(data),
                    num_rows(num)
            {}

        private:
            // Serialization support: even if all of the code below runs on one
            // locality only, we need to provide an (empty) implementation for the
            // serialization as all arguments passed to actions have to support this.
            friend class hpx::serialization::access;

            template <typename Archive>
            void serialize(Archive& ar, const unsigned int version)
            {
                ar & samples & num_rows;
            }
        };

        struct rf_node_data
        {
            public:

                rf_node_data()
                {}

                // Create a new (uninitialized) partition of the given size.
                rf_node_data(std::uint64_t value, std::string filename, algo::randomforest::test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes)
                {
                    id_num = value;
                    std::cout << "beginning execution" << std::endl;
                    result = train_and_predict(filename, test_data, num_samples, num_attributes);
                }

            algo::randomforest::prediction_result_struct train_and_predict(std::string filename, algo::randomforest::test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes)
                {
                    using namespace cv;
                    Mat training_data =Mat(num_samples, num_attributes, CV_32FC1);
                    Mat training_classifications = Mat(num_samples, 1, CV_32FC1);

                    algo::randomforest::read_data_from_csv(filename, training_data, training_classifications, num_samples, num_attributes);

                    Mat var_type = Mat(num_attributes + 1, 1, CV_8U );
                    var_type.setTo(Scalar(CV_VAR_NUMERICAL) ); // all inputs are numerical
                    var_type.at<uchar>(num_attributes, 0) = CV_VAR_CATEGORICAL;

                    float priors[] = {1,1,1,1,1,1,1,1,1,1};   // weights of each classification for classes
                    CvRTParams params = CvRTParams(25, // max depth
                                                   5, // min sample count
                                                   0, // regression accuracy: N/A here
                                                   false, // compute surrogate split, no missing data
                                                   15, // max number of categories (use sub-optimal algorithm for larger numbers)
                                                   priors, // the array of priors
                                                   false,  // calculate variable importance
                                                   4,       // number of variables randomly selected at node and used to find the best split(s).
                                                   100,	 // max number of trees in the forest
                                                   0.01, // forrest accuracy
                                                   CV_TERMCRIT_ITER |	CV_TERMCRIT_EPS // termination criteria
                    );
                    CvRTrees* rtree = new CvRTrees;
                    rtree->train(training_data, CV_ROW_SAMPLE, training_classifications, Mat(), Mat(), var_type, Mat(), params);

                    Mat test_data_class = Mat(test_data.num_rows, 1, CV_32FC1);
                    for (int tsample = 0; tsample < test_data.num_rows; tsample++)
                    {
                        Mat test_sample = test_data.samples.row(tsample);
                        test_data_class.at<float>(tsample, 0) = rtree->predict(test_sample, Mat());
                    }
                    algo::randomforest::prediction_result_struct result(test_data_class, test_data.num_rows);
                    return result;
                }

                rf_node_data(rf_node_data const& base) :
                        id_num(base.id_num),
                        result(base.result)
                {}

            private:
                // Serialization support: even if all of the code below runs on one
                // locality only, we need to provide an (empty) implementation for the
                // serialization as all arguments passed to actions have to support this.
                friend class hpx::serialization::access;

                template <typename Archive>
                void serialize(Archive& ar, const unsigned int version)
                {
                    ar & id_num & result;
                }

            public:
                std::uint64_t id_num;
            algo::randomforest::prediction_result_struct result;
        };
    }
}


#endif //ALGO_RANDOMFOREST_RF_DATASTRUCTURES_CPP