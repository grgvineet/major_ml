//
// Created by vineet on 31/10/16.
//

#ifndef ALGORITHMS_LINEARREG_LINEARREG_H
#define ALGORITHMS_LINEARREG_LINEARREG_H

#include "algorithms/algo_base.h"
#include "utils/data/big_data.h"

#include "linearreg_client.h"

namespace algo {

    namespace linearreg {
        struct linearreg : algo_base {

        private:
            int label_col;
            std::string label_colname;

            std::vector<double> _theta;
            int _bias_index;

            std::vector<linearreg_client> clients;

        public:
            linearreg(int seed = std::rand());

            ~linearreg();

            static boost::program_options::options_description
            configure_args(int argc, char **argv);

            void train(utils::data::big_data &training_data, int label_col);
            void train(utils::data::big_data &training_data, std::string label_colname);

            utils::data::big_data test(utils::data::big_data test_data);
        };
    }
}



#endif //ALGORITHMS_LINEARREG_LINEARREG_H
