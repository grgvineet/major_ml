//
// Created by vineet on 2/11/16.
//

#include "data_frame.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "../utils.cpp"

namespace utils {

    namespace data {

        data_frame* read_csv(std::string path, bool header) {
            std::ifstream file;
            file.open(path);

            if (!file.is_open()) {
                return nullptr;
            }

            std::string first_row;
            std::getline(file, first_row);

            std::vector<std::string> colnames;
            if (header) {
                colnames = utils::split<std::string>(first_row);
            } else {
                std::vector<double> first_row_data;
                first_row_data = utils::split<double>(first_row);
                for(int i=0; i<first_row_data.size() ; i++) {
                    colnames.push_back("C" + std::to_string(i));
                }
            }

            int ncols = colnames.size();
            data_frame* df = new data_frame(ncols);
            df->initialise_colnames(colnames);

            if (!header) {
                std::vector<double> v = utils::split<double>(first_row);
                df->insert_row(v);
            }

            std::string data;
            while(getline(file, data)) {
                std::vector<double> row = utils::split<double>(data);
                df->insert_row(row);
            }
            // TODO : df can cause memory leak if not deleted by user
            return df;
        }

    }
}
