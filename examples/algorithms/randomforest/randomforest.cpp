#include <iostream>

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

#include "algorithms/randomforest/randomforest.h"

int hpx_main(boost::program_options::variables_map& vm)
{
    std::uint64_t no_localities = vm["nl"].as<std::uint64_t>();   // Number of partitions.

    algo::randomforest::randomforest rf(3);
    rf.setParameters(1125000, 500000, 18, 2);
    rf.train_and_predict(no_localities);

    return hpx::finalize();
}

int main(int argc, char* argv[]) {
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
            ("nl", value<std::uint64_t>()->default_value(1), "Number of localities");

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}