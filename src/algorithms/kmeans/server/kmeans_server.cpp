//
// Created by vineet on 31/10/16.
//

#include "kmeans_server.h"

#include "utils/math/euclidean_distance.h"

namespace algo {

    namespace kmeans {

        namespace server {

            std::vector<std::vector<double>>
            kmeans_server::train(utils::data::data_frame data_frame, std::vector<std::vector<double>> points) {
                int nrows = data_frame.get_size().get();
                int ncols = data_frame.get_ncols().get();

                std::vector<std::vector<double>> res = std::vector<std::vector<double>>(points.size(), std::vector<double>(ncols+1, 0));

                for(int i=0; i<nrows; i++) {
                    std::vector<double> data_row(data_frame.get_row(i).get());
                    double min_dist = DBL_MAX;
                    int min_index;
                    for(int j=0; j<points.size(); j++) {
                        double dist = utils::math::euclid_square(data_row, points[j]);
                        if (dist < min_dist) {
                            min_dist = dist;
                            min_index = j;
                        }
                    }
                    for(int j=0; j<data_row.size(); j++) {
                        res[min_index][j] += data_row[j];
                    }
                    res[min_index][res[min_index].size()-1]++;
                }
                return res;
            }

            std::vector<double>
            kmeans_server::kmeanspp(utils::data::data_frame data_frame, std::vector<std::vector<double>> points) {
                // FIXME : Not according to proper algo, but will work
                double max_distance = DBL_MIN;
                std::vector<double> farthest_point;

                for(int i=0; i<data_frame.get_size().get(); i++) {
                    double this_point_max_distance = 0;
                    std::vector<double> this_row = data_frame.get_row(i).get();
                    for(int j=0; j<points.size(); j++) {
                        double dist = utils::math::euclid_square(this_row, points[j]);
                        this_point_max_distance += dist;
                    }
                    if (this_point_max_distance > max_distance) {
                        max_distance = this_point_max_distance;
                        farthest_point = std::move(this_row);
                    }
                }

                // Piggybacking max distance at farthest point
                farthest_point.push_back(max_distance);
                return farthest_point;
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
