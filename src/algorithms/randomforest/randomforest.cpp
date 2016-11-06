//
// Created by vineet on 31/10/16.
//

#include "randomforest.h"

algo::randomforest::randomforest(int seed) : algo_base(seed) {
    _name = "randomforest";
}

algo::randomforest::~randomforest() {

}

void algo::randomforest::say_hello() {
    std::cout << "Hello from " << _name << std::endl;
}
