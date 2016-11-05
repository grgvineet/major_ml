//
// Created by vineet on 31/10/16.
//

#include "algo_base.h"

boost::program_options::options_description
algo::algo_base::configure_args(boost::program_options::options_description &description) {
    using namespace boost::program_options;
    description.add_options()
            ("training-data", value<std::string>(), "File path for training data")
            ("test-data", value<std::string>(),"File path for test data")
            ("help","Print this help");
    return description;
}
