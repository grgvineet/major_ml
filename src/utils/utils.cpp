//
// Created by vineet on 2/11/16.
//

#include "utils.h"

#include <sstream>
#include <iterator>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

namespace utils {

    /*
     * Splits a string into defined types
     * Return a vector
     * Ignores single character delimeter
     */
    template <typename Type>
    std::vector<Type> split(const std::string &s) {
        std::vector<Type> elems;

        boost::char_separator<char> delimiter(",");
        boost::tokenizer<boost::char_separator<char>> tokenizer(s, delimiter);
        BOOST_FOREACH(std::string t, tokenizer)
        {
            elems.push_back(boost::lexical_cast<Type>(t));
        }
        return elems;
    }

    template <>
    std::vector<double> split<double>(const std::string &s) {
        std::vector<double> elems;

        boost::char_separator<char> delimiter(",");
        boost::tokenizer<boost::char_separator<char>> tokenizer(s, delimiter);
        BOOST_FOREACH(std::string t, tokenizer)
        {
            double d;
            std::sscanf(t.data(), "%lf", &d);
            elems.push_back(d);
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

    template<typename Type>
    bool does_not_contain(std::vector<std::vector<Type>>& source, std::vector<Type>& value) {
        if (source.empty()) {
            return true;
        }
        auto source_begin = source.begin();
        auto source_end = source.end();
        for(; source_begin != source_end; source_begin++) {
            if (std::equal(source_begin->begin(), source_begin->end(), value.begin())) {
                return false;
            }
        }
        return true;
    }
}
