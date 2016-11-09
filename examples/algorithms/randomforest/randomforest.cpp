//
// Created by vineet on 31/10/16.
//

#include <iostream>

#include "algorithms/randomforest/randomforest.h"

int main() {
    std::cout << "Hello randomforest" << std::endl;

    algo::randomforest::randomforest rf(3);
    rf.train_and_predict();
    return 0;
}
