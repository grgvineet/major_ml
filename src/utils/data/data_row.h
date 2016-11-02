//
// Created by vineet on 2/11/16.
//

#ifndef DATA_ROW_H
#define DATA_ROW_H

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

            int get_size();
        };

    }
}


#endif //DATA_ROW_H
