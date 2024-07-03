#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "matrix.h"

//class matrixClass;
/*matrixClass Minor(matrixClass A, int Mrow, int Mcol);
matrixClass Transpose(matrixClass A);
matrixClass Matrix_product(matrixClass A, matrixClass B);
double Determinant(matrixClass A);
matrixClass Invert(matrixClass A);
void Print_matrix(matrixClass a);*/



matrixClass Invert(matrixClass A) {

    int rows = A.rows();
    int col = A.columns();
    double det = Determinant(A);
    if (rows != col || det == 0) {return A;}
    matrixClass MatrixCofactors(rows, col);
    for (int r=0; r<rows; r++)
        {
        for (int c=0; c<col; c++)
        {
        MatrixCofactors.matrix[r][c] = pow(-1, r+c)*Determinant(Minor(A, r, c));

        }
        }
    matrixClass InvertedMatrix = Transpose(MatrixCofactors);
    for (int r=0; r<InvertedMatrix.rows(); r++) {
        for (int c=0; c<InvertedMatrix.columns(); c++) {
            InvertedMatrix.matrix[r][c] /= det;
        }
    }
    return InvertedMatrix;
}


matrixClass Transpose(matrixClass A){

    matrixClass value(A.columns(), A.rows());
    for (int r=0; r<A.rows(); r++) {
        for (int c=0; c<A.columns(); c++) {
            value.matrix[c][r] = A.matrix[r][c];
        }
    }
    return value;
}



void Print_matrix(matrixClass a)
{
    int rows = a.rows();
    int col = a.columns();
    std::cout << "\nPrint_Matrix" << std::endl;
    std::cout << std::setfill('_') << std::setw((rows*3)+10) << "____________" << std::endl;
    for (int r=0; r<rows; r++)
        {
        std::cout << std::setfill(' ') << "row:" << std::setw(4) << r+1 << " | ";
        for (int n=0; n<col; n++)
        {
        std::cout << a.matrix[r][n] << " ";

        }
        std::cout << std::endl;
        }
}


matrixClass Matrix_product(matrixClass A, matrixClass B)
{
matrixClass C(A.rows(), B.columns());
for (int r=0; r<A.rows(); r++)
        {

            for (int c=0; c<B.columns(); c++)
            {
                double tmp = 0;
                  for (int j=0; j<A.columns(); j++) {
                    tmp += A.matrix[r][j]*B.matrix[j][c];

                  }
                C.matrix[r][c] = tmp;

            }
        }
return C;
}



double Determinant(matrixClass A)
{
    int rows = A.rows();
    int col = A.columns();
    double Det = 0;
    if (rows == 1 && col == 1){return A.matrix[0][0];}
    if (rows == 2 && col == 2)
    {
        Det = (A.matrix[0][0]*A.matrix[1][1]) - (A.matrix[0][1]*A.matrix[1][0]);
    }
    if (rows == col && col != 2) {
        //for (int r=0; r<rows; r++)
           // {
            for (int n=0; n<col; n++)
            {
            Det += A.matrix[0][n]*pow(-1, n)*Determinant(Minor(A, 0, n));
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            }
        //}

    }
    return Det;

}
matrixClass Minor(matrixClass A, int Mrow, int Mcol) {
    int rows = A.rows();
    int col = A.columns();
    std::vector<double> tmp;
    matrixClass M(rows-1, col-1);
    for (int r=0; r<rows; r++) {
        for (int c=0; c<col; c++) {
            if (r == Mrow || c==Mcol){continue;}
                    //cout << A.matrix[r][c];
                    tmp.push_back(A.matrix[r][c]);


            }
        }
    for (int i=0; i<M.rows(); i++) {
        for (int j=0; j<M.columns(); j++) {
            M.matrix[i][j] = tmp.front();
            tmp.erase(tmp.begin());

        }
    }
    return M;
}
matrixClass ScalarMultiply(matrixClass a, double b) {
    for (int r=0; r<a.rows(); r++) {
        for (int c=0; c<a.columns(); c++) {
            a.matrix[r][c] *= b;
        }
    }
return a;
}
matrixClass ScalarDivide(matrixClass a, double b) {
    for (int r=0; r<a.rows(); r++) {
        for (int c=0; c<a.columns(); c++) {
            a.matrix[r][c] /= b;
        }
    }
return a;
}
matrixClass ScalarAdd(matrixClass a, double b) {
    for (int r=0; r<a.rows(); r++) {
        for (int c=0; c<a.columns(); c++) {
            a.matrix[r][c] += b;
        }
    }
return a;
}
matrixClass FlipMatrix(matrixClass A, char axis) {
    int rows = A.rows();
    int col = A.columns();
    matrixClass Result(rows, col);
    if (axis == 'x') {
    for (int r=0; r<rows; r++) {
        for (int c=0; c<col; c++) {
            Result[rows-1-r][c] = A.matrix[r][c];
        }
    }
    }
    else if (axis == 'y') {
    for (int r=0; r<rows; r++) {
        for (int c=0; c<col; c++) {
            Result[r][col-1-c] = A.matrix[r][c];
        }
    }
    }
    else {
        std::cout << "Not a valid axis";
    }
    return Result;
}

