#ifndef ALGORITHMS_RANDOMFOREST_RANDOMFOREST_H
#define ALGORITHMS_RANDOMFOREST_RANDOMFOREST_H

#include "../algo_base.h"
#include "rf_client.cpp"
#include "serialise_opencv_mat.h"


namespace algo {

    namespace randomforest {

        struct randomforest : public algo_base {
        public:
            randomforest(int seed = std::rand());

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

            int train_and_predict(std::uint64_t np);

        private:
            int num_training_samples;
            int num_attributes_per_sample;
            int num_testing_samples;
            int num_sample_classes;
            std::string testing_file_path, training_file_path;
        };
    }
}

#endif //ALGORITHMS_RANDOMFOREST_RANDOMFOREST_H
