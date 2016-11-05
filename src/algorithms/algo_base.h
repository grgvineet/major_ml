//
// Created by vineet on 31/10/16.
//

#ifndef ALGORITHMS_ALGO_BASE_H
#define ALGORITHMS_ALGO_BASE_H

#include <iostream>

#include <hpx/util/parse_command_line.hpp>

namespace algo {

    struct algo_base {

    protected:
        std::string _name;

    public:

        static boost::program_options::options_description
        configure_args(boost::program_options::options_description &description);
    };
}


#endif //ALGORITHMS_ALGO_BASE_H
