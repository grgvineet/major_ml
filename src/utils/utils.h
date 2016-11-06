//
// Created by vineet on 6/11/16.
//

#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#include <string>
#include <vector>

namespace utils {

    template <typename Type>
    std::vector<Type> split(const std::string &s);

    template <>
    std::vector<double> split<double>(const std::string &s);

    template <>
    std::vector<std::string> split<std::string>(const std::string &s);

    template<typename Type>
    bool does_not_contain(std::vector<std::vector<Type>>& source, std::vector<Type>& value);
}
#endif //UTILS_UTILS_H
