//
// Created by vineet on 31/10/16.
//

#include "armadillo"
#include "linearreg.h"
#include "linearreg_client.h"

#include "utils/utils.cpp"
#include "utils/math/euclidean_distance.h"

namespace algo {

    namespace  linearreg {

        linearreg::linearreg(int seed) : algo::algo_base(seed){
            _name = "linearreg";
        }

        linearreg::~linearreg() {

        }

        boost::program_options::options_description
        linearreg::configure_args(int argc, char** argv) {
            using namespace boost::program_options;
            options_description description("Allowed options");
            return algo::algo_base::configure_args(description);
        }

        void linearreg::train(utils::data::big_data &training_data, int label_col) {
            using namespace utils::data;

            // TODO : Check is label_col valid
            hpx::util::high_resolution_timer tim;
            int num_localities = training_data.get_num_data_frames();
            // Initialise client for each node

            _clients.reserve(training_data.get_num_data_frames());
            for(int i=0; i<training_data.get_num_data_frames(); i++) {
                hpx::id_type loc_id = hpx::get_colocation_id(hpx::launch::sync, training_data.get_data_frame(i).get_id());
                utils::data::data_frame df(loc_id, 0);
                _train_out.add_data_frame(df);
                _clients.push_back(linearreg_client(loc_id, _seed));
            }
            _train_out.set_this_data_frame_index(training_data.get_this_data_frame_index());

            // Fetch details about data from data frame on this locality for faster computation
            data_frame this_df = training_data.get_this_data_frame();
            int ncols = this_df.get_ncols().get();

            // Calculate X'X
            std::vector<hpx::future<std::vector<std::vector<double>>>> x_trans_x_fut;
            x_trans_x_fut.reserve(num_localities);
            for(int i=0; i<num_localities; i++) {
                x_trans_x_fut.push_back(_clients[i].calculate_x_trans_x(training_data.get_data_frame(i), label_col));
            }
            hpx::wait_all(x_trans_x_fut);

            // Calculate x_trans_x by summing results from all the localities

            arma::mat x_trans_x(ncols, ncols, arma::fill::zeros);
            for(int k=0; k<num_localities; k++) {
                std::vector<std::vector<double>> locality_res(x_trans_x_fut[k].get());
                for(int i=0; i<ncols; i++) {
                    for(int j=0; j<ncols; j++) {
                        x_trans_x(i,j) += locality_res[i][j];
                    }
                }
            }

            // Compute x_trans_y on all localities
            std::vector<hpx::future<std::vector<double>>> x_trans_y_fut;
            x_trans_y_fut.reserve(num_localities);
            for(int i=0; i<num_localities; i++) {
                x_trans_y_fut.push_back(_clients[i].calculate_x_trans_y(training_data.get_data_frame(i), label_col));
            }
            hpx::wait_all(x_trans_y_fut);

            // Calculate x_trans_y from locality result
            arma::vec x_trans_y(ncols, arma::fill::zeros);
            for (int j=0; j<num_localities; j++) {
                std::vector<double> locality_res(x_trans_y_fut[j].get());
                for(int i=0; i<ncols; i++) {
                    x_trans_y(i) += locality_res[i];
                }
            }

            arma::vec coefficients = arma::solve(x_trans_x, x_trans_y);

            std::vector<double> result(ncols, 0);
            for(int i=0; i<ncols; i++) {
                result[i] = coefficients(i);
            }
            _theta = std::move(result);
            _bias_index = label_col;
            std::cerr << "================ Beta ===================" << std::endl;
            for(int i=0; i<_theta.size(); i++) {
                std::cerr << _theta[i] << std::endl;
            }


            // Do post coefficient determination computation
            // This calculates residues and fitted values
            post_training_computation(training_data);
            std::cout << "Total time :" << tim.elapsed() << std::endl;
        }

        void linearreg::train(utils::data::big_data &training_data, std::string label_colname) {
            int label_col = 0;
            // TODO : Fetch label_col from label_calname
            train(training_data, label_col);
        }

        void linearreg::post_training_computation(utils::data::big_data &training_data) {

            int clients_size = _clients.size();
            std::vector<hpx::future<void>> fut;
            fut.reserve(clients_size);
            for(int i=0; i<clients_size; i++) {
                fut.push_back(_clients[i].post_training_computation(training_data.get_data_frame(i)
                                                                    , _train_out.get_data_frame(i)
                                                                    , _theta
                                                                    , _bias_index));
            }
            hpx::wait_all(fut);
        }

        utils::data::big_data linearreg::test(utils::data::big_data test_data) {
            using namespace utils::data;

            if (_clients.empty()) {
                std::cerr << "Not yet trained, labels not generated" << std::endl;
                return big_data();
            }

            std::vector<utils::data::data_frame> label_frames;
            std::vector<hpx::future<void>> fut;
            for(int i=0; i<_clients.size(); i++) {
                utils::data::data_frame labels(hpx::get_colocation_id(_clients[i].get_id()).get(), 0);
                fut.push_back(_clients[i].test(labels, test_data.get_data_frame(i), _theta, _bias_index));
                label_frames.push_back(labels);
            }
            hpx::wait_all(fut);

            return big_data(std::move(label_frames));
        }

    }
}
