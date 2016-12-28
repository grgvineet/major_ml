#include <opencv2/highgui/highgui.hpp>
#include "opencv2/ml/ml.hpp"
#include <iostream>
#include <fstream>
#include <string>     // std::string, std::stod
#include <stdio.h>
using namespace cv;

#define NUMBER_OF_TRAINING_SAMPLES 4000
#define ATTRIBUTES_PER_SAMPLE 11
#define NUMBER_OF_TESTING_SAMPLES 700
#define NUMBER_OF_CLASSES 10

int read_data_from_csv(const char* filename, Mat trainData, Mat trainClasses, Mat testData, Mat testClasses)
{
    float tmp;
    string str,var;
    std::string::size_type sz;     // alias of size_t
    std::ifstream myfile( filename );
    if( !myfile )
    {
        printf("ERROR: cannot read file %s\n",  filename);
        return 0; // all not OK
    }

    for(int line = 0; line < NUMBER_OF_TRAINING_SAMPLES; line++)
    {
        std::getline(myfile, str);
        std::istringstream ss(str);

        for(int attribute = 0; attribute < ATTRIBUTES_PER_SAMPLE + 1; attribute++)
        {
            if (attribute < ATTRIBUTES_PER_SAMPLE)
            {
                std::getline(ss, var, ',');
                trainData.at<float>(line, attribute) = std::stof (var,&sz);
            }
            else if (attribute == ATTRIBUTES_PER_SAMPLE)
            {
                std::getline(ss, var, ',');
                trainClasses.at<float>(line, 0) =  std::stof (var,&sz);
            }
        }
    }

    for(int line = 0; line < NUMBER_OF_TESTING_SAMPLES; line++)
    {
        std::getline(myfile, str);
        std::istringstream ss(str);

        for(int attribute = 0; attribute < ATTRIBUTES_PER_SAMPLE + 1; attribute++)
        {
            if (attribute < ATTRIBUTES_PER_SAMPLE)
            {
                std::getline(ss, var, ',');
                testData.at<float>(line, attribute) =  std::stof (var,&sz);
            }
            else if (attribute == ATTRIBUTES_PER_SAMPLE)
            {
                std::getline(ss, var, ',');
                testClasses.at<float>(line, 0) = std::stof (var,&sz);
            }
        }
    }
    return 1; // all OK
}

int main( int argc, char** argv )
{
    // lets just check the version first
    printf ("OpenCV version %s (%d.%d.%d)\n",
            CV_VERSION,
            CV_MAJOR_VERSION,
            CV_MINOR_VERSION,
            CV_SUBMINOR_VERSION);

    // define training data storage matrices (one for attribute examples, one
    // for classifications)
    Mat training_data = Mat(NUMBER_OF_TRAINING_SAMPLES, ATTRIBUTES_PER_SAMPLE, CV_32FC1);
    Mat training_classifications = Mat(NUMBER_OF_TRAINING_SAMPLES, 1, CV_32FC1);

    //define testing data storage matrices
    Mat testing_data = Mat(NUMBER_OF_TESTING_SAMPLES, ATTRIBUTES_PER_SAMPLE, CV_32FC1);
    Mat testing_classifications = Mat(NUMBER_OF_TESTING_SAMPLES, 1, CV_32FC1);

    // define all the attributes as numerical
    // alternatives are CV_VAR_CATEGORICAL or CV_VAR_ORDERED(=CV_VAR_NUMERICAL)
    // that can be assigned on a per attribute basis
    Mat var_type = Mat(ATTRIBUTES_PER_SAMPLE + 1, 1, CV_8U );
    var_type.setTo(Scalar(CV_VAR_NUMERICAL) ); // all inputs are numerical

    // this is a classification problem (i.e. predict a discrete number of class
    // outputs) so reset the last (+1) output var_type element to CV_VAR_CATEGORICAL
    var_type.at<uchar>(ATTRIBUTES_PER_SAMPLE, 0) = CV_VAR_CATEGORICAL;

    float result; // value returned from a prediction

    if (read_data_from_csv(argv[1], training_data, training_classifications, testing_data, testing_classifications))
    {
        // define the parameters for training the random forest (trees)
        float priors[] = {1,1,1,1,1,1,1,1,1,1};  // weights of each classification for classes
        // (all equal as equal samples of each digit)

        CvRTParams params = CvRTParams(25, // max depth
                                       5, // min sample count
                                       0, // regression accuracy: N/A here
                                       false, // compute surrogate split, no missing data
                                       15, // max number of categories (use sub-optimal algorithm for larger numbers)
                                       priors, // the array of priors
                                       false,  // calculate variable importance
                                       4,       // number of variables randomly selected at node and used to find the best split(s).
                                       100,	 // max number of trees in the forest
                                       0.01,				// forrest accuracy
                                       CV_TERMCRIT_ITER |	CV_TERMCRIT_EPS // termination criteria
        );

        // train random forest classifier (using training data)

        printf( "\nUsing training database: %s\n\n", argv[1]);
        CvRTrees* rtree = new CvRTrees;

        rtree->train(training_data, CV_ROW_SAMPLE, training_classifications,
                     Mat(), Mat(), var_type, Mat(), params);

        // perform classifier testing and report results
        Mat test_sample;
        int correct_class = 0;
        int wrong_class = 0;
        int false_positives [NUMBER_OF_CLASSES] = {0,0,0,0,0,0,0,0,0,0};

        printf( "\nUsing testing database: %s\n\n", argv[1]);

        for (int tsample = 0; tsample < NUMBER_OF_TESTING_SAMPLES; tsample++)
        {
            test_sample = testing_data.row(tsample);
            result = rtree->predict(test_sample, Mat());

            printf("Testing Sample %i -> class result: %f, expected: %f\n", tsample, result, testing_classifications.at<float>(tsample, 0));

            if (fabs(result - testing_classifications.at<float>(tsample, 0)) >= FLT_EPSILON)
            {
                wrong_class++;
                false_positives[(int) result]++;
            }
            else
            {
                correct_class++;
            }
        }

        printf( "\nResults on the testing database: %s\n"
                        "\tCorrect classification: %d (%g%%)\n"
                        "\tWrong classifications: %d (%g%%)\n",
                argv[1],
                correct_class, (float) correct_class*100/NUMBER_OF_TESTING_SAMPLES,
                wrong_class, (float) wrong_class*100/NUMBER_OF_TESTING_SAMPLES);

        for (int i = 0; i < NUMBER_OF_CLASSES; i++)
        {
            printf( "\tClass (digit %d) false postives 	%d (%g%%)\n", i,
                    false_positives[i],
                    (float) false_positives[i]*100/NUMBER_OF_TESTING_SAMPLES);
        }


        // all matrix memory free by destructors


        // all OK : main returns 0

        return 0;
    }

    // not OK : main returns -1

    return -1;
}
// g++ -ggdb `pkg-config --cflags opencv` -o `basename sample.cpp .cpp` sample.cpp `pkg-config --libs opencv`
