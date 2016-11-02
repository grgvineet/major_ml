//
// Created by vineet on 2/11/16.
//

#ifndef DATA_FRAME_H
#define DATA_FRAME_H

#include <vector>
#include <map>

#include "data_row.h"

namespace utils {

    namespace data {

        class data_frame {

            int _ncols;
            std::vector<std::vector<double> > _data;
            std::map<std::string, int> _colnames_index;
            std::vector<std::string> _colnames;

        public:
            explicit data_frame(int ncols);

            void initialise_colnames(int ncols);
            void initialise_colnames(std::vector<std::string>& colnames);

            bool remove_row(int index);
            bool insert_row(data_row dr);
            bool insert_row(std::vector<double>& row);

            bool insert_column(std::vector<double>& col);
            bool insert_column(std::vector<double>& col, std::string colname);
            bool remove_column(const int index);
            bool remove_column(const std::string& colname);

            data_row operator[](const int index); // Return row
            data_row operator[](const std::string& colname);

            int get_size();
            void print(int rows = -1);
            void print_col_names();
        };

    }
}



#endif //DATA_FRAME_H
