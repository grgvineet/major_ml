//
// Created by vineet on 31/10/16.
//

#ifndef ALGORITHMS_LINEARREG_LINEARREG_H
#define ALGORITHMS_LINEARREG_LINEARREG_H

#include "../algo_base.h"

namespace algo {

    class linearreg : public algo_base {
    public:
        linearreg(int seed);
        ~linearreg();

        void say_hello();
    };

}


#endif //ALGORITHMS_LINEARREG_LINEARREG_H
