//
// Created by vineet on 31/10/16.
//

#include <iostream>

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

#include "algorithms/linearreg/linearreg.h"

int hpx_main(boost::program_options::variables_map& vm)
{
    utils::data::big_data bg("million.csv", false);
    std::cerr << bg.get_size() << std::endl;
    algo::linearreg::linearreg linearreg;
    linearreg.train(bg, 1); // 0 based indexing
    return hpx::finalize();
}

int main(int argc, char* argv[]) {
    boost::program_options::options_description desc = algo::linearreg::linearreg::configure_args(argc, argv);
    return hpx::init(desc, argc, argv);
}
