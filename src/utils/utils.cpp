//
// Created by vineet on 2/11/16.
//

#ifndef UTILS_UTILS_CPP
#define UTILS_UTILS_CPP

#include <string>
#include <sstream>
#include <vector>

namespace utils {

    /*
     * Splits a string into defined types
     * Return a vector
     * Ignores single character delimeter
     */
    template <typename Type>
    std::vector<Type> split(const std::string &s) {
        Type item;
        std::vector<Type> elems;

        std::stringstream ss;
        ss.str(s);
        while(ss >> item) {
            elems.push_back(item);
            ss.ignore(1);
        }
        return elems;
    }

    template <>
    std::vector<std::string> split<std::string>(const std::string &s) {
        std::string item;
        std::vector<std::string> elems;

        std::stringstream ss;
        ss.str(s);
        while(getline(ss, item, ',')) {
            elems.push_back(item);
        }
        return elems;
    }
}

#endif // UTILS_UTILS_CPP

