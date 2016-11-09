#ifndef ALGO_RANDOMFOREST_SERVER_RFCLIENT_CPP
#define ALGO_RANDOMFOREST_SERVER_RFCLIENT_CPP

#include <hpx/hpx.hpp>
#include "rf_server.h"
#include "rf_data_structures.cpp"
#include "serialise_opencv_mat.h"
#include <string>

namespace algo{

    namespace randomforest{

        ///////////////////////////////////////////////////////////////////////////////////////
        // This is a client side helper class allowing to hide some of the tedious boilerplate.
        struct rf_client : hpx::components::client_base<rf_client, algo::randomforest::rf_server>
        {
            typedef hpx::components::client_base<rf_client, algo::randomforest::rf_server> base_type;

            rf_client() {}

            rf_client(hpx::id_type where, algo::randomforest::rf_node_data initial_value)
                    : base_type(hpx::new_<algo::randomforest::rf_server>(hpx::colocated(where), initial_value))
            {}

            // Create new component on locality 'where' and initialize the held data
            rf_client(hpx::id_type where, std::uint64_t value, std::string filename, algo::randomforest::test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes)
                    : base_type(hpx::new_<algo::randomforest::rf_server>(where, value, filename, test_data, num_samples, num_attributes))
            {}

            // Attach a future representing a (possibly remote) partition.
            rf_client(hpx::future<hpx::id_type> && id)
                    : base_type(std::move(id))
            {}

            // Unwrap a future<partition> (a partition already holds a future to the
            // id of the referenced object, thus unwrapping accesses this inner future).
            rf_client(hpx::future<algo::randomforest::rf_client> && c)
                    : base_type(std::move(c))
            {}

            ///////////////////////////////////////////////////////////////////////////
            // Invoke the (remote) member function which gives us access to the data.
            // This is a pure helper function hiding the async.
            hpx::future<algo::randomforest::prediction_result_struct> get_result() const
            {
                algo::randomforest::rf_server::get_result_action act;
                return hpx::async(act, get_id());
            }

            hpx::future<algo::randomforest::prediction_result_struct> train_and_predict(std::string filename, algo::randomforest::test_data_struct data, std::uint64_t num_samples, std::uint64_t num_attributes) const
            {
                algo::randomforest::rf_server::train_and_predict_action act;
                return hpx::async(act, get_id(), filename, data, num_samples, num_attributes);
            }
        };
    }
}


#endif  //ALGO_RANDOMFOREST_SERVER_RFCLIENT_CPP