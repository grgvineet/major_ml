#include "randomforest.h"

algo::randomforest::randomforest::randomforest()
{}

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

void algo::randomforest::randomforest::setDataFilePaths(std::string test_file_path, std::string train_file_path)
{
    testing_file_path = test_file_path;
    training_file_path = train_file_path;
}

int algo::randomforest::randomforest::organise_computation(std::uint64_t num_partitions)
{
    std::vector<hpx::id_type> localities = hpx::find_all_localities();
    std::size_t nl = localities.size();                // Number of localities

    if (num_partitions < nl)
    {
        std::cout << "The number of partitions should not be smaller than the number of localities" << std::endl;
        return hpx::finalize();
    }

    std::vector<hpx::future<prediction_result_struct> >next(num_partitions);

    //define testing data storage matrices
    test_data_struct test_data;
    test_data.test_rows = num_testing_samples;
    test_data.train_rows = num_training_samples;
    test_data.num_attributes = num_attributes_per_sample;
    test_data.testing_file_path = testing_file_path;
    test_data.training_file_path = training_file_path;

    train_and_predict_action predict;

    hpx::id_type temp = localities[0];
    localities[0] = localities[num_partitions-1];
    localities[num_partitions-1] = temp;

    std::cout << "Beginning computation" << std::endl;

    for (std::size_t i = 0; i != num_partitions; ++i)
    {
        next[i] = hpx::async(predict,
                             localities[locidx(i, num_partitions, nl)],
                             test_data);
    }

    std::cout << "Work has been distributed, waiting for results" << std::endl;

    int classifications[num_testing_samples+2][2] = {0};

    hpx::wait_all(next);
    for (std::size_t i = 0; i != num_partitions; ++i)
    {
        prediction_result_struct result = next[i].get();

        for (int tsample = 0; tsample < num_testing_samples; tsample++)
        {
            if (fabs(result.classification_result.at<float>(tsample, 0) - 0.00) >= FLT_EPSILON)
                classifications[tsample][1]++;
            else
                classifications[tsample][0]++;
        }
    }

    {
        using namespace cv;

        Mat testing_data = Mat(num_testing_samples, num_attributes_per_sample, CV_32FC1);
        Mat testing_classifications = Mat(num_testing_samples, 1, CV_32FC1);

        read_data_from_csv(testing_file_path, testing_data, testing_classifications, num_testing_samples, num_attributes_per_sample);
        int correct_class = 0;
        int wrong_class = 0;

        for (int tsample = 0; tsample < num_testing_samples; tsample++)
        {
            if (fabs((float)(classifications[tsample][1] >= classifications[tsample][0] ? 1 : 0)
                     - testing_classifications.at<float>(tsample, 0)) >= FLT_EPSILON)
            {
                wrong_class++;
            }
            else
            {
                correct_class++;
            }
        }
        printf( "Results computed:\n\tCorrect classifications: %d (%g%%)\n\tWrong classifications: %d (%g%%)\n",
                correct_class, (float) correct_class*100/num_testing_samples,
                wrong_class, (float) wrong_class*100/num_testing_samples);
    }

    return hpx::finalize();
}

HPX_REGISTER_ACTION(algo::randomforest::train_and_predict_action, train_and_predict_action);

