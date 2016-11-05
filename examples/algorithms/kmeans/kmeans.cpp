//
// Created by vineet on 31/10/16.
//

#include <iostream>

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

#include "algorithms/kmeans/kmeans.h"

int hpx_main(boost::program_options::variables_map& vm)
{
    utils::data::big_data bg("cat_data.csv", false);
    std::cout << bg.get_size();
    algo::kmeans::kmeans kmeans(3);
    kmeans.train(bg);
    return hpx::finalize();
}

int main(int argc, char* argv[]) {
    algo::kmeans::kmeans::configure_args(argc, argv);
    return hpx::init(argc, argv);
}
