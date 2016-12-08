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

                utils::data::server::data_frame_server* _data_frame =
                        reinterpret_cast<utils::data::server::data_frame_server*>(data_frame.get_local_ptr().get());

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

                utils::data::server::data_frame_server* _data_frame =
                        reinterpret_cast<utils::data::server::data_frame_server*>(data_frame.get_local_ptr().get());

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

            void linearreg_server::test(utils::data::data_frame labels, utils::data::data_frame data_frame,
                                        std::vector<double> theta, int bias_index) {

                utils::data::server::data_frame_server* test_data =
                        reinterpret_cast<utils::data::server::data_frame_server*>(data_frame.get_local_ptr().get());

                utils::data::server::data_frame_server* labels_data =
                        reinterpret_cast<utils::data::server::data_frame_server*>(labels.get_local_ptr().get());

                int nrows = test_data->get_size();
                int ncols = test_data->get_ncols();

                std::vector<double> labels_value;
                labels_value.reserve(nrows);

//                if (ncols != theta.size()-1) {
//                    // FIXME : For now we assume that at columns will be used in training, but it might be not the case
//                    return;
//                }

                for(int i=0; i<nrows; i++) {
                    std::vector<double> row = test_data->get_row(i);
                    double val = 0;
                    for(int j=0, k=0; j<theta.size(); j++) {
                        if (j == bias_index) {
                            val += theta[j];
                            continue;
                        }
                        val += row[j]*theta[k];
                        k++;
                    }
                    labels_value.push_back(val);
                }
                labels_data->insert_column(labels_value, "labels");
            }
        }
    }
}

typedef hpx::components::component<algo::linearreg::server::linearreg_server> linearreg_server_type;
HPX_REGISTER_COMPONENT(linearreg_server_type, linearreg_server);

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.
HPX_REGISTER_ACTION(algo::linearreg::server::linearreg_server::calculate_x_trans_x_action);
HPX_REGISTER_ACTION(algo::linearreg::server::linearreg_server::calculate_x_trans_y_action);
HPX_REGISTER_ACTION(algo::linearreg::server::linearreg_server::test_action);

