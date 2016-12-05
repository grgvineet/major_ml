//
// Created by vineet on 31/10/16.
//

#include "linearreg_server.h"

#include "utils/math/euclidean_distance.h"
#include <hpx/runtime/get_ptr.hpp>

namespace algo {

    namespace linearreg {

        namespace server {

            std::vector<std::vector<double>>
            linearreg_server::calculate_x_trans_x(utils::data::data_frame data_frame, int label_col) {
                int nrows = _data_frame->get_size();
                int ncols = _data_frame->get_ncols();

                if (nrows == 0 || ncols == 0) {
                    // Handle the case where data frame is not present on this server
                    // May be entered filename is not present
                    return std::vector<std::vector<double>>(ncols, std::vector<double>(ncols, 0));
                }

                std::vector<std::vector<double>> res;
                res.reserve(ncols);
                std::vector<double> ones(nrows, 1);
                for(int i=0; i<ncols; i++) {
                    std::vector<double> a = _data_frame->get_column(i);
                    if (i == label_col) {
                        a = ones;
                    }
                    std::vector<double> row;
                    row.reserve(ncols);
                    for(int j=0; j<ncols; j++) {
                        std::vector<double> b = _data_frame->get_column(j);
                        if (j == label_col) {
                            b = ones;
                        }
                        double temp = 0;
                        for (int k=0; k<nrows; k++) {
                            temp += a[k]*b[k];
                        }
                        row.push_back(temp);
                    }
                    res.push_back(std::move(row));
                }
                return res;
            }

            std::vector<double>
            linearreg_server::calculate_x_trans_y(utils::data::data_frame data_frame, int label_col) {
                int nrows = _data_frame->get_size();
                int ncols = _data_frame->get_ncols();

                if (nrows == 0 || ncols == 0) {
                    // Handle the case where data frame is not present on this server
                    // May be entered filename is not present
                    return std::vector<double>(ncols, 0);
                }

                std::vector<double> res;
                std::vector<double> labels = _data_frame->get_column(label_col);
                std::vector<double> ones(nrows, 1);

                res.reserve(ncols);
                for(int i=0; i<ncols; i++) {
                    std::vector<double> col = _data_frame->get_column(i);
                    if (i == label_col) {
                        col = ones;
                    }
                    double temp = 0;
                    for(int j=0; j<nrows; j++) {
                        temp += labels[j]*col[j];
                    }
                    res.push_back(temp);
                }
                return res;
            }

            void linearreg_server::store_data_frame_pointer(utils::data::data_frame data_frame) {
                _data_frame =
                        hpx::get_ptr<utils::data::server::data_frame_server>(data_frame.get_gid()).get();

            }
        }
    }
}

typedef hpx::components::component<algo::linearreg::server::linearreg_server> linearreg_server_type;
HPX_REGISTER_COMPONENT(linearreg_server_type, linearreg_server);

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.
typedef algo::linearreg::server::linearreg_server::calculate_x_trans_x_action calculate_x_trans_x_action;
HPX_REGISTER_ACTION(calculate_x_trans_x_action);
typedef algo::linearreg::server::linearreg_server::calculate_x_trans_y_action calculate_x_trans_y_action;
HPX_REGISTER_ACTION(calculate_x_trans_y_action);
typedef algo::linearreg::server::linearreg_server::store_data_frame_pointer_action store_data_frame_pointer_action;
HPX_REGISTER_ACTION(store_data_frame_pointer_action);
