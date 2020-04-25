#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <omp.h>

void calcDetermSeq(std::vector<std::vector<double>> input); // single thread det calculation
void calcDetermPar(std::vector<std::vector<double>> input); // multithread det calculation
std::vector<std::vector<double>> setSpecValues(); // fill matrix by user-specified values
std::vector<std::vector<double>> setRandValues(); // fill matrix by random values

int main()
{
    signed int option; // select way to initialize matrix
    std::vector<std::vector<double>> matrixSeq;
    std::vector<std::vector<double>> matrixPar;
    std::cout << "Nice to see you'd like to solve such a boring task.\n"
                 "Select an option to proceed:\n"
                 "1 - specific matrix input\n2 - random matrix input (dimensions to be specified)" << std::endl;

    std::cin >> option;

    switch(option) {
        case 1:
            matrixSeq = setSpecValues(); // type in values if matrix specified
            std::cout << std::endl;
            break;
        case 2:
            matrixSeq = setRandValues(); // wait for random matrix generation
            std::cout << std::endl;
            break;
        default:
            std::cout << option << " isn't allowed option number.\nRerun to select valid one.\n";
            return 0;
    }

    matrixPar = matrixSeq; // make sure single thread algorithm passed same input as multithread received

    // display matrix initialized
    /*for(int i = 0; i < matrixPar.size(); i++) {
        for (int j = 0; j < matrixPar.size(); j++) {
            std::cout << matrixPar[i][j] << " ";
        }
        std::cout << std::endl;
    }*/

    calcDetermSeq(matrixSeq); // single thread running
    std::cout << std::endl;
    calcDetermPar(matrixPar); // multithread running

    return 0;
}

// initialize matrix by specified inputs
std::vector<std::vector<double>> setSpecValues()
{
    int dim, val;
    int x = 0;
    std::cout << "Well, enter any positive integer to set matrix dimensions:" << std::endl;
    std::cin >> dim;
    std::cout << "Pretty enough, we want to deal with matrix as large as " << dim << "x" << dim << ".\n" <<
              "Just enter its elements separately to see quick solution: "<< std::endl;

    std::vector<std::vector<double>> input (dim, std::vector<double>(dim));
    for(int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            std::cin >> val;
            input[i][j] = val;
        }
    }

    /*
    std::cout << std::endl;
    std::cout << "Printing out your matrix input...\n";
    for(int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            std::cout << input[i][j] << " ";
        }
        std::cout << std::endl;
    }
     */

    return input;
}

// initialize at random (within user-specified dimensions)
std::vector<std::vector<double>> setRandValues()
{
    int dim, val;
    int x = 0;
    std::cout << "Well, enter any positive integer to set matrix dimensions:" << std::endl;
    std::cin >> dim;
    std::cout << "Pretty enough, it would need just a moment to initialize " << dim << "x" << dim << " matrix" << std::endl;

    std::vector<std::vector<double>> input (dim, std::vector<double>(dim));
    std::srand(unsigned (std::time(0)));
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            int randNum = std::rand();
            input[i][j] = rand() % 31;
        }
    }

    /*
    std::cout << std::endl;
    std::cout << "Printing out your matrix input...\n";
    for(int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            std::cout << input[i][j] << " ";
        }
        std::cout << std::endl;
    }
     */

    return input;
}

// sequential solution
void calcDetermSeq(std::vector<std::vector<double>> input)
{
    int i, j, k, index, temp;
    long double det = 1;
    long double factor;
    int matSize = input.size();
    //std::cout.precision(5);

    auto startSeq = std::chrono::high_resolution_clock::now();// single thread algorithm entry

    for(i = 0; i < matSize; i++) {

        index = i;

        while(input[index][i] == 0 && index < matSize)
            index++;

        if(index == matSize)
            continue;

        if(index != i) {
            for(j = 0; j < matSize; j++) {
                temp = input[i][j];
                input[i][j] = input[index][i];
                input[index][j] = temp;
            }
            det *= -1;
        }

        for(j = i + 1; j < matSize; j++) {
            factor = input[j][i] / input[i][i];
            for(k = 0; k < matSize; k++)
                input[j][k] = input[j][k]- (factor * input[i][k]);
        }
    }

    for(i = 0; i < matSize; i++)
        det = det * input[i][i];

    auto finishSeq = std::chrono::high_resolution_clock::now(); // single thread algorithm run-off
    auto timeCostSeq = std::chrono::duration_cast<std::chrono::microseconds>(
            finishSeq - startSeq); // single thread time cost

    //std::cout << "\nSingle thread calculation is complete.\nMatrix determinant: " << std::fixed << det << std::endl;
    std::cout << "\nSingle thread calculation is complete.\nMatrix determinant: " << det << std::endl;
    std::cout << "Execution finished in " << timeCostSeq.count() <<
              " microseconds (by single thread)\n" << std::endl;
}

// parallel solution
void calcDetermPar(std::vector<std::vector<double>> input)
{
    int i, j, k, index, temp;
    long double det = 1;
    long double factor;
    int matSize = input.size();
    int numOfThreads = 2;
    //std::cout.precision(5);

    omp_set_dynamic(0);
    omp_set_num_threads(numOfThreads);

    auto startPar = std::chrono::high_resolution_clock::now();// multithread algorithm entry

    for(i = 0; i < matSize; i++) {

        index = i;

        while(input[index][i] == 0 && index < matSize)
            index++;

        if(index == matSize)
            continue;

        if(index != i) {
            #pragma omp parallel for shared(input) private (j)
            for(j = 0; j < matSize; j++) {
                temp = input[i][j];
                input[i][j] = input[index][i];
                input[index][j] = temp;
            }
            det *= -1;
        }

        for(j = i + 1; j < matSize; j++) {
            factor = input[j][i] / input[i][i];
            #pragma omp parallel for shared(input) private (k)
            for(k = 0; k < matSize; k++) {
                input[j][k] = input[j][k]- (factor * input[i][k]);
            }
        }
    }

    #pragma omp parallel for reduction(*:det)
    for(i = 0; i < matSize; i++)
        det = det * input[i][i];

    auto finishPar = std::chrono::high_resolution_clock::now(); // multithread algorithm run-off
    auto timeCostPar = std::chrono::duration_cast<std::chrono::microseconds>(
            finishPar - startPar); // single thread time cost

    //std::cout << "\nMultithread calculation is complete.\nMatrix determinant: " << std::fixed << det << std::endl;
    std::cout << "\nMultithread calculation is complete.\nMatrix determinant: " << det << std::endl;
    std::cout << "Execution finished in " << timeCostPar.count() <<
              " microseconds (by " << numOfThreads << " threads)\n" << std::endl;
}