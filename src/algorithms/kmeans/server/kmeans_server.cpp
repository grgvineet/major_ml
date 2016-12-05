//
// Created by vineet on 31/10/16.
//

#include "kmeans_server.h"

#include "utils/math/euclidean_distance.h"
#include <hpx/runtime/get_ptr.hpp>

namespace algo {

    namespace kmeans {

        namespace server {

            std::vector<std::vector<double>>
            kmeans_server::train(utils::data::data_frame data_frame, std::vector<std::vector<double>> points) {
                utils::data::server::data_frame_server* _data_frame =
                        reinterpret_cast<utils::data::server::data_frame_server*>(data_frame.get_local_ptr().get());
                int nrows = _data_frame->get_size();
                int ncols = _data_frame->get_ncols();

                if (nrows == 0 || ncols == 0) {
                    // Handle the case where data frame is not present on this server
                    // May be entered filename is not present
                    return std::vector<std::vector<double>>(points.size(), std::vector<double>(ncols+1, 0));
                }

                std::vector<std::vector<double>> res = std::vector<std::vector<double>>(points.size(), std::vector<double>(ncols+1, 0));

                for(int i=0; i<nrows; i++) {
                    // FIXME : Optimise this, as data frame resides on this locality the following can be optimised,
                    // FIXME : i.e if raw ptr is used
                    std::vector<double> data_row(_data_frame->get_row(i));
                    double min_dist = DBL_MAX;
                    int min_index;
                    int points_size = points.size();
                    for(int j=0; j<points_size; j++) {
                        double dist = utils::math::euclid_square(data_row, points[j]);
                        if (dist < min_dist) {
                            min_dist = dist;
                            min_index = j;
                        }
                    }
                    int data_row_size = data_row.size();
                    for(int j=0; j<data_row_size; j++) {
                        res[min_index][j] += data_row[j];
                    }
                    res[min_index].back()++;
                }
                return res;
            }

            std::vector<double>
            kmeans_server::kmeanspp(utils::data::data_frame data_frame, std::vector<double> point) {
                // FIXME : Not according to proper algo, but will work

                utils::data::server::data_frame_server* _data_frame =
                        reinterpret_cast<utils::data::server::data_frame_server*>(data_frame.get_local_ptr().get());

                int nrows =_data_frame->get_size();
                int ncols = _data_frame->get_ncols();

                if (nrows == 0 || ncols == 0) {
                    // Handle the case where data frame is not present on this server
                    // May be entered filename is not present
                    // Cannot find a farther point, thus return source point itself with distance zero
                    point.push_back(0);
                    return point;
                }

                if (_kmeanspp_dist.empty()) {
                    // Should initialise _kmeanspp_dist with zero
                    _kmeanspp_dist.reserve(nrows);
                }

                double max_distance = DBL_MIN;
                std::vector<double> farthest_point;

                for(int i=0; i<nrows; i++) {
                    // FIXME : Optimise this, as data frame resides on this locality the following can be optimised,
                    // FIXME : i.e if raw ptr is used
//                    std::vector<double> this_row = data_frame.get_row(i).get();
                    std::vector<double> this_row(_data_frame->get_row(i));
                    double dist = utils::math::euclid_square(this_row, point);
                    _kmeanspp_dist[i] += dist;
                    if (_kmeanspp_dist[i] > max_distance) {
                        max_distance = _kmeanspp_dist[i];
                        farthest_point = std::move(this_row);
                    }
                }

                // Piggybacking max distance at farthest point
                farthest_point.push_back(max_distance);
                return farthest_point;
            }

            void kmeans_server::kmeanspp_clear_state() {
                _kmeanspp_dist.clear();
            }
        }
    }
}

typedef hpx::components::component<algo::kmeans::server::kmeans_server> kmeans_server_type;
HPX_REGISTER_COMPONENT(kmeans_server_type, kmeans_server);

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.
typedef algo::kmeans::server::kmeans_server::train_action train_action;
HPX_REGISTER_ACTION(train_action);
typedef algo::kmeans::server::kmeans_server::kmeanspp_action kmeanspp_action;
HPX_REGISTER_ACTION(kmeanspp_action);
typedef algo::kmeans::server::kmeans_server::kmeanspp_clear_state_action kmeanspp_clear_state_action;
HPX_REGISTER_ACTION(kmeanspp_clear_state_action);
