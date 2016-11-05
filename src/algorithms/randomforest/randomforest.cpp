//
// Created by vineet on 31/10/16.
//

#include "randomforest.h"

algo::randomforest::randomforest() {
    _name = "randomforest";
}

algo::randomforest::~randomforest() {

}

void algo::randomforest::say_hello() {
    std::cout << "Hello from " << _name << std::endl;
}
