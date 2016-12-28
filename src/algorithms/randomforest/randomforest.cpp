#include "randomforest.h"
#include <vector>
#include <iostream>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <hpx/include/thread_executors.hpp>

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

int algo::randomforest::randomforest::organise_computation(std::uint64_t no_partitions) {

    std::vector<hpx::id_type> localities = hpx::find_all_localities();
    std::size_t nl = localities.size();                // Number of localities

    if (no_partitions < nl)
    {
        std::cout << "The number of partitions should not be smaller than the number of localities" << std::endl;
        return hpx::finalize();
    }

    std::vector<hpx::future<prediction_result_struct> >next(no_partitions);

    //define testing data storage matrices
    test_data_struct test_data;
    test_data.test_rows = num_testing_samples;
    test_data.train_rows = num_training_samples;
    test_data.num_attributes = num_attributes_per_sample;
    test_data.testing_file_path = testing_file_path;
    test_data.training_file_path = training_file_path;

    train_and_predict_action predict;
    hpx::id_type temp = localities[0];
    localities[0] = localities[no_partitions-1];
    localities[no_partitions-1] = temp;

    for (std::size_t i = 0; i != no_partitions; ++i)
    {
        next[i] = hpx::async(predict,
                             localities[locidx(i, no_partitions, nl)],
                             test_data);
    }
    int classifications[num_testing_samples+2][2] = {0};

    hpx::wait_all(next);
    for (std::size_t i = 0; i != no_partitions; ++i)
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
        printf( "\tCorrect classification: %d (%g%%)\n\tWrong classifications: %d (%g%%)\n",
                correct_class, (float) correct_class*100/num_testing_samples,
                wrong_class, (float) wrong_class*100/num_testing_samples);
    }

    return hpx::finalize();
}

algo::randomforest::prediction_result_struct algo::randomforest::train_and_predict(algo::randomforest::test_data_struct test_data)
{
    using namespace cv;

    Mat testing_data = Mat(test_data.test_rows, test_data.num_attributes, CV_32FC1);
    Mat testing_classifications = Mat(test_data.test_rows, 1, CV_32FC1);
    read_data_from_csv(test_data.testing_file_path, testing_data, testing_classifications, test_data.test_rows, test_data.num_attributes);

    Mat training_data =Mat(test_data.train_rows, test_data.num_attributes, CV_32FC1);
    Mat training_classifications = Mat(test_data.train_rows, 1, CV_32FC1);
    read_data_from_csv(test_data.training_file_path, training_data, training_classifications, test_data.train_rows, test_data.num_attributes);

    Mat var_type = Mat(test_data.num_attributes + 1, 1, CV_8U );
    var_type.setTo(Scalar(CV_VAR_NUMERICAL) ); // all inputs are numerical
    var_type.at<uchar>(test_data.num_attributes, 0) = CV_VAR_CATEGORICAL;

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

    Mat test_data_class = Mat(test_data.test_rows, 1, CV_32FC1);
    for (int tsample = 0; tsample < test_data.test_rows; tsample++)
    {
        Mat test_sample = testing_data.row(tsample);
        test_data_class.at<float>(tsample, 0) = rtree->predict(test_sample, Mat());
    }
    prediction_result_struct result(test_data_class, test_data.test_rows);
    return result;
}

void algo::randomforest::read_data_from_csv(std::string filename, cv::Mat data, cv::Mat classes, std::uint64_t num_samples, std::uint64_t num_attributes)
{
    float tmp;
    std::string str,var;
    std::string::size_type sz;     // alias of size_t
    std::ifstream myfile( filename );
    if( !myfile )
    {
        printf("ERROR: cannot read file %s\n",  filename.c_str());
        return ; // all not OK
    }

    boost::char_separator<char> delimiter(",");
    for(int line = 0; line < num_samples; line++)
    {
        std::getline(myfile, str);
        boost::tokenizer<boost::char_separator<char>> tokenizer(str, delimiter);
        int attribute = -1;

        BOOST_FOREACH(std::string aux, tokenizer)
        {
            if(attribute >= 0)
            {
                std::sscanf(aux.data(), "%f", &(data.at<float>(line, attribute)));
            }
            else
            {
                std::sscanf(aux.data(), "%f", &(classes.at<float>(line, 0)));
            }
            attribute++;
        }
    }
    return ; // all OK
}

std::size_t  algo::randomforest::locidx(std::size_t i, std::size_t np, std::size_t nl)
{
    return i / (np/nl);
}

HPX_REGISTER_ACTION(algo::randomforest::train_and_predict_action, train_and_predict_action);



