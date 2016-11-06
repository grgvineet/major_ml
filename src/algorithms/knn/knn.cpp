//
// Created by vineet on 31/10/16.
//

#include "knn.h"

algo::knn::knn(int seed) : algo_base(seed) {
    _name = "knn";
}

algo::knn::~knn() {

}

void algo::knn::say_hello() {
    std::cout << "Hello from " << _name << std::endl;
}
