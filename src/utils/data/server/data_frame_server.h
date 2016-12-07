//
// Created by vineet on 2/11/16.
//

#ifndef UTILS_DATA_SERVER_DATA_FRAME_H
#define UTILS_DATA_SERVER_DATA_FRAME_H

#include <vector>
#include <map>
#include <hpx/include/actions.hpp>
#include <hpx/runtime/components/server/component_base.hpp>
#include <hpx/hpx.hpp>

#include "utils/data/data_row.h"

namespace utils {

    namespace data { namespace server {

        struct data_frame_server : hpx::components::component_base<data_frame_server> {

            typedef utils::data::data_row data_row;

            data_frame_server();
            explicit data_frame_server(int ncols);
            explicit data_frame_server(std::string path, bool header = false);

            // To be used only if data frame is accessed from same locality
            std::uint64_t get_local_ptr();

            void set_col_names(std::vector<std::string>& colnames);
            bool remove_row(int index);
            bool insert_row(data_row dr);
            bool insert_row(std::vector<double>& row);

            bool insert_column(std::vector<double>& col);
            bool insert_column(std::vector<double>& col, std::string colname);
            bool remove_column(const int index);
            bool remove_column(const std::string& colname);

            std::vector<double> get_row(int index);

            data_row operator[](const int index); // Return row
            data_row operator[](const std::string& colname);

            std::vector<double>& get_column(int index);


            int get_size() const;
            int get_ncols() const;
            void print(int rows = -1);
            void print_col_names();

            int get_nrows_from_file(std::string& path, bool header = false);

            void write(std::string path);

            ///////////////////////////////////////////////////////////////////////
            // Every member function which has to be invoked remotely needs to be
            // wrapped into a component action. The macro below defines a new type
            // 'get_data_action' which represents the (possibly remote) member function
            // partition::get_data().
            HPX_DEFINE_COMPONENT_DIRECT_ACTION(data_frame_server, get_local_ptr, get_local_ptr_action);
            HPX_DEFINE_COMPONENT_DIRECT_ACTION(data_frame_server, get_ncols, get_ncols_action);
            HPX_DEFINE_COMPONENT_DIRECT_ACTION(data_frame_server, remove_row, remove_row_action);
            HPX_DEFINE_COMPONENT_DIRECT_ACTION(data_frame_server, print, print_action);
            HPX_DEFINE_COMPONENT_DIRECT_ACTION(data_frame_server, get_size, get_size_action);
            HPX_DEFINE_COMPONENT_DIRECT_ACTION(data_frame_server, get_row, get_row_action);
            HPX_DEFINE_COMPONENT_DIRECT_ACTION(data_frame_server, write, write_action);

        private:

            int _ncols;
            std::vector<std::vector<double> > _data;
            std::map<std::string, int> _colnames_index;
            std::vector<std::string> _colnames;

            void initialise_colnames(int ncols);
            void initialise_colnames(std::vector<std::string>& colnames);
        };

    }}
}



#endif //UTILS_DATA_SERVER_DATA_FRAME_H
