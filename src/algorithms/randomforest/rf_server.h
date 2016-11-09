#ifndef ALGO_RANDOMFOREST_SERVER_RFSERVER_CPP
#define ALGO_RANDOMFOREST_SERVER_RFSERVER_CPP

#include "rf_data_structures.cpp"
#include "serialise_opencv_mat.h"
#include <string>

namespace algo{

    namespace randomforest{

        ///////////////////////////////////////////////////////////////////////////////
        // This is the server side representation of the data. We expose this as a HPX
        // component which allows for it to be created and accessed remotely through
        // a global address (hpx::id_type).
        struct rf_server : hpx::components::component_base<algo::randomforest::rf_server>
        {
            rf_server()
            {}

            rf_server(algo::randomforest::rf_node_data value) : data(value)
            {}

            rf_server(std::uint64_t idx, std::string filename, algo::randomforest::test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes) :
                    data(idx, filename, test_data, num_samples, num_attributes)
            {}

            algo::randomforest::prediction_result_struct train_and_predict(std::string filename, algo::randomforest::test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes);

            algo::randomforest::prediction_result_struct get_result() const ;

            // Every member function which has to be invoked remotely needs to be
            // wrapped into a component action. The macro below defines a new type
            // 'get_data_action' which represents the (possibly remote) member function
            // partition::get_data().
            HPX_DEFINE_COMPONENT_DIRECT_ACTION(rf_server, get_result, get_result_action);
            HPX_DEFINE_COMPONENT_DIRECT_ACTION(rf_server, train_and_predict, train_and_predict_action);

        private:
            algo::randomforest::rf_node_data data;

        };
    }
}

#endif // ALGO_RANDOMFOREST_SERVER_RFSERVER_CPP