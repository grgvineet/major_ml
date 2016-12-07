//
// Created by vineet on 2/11/16.
//

#ifndef UTILS_DATA_DATA_FRAME_H
#define UTILS_DATA_DATA_FRAME_H

#include <vector>
#include <map>

#include "data_row.h"

#include "utils/data/server/data_frame_server.h"

namespace utils {

    namespace data {

        struct data_frame : public hpx::components::client_base<data_frame, utils::data::server::data_frame_server>{

            typedef utils::data::data_row data_row;

        public:

            typedef hpx::components::client_base<data_frame, server::data_frame_server> base_type;

            data_frame() {}

            // Create new component on locality 'where' and initialize the held data
            data_frame(hpx::id_type where, int ncols)
            : base_type(hpx::new_<server::data_frame_server>(where, ncols))
            {}

            data_frame(hpx::id_type where, std::string path, bool header)
                    : base_type(hpx::new_<server::data_frame_server>(where, path, header))
            {}

//            // Create a new component on the locality co-located to the id 'where'. The
//            // new instance will be initialized from the given partition_data.
//            data_frame(hpx::id_type where, int const& ncols)
//            : base_type(hpx::new_<server::data_frame_server>(hpx::colocated(where), ncols))
//            {}

            // Attach a future representing a (possibly remote) partition.
            data_frame(hpx::future<hpx::id_type> && id)
            : base_type(std::move(id))
            {}

            // Unwrap a future<partition> (a partition already is a future to the
            // id of the referenced object, thus unwrapping accesses this inner future).
            data_frame(hpx::future<data_frame> && c)
            : base_type(std::move(c))
            {}

            // FIXME : Use some hpx defined method
            // I can't seem to find any hpx method to get local ptr, one that is preset causes the program to crash
            hpx::future<std::uint64_t> get_local_ptr() {
                utils::data::server::data_frame_server::get_local_ptr_action act;
                return hpx::async(act, get_id());
            }

            hpx::future<void> print(int rows = -1) const
            {
                utils::data::server::data_frame_server::print_action act;
                return hpx::async(act, get_id(), rows);
            }

            hpx::future<int> get_size() const {
                utils::data::server::data_frame_server::get_size_action act;
                return hpx::async(act, get_id());
            }

            hpx::future<int> get_ncols() const {
                utils::data::server::data_frame_server::get_ncols_action act;
                return hpx::async(act, get_id());
            }

            hpx::future<std::vector<double>> get_row(int index) {
                utils::data::server::data_frame_server::get_row_action act;
                return hpx::async(act, get_id(), index);
            }

            hpx::future<void> write(std::string path) {
                utils::data::server::data_frame_server::write_action act;
                return hpx::async(act, get_id(), path);
            }

        };

    }
}



#endif //UTILS_DATA_DATA_FRAME_H
