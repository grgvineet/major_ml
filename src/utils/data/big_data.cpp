//
// Created by vineet on 4/11/16.
//

#include "big_data.h"

namespace utils {

    namespace data {

        big_data::big_data() {

        }

        big_data::big_data(std::vector<data_frame> &df) {
            _data_frames = df;
        }

        big_data::big_data(std::vector<data_frame> &&df) {
            _data_frames = df;
        }

        big_data::big_data(int ncols) {
            std::vector<hpx::naming::id_type> localities = hpx::find_all_localities();
            _data_frames.reserve(localities.size());
            for(int i=0; i<localities.size(); i++) {
                if (localities[i] == hpx::find_here()) {
//                    _this_data_frame_index = i;
                    continue;
                }
                _data_frames.push_back(data_frame(localities[i], ncols));
            }
            _data_frames.push_back(data_frame(hpx::find_here(), ncols));
            _this_data_frame_index = _data_frames.size() - 1;
        }

        big_data::big_data(const std::string &path, const bool header) {
            read_data(path, header);
        }

        void big_data::read_data(const std::string &path, const bool header) {
            std::vector<hpx::naming::id_type> localities = hpx::find_all_localities();
            _data_frames.reserve(localities.size());
            for(int i=0; i<localities.size(); i++) {
                if (localities[i] == hpx::find_here()) {
//                    _this_data_frame_index = i;
                    continue;
                }
                _data_frames.push_back(data_frame(localities[i], path, header));

            }
            _data_frames.push_back(data_frame(hpx::find_here(), path, header));
            _this_data_frame_index = _data_frames.size()-1;
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

        int big_data::get_num_data_frames() const{
            return _data_frames.size();
        }

        data_frame& big_data::get_data_frame(int index) {
            if (_data_frames.empty()) {
                std::cerr << __PRETTY_FUNCTION__ << " : data frames not yet initialised" << std::endl;
                //FIXME : Return something else
                return _data_frames[0];
            }
            if (index < 0 || index >= _data_frames.size()) {
                std::cerr << __PRETTY_FUNCTION__ << " : Index " << index << " out of bound" << std::endl;
                return _data_frames[0];
            }
            return _data_frames[index];
        }

        data_frame& big_data::get_this_data_frame() {
            if (_data_frames.empty()) {
                std::cerr << __PRETTY_FUNCTION__ << " : data frames not yet initialised" << std::endl;
                //FIXME : Return something else
                return _data_frames[0];
            }
            return _data_frames[_this_data_frame_index];
        }

        void big_data::write(std::string filename) {
            std::vector<hpx::future<void>> f;
            f.reserve(_data_frames.size());
            for(int i=0; i<_data_frames.size(); i++) {
                f.push_back(_data_frames[i].write(filename));
            }
            hpx::wait_all(f);
        }

        void big_data::add_data_frame(utils::data::data_frame &df) {
            _data_frames.push_back(df);
        }

        void big_data::set_this_data_frame_index(int index) {
            _this_data_frame_index = index;
        }
    }
}
