#ifndef RLE_H
#define RLE_H

#include <iostream>
#include "matrix.h"
#include <vector>
#include <bitset>

std::vector<unsigned char> GetRunlengthSize (matrixClass value);
std::vector<unsigned char> Calculate_size (matrixClass value);
std::vector<unsigned char> Runlength(matrixClass AC);
std::vector<int> DCDiff(std::vector<int> DC);
unsigned char calcSize(int Value);
#endif // RLE_H
