//
// Created by vineet on 31/10/16.
//

#include "kmeans.h"
#include "kmeans_client.h"

#include "utils/utils.cpp"
#include "utils/math/euclidean_distance.h"

algo::kmeans::kmeans::kmeans(int k, int max_iter, int seed) : algo::algo_base(seed){
    _name = "kmeans";
    _k = k;
    _max_iter = max_iter;

    std::cerr << "Seed is " << _seed << std::endl;
    _points.reserve(k);
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

void algo::kmeans::kmeans::train(utils::data::big_data &training_data) {
    using namespace utils::data;

    // Initialise client for each node
    std::vector<kmeans_client> clients;
    clients.reserve(training_data.get_num_data_frames());
    for(int i=0; i<training_data.get_num_data_frames(); i++) {
        clients.push_back(kmeans_client(hpx::get_colocation_id(training_data.get_data_frame(i).get_id()).get(), _k, _max_iter, _seed));
        clients[i].store_data_frame_pointer(training_data.get_data_frame(i)).get();
    }

    // Fetch details about data from data frame on this locality for faster computation
    data_frame this_df = training_data.get_this_data_frame();
    int ncols = this_df.get_ncols().get();

    // Initialise K random points from data set, this is done using kMeans++ algorithm
    int this_num_rows = this_df.get_size().get();
    _points.push_back(this_df.get_row(std::rand()%this_num_rows).get());
    hpx::util::high_resolution_timer t; t.restart();
    for(int i=0; i<_k-1; i++) {
        std::vector<hpx::future<std::vector<double>>> points_future;
        points_future.reserve(clients.size());
        for(int j=0; j<clients.size(); j++) {
            points_future.push_back(clients[j].kmeanspp(training_data.get_data_frame(j), _points[i]));
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
        _points.push_back(std::move(farthest_point));
    }
    std::cerr << "kMeans++ time :" << t.elapsed() << std::endl;
    // K random points initialise, print
//    std::cout << "Selected k random points are " << std::endl;
//    for(int i=0; i<_k; i++) {
//        for(int j=0; j<ncols; j++) {
//            std::cout << _points[i][j] <<  " ";
//        }
//        std::cout << std::endl;
//    }
    ////////////////////////////////////////////////////

    // Run kmeans until converged

    for(int iterations = 0; iterations < _max_iter; iterations++) {
        typedef std::vector<std::vector<double>> double2d;
        std::vector<hpx::future<double2d>> fut_points;
        fut_points.reserve(clients.size());
        for (int i = 0; i < clients.size(); i++) {
            fut_points.push_back(clients[i].train(training_data.get_data_frame(i), _points));
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
        if (utils::math::is_converged(_points, new_points)) {
            break;
        } else {
            _points = new_points;
        }
    }
    std::cerr << "Final cluster centers " << std::endl;
    for(int i=0; i<_k; i++) {
        for(int j=0; j<ncols; j++) {
            std::cout << _points[i][j] <<  " ";
        }
        std::cout << std::endl;
    }
}

