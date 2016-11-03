//
// Created by vineet on 2/11/16.
//

#include "data_row.h"

#include <iostream>

namespace utils {

    namespace data {

        data_row::data_row(std::vector<double> &data, std::vector<std::string> &colnames) {
            _data = data;
            _ncols = data.size();
            _colnames = colnames;
            for(int i=0; i<colnames.size(); i++) {
                _colnames_index[colnames[i]] = i;
            }
        }

        double data_row::operator[](int index) {
            return get(index);
        }

        double data_row::operator[](const std::string& colname) {
            return get(colname);

        }

        double data_row::get(int index) {
            if (index < 0 || index >= get_size()) {
                std::cerr << __PRETTY_FUNCTION__ << " : Index " << index
                          << " out of bound"<< std::endl;
                return 0;
            }
            return _data[index];
        }

        double data_row::get(const std::string& colname) {
            if (_colnames_index.find(colname) == _colnames_index.end()) {
                std::cerr << __PRETTY_FUNCTION__ << " : Invalid column name specified("
                          << colname << ")"<< std::endl;
                return get(-1);
            }
            return get(_colnames_index[colname]);

        }

        int data_row::get_size() {
            return _data.size();
        }
    }
}
