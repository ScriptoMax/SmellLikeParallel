#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <omp.h>

void calcDeterm(long n);

int main() {

    long num;

    std::cout << "Nice to see you'd like to solve such a boring task.\n"
                 "Enter any natural number to initialize dimensions of your matrix\n"
                 "(just enter a single number which means to be the same for rows as for columns):" << std::endl;

    std::cin >> num;

    calcDeterm(num);

    return 0;
}

// combines both computation algorithms
void calcDeterm(long n) {
    int i, j, k;
    double temp;

    long double determSeq = 1; // single thread matrix det
    long double determPar = 1; // multithread matrix det

    // initializing both matrices
    srand(10);
    std::vector<std::vector<double>> matSeq(n, std::vector<double>(n));// single thread input matrix
    std::vector<std::vector<double>> matPar(n, std::vector<double>(n));// multithread input matrix

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            matSeq[i][j] = rand() % 17;
    }

    std::cout << "\n";

    /*
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            std::cout << matSeq[i][j] << " ";
        std::cout << "\n";
    }
     */

    matPar = matSeq; // same inputs for regular performance comparison

    auto startSeq = std::chrono::high_resolution_clock::now();// single thread algorithm launch
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (matSeq[i][i] == 0) {
                if (matSeq[i][j] == 0)
                    temp = 0;
            } else
                temp = matSeq[j][i] / matSeq[i][i];
            for (k = i; k < n; k++)
                matSeq[j][k] = matSeq[j][k] - matSeq[i][k] * temp;
        }
        determSeq *= matSeq[i][i];
    }

    auto finishSeq = std::chrono::high_resolution_clock::now(); // single thread algorithm run-off
    auto timeslotSeq = std::chrono::duration_cast<std::chrono::microseconds>(finishSeq - startSeq); // single thread time cost

    std::cout << "\nSingle thread calculation is complete.\nDeterminant comes to: " << determSeq << std::endl;
    std::cout << "Single thread program finished in " << timeslotSeq.count() <<
              " microseconds\n" << std::endl;


    /*
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            std::cout << matPar[i][j] << " ";
        std::cout << "\n";
    }*/

    auto startPar = std::chrono::high_resolution_clock::now();// multithread algorithm launch

    for (i = 0; i < n; i++) {
        #pragma omp parallel for private(j, k, temp) shared(i)
        for (j = i + 1; j < n; j++) {
            if (matPar[i][i] == 0) {
                if (matPar[i][j] == 0)
                    temp = 0;
            } else
                temp = matPar[j][i] / matPar[i][i];
            for (k = i; k < n; k++)
                matPar[j][k] = matPar[j][k] - matPar[i][k] * temp;
        }
    }

    #pragma omp parallel for reduction(*:determPar)
    for(i = 0; i < n; i++)
        determPar *= matPar[i][i];

    auto finishPar = std::chrono::high_resolution_clock::now();// multithread algorithm run-off
    auto timeslotPar = std::chrono::duration_cast<std::chrono::microseconds>(finishPar - startPar);// multithread time cost

    std::cout << "\nMultithread calculation is complete.\nDeterminant comes to: " << determPar << std::endl;
    std::cout << "Multithread program finished in " << timeslotPar.count() <<
              " microseconds\n" << std::endl;
}
