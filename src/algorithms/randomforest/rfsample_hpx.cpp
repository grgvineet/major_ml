#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/ml/ml.hpp"
#include <hpx/runtime/serialization/serialize.hpp>
#include <iostream>
#include <vector>

#define NUMBER_OF_TRAINING_SAMPLES 4000
#define ATTRIBUTES_PER_SAMPLE 11
#define NUMBER_OF_TESTING_SAMPLES 700
#define NUMBER_OF_CLASSES 10

inline std::size_t locidx(std::size_t i, std::size_t np, std::size_t nl)
{
    return i / (np/nl);
}

struct prediction_result_struct{

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

struct test_data_struct{
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

void read_data_from_csv(std::string filename, cv::Mat data, cv::Mat classes, std::uint64_t num_samples, std::uint64_t num_attributes)
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

namespace hpx
{
    namespace serialization
    {
        void serialize(hpx::serialization::input_archive& ar, cv::Mat& m, int version)
        {
            // load
            int cols, rows;
            size_t elem_size, elem_type;

            ar >> cols;
            ar >> rows;
            ar >> elem_size;
            ar >> elem_type;

            m.create(rows, cols, elem_type);

            size_t data_size = m.cols * m.rows * elem_size;
            ar >> hpx::serialization::make_array(m.ptr(), data_size);
        }

        void serialize(hpx::serialization::output_archive& ar, cv::Mat& m, int version)
        {
            // normal save ...
            size_t elem_size = m.elemSize(), elem_type = m.type();

            ar << m.cols;
            ar << m.rows;
            ar << elem_size;
            ar << elem_type;

            const size_t data_size = m.cols * m.rows * elem_size;
            ar << hpx::serialization::make_array(m.ptr(), data_size);
        }
    }
}

struct rf_node_data
{
    public:

        rf_node_data()
        {}

        // Create a new (uninitialized) partition of the given size.
        rf_node_data(std::uint64_t value, std::string filename, test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes)
        {
            id_num = value;
            std::cout << "beginning execution" << std::endl;
            result = train_and_predict(filename, test_data, num_samples, num_attributes);
        }

        prediction_result_struct train_and_predict(std::string filename, test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes)
        {
            using namespace cv;
            Mat training_data =Mat(num_samples, num_attributes, CV_32FC1);
            Mat training_classifications = Mat(num_samples, 1, CV_32FC1);

            read_data_from_csv(filename, training_data, training_classifications, num_samples, num_attributes);

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
        prediction_result_struct result;
};

///////////////////////////////////////////////////////////////////////////////
// This is the server side representation of the data. We expose this as a HPX
// component which allows for it to be created and accessed remotely through
// a global address (hpx::id_type).
struct rf_server : hpx::components::component_base<rf_server>
{
    rf_server()
    {}

    rf_server(rf_node_data value) : data(value)
    {}

    rf_server(std::uint64_t idx, std::string filename, test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes) :
            data(idx, filename, test_data, num_samples, num_attributes)
    {}

    prediction_result_struct train_and_predict(std::string filename, test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes)
    {
        return data.train_and_predict(filename, test_data, num_samples, num_attributes);
    }

    prediction_result_struct get_result() const
    {
        return data.result;
    }

    // Every member function which has to be invoked remotely needs to be
    // wrapped into a component action. The macro below defines a new type
    // 'get_data_action' which represents the (possibly remote) member function
    // partition::get_data().
    HPX_DEFINE_COMPONENT_DIRECT_ACTION(rf_server, get_result, get_result_action);
    HPX_DEFINE_COMPONENT_DIRECT_ACTION(rf_server, train_and_predict, train_and_predict_action);

private:
    rf_node_data data;

};

// The macros below are necessary to generate the code required for exposing
// our partition type remotely.
//
// HPX_REGISTER_COMPONENT() exposes the component creation
// through hpx::new_<>().
typedef hpx::components::component<rf_server> rf_server_type;
HPX_REGISTER_COMPONENT(rf_server_type, rf_server);

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.
typedef rf_server::get_result_action get_result_action;
HPX_REGISTER_ACTION(get_result_action);

typedef rf_server::train_and_predict_action train_and_predict_action;
HPX_REGISTER_ACTION(train_and_predict_action);


///////////////////////////////////////////////////////////////////////////////////////
// This is a client side helper class allowing to hide some of the tedious boilerplate.
struct rf_client : hpx::components::client_base<rf_client, rf_server>
{
    typedef hpx::components::client_base<rf_client, rf_server> base_type;

    rf_client() {}

    rf_client(hpx::id_type where, rf_node_data initial_value)
            : base_type(hpx::new_<rf_server>(hpx::colocated(where), initial_value))
    {}

    // Create new component on locality 'where' and initialize the held data
    rf_client(hpx::id_type where, std::uint64_t value, std::string filename, test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes)
            : base_type(hpx::new_<rf_server>(where, value, filename, test_data, num_samples, num_attributes))
    {}

    // Attach a future representing a (possibly remote) partition.
    rf_client(hpx::future<hpx::id_type> && id)
            : base_type(std::move(id))
    {}

    // Unwrap a future<partition> (a partition already holds a future to the
    // id of the referenced object, thus unwrapping accesses this inner future).
    rf_client(hpx::future<rf_client> && c)
            : base_type(std::move(c))
    {}

    ///////////////////////////////////////////////////////////////////////////
    // Invoke the (remote) member function which gives us access to the data.
    // This is a pure helper function hiding the async.
    hpx::future<prediction_result_struct> get_result() const
    {
        rf_server::get_result_action act;
        return hpx::async(act, get_id());
    }

    hpx::future<prediction_result_struct> train_and_predict(std::string filename, test_data_struct data, std::uint64_t num_samples, std::uint64_t num_attributes) const
    {
        rf_server::train_and_predict_action act;
        return hpx::async(act, get_id(), filename, data, num_samples, num_attributes);
    }
};

///////////////////////////////////////////////////////////////////////////////
int hpx_main(boost::program_options::variables_map& vm)
{
    std::uint64_t np = vm["np"].as<std::uint64_t>();   // Number of partitions.

    std::vector<hpx::id_type> localities = hpx::find_all_localities();
    std::size_t nl = localities.size();                // Number of localities

    if (np < nl)
    {
        std::cout << "The number of partitions should not be smaller than the number of localities" << std::endl;
        return hpx::finalize();
    }

    // Measure execution time.
    std::uint64_t t = hpx::util::high_resolution_clock::now();

    std::vector<rf_client>current(np);
    std::vector<hpx::future<prediction_result_struct> >next(np);

    //define testing data storage matrices
    test_data_struct test_data;
    cv::Mat testing_classifications;
    {
        using namespace cv;

        Mat testing_data = Mat(NUMBER_OF_TESTING_SAMPLES, ATTRIBUTES_PER_SAMPLE, CV_32FC1);
        testing_classifications = Mat(NUMBER_OF_TESTING_SAMPLES, 1, CV_32FC1);

        read_data_from_csv("file5.csv", testing_data, testing_classifications, NUMBER_OF_TESTING_SAMPLES, ATTRIBUTES_PER_SAMPLE);
        test_data.samples = testing_data;
        test_data.num_rows = NUMBER_OF_TESTING_SAMPLES;
    }

    std::string filename = "file.csv";
    // Initial conditions: f(0, i) = i
    for (std::size_t i = 0; i != np; ++i)
    {
        current[i] = rf_client(localities[locidx(i, np, nl)],
                               std::uint64_t(i+1),
                               filename,
                               test_data,
                               NUMBER_OF_TRAINING_SAMPLES,
                               ATTRIBUTES_PER_SAMPLE);
    }

    hpx::wait_all(current);
    for (std::size_t i = 0; i != np; ++i)
    {
        next[i] = current[i].get_result();
    }

    hpx::wait_all(next);
    for (std::size_t i = 0; i != np; ++i)
    {
        prediction_result_struct result = next[i].get();

        std::cout << "For locality" << i <<", results:" << std::endl;
        int correct_class = 0;
        int wrong_class = 0;

        for (int tsample = 0; tsample < NUMBER_OF_TESTING_SAMPLES; tsample++)
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
                correct_class, (float) correct_class*100/NUMBER_OF_TESTING_SAMPLES,
                wrong_class, (float) wrong_class*100/NUMBER_OF_TESTING_SAMPLES);
    }

    std::uint64_t elapsed = hpx::util::high_resolution_clock::now() - t;

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
            ("np", value<std::uint64_t>()->default_value(4), "Number of partitions")
            ( "no-header", "do not print out the csv header row");

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}

//g++ -o `basename rfsample_hpx.cpp .cpp` rfsample_hpx.cpp  `pkg-config --cflags --libs opencv hpx_application_debug opencv`  -lhpx_iostreamsd -DHPX_APPLICATION_NAME=rfsample_hpx