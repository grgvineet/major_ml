//
// Created by vineet on 31/10/16.
//

#ifndef ALGORITHMS_KMEANS_KMEANS_H
#define ALGORITHMS_KMEANS_KMEANS_H

#include "algorithms/algo_base.h"
#include "utils/data/big_data.h"

#include "server/kmeans_server.h"

namespace algo {

    namespace kmeans {
        struct kmeans : algo_base {

        private:
            int _k; // K for kmeans_server
            std::vector<std::vector<double>> _points;

        public:
            kmeans(int k);

            ~kmeans();

            static boost::program_options::options_description
            configure_args(int argc, char **argv);

            void train(utils::data::big_data &training_data);
//        std::vector<int> test(data_frame* test_data);

        };
    }
}



#endif //ALGORITHMS_KMEANS_KMEANS_H
