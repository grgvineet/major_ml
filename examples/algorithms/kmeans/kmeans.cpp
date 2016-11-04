//
// Created by vineet on 31/10/16.
//

#include <iostream>

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

#include "utils/data/big_data.h"


int hpx_main(boost::program_options::variables_map& vm)
{
    utils::data::big_data bg("header.csv", true);
    std::cout << bg.get_size();
    return hpx::finalize();
}

int main(int argc, char* argv[]) {
    std::cout << "Hello kmeans" << std::endl;
    return hpx::init(argc, argv);
}
