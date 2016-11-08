//
// Created by vineet on 31/10/16.
//

#include <iostream>

#include "algorithms/randomforest/randomforest.h"

int main() {
    std::cout << "Hello randomforest" << std::endl;

    algo::randomforest randomforest(0);
    randomforest.say_hello();
    return 0;
}
