//
// Created by vineet on 31/10/16.
//

#ifndef ALGORITHMS_KMEANS_SERVER_KMEANS_H
#define ALGORITHMS_KMEANS_SERVER_KMEANS_H

#include "algorithms/algo_base.h"
#include "utils/data/data_frame.h"

namespace algo {

    namespace kmeans {
        namespace server {

            struct kmeans_server : hpx::components::component_base<kmeans_server> {

            public:

                // construct new instances
                kmeans_server() {}

                kmeans_server(int k, int max_iter, int seed) :
                        _k(k),
                        _max_iter(max_iter),
                        _seed(seed) {}

                int get_k() const {
                    return _k;
                }

                std::vector<std::vector<double>>
                train(utils::data::data_frame data_frame, std::vector<std::vector<double>> points);

                void store_data_frame_pointer(utils::data::data_frame data_frame);

                void kmeanspp_clear_state(); // Don't think it is required, but still!!
                std::vector<double> kmeanspp(utils::data::data_frame data_frame, std::vector<double> points);
                // Every member function which has to be invoked remotely needs to be
                // wrapped into a component action. The macro below defines a new type
                // 'get_data_action' which represents the (possibly remote) member function
                // partition::get_data().
                HPX_DEFINE_COMPONENT_DIRECT_ACTION(kmeans_server, train, train_action);
                HPX_DEFINE_COMPONENT_DIRECT_ACTION(kmeans_server, kmeanspp_clear_state, kmeanspp_clear_state_action);
                HPX_DEFINE_COMPONENT_DIRECT_ACTION(kmeans_server, kmeanspp, kmeanspp_action);
                HPX_DEFINE_COMPONENT_DIRECT_ACTION(kmeans_server, get_k, get_k_action);

                HPX_DEFINE_COMPONENT_DIRECT_ACTION(kmeans_server, store_data_frame_pointer, store_data_frame_pointer_action);

            private:
                int _k; // K for k-means
                int _max_iter, _seed;

                // FIXME : Remove it from here, dirty hack to improve kmeans++ time complexity
                std::vector<double> _kmeanspp_dist;
                std::shared_ptr<utils::data::server::data_frame_server> _data_frame;

            };
        }
    }

}



#endif //ALGORITHMS_KMEANS_SERVER_KMEANS_H
