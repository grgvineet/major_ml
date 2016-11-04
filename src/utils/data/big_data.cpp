//
// Created by vineet on 4/11/16.
//

#include "big_data.h"

namespace utils {

    namespace data {

        big_data::big_data() {

        }

        big_data::big_data(const std::string &path, const bool header) {
            read_data(path, header);
        }

        void big_data::read_data(const std::string &path, const bool header) {
            std::vector<hpx::naming::id_type> localities = hpx::find_all_localities();
            for(int i=0; i<localities.size(); i++) {
                _data_frames.push_back(data_frame(localities[i], path, header));
            }
        }

        int big_data::get_size() {
            int answer = 0;
            std::vector<hpx::future<int> > futures;
            for(int i=0; i<_data_frames.size(); i++) {
                futures.push_back(_data_frames[i].get_size());
            }
            hpx::wait_all(futures);
            for(int i=0; i<futures.size(); i++) {
                answer += futures[i].get();
            }
            return answer;
        }
    }
}
