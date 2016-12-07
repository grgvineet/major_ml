//
// Created by vineet on 2/11/16.
//

#include <iostream>
#include "data_frame_server.h"

#include "utils/utils.h"

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/algorithm/string.hpp>

namespace utils {
    
    namespace data { namespace server {

        data_frame_server::data_frame_server() {
            _ncols = 0;
            _data = std::vector<std::vector<double> >(_ncols, std::vector<double>(0));
        }

        data_frame_server::data_frame_server(int ncols) {
            _ncols = ncols;
            _data = std::vector<std::vector<double> >(_ncols, std::vector<double>(0));
        }

        data_frame_server::data_frame_server(std::string path, bool header) {

            // Optimised way to get number of lines
            int num_lines = get_nrows_from_file(path, header);

            std::ifstream file;
            file.open(path);
            if (!file.is_open()) {
                std::cerr << __PRETTY_FUNCTION__ << " : Cannot open file "
                          << path << std::endl;
                _ncols = 0;
                _data = std::vector<std::vector<double> >(_ncols, std::vector<double>(0));
                return;
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

            _ncols = colnames.size();
            _data = std::vector<std::vector<double> >(_ncols, std::vector<double>());
            for(int i=0; i<_ncols; i++) _data[i].reserve(num_lines);

            initialise_colnames(colnames);

            if (!header) {
                std::vector<double> v = utils::split<double>(first_row);
                insert_row(v);
            }
            std::string data;
            while(getline(file, data)) {
                std::vector<double> row = utils::split<double>(data);
                insert_row(row);
            }
//            print();
        }

        std::uint64_t data_frame_server::get_local_ptr() {
            return reinterpret_cast<std::uint64_t >(this);
        }

        void data_frame_server::initialise_colnames(int ncols) {
            if (ncols < 0) {
                std::cerr << __PRETTY_FUNCTION__ << " : Invalid number of columns" << std::endl;
                return;
            }
            std::vector<std::string> colnames;
            for(int i=0; i<ncols; i++) {
                colnames.push_back("C" + std::to_string(i));
            }
            initialise_colnames(colnames);
        }

        void data_frame_server::initialise_colnames(std::vector<std::string>& colnames) {
            _colnames = colnames;
            // Col number start from 0
            for(int i=0; i<colnames.size(); i++) {
                _colnames_index[colnames[i]] = i;
            }
        }

        bool data_frame_server::remove_row(int index) {
            if (index < 0 || index >= get_size()) {
                std::cerr << __PRETTY_FUNCTION__ << " : Index " << index
                          << " out of bound"<< std::endl;
                return false;
            }
            for(int i=0; i<_ncols; i++) {
                _data[i].erase(_data[i].begin() + index);
            }
            return true;
        }

        bool data_frame_server::insert_row(data_row dr) {
            if (dr.get_size() != _ncols) {
                std::cerr << __PRETTY_FUNCTION__ << " : Number of columns in data row( "
                          << dr.get_size() << ") does not match number of columns in data frame("
                          << _ncols  << ")"<< std::endl;
                return false;
            }
            for(int i=0; i<_ncols; i++) {
                _data[i].push_back(dr[i]);
            }
            return true;
        }

        bool data_frame_server::insert_row(std::vector<double>& row) {
            if (row.size() != _ncols) {
                std::cerr << __PRETTY_FUNCTION__ << " : Number of columns in data row( "
                          << row.size() << ") does not match number of columns in data frame("
                          << _ncols << ")" << std::endl;
                return false;
            }
            for(int i=0; i<_ncols; i++) {
                _data[i].push_back(row[i]);
            }
        }

        bool data_frame_server::insert_column(std::vector<double>& col) {
            return insert_column(col, "C" + std::to_string(_ncols+1));
        }

        bool data_frame_server::insert_column(std::vector<double>& col, std::string colname) {
            if (get_size() != col.size() && _ncols != 0) {
                std::cerr << __PRETTY_FUNCTION__ << " : Number of rows in data column( "
                          << col.size() << ") does not match number of rows in data frame(" << get_size()
                          << ")" << std::endl;
                return false;
            }
            _colnames.push_back(colname);
            _colnames_index[colname] = _ncols;
            _ncols++;
            _data.push_back(col);
            return true;
        }

        bool data_frame_server::remove_column(int index) {
            if (index < 0 || index >= _ncols) {
                std::cerr << __PRETTY_FUNCTION__ << " : Index " << index
                          << " out of bound"<< std::endl;
                return false;
            }
            _ncols--;
            std::string colname = _colnames[index];
            _colnames.erase(_colnames.begin() + index);
            _colnames_index.erase(colname);
            _data.erase(_data.begin() + index);
            return true;
        }

        bool data_frame_server::remove_column(const std::string& colname) {
            if (_colnames_index.find(colname) == _colnames_index.end()) {
                std::cerr << __PRETTY_FUNCTION__ << " : Invalid column name specified("
                          << colname << ")"<< std::endl;
            }
            return remove_column(_colnames_index[colname]);
        }

        utils::data::data_row data_frame_server::operator[](int index) {
            std::vector<double> data;
            if (index < 0 || index >= get_size()) {
                std::cerr << __PRETTY_FUNCTION__ << " : Index " << index
                          << " out of bound"<< std::endl;
                std::vector<std::string> empty_colnames;
                return data_row(data, empty_colnames);
            }

            for(int i=0; i<_ncols; i++) {
                data.push_back(_data[i][index]);
            }
            return data_row(data, _colnames);
        }

        utils::data::data_row data_frame_server::operator[](const std::string& colname) {
            if (_colnames_index.find(colname) == _colnames_index.end()) {
                std::cerr << __PRETTY_FUNCTION__ << " : Invalid column name specified("
                          << colname << ")"<< std::endl;
                return operator[](-1);
            }
            int index = _colnames_index[colname];
            return operator[](index);
        }

        int data_frame_server::get_size() const{
            if (_ncols == 0) return 0;
            return _data[0].size();
        }

        void data_frame_server::print(int rows) {
            if (rows == -1) rows = get_size();
            else if (rows > get_size()) rows = get_size();
            for(int i=0; i<_ncols; i++) {
                std::cout << _colnames[i] << " ";
            }
            std::cout << std::endl;
            for(int i=0; i<rows; i++) {
                for(int j=0; j<_ncols ; j++) {
                    std::cout << (*this)[i][j] << " ";
                }
                std::cout << std::endl;
            }
        }

        void data_frame_server::print_col_names() {
            for(int i=0; i<_colnames.size(); i++) {
                std::cout << _colnames[i] << " " ;
            }
            std::cout << std::endl;
        }

        int data_frame_server::get_ncols() const {
            return _ncols;
        }

        std::vector<double> data_frame_server::get_row(int index) {
            return this->operator[](index).get_data();
        }

        int data_frame_server::get_nrows_from_file(std::string &path, bool header) {
            // Optimised method to number of data rows from input file
            boost::iostreams::mapped_file mmap(path, boost::iostreams::mapped_file::readonly);
            auto f = mmap.const_data();
            auto l = f + mmap.size();
            int nrows = 0;
            while(f && f!=l) {
                if ((f = static_cast<const char*>(memchr(f, '\n', l-f)))) {
                    nrows++; f++;
                }
            }
            mmap.close();
            if (header) nrows--;
            return nrows;
        }

            std::vector<double> &data_frame_server::get_column(int index) {
                return _data[index];
            }

            void data_frame_server::write(std::string path) {

                std::ofstream file;
                file.open(path);
                if (!file.is_open()) {
                    std::cerr << __PRETTY_FUNCTION__ << " : Cannot open file "
                              << path << std::endl;
                    return;
                }

                int nrows = get_size();
                for(int i=0; i<nrows; i++) {
                    int j=0;
                    for(; j<_ncols-1; j++) {
                        file << _data[j][i] << ",";
                    }
                    if (j < _ncols) {
                        file << _data[j][i] << "\n";
                    }
                }
                file.close();
            }

        }}// end namespace data
} // end namespace utils

// HPX_REGISTER_COMPONENT() exposes the component creation
// through hpx::new_<>().
typedef hpx::components::component<utils::data::server::data_frame_server> data_frame_server_type;
HPX_REGISTER_COMPONENT(data_frame_server_type, data_frame_server);

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.
typedef utils::data::server::data_frame_server::get_local_ptr_action get_local_ptr_action;
HPX_REGISTER_ACTION(get_local_ptr_action);
typedef utils::data::server::data_frame_server::get_ncols_action get_ncols_action;
HPX_REGISTER_ACTION(get_ncols_action);
typedef utils::data::server::data_frame_server::remove_row_action remove_row_action;
HPX_REGISTER_ACTION(remove_row_action);
typedef utils::data::server::data_frame_server::print_action print_action;
HPX_REGISTER_ACTION(print_action);
typedef utils::data::server::data_frame_server::get_size_action get_size_action;
HPX_REGISTER_ACTION(get_size_action);
typedef utils::data::server::data_frame_server::get_row_action get_row_action;
HPX_REGISTER_ACTION(get_row_action);
HPX_REGISTER_ACTION(utils::data::server::data_frame_server::write_action);
