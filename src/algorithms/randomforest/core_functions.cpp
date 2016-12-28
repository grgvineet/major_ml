#include "core_functions.h"

std::size_t algo::randomforest::locidx(std::size_t i, std::size_t np, std::size_t nl)
{
    return i / (np / nl);
}

void algo::randomforest::read_data_from_csv(std::string filename,
                                            cv::Mat data,
                                            cv::Mat classes,
                                            std::uint64_t num_samples,
                                            std::uint64_t num_attributes)
{
    float tmp;
    std::string str, var;
    std::string::size_type sz;     // alias of size_t
    std::ifstream myfile(filename);
    if (!myfile)
    {
        printf("ERROR: cannot read file %s\n", filename.c_str());
        return; // all not OK
    }

    boost::char_separator<char> delimiter(",");
    for (int line = 0; line < num_samples; line++)
    {
        std::getline(myfile, str);
        boost::tokenizer<boost::char_separator<char>> tokenizer(str, delimiter);
        int attribute = -1;

        BOOST_FOREACH(std::string aux, tokenizer)
        {
            if (attribute >= 0) {
                std::sscanf(aux.data(), "%f", &(data.at<float>(line, attribute)));
            } else {
                std::sscanf(aux.data(), "%f", &(classes.at<float>(line, 0)));
            }
            attribute++;
        }
    }
    return; // all OK
}

algo::randomforest::prediction_result_struct
    algo::randomforest::train_and_predict(algo::randomforest::test_data_struct test_data)
{
    using namespace cv;

    std::cout << "Loading test and train data" << std::endl;

    Mat testing_data = Mat(test_data.test_rows, test_data.num_attributes, CV_32FC1);
    Mat testing_classifications = Mat(test_data.test_rows, 1, CV_32FC1);
    algo::randomforest::read_data_from_csv(test_data.testing_file_path,
                                           testing_data,
                                           testing_classifications,
                                           test_data.test_rows,
                                           test_data.num_attributes);

    Mat training_data = Mat(test_data.train_rows, test_data.num_attributes, CV_32FC1);
    Mat training_classifications = Mat(test_data.train_rows, 1, CV_32FC1);
    algo::randomforest::read_data_from_csv(test_data.training_file_path,
                                           training_data,
                                           training_classifications,
                                           test_data.train_rows,
                                           test_data.num_attributes);

    Mat var_type = Mat(test_data.num_attributes + 1, 1, CV_8U);
    var_type.setTo(Scalar(CV_VAR_NUMERICAL)); // all inputs are numerical
    var_type.at<uchar>(test_data.num_attributes, 0) = CV_VAR_CATEGORICAL;

    float priors[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};   // weights of each classification for classes
    CvRTParams params = CvRTParams(25, // max depth
                                   5, // min sample count
                                   0, // regression accuracy: N/A here
                                   false, // compute surrogate split, no missing data
                                   15, // max number of categories (use sub-optimal algorithm for larger numbers)
                                   priors, // the array of priors
                                   false,  // calculate variable importance
                                   4,       // number of variables randomly selected at node and used to find the best split(s).
                                   100,     // max number of trees in the forest
                                   0.01, // forrest accuracy
                                   CV_TERMCRIT_ITER | CV_TERMCRIT_EPS // termination criteria
    );
    CvRTrees *rtree = new CvRTrees;
    rtree->train(training_data, CV_ROW_SAMPLE, training_classifications, Mat(), Mat(), var_type, Mat(), params);

    std::cout << "Training process has completed, initiating prediction process" << std::endl;
    Mat test_data_class = Mat(test_data.test_rows, 1, CV_32FC1);
    for (int tsample = 0; tsample < test_data.test_rows; tsample++)
    {
        Mat test_sample = testing_data.row(tsample);
        test_data_class.at<float>(tsample, 0) = rtree->predict(test_sample, Mat());
    }
    algo::randomforest::prediction_result_struct result(test_data_class, test_data.test_rows);

    std::cout << "Computation ends successfully, sending back results" << std::endl;
    return result;
}