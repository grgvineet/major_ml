#include "rf_server.h"

algo::randomforest::prediction_result_struct
algo::randomforest::rf_server::train_and_predict(std::string filename, algo::randomforest::test_data_struct test_data, std::uint64_t num_samples, std::uint64_t num_attributes)
{
    return data.train_and_predict(filename, test_data, num_samples, num_attributes);
}

algo::randomforest::prediction_result_struct
algo::randomforest::rf_server::get_result() const
{
    return data.result;
}


// The macros below are necessary to generate the code required for exposing
// our partition type remotely.
//
// HPX_REGISTER_COMPONENT() exposes the component creation
// through hpx::new_<>().
typedef hpx::components::component<algo::randomforest::rf_server> rf_server_type;
HPX_REGISTER_COMPONENT(rf_server_type, rf_server);

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.
typedef algo::randomforest::rf_server::get_result_action get_result_action;
HPX_REGISTER_ACTION(get_result_action);

typedef algo::randomforest::rf_server::train_and_predict_action train_and_predict_action;
HPX_REGISTER_ACTION(train_and_predict_action);
