//
// Created by vineet on 31/10/16.
//

#include "linearreg.h"
#include "linearreg_client.h"

#include "utils/utils.cpp"
#include "utils/math/euclidean_distance.h"

namespace algo {

    namespace  linearreg {

        linearreg::linearreg(int seed) : algo::algo_base(seed){
            _name = "linearreg";

            std::cerr << "Seed is " << _seed << std::endl;
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

            int num_localities = training_data.get_num_data_frames();
            // Initialise client for each node
            std::vector<linearreg_client> clients;
            clients.reserve(num_localities);
            for(int i=0; i<training_data.get_num_data_frames(); i++) {
                clients.push_back(linearreg_client(hpx::get_colocation_id(training_data.get_data_frame(i).get_id()).get(), _seed));
                clients[i].store_data_frame_pointer(training_data.get_data_frame(i)).get();
            }

            // Fetch details about data from data frame on this locality for faster computation
            data_frame this_df = training_data.get_this_data_frame();
            int ncols = this_df.get_ncols().get();

            // Calculate X'X
            std::vector<hpx::future<std::vector<std::vector<double>>>> x_trans_x_fut;
            x_trans_x_fut.reserve(num_localities);
            for(int i=0; i<num_localities; i++) {
                x_trans_x_fut.push_back(clients[i].calculate_x_trans_x(training_data.get_data_frame(i), label_col));
            }
            hpx::wait_all(x_trans_x_fut);

            // Calculate x_trans_x by summing results from all the localities
            std::vector<std::vector<double>> x_trans_x(ncols, std::vector<double>(ncols, 0));
            for(int k=0; k<num_localities; k++) {
                std::vector<std::vector<double>> locality_res(x_trans_x_fut[k].get());
                for(int i=0; i<ncols; i++) {
                    for(int j=0; j<ncols; j++) {
                        x_trans_x[i][j] += locality_res[i][j];
                    }
                }
            }

            // Take inverse of x_trans_x
            std::vector<std::vector<double>> inverse(ncols, std::vector<double>(ncols, 0));
            utils::math::inverse(x_trans_x, inverse);
            // TODO : Check if false returned

            // Compute x_trans_y on all localities
            std::vector<hpx::future<std::vector<double>>> x_trans_y_fut;
            x_trans_y_fut.reserve(num_localities);
            for(int i=0; i<num_localities; i++) {
                x_trans_y_fut.push_back(clients[i].calculate_x_trans_y(training_data.get_data_frame(i), label_col));
            }
            hpx::wait_all(x_trans_y_fut);

            // Calculate x_trans_y from locality result
            std::vector<double> x_trans_y(ncols, 0);
            for (int j=0; j<num_localities; j++) {
                std::vector<double> locality_res(x_trans_y_fut[j].get());
                for(int i=0; i<ncols; i++) {
                    x_trans_y[i] += locality_res[i];
                }
            }

            // Multiply x_trans_x * x_trans_y
            std::vector<double> result(ncols, 0);
            for(int i=0; i<ncols; i++) {
                double temp = 0;
                for(int j=0; j<ncols; j++) {
                    temp += inverse[i][j] * x_trans_y[j];
                }
                result[i] = temp;
            }
            _theta = std::move(result);
            std::cerr << "================ Beta ===================" << std::endl;
            for(int i=0; i<_theta.size(); i++) {
                std::cerr << _theta[i] << std::endl;
            }
        }

        void linearreg::train(utils::data::big_data &training_data, std::string label_colname) {
            int label_col = 0;
            // TODO : Fetch label_col from label_calname
            train(training_data, label_col);
        }

    }
}
