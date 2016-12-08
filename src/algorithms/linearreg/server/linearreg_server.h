//
// Created by vineet on 31/10/16.
//

#ifndef ALGORITHMS_LINEARREG_SERVER_LINEARREG_H
#define ALGORITHMS_LINEARREG_SERVER_LINEARREG_H

#include "algorithms/algo_base.h"
#include "utils/data/data_frame.h"

namespace algo {

    namespace linearreg {
        namespace server {

            struct linearreg_server : hpx::components::component_base<linearreg_server> {

            public:

                // construct new instances
                linearreg_server() {}

                linearreg_server(int seed) :
                        _seed(seed) {}


                std::vector<std::vector<double>>
                calculate_x_trans_x(utils::data::data_frame data_frame, int label_col);

                std::vector<double>
                calculate_x_trans_y(utils::data::data_frame data_frame, int label_col);

                void
                test(utils::data::data_frame labels, utils::data::data_frame data_frame, std::vector<double> theta, int _bias_index);

                // Every member function which has to be invoked remotely needs to be
                // wrapped into a component action. The macro below defines a new type
                // 'get_data_action' which represents the (possibly remote) member function
                // partition::get_data().
                HPX_DEFINE_COMPONENT_DIRECT_ACTION(linearreg_server, calculate_x_trans_x, calculate_x_trans_x_action);
                HPX_DEFINE_COMPONENT_DIRECT_ACTION(linearreg_server, calculate_x_trans_y, calculate_x_trans_y_action);
                HPX_DEFINE_COMPONENT_DIRECT_ACTION(linearreg_server, test, test_action);

            private:
                int _seed;

            };
        }
    }

}



#endif //ALGORITHMS_LINEARREG_SERVER_LINEARREG_H
