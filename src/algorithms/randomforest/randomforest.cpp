#include "randomforest.h"
#include <vector>
#include <iostream>
#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/runtime/serialization/serialize.hpp>
#include <hpx/runtime/serialization/array.hpp>

algo::randomforest::randomforest::randomforest(int seed) : algo::algo_base(seed)
{
    testing_file_path = "test_file.csv";
    training_file_path = "train_file.csv";
}

algo::randomforest::randomforest::~randomforest()
{}

int algo::randomforest::randomforest::getNum_training_samples() const {
    return num_training_samples;
}

int algo::randomforest::randomforest::getNum_attributes_per_sample() const {
    return num_attributes_per_sample;
}

void algo::randomforest::randomforest::setNum_attributes_per_sample(int num_attributes_per_sample) {
    randomforest::num_attributes_per_sample = num_attributes_per_sample;
}

int algo::randomforest::randomforest::getNum_testing_samples() const {
    return num_testing_samples;
}

void algo::randomforest::randomforest::setNum_testing_samples(int num_testing_samples) {
    randomforest::num_testing_samples = num_testing_samples;
}

int algo::randomforest::randomforest::getNum_sample_classes() const {
    return num_sample_classes;
}

void algo::randomforest::randomforest::setNum_sample_classes(int num_sample_classes) {
    randomforest::num_sample_classes = num_sample_classes;
}

void algo::randomforest::randomforest::setNum_training_samples(int num_training_samples) {
    randomforest::num_training_samples = num_training_samples;
}

void algo::randomforest::randomforest::setParameters(int training_samples, int testing_samples, int attributes, int classes)
{
    num_training_samples = training_samples;
    num_attributes_per_sample = attributes;
    num_testing_samples = testing_samples;
    num_sample_classes = classes;
}

int algo::randomforest::randomforest::train_and_predict(std::uint64_t np) {

    std::vector<hpx::id_type> localities = hpx::find_all_localities();
    std::size_t nl = localities.size();                // Number of localities

    if (np < nl)
    {
        std::cout << "The number of partitions should not be smaller than the number of localities" << std::endl;
        return hpx::finalize();
    }

    // Measure execution time.
    std::uint64_t t = hpx::util::high_resolution_clock::now();

    std::vector<algo::randomforest::rf_client>current(np);
    std::vector<hpx::future<algo::randomforest::prediction_result_struct> >next(np);

    //define testing data storage matrices
    algo::randomforest::test_data_struct test_data;
    cv::Mat testing_classifications;
    {
        using namespace cv;

        Mat testing_data = Mat(num_testing_samples, num_attributes_per_sample, CV_32FC1);
        testing_classifications = Mat(num_testing_samples, 1, CV_32FC1);

        algo::randomforest::read_data_from_csv(testing_file_path, testing_data, testing_classifications, num_testing_samples, num_attributes_per_sample);
        test_data.samples = testing_data;
        test_data.num_rows = num_testing_samples;
    }

    for (std::size_t i = 0; i != np; ++i)
    {
        current[i] = algo::randomforest::rf_client(localities[algo::randomforest::locidx(i, np, nl)],
                               std::uint64_t(i+1),
                               training_file_path,
                               test_data,
                               num_training_samples,
                               num_attributes_per_sample);
    }

    hpx::wait_all(current);
    for (std::size_t i = 0; i != np; ++i)
    {
        next[i] = current[i].get_result();
    }

    hpx::wait_all(next);
    for (std::size_t i = 0; i != np; ++i)
    {
        algo::randomforest::prediction_result_struct result = next[i].get();

        std::cout << "For locality" << i <<", results:" << std::endl;
        int correct_class = 0;
        int wrong_class = 0;

        for (int tsample = 0; tsample < num_testing_samples; tsample++)
        {
            if (fabs(result.classification_result.at<float>(tsample, 0)
                     - testing_classifications.at<float>(tsample, 0)) >= FLT_EPSILON)
            {
                wrong_class++;
            }
            else
            {
                correct_class++;
            }
        }

        printf( "\tCorrect classification: %d (%g%%)\n\tWrong classifications: %d (%g%%)\n",
                correct_class, (float) correct_class*100/num_testing_samples,
                wrong_class, (float) wrong_class*100/num_testing_samples);
    }

    std::uint64_t elapsed = hpx::util::high_resolution_clock::now() - t;

    return hpx::finalize();

}


