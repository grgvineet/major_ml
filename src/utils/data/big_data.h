//
// Created by vineet on 4/11/16.
//

#ifndef UTILS_DATA_BIG_DATA_H
#define UTILS_DATA_BIG_DATA_H

#include "data_frame.h"

namespace utils {

    namespace data {

        struct big_data {

            big_data();
            big_data(int ncols);
            big_data(const std::string& path, const bool header = false);
            big_data(std::vector<data_frame>& df);
            big_data(std::vector<data_frame>&& df);

            void read_data(const std::string& path, const bool header = false);

            int get_size();
            int get_num_data_frames() const;
            data_frame& get_data_frame(int index);

            void add_data_frame(utils::data::data_frame& df);
            void set_this_data_frame_index(int index);
            int get_this_data_frame_index() { return _this_data_frame_index; }

            data_frame& get_this_data_frame(); // return data_frame on this locality

            void write(std::string filename);
            // TODO :: Add method to apply an action

        private:
            std::vector<data_frame> _data_frames;
            int _this_data_frame_index; // To optimise general queries about data from data on this node
        };
    }
}



#endif //UTILS_DATA_BIG_DATA_H
