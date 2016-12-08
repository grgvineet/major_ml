//
// Created by vineet on 31/10/16.
//

#ifndef ALGORITHMS_LINEARREG_LINEARREG_CLIENT_H
#define ALGORITHMS_LINEARREG_LINEARREG_CLIENT_H

#include "algorithms/algo_base.h"
#include "utils/data/big_data.h"

#include "algorithms/linearreg/server/linearreg_server.h"

namespace algo {

    namespace linearreg {
        struct linearreg_client : hpx::components::client_base<linearreg_client, server::linearreg_server> {

        public:
            ~linearreg_client();


//        std::vector<int> test(data_frame* test_data);

            typedef hpx::components::client_base<linearreg_client, server::linearreg_server> base_type;

            linearreg_client() {}

//            linearreg_client(hpx::id_type where, int const& k)
//                    : base_type(hpx::new_<server::linearreg_server>(hpx::colocated(where), k)) {}

            // Create new component on locality 'where' and initialize the held data
            linearreg_client(hpx::id_type where, int seed)
                    : base_type(hpx::new_<server::linearreg_server>(where, seed)) {}

            // Attach a future representing a (possibly remote) partition.
            linearreg_client(hpx::future<hpx::id_type> &&id)
                    : base_type(std::move(id)) {}

            // Unwrap a future<partition> (a partition already holds a future to the
            // id of the referenced object, thus unwrapping accesses this inner future).
            linearreg_client(hpx::future<linearreg_client> &&c)
                    : base_type(std::move(c)) {}

            ///////////////////////////////////////////////////////////////////////////
            // Invoke the (remote) member function which gives us access to the data.
            // This is a pure helper function hiding the async.

            hpx::future<std::vector<std::vector<double>>> calculate_x_trans_x(utils::data::data_frame df, int label_col) {
                server::linearreg_server::calculate_x_trans_x_action act;
                return hpx::async(act, get_id(), df, label_col);
            }

            hpx::future<std::vector<double>> calculate_x_trans_y(utils::data::data_frame df, int label_col) {
                server::linearreg_server::calculate_x_trans_y_action act;
                return hpx::async(act, get_id(), df, label_col);
            }

            hpx::future<void> test(utils::data::data_frame labels, utils::data::data_frame df, std::vector<double> theta, int bias_index) {
                server::linearreg_server::test_action act;
                return hpx::async(act, get_id(), labels, df, theta, bias_index);
            }

        };
    }

}



#endif //ALGORITHMS_LINEARREG_LINEARREG_H
