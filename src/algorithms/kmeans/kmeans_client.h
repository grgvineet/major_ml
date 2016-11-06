//
// Created by vineet on 31/10/16.
//

#ifndef ALGORITHMS_KMEANS_KMEANS_CLIENT_H
#define ALGORITHMS_KMEANS_KMEANS_CLIENT_H

#include "algorithms/algo_base.h"
#include "utils/data/big_data.h"

#include "server/kmeans_server.h"

namespace algo {

    namespace kmeans {
        struct kmeans_client : hpx::components::client_base<kmeans_client, server::kmeans_server> {

            int _k; // K for kmeans_server

        public:
            ~kmeans_client();


//        std::vector<int> test(data_frame* test_data);

            typedef hpx::components::client_base<kmeans_client, server::kmeans_server> base_type;

            kmeans_client() {}

//            kmeans_client(hpx::id_type where, int const& k)
//                    : base_type(hpx::new_<server::kmeans_server>(hpx::colocated(where), k)) {}

            // Create new component on locality 'where' and initialize the held data
            kmeans_client(hpx::id_type where, int k, int max_iter, int seed)
                    : base_type(hpx::new_<server::kmeans_server>(where, k, max_iter, seed)) {}

            // Attach a future representing a (possibly remote) partition.
            kmeans_client(hpx::future<hpx::id_type> &&id)
                    : base_type(std::move(id)) {}

            // Unwrap a future<partition> (a partition already holds a future to the
            // id of the referenced object, thus unwrapping accesses this inner future).
            kmeans_client(hpx::future<kmeans_client> &&c)
                    : base_type(std::move(c)) {}

            ///////////////////////////////////////////////////////////////////////////
            // Invoke the (remote) member function which gives us access to the data.
            // This is a pure helper function hiding the async.
            hpx::future<int> get_k() const {
                server::kmeans_server::get_k_action act;
                return hpx::async(act, get_id());
            }

            hpx::future<std::vector<std::vector<double>>> train(utils::data::data_frame df, std::vector<std::vector<double>> points) {
                server::kmeans_server::train_action act;
                return hpx::async(act, get_id(), df, points);
            }

            hpx::future<std::vector<double>> kmeanspp(utils::data::data_frame df, std::vector<double> point) {
                server::kmeans_server::kmeanspp_action act;
                return hpx::async(act, get_id(), df, point);
            }

            hpx::future<void> kmeanspp_clear_state() {
                server::kmeans_server::kmeanspp_clear_state_action act;
                return hpx::async(act, get_id());
            }

            hpx::future<void> store_data_frame_pointer(utils::data::data_frame df) {
                server::kmeans_server::store_data_frame_pointer_action act;
                return hpx::async(act, get_id(), df);
            }

        };
    }

}



#endif //ALGORITHMS_KMEANS_KMEANS_H
