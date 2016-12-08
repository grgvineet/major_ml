//
// Created by vineet on 31/10/16.
//

#include "kmeans.h"
#include "kmeans_client.h"

#include "utils/utils.cpp"
#include "utils/math/euclidean_distance.h"

algo::kmeans::kmeans::kmeans(utils::data::big_data &training_data,
                             int k,
                             int max_iter,
                             std::vector<std::vector<double>> cluster_centers, int seed)
        : algo::algo_base(seed){
    _name = "kmeans";
    _k = k;
    _max_iter = max_iter;

    if (cluster_centers.empty()) {
        _cluster_centers = std::move(cluster_centers);
    } else {
        _cluster_centers.reserve(k);
    }
    train(training_data);
}

algo::kmeans::kmeans::~kmeans() {

}

boost::program_options::options_description
algo::kmeans::kmeans::configure_args(int argc, char** argv) {
    using namespace boost::program_options;
    options_description description("Allowed options");
    description.add_options()
            ("k", value<int>(), "Number of clusters to form")
            ("max-iter", value<int>()->default_value(10),"Max number of iterations");
    return algo::algo_base::configure_args(description);
}

void algo::kmeans::kmeans::find_initial_centers(utils::data::big_data &training_data) {

    // Fetch details about data from data frame on this locality for faster computation
    utils::data::server::data_frame_server* this_df =
            reinterpret_cast<utils::data::server::data_frame_server*>(training_data.get_this_data_frame().get_local_ptr().get());
    int ncols = this_df->get_ncols();
    // Initialise K random points from data set, this is done using kMeans++ algorithm
    int this_num_rows = this_df->get_size();
    _cluster_centers.push_back(this_df->get_row(std::rand()%this_num_rows));
    hpx::util::high_resolution_timer t; t.restart();
    for(int i=0; i<_k-1; i++) {
        std::vector<hpx::future<std::vector<double>>> points_future;
        points_future.reserve(_clients.size());
        for(int j=0; j<_clients.size(); j++) {
            points_future.push_back(_clients[j].kmeanspp(training_data.get_data_frame(j), _cluster_centers[i]));
        }
        hpx::wait_all(points_future);
        double distance = DBL_MIN;
        std::vector<double> farthest_point;
        int points_future_size = points_future.size();
        for(int j=0; j<points_future_size; j++) {
            std::vector<double> temp(points_future[j].get());
            double temp_distance = temp.back();
            temp.pop_back();
            if (temp_distance > distance) {
                distance = temp_distance;
                farthest_point = std::move(temp);
            }
        }
        _cluster_centers.push_back(std::move(farthest_point));
    }
    std::cerr << "kMeans++ time :" << t.elapsed() << std::endl;
    // K random points initialise, print
//    std::cout << "Selected k random points are " << std::endl;
//    for(int i=0; i<_k; i++) {
//        for(int j=0; j<ncols; j++) {
//            std::cout << _cluster_centers[i][j] <<  " ";
//        }
//        std::cout << std::endl;
//    }
    ////////////////////////////////////////////////////
}

void algo::kmeans::kmeans::train(utils::data::big_data &training_data) {
    using namespace utils::data;

    // Initialise client for each node and make big data object for cluster computed
    _clients.reserve(training_data.get_num_data_frames());
    for(int i=0; i<training_data.get_num_data_frames(); i++) {
        hpx::id_type loc_id = hpx::get_colocation_id(hpx::launch::sync, training_data.get_data_frame(i).get_id());
        utils::data::data_frame df(loc_id, 0);
        _clusters.add_data_frame(df);
        _clients.push_back(kmeans_client(loc_id, _k, _max_iter, _seed));
    }
    _clusters.set_this_data_frame_index(training_data.get_this_data_frame_index());

    // Fetch details about data from data frame on this locality for faster computation
    utils::data::server::data_frame_server* this_df =
            reinterpret_cast<utils::data::server::data_frame_server*>(training_data.get_this_data_frame().get_local_ptr().get());
    int ncols = this_df->get_ncols();

    // Setting book keeping variables
    _ncols = ncols;
    _colnames = this_df->get_colnames();

    // Find cluster centers
    if (_cluster_centers.empty()) {
        find_initial_centers(training_data);
    }

    // Run kmeans until converged
    for(_iterations = 0; _iterations < _max_iter; _iterations++) {
        typedef std::vector<std::vector<double>> double2d;
        std::vector<hpx::future<double2d>> fut_points;
        fut_points.reserve(_clients.size());
        for (int i = 0; i < _clients.size(); i++) {
            fut_points.push_back(_clients[i].train(training_data.get_data_frame(i), _clusters.get_data_frame(i), _cluster_centers));
        }
        hpx::wait_all(fut_points);
        double2d new_points(_k, std::vector<double>(ncols+1, 0));
        for(int i=0; i<fut_points.size(); i++) {
            double2d loc_points(fut_points[i].get());
            for(int j=0; j<_k; j++) {
                for(int k=0; k<ncols; k++) {
                    new_points[j][k] += loc_points[j][k];
                }
                new_points[j].back() += loc_points[j].back();
            }
        }
        for(int i=0; i<_k; i++) {
            for(int j=0; j<ncols; j++) {
                new_points[i][j] /= new_points[i].back();
            }
            new_points[i].pop_back();
        }
        if (utils::math::is_converged(_cluster_centers, new_points)) {
            break;
        } else {
            _cluster_centers = new_points;
        }
    }

}

void algo::kmeans::kmeans::print_cluster_centers() {
    std::cerr << "================ Cluster centers ================" << std::endl;
    for(int j=0; j<_ncols; j++) {
        std::cout.width(10); std::cout <<  std::right << _colnames[j] ;
    }
    std::cout << std::endl;

    for(int i=0; i<_k; i++) {
        for(int j=0; j<_ncols; j++) {
            std::cout.width(10); std::cout <<  std::right << _cluster_centers[i][j];
        }
        std::cout << std::endl;
    }
}

