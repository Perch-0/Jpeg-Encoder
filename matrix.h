#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

class matrixClass {

public:
    std::vector<std::vector<double>> matrix;
    int columns() const {return matrix[0].size();}
    int rows() const {return matrix.size();}
    //array<array<int, N>, M> matrix;
    matrixClass(int row, int col) {
    matrix.resize(row);
         for (int r=0; r<row; r++)
        {
            matrix[r].resize(col);
            //for (int n=0; n<col; n++)
            //{

            //}
        }


    }
    std::vector<double> operator[](int index) const {
    if (index >= rows()) {
        std::cout << index << " is out of bounds \n";
        exit(0);
    }
    return matrix[index];
}
    std::vector<double>& operator[](int index) {
    if (index >= rows()) {
        std::cout << index << " is out of bounds \n";
        exit(0);
    }
    return matrix[index];
}
};
matrixClass FlipMatrix(matrixClass A, char axis);
matrixClass Minor(matrixClass A, int Mrow, int Mcol);
matrixClass Transpose(matrixClass A);
matrixClass Matrix_product(matrixClass A, matrixClass B);
double Determinant(matrixClass A);
matrixClass Invert(matrixClass A);
void Print_matrix(matrixClass a);
matrixClass ScalarMultiply(matrixClass a, double b);
matrixClass ScalarDivide(matrixClass a, double b);
matrixClass ScalarAdd(matrixClass a, double b);

#endif // MATRIX_H
