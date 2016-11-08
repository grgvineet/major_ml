//
// Created by vineet on 5/11/16.
//

#include <iostream>
#include <cmath>
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
            return std::sqrt(ans);
        }

        bool is_converged(std::vector<std::vector<double>>& a, std::vector<std::vector<double>>& b) {
            if (a.size() == 0 || b.size() == 0) {
                std::cerr << __PRETTY_FUNCTION__ << " : One of the matrix is empty" << std::endl;
                return true; // Break the loop where it is called
            }
            if (a.size() != b.size()) {
                std::cerr << __PRETTY_FUNCTION__ << " : Dimensions of matrix a and b does not match" << std::endl;
                return true; // Break the loop where it is called
            }
            for(int i=0; i<a.size(); i++) {
                if (a[i].size() != b[i].size()) {
                    std::cerr << __PRETTY_FUNCTION__ << " : Dimensions of matrix a and b does not match" << std::endl;
                    return true; // Break the loop where it is called
                }
                for(int j=0; j<a[i].size(); j++) {
                    double dist = std::fabs(a[i][j]-b[i][j]);
                    if (dist > 1e-2) {
                        return false;
                    }
                }
            }
            return true;
        }

        // Function to get cofactor of A[p][q] in temp[][]. n is current
        // dimension of A[][]
        void getCofactor(std::vector<std::vector<double>>& source, std::vector<std::vector<double>>& temp, int p, int q, int n)
        {
            int i = 0, j = 0;

            // Looping for each element of the matrix
            for (int row = 0; row < n; row++)
            {
                for (int col = 0; col < n; col++)
                {
                    //  Copying into temporary matrix only those element
                    //  which are not in given row and column
                    if (row != p && col != q)
                    {
                        temp[i][j++] = source[row][col];

                        // Row is filled, so increase row index and
                        // reset col index
                        if (j == n - 1)
                        {
                            j = 0;
                            i++;
                        }
                    }
                }
            }
        }

/* Recursive function for finding determinant of matrix.
   n is current dimension of A[][]. */
        double determinant(std::vector<std::vector<double>>& source, int n)
        {
            int dimension = source.size();
            double D = 0; // Initialize result

            //  Base case : if matrix contains single element
            if (n == 1)
                return source[0][0];

            std::vector<std::vector<double>> temp(dimension, std::vector<double>(dimension)); // To store cofactors

            int sign = 1;  // To store sign multiplier

            // Iterate for each element of first row
            for (int f = 0; f < n; f++)
            {
                // Getting Cofactor of A[0][f]
                getCofactor(source, temp, 0, f, n);
                D += sign * source[0][f] * determinant(temp, n - 1);

                // terms are to be added with alternate sign
                sign = -sign;
            }

            return D;
        }

        // Function to get adjoint of A[N][N] in adj[N][N].
        void adjoint(std::vector<std::vector<double>>& source, std::vector<std::vector<double>>& adj)
        {
            int dimension = source.size();
            if (dimension == 1) {
                adj[0][0] = 1;
                return;
            }

            // temp is used to store cofactors of A[][]
            int sign = 1;
            std::vector<std::vector<double>> temp(dimension, std::vector<double>(dimension));

            for (int i=0; i<dimension; i++)
            {
                for (int j=0; j<dimension; j++)
                {
                    // Get cofactor of A[i][j]
                    getCofactor(source, temp, i, j, dimension);

                    // sign of adj[j][i] positive if sum of row
                    // and column indexes is even.
                    sign = ((i+j)%2==0)? 1: -1;

                    // Interchanging rows and columns to get the
                    // transpose of the cofactor matrix
                    adj[j][i] = (sign)*(determinant(temp, dimension-1));
                }
            }
        }

// Function to calculate and store inverse, returns false if
// matrix is singular
        bool inverse(std::vector<std::vector<double>>& source, std::vector<std::vector<double>>& result)
        {
            // Find determinant of A[][]
            int dimension = source.size();
            double det = determinant(source, dimension);
            if (std::abs(det) < 1e-9) {
                return false;
            }

            // Find adjoint
            std::vector<std::vector<double>> adj(dimension, std::vector<double>(dimension));
            adjoint(source, adj);

            // Find Inverse using formula "inverse(A) = adj(A)/det(A)"
            for (int i=0; i<dimension; i++)
                for (int j=0; j<dimension; j++)
                    result[i][j] = adj[i][j]/double(det);

            return true;
        }
    }
}
