//
// Created by vineet on 31/10/16.
//

#ifndef ALGORITHMS_KMEANS_KMEANS_H
#define ALGORITHMS_KMEANS_KMEANS_H

#include "algorithms/algo_base.h"
#include "utils/data/big_data.h"

#include "kmeans_client.h"

namespace algo {

    namespace kmeans {
        struct kmeans : algo_base {

        public:
            static const int DEFAULT_MAX_ITER = 10;

        private:
            int _k; // K for kmeans_server
            int _max_iter;
            int _iterations;
            std::vector<kmeans_client> _clients;
            std::vector<std::vector<double>> _cluster_centers;
            utils::data::big_data _clusters;

            // Some book keeping for other functions
            std::vector<std::string> _colnames;
            int _ncols;

            void find_initial_centers(utils::data::big_data &training_data);
            void train(utils::data::big_data &training_data);

        public:
            kmeans(utils::data::big_data &training_data, int k, int max_iter = DEFAULT_MAX_ITER,
                   std::vector<std::vector<double>> cluster_centers = std::vector<std::vector<double>>(0, std::vector<double>(0)),
                   int seed = std::rand());

            ~kmeans();

            static boost::program_options::options_description
            configure_args(int argc, char **argv);

            std::vector<std::vector<double>> cluster_centers() { return _cluster_centers; }
            utils::data::big_data clusters() { return _clusters; }
            int num_clusters() { return _k; }
            int iter() { return _iterations; }

            void print_cluster_centers();

        };
    }
}



#endif //ALGORITHMS_KMEANS_KMEANS_H
