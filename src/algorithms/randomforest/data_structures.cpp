#ifndef ALGORITHMS_RANDOMFOREST_DATA_STRUCTURES_CPP
#define ALGORITHMS_RANDOMFOREST_DATA_STRUCTURES_CPP

#include <iostream>
#include <hpx/hpx.hpp>
#include <opencv2/opencv.hpp>

namespace algo {

    namespace randomforest {

        struct prediction_result_struct
        {
            public:
                cv::Mat classification_result;
                std::uint64_t num_rows;

                prediction_result_struct()
                {}

                prediction_result_struct(cv::Mat result, std::uint64_t num) :
                        classification_result(result),
                        num_rows(num)
                {}

            private:
                // Serialization support: even if all of the code below runs on one
                // locality only, we need to provide an (empty) implementation for the
                // serialization as all arguments passed to actions have to support this.
                friend class hpx::serialization::access;

                template<typename Archive>
                void serialize(Archive &ar, const unsigned int version)
                {
                    ar & classification_result & num_rows;
                }
        };

        struct test_data_struct
        {
            public:
                std::uint64_t train_rows, test_rows, num_attributes;
                std::string training_file_path, testing_file_path;

                test_data_struct(std::uint64_t train_rows, std::uint64_t test_rows,
                                 std::uint64_t num_attributes, std::string training_file_path,
                                 std::string testing_file_path) :
                        train_rows(train_rows),
                        test_rows(test_rows),
                        num_attributes(num_attributes),
                        training_file_path(training_file_path),
                        testing_file_path(testing_file_path)
                {}

                test_data_struct()
                {}

            private:
                // Serialization support: even if all of the code below runs on one
                // locality only, we need to provide an (empty) implementation for the
                // serialization as all arguments passed to actions have to support this.
                friend class hpx::serialization::access;

                template<typename Archive>
                void serialize(Archive &ar, const unsigned int version)
                {
                    ar & train_rows & test_rows & num_attributes & training_file_path & testing_file_path;
                }
        };
    }
}

#endif //ALGORITHMS_RANDOMFOREST_DATA_STRUCTURES_CPP