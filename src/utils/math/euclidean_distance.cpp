//
// Created by vineet on 5/11/16.
//

#include <iostream>
#include "euclidean_distance.h"

namespace utils {
    namespace math {

        double euclid_square(std::vector<double>& a, std::vector<double>& b) {
            if (a.size() != b.size()) {
                std::cerr << __PRETTY_FUNCTION__ << " : Dimensions of vector a and b does not match" << std::endl;
                return 0;
            }
            double ans = 0;
            for(int i=0; i<a.size(); i++) {
                double temp = a[i] - b[i];
                temp *= temp;
                ans += temp;
            }
            return ans;
        }

        bool is_converged(std::vector<std::vector<double>>& a, std::vector<std::vector<double>>& b) {
            if (a.size() != b.size()) {
                std::cerr << __PRETTY_FUNCTION__ << " : Dimensions of matrix a and b does not match" << std::endl;
                return true; // Break the loop where it is called
            }
            for(int i=0; i<a.size(); i++) {
                double dist = euclid_square(a[i], b[i]);
                if (dist > 1e-4) {
                    return false;
                }
            }
            return true;
        }
    }
}
