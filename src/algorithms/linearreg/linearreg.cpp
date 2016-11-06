//
// Created by vineet on 31/10/16.
//

#include "linearreg.h"


algo::linearreg::linearreg(int seed) : algo_base(seed) {
    _name = "linearreg";
}

algo::linearreg::~linearreg() {

}

void algo::linearreg::say_hello() {
    std::cout << "Hello from " << _name << std::endl;
}
