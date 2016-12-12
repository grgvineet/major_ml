//
// Created by vineet on 31/10/16.
//

#include <iostream>

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

#include "algorithms/kmeans/kmeans.h"

int hpx_main(boost::program_options::variables_map& vm)
{
    utils::data::big_data bg("sample.csv", false);
    std::cerr << bg.get_size() << std::endl;
    hpx::util::high_resolution_timer t;
    algo::kmeans::kmeans kmeans(bg, 5, 25);
    std::cout << "Time taken : " << t.elapsed() << std::endl;
    std::cout << "Iterations done :" << kmeans.iter() << std::endl;
    kmeans.print_cluster_centers();

    kmeans.clusters().write("clusters.csv");
    return hpx::finalize();
}

int main(int argc, char* argv[]) {
    boost::program_options::options_description desc = algo::kmeans::kmeans::configure_args(argc, argv);
    return hpx::init(desc, argc, argv);
}
