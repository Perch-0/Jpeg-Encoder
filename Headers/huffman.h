#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdint>
class huffmanTables {
public:
    uint32_t* EHUFCO;
    unsigned char* EHUFSI;
    unsigned char* BITS;
    unsigned char* HUFFVAL;
    huffmanTables(uint32_t* CO, unsigned char* SI, unsigned char* IBITS, unsigned char* IHUFFVAL) {
        EHUFCO = CO;
        EHUFSI = SI;
        BITS = IBITS;
        HUFFVAL = IHUFFVAL;
    }
    void Destroy() {
        delete[] EHUFCO;
        delete[] EHUFSI;
        delete[] BITS;
        delete[] HUFFVAL;
    }

};
void codesize(unsigned int* freq, unsigned char* code_size, int* others);
void Count_BITS (unsigned char* code_size, unsigned char* BITS);
void Adjust_BITS(unsigned char* BITS);
void Gen_size_table (unsigned char* BITS, unsigned char* HUFFSIZE, int& LASTK);
void Gen_code_table (unsigned char* HUFFSIZE, uint32_t* HUFFCODE);
void Sort_input (unsigned char* code_size, unsigned char* HUFFVAL);
void Order_codes (unsigned char* HUFFVAL, unsigned char* HUFFSIZE, uint32_t* HUFFCODE, unsigned char* EHUFSI, uint32_t* EHUFCO, int& LASTK);
huffmanTables GenHuffTables(std::vector<unsigned char> data);

#endif // HUFFMAN_H
