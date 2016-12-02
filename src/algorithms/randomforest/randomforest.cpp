#include "randomforest.h"
#include <vector>
#include <iostream>

HPX_REGISTER_ACTION(algo::randomforest::algo_randomforest_make_prediction_action, my_action);

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

    std::vector<hpx::future<prediction_result_struct> >next(np);

    //define testing data storage matrices
    test_data_struct test_data;
    cv::Mat testing_classifications;
    {
        using namespace cv;

        Mat testing_data = Mat(num_testing_samples, num_attributes_per_sample, CV_32FC1);
        testing_classifications = Mat(num_testing_samples, 1, CV_32FC1);

        algo::randomforest::read_data_from_csv(testing_file_path, testing_data, testing_classifications, num_testing_samples, num_attributes_per_sample);
        test_data.samples = testing_data;
        test_data.num_rows = num_testing_samples;
    }

    algo_randomforest_make_prediction_action predict;
    for (std::size_t i = 0; i != np; ++i)
    {
        next[i] = hpx::async(predict,
                             localities[algo::randomforest::locidx(i, np, nl)],
                             training_file_path,
                             test_data,
                             num_training_samples,
                             num_attributes_per_sample);
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
    std::cout << "Elapsed time: " << elapsed << std::endl;

    return hpx::finalize();

}

algo::randomforest::prediction_result_struct algo::randomforest::make_prediction(std::string filename, algo::randomforest::test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes)
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
    prediction_result_struct result(test_data_class, test_data.num_rows);
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

    for(int line = 0; line < num_samples; line++)
    {
        std::getline(myfile, str);
        std::istringstream ss(str);

        for(int attribute = 0; attribute < num_attributes + 1; attribute++)
        {
            if (attribute < num_attributes)
            {
                std::getline(ss, var, ',');
                data.at<float>(line, attribute) = std::stof (var,&sz);
            }
            else if (attribute == num_attributes)
            {
                std::getline(ss, var, ',');
                classes.at<float>(line, 0) =  std::stof (var,&sz);
            }
        }
    }
    return ; // all OK
}

std::size_t  algo::randomforest::locidx(std::size_t i, std::size_t np, std::size_t nl)
{
    return i / (np/nl);
}


