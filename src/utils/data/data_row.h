//
// Created by vineet on 2/11/16.
//

#ifndef UTILS_DATA_DATA_ROW_H
#define UTILS_DATA_DATA_ROW_H

#include <vector>
#include <string>
#include <map>

namespace utils {

    namespace data {

        class data_row {

            int _ncols;
            std::vector<double> _data;
            std::map<std::string, int> _colnames_index;
            std::vector<std::string> _colnames;

        public:
            data_row(std::vector<double>& data, std::vector<std::string>& colnames);

            double operator[](int index);
            double operator[](const std::string& colname);

            double get(int index);
            double get(const std::string& colname);

            std::vector<double> get_data();

            int get_size();
        };

    }
}


#endif //UTILS_DATA_DATA_ROW_H
