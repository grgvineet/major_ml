//
// Created by vineet on 31/10/16.
//

#include "randomforest.h"

algo::randomforest::randomforest() {
    class_name = "randomforest";
}

algo::randomforest::~randomforest() {

}

void algo::randomforest::say_hello() {
    std::cout << "Hello from " << class_name << std::endl;
}
