#include <iostream>

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

#include "algorithms/randomforest/randomforest.h"

int hpx_main(boost::program_options::variables_map& vm)
{
    std::uint64_t no_partitions = vm["np"].as<std::uint64_t>();   // Number of partitions.

    algo::randomforest::randomforest rf;
    rf.setParameters(1000, 100, 18, 2);
    rf.setDataFilePaths("susy_test.csv", "susy_train.csv");
    rf.organise_computation(no_partitions);

    return hpx::finalize();
}

int main(int argc, char* argv[]) {
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
            ("np", value<std::uint64_t>()->default_value(1), "Number of partitions");

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}