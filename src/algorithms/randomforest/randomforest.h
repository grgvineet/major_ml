#ifndef ALGORITHMS_RANDOMFOREST_RANDOMFOREST_H
#define ALGORITHMS_RANDOMFOREST_RANDOMFOREST_H

#include "serialise_opencv_mat.h"
#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/runtime/serialization/serialize.hpp>
#include <hpx/runtime/serialization/array.hpp>
#include <hpx/include/actions.hpp>


namespace algo {

    namespace randomforest {

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
            std::uint64_t train_rows, test_rows, num_attributes;
            std::string training_file_path, testing_file_path;

            test_data_struct(std::uint64_t train_rows, std::uint64_t test_rows,
                             std::uint64_t num_attributes, std::string training_file_path, std::string testing_file_path):
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

            template <typename Archive>
            void serialize(Archive& ar, const unsigned int version)
            {
                ar & train_rows & test_rows & num_attributes & training_file_path & testing_file_path;
            }
        };

        struct randomforest{
        public:
            randomforest();

            ~randomforest();

            int getNum_training_samples() const;

            void setNum_training_samples(int num_training_samples);

            int getNum_attributes_per_sample() const;

            void setNum_attributes_per_sample(int num_attributes_per_sample);

            int getNum_testing_samples() const;

            void setNum_testing_samples(int num_testing_samples);

            int getNum_sample_classes() const;

            void setNum_sample_classes(int num_sample_classes);

            void setParameters(int training_samples, int testing_samples, int attributes, int classes);

            void setDataFilePaths(std::string test_file_path, std::string train_file_path);

            int organise_computation(std::uint64_t np);

        private:
            int num_training_samples;
            int num_attributes_per_sample;
            int num_testing_samples;
            int num_sample_classes;
            std::string testing_file_path, training_file_path;

        };

        algo::randomforest::prediction_result_struct train_and_predict(algo::randomforest::test_data_struct test_data);

        void read_data_from_csv(std::string filename, cv::Mat data, cv::Mat classes, std::uint64_t num_samples, std::uint64_t num_attributes);

        std::size_t locidx(std::size_t i, std::size_t np, std::size_t nl);

        HPX_DEFINE_PLAIN_ACTION(train_and_predict, train_and_predict_action);
    }
}

#endif //ALGORITHMS_RANDOMFOREST_RANDOMFOREST_H
