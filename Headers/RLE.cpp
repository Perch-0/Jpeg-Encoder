#include <iostream>
#include "matrix.h"
#include <vector>
#include <bitset>
#include "RLE.h"
std::vector<unsigned char> GetRunlengthSize (matrixClass value) {

    std::vector<unsigned char> RL = Runlength(value);
    std::vector<unsigned char> Size = Calculate_size(value);
    std::vector<unsigned char> result;
    for (int i=0; i<RL.size(); i++) {
        result.push_back(RL[i]+Size[i]);
    }
    return result;

}

std::vector<unsigned char> Calculate_size (matrixClass value) {
    std::vector<unsigned char> result;
    int RunLen = 0;
    int ZeroBuf = 0;
    for (int i=0; i<value.columns(); i++) {

        int csize = 0;
        int val = value[0][i];
        for (;;) {
        if (val == 0) {
        RunLen++;
        break;
        }
        val /= 2;
        csize++;
        if (val == 0) {
            break;
        }
        continue;
        }
        if (csize != 0) {
        for (int j=0; j<ZeroBuf; j++) {
            result.push_back(0);
        }
        ZeroBuf = 0;
        result.push_back(csize);
        RunLen = 0;
        }
        else if (!(RunLen < 15)) {

            ZeroBuf++;
            RunLen = 0;
        }

    }
    result.push_back(0);
    //for (int i=0; i<result.size(); i++)
    //{

        //std::cout << (int)result[i] << std::endl;
    //}
    return result;
}
/*matrixClass DCDiff(matrixClass DC) {
    matrixClass Result(1, DC.columns());
    int PrevVal = 0;
    for (int i=0; i<DC.columns(); i++) {
        Result[0][i] = DC[0][i] - PrevVal;
        PrevVal = DC[0][i];
    }
    return Result;
}
*/
std::vector<int> DCDiff(std::vector<int> DC) {
    std::vector<int> Result;
    int PrevVal = 0;
    for (int i=0; i<DC.size(); i++) {
        Result.push_back(DC[i] - PrevVal);
        PrevVal = DC[i];
    }
    return Result;
}

std::vector<unsigned char> Runlength(matrixClass AC) {
    std::vector<unsigned char> RunlengthArray;
    int RunLen = 0;
    int ZeroBuf = 0;
    for (int i=0; i<AC.columns(); i++) {

        if (AC[0][i] == 0 && RunLen < 15) {
            RunLen++;
            //std::cout << "Run: "<< RunLen << std::endl;
            continue;
        }
        else if (RunLen == 15) {
        ZeroBuf++;
        RunLen = 0;
        continue;
        }
        for (int j=0; j<ZeroBuf; j++) {
            RunlengthArray.push_back(15*16);
        }
        ZeroBuf = 0;
        RunlengthArray.push_back(RunLen*16);
        //std::cout << "Pushback: " << RunLen*16 << "\n";
        RunLen = 0;
    }
    RunlengthArray.push_back(0);
    //for (int i=0; i<RunlengthArray.size(); i++)
    //{

        //std::cout << (int)RunlengthArray[i] << std::endl;
    //}


    return RunlengthArray;
}
unsigned char calcSize(int Value)
{
    unsigned char ValSize = 0;
    for (;;) {
        if (Value == 0) {
            break;
        }
        Value /= 2;
        ValSize++;
    }
    //if (ValSize==0) {ValSize++;}
}
