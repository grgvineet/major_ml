//
// Created by vineet on 4/11/16.
//

#ifndef BIG_DATA_H
#define BIG_DATA_H

#include "data_frame.h"

namespace utils {

    namespace data {

        struct big_data {

            big_data();
            big_data(const std::string& path, const bool header = false);

            void read_data(const std::string& path, const bool header = false);

            int get_size();

            // TODO :: Add method to apply an action

        private:
            std::vector<data_frame> _data_frames;
        };
    }
}



#endif //BIG_DATA_H
