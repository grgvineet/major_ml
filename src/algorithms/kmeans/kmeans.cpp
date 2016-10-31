//
// Created by vineet on 31/10/16.
//

#include "kmeans.h"

algo::kmeans::kmeans() {
    class_name = "kmeans";
}

algo::kmeans::~kmeans() {

}

void algo::kmeans::say_hello() {
    std::cout << "Hello from " << class_name << std::endl;
}
