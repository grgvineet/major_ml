//
// Created by vineet on 31/10/16.
//

#ifndef ALGORITHMS_LINEARREG_LINEARREG_H
#define ALGORITHMS_LINEARREG_LINEARREG_H

#include "algorithms/algo_base.h"
#include "utils/data/big_data.h"

#include "algorithms/linearreg/server/linearreg_server.h"

namespace algo {

    namespace linearreg {
        struct linearreg : algo_base {

        private:
            int label_col;
            std::string label_colname;
            std::vector<double> _theta;

        public:
            linearreg(int seed = std::rand());

            ~linearreg();

            static boost::program_options::options_description
            configure_args(int argc, char **argv);

            void train(utils::data::big_data &training_data, int label_col);
            void train(utils::data::big_data &training_data, std::string label_colname);
//        std::vector<int> test(data_frame* test_data);

        };
    }
}



#endif //ALGORITHMS_LINEARREG_LINEARREG_H
