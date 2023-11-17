#include <iostream>
#include <ctime>
#include <pthread.h>
#include <vector>
#include <cstdlib>
#include <chrono>

using namespace std;

/*
 * Multiplying two square matrices sequentially and/or in parallel
 *
 * USE_PARALLEL: will only run parallel version of code, if USE_PARALLEL is false: will only run sequential
 * USE_BOTH: Will run both versions regardless of USE_PARALLEL; if false, will default to USE_PARALLEL settings
 * MATRIX_SIZE: the length of one side of the square matrix, ie: MATRIX_SIZE 1000 => 1000x1000 matrices are created and calculated
 */
#define MATRIX_SIZE 1000
#define USE_PARALLEL true
#define USE_BOTH true

vector<vector<double>>
        finalMatrix(MATRIX_SIZE, vector<double>(MATRIX_SIZE)),
        matrix1(MATRIX_SIZE, vector<double>(MATRIX_SIZE)),
        matrix2(MATRIX_SIZE, vector<double>(MATRIX_SIZE));

int generateRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void fillMatrixWithRandomNumbers(vector<vector<double>> &matrix) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix[i][j] = generateRandomNumber(0, 10);
        }
    }
}

void printMatrix(const vector<vector<double>> &matrix) {
    for (auto &row: matrix) {
        for (double value: row) {
            cout << value << " ";
        }
        cout << endl;
    }
}

double multiplyTwoRows(const vector<double> &hor, const vector<double> &ver) {
    double final = 0;
    for (int i = 0; i < MATRIX_SIZE; i++) {
        final += hor[i] * ver[i];
    }
    return final;
}

vector<double> getColumn(int columnIndex, const vector<vector<double>> &matrix) {
    vector<double> final(MATRIX_SIZE);
    for (int i = 0; i < MATRIX_SIZE; i++) {
        final[i] = matrix[i][columnIndex];
    }
    return final;
}

void *thread_ColumnCalculation(void *args) {
    long columnIndex = (long) args;
    vector<double> columnVector = getColumn(columnIndex, matrix2);
    for (int i = 0; i < MATRIX_SIZE; i++) {
        finalMatrix[i][columnIndex] = multiplyTwoRows(matrix1.at(i), columnVector);
    }
}


int main(int argc, char *argv[]) {
    srand(static_cast<unsigned int>(time(0)));
    fillMatrixWithRandomNumbers(matrix1);
    fillMatrixWithRandomNumbers(matrix2);
    printf("\nMatrices with size %d created...\n", MATRIX_SIZE);

    // printf("Matrix One:\n");
    // printMatrix(matrix1);
    // printf("Matrix Two:\n");
    // printMatrix(matrix2);

    /**********SEQUENTIAL IMPLEMENTATION**********/

    if (!USE_PARALLEL || USE_BOTH) {
        auto start = chrono::high_resolution_clock::now();
        vector<double> columnVector;
        for (int columnIndex = 0; columnIndex < MATRIX_SIZE; columnIndex++) {
            columnVector = getColumn(columnIndex, matrix2);
            for (int rowIndex = 0; rowIndex < MATRIX_SIZE; rowIndex++) {
                finalMatrix[rowIndex][columnIndex] = multiplyTwoRows(matrix1[rowIndex], columnVector);
            }
        }
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << "Time taken by sequential implementation: " << duration.count() / 1000.0l << " milliseconds" << endl;
    }

    /*********************************************/


    /**********PARALLEL IMPLEMENTATION************/

    if (USE_PARALLEL || USE_BOTH) {
        pthread_t columnThreads[MATRIX_SIZE];
        auto start = chrono::high_resolution_clock::now();
        for (long columnIndex = 0; columnIndex < MATRIX_SIZE; columnIndex++) {
            pthread_create(&columnThreads[columnIndex], nullptr, thread_ColumnCalculation, (void *) columnIndex);
        }
        for (auto& columnThread : columnThreads) {
            pthread_join(columnThread, nullptr);
        }
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << "Time taken by threaded implementation: " << duration.count() / 1000.0l << " milliseconds" << endl;
    }

    /*********************************************/

    // printf("Final:\n");
    // printMatrix(finalMatrix);
    return 0;
}