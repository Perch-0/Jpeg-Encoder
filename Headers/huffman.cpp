#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "huffman.h"


/*int main () {



    unsigned char* freq = new unsigned char[256]{};
    unsigned char* code_size = new unsigned char[256]{};
    unsigned char* BITS = new unsigned char[32]{};
    int* others = new int[256]{};

    unsigned char* HUFFSIZE = new unsigned char[256]{};
    unsigned char* HUFFVAL = new unsigned char[256]{};
    uint32_t* HUFFCODE = new uint32_t[256]{};

    uint32_t* EHUFCO = new uint32_t[256]{};
    unsigned char* EHUFSI = new unsigned char[256]{};

    int LASTK;
    std::fill_n(others, 256, -1);
    //Find Freq(v) for v = 0-255 freq(v) for v=256 = 1
    std::string Data =
    "The last line of the code is very important. You may get a case where specifying a quality factor would result in 0 elements."
    "This would mean that when you use the quantization matrix, problems will surface due to divide by zero errors. "
    "Therefore a final check would be to set those 0 elements to 1 so that the effect is ignoring these locations in the output result.";


    for (int i=0; i<Data.length(); i++) {
        //std::cout << (int)Data[i] << "\n";
        freq[(int)Data[i]]++;
    }

    codesize(freq, code_size, others);
    for (int i=48; i<123; i++) {
        std::cout << (char)i << ": " << (int)code_size[i] << "\n";
    }
    Count_BITS(code_size, BITS);
    for (int i=0; i<16; i++) {
        std::cout << "BITS[" << i << "]:" << (int)BITS[i] << "\n";
    }
    Sort_input(code_size, HUFFVAL);
    std::cout << "SORT\n";
    Gen_size_table(BITS, HUFFSIZE, LASTK);
    for (int i=0; i<256; i++) {
        std::cout << "HUFFSIZE[" << i << "]:" << (int)HUFFSIZE[i] << "\n";
    }
    Gen_code_table(HUFFSIZE, HUFFCODE);
    for (int i=0; i<256; i++) {
        std::cout << "HUFFCODE[" << i << "]:" << (int)HUFFCODE[i] << "\n";
    }
    Order_codes(HUFFVAL, HUFFSIZE, HUFFCODE, EHUFSI, EHUFCO, LASTK);
    for (int i=0; i<256; i++) {
        std::cout << "HUFFVAL[" << i << "]:" << (int)HUFFVAL[i] << "\n";
    }
    for (int i=0; i<256; i++) {
        std::cout << "EHUFCO[" << i << "]:" << std::bitset<16>((int)EHUFCO[i]) <<  "            " << "EHUFSI[" << i << "]:" << (int)EHUFSI[i] << "\n";
    }




    std::cout << LASTK;
    delete (HUFFSIZE);
    delete (HUFFVAL);
    delete (HUFFCODE)
    delete (BITS);
    delete (others);
    delete (code_size);
    delete (freq);
    return 0;
}*/
///MAIN_END
///--------------------------------------------------------------------------------
///https://www.w3.org/Graphics/JPEG/itu-t81.pdf
huffmanTables GenHuffTables(std::vector<unsigned char> data) {
    //if all symbols are the same Gen_code_table crashes
    bool allSame = true;
    for (int i=0; i<data.size(); i++) {
        if (data[i] != data[0]) {
            allSame = false;
            break;
        }
    }

    unsigned int* freq = new unsigned int[257]{};
    unsigned char* code_size = new unsigned char[257]{};
    unsigned char* BITS = new unsigned char[33]{};
    int* others = new int[257]{};

    unsigned char* HUFFSIZE = new unsigned char[257]{};
    unsigned char* HUFFVAL = new unsigned char[257]{};
    uint32_t* HUFFCODE = new uint32_t[257]{};

    uint32_t* EHUFCO = new uint32_t[257]{};
    unsigned char* EHUFSI = new unsigned char[257]{};
    int LASTK;
    std::fill_n(others, 257, -1);
    std::fill_n(freq, 257, 0);
    std::fill_n(code_size, 257, 0);
    std::fill_n(BITS, 32, 0);
    std::fill_n(HUFFSIZE, 257, 0);
    std::fill_n(HUFFVAL, 257, 0);
    std::fill_n(HUFFCODE, 257, 0);
    std::fill_n(EHUFCO, 257, 0);
    std::fill_n(EHUFSI, 257, 0);

    //Find Freq(v) for v = 0-255 freq(v) for v=257 = 1


    for (int i=0; i<data.size(); i++) {
        //std::cout << (int)data[i] << "\n";
        freq[(int)data[i]]++;
    }

    freq[256] = 1;
    if (allSame) {
     EHUFCO[data[0]] = 0;
     EHUFSI[data[0]] = 1;
     BITS[1] = 1;
     HUFFVAL[0] = data[0];
    }
    if (!allSame) {
    codesize(freq, code_size, others);
    /*for (int i=0; i<257; i++) {
        std::cout << (int)i << ": " << (int)code_size[i] << "\n";
    }*/
    //std::cout << "codesize FINISHED\n";
    Count_BITS(code_size, BITS);
    //std::cout << "COUNT BITS FINISHED\n";
    Sort_input(code_size, HUFFVAL);
    //std::cout << "Sortinput FINISHED\n";
    Gen_size_table(BITS, HUFFSIZE, LASTK);
    // for (int i=0; i<257; i++) {
    //    std::cout << (char)i << ": " << (int)HUFFVAL[i] << "\n";
    //}

    Gen_code_table(HUFFSIZE, HUFFCODE);
    //std::cout << "GENCODETABLE FINISHED\n";
    Order_codes(HUFFVAL, HUFFSIZE, HUFFCODE, EHUFSI, EHUFCO, LASTK);
    //std::cout << "ORDERCODES FINISHED\n";
    }


    huffmanTables Result(EHUFCO, EHUFSI, BITS, HUFFVAL);
    //std::cout << "Tables FINISHED\n";
    delete (HUFFSIZE);
    //std::cout << "Delete HUFFSIZE FINISHED\n";
    //delete (HUFFVAL);
    //std::cout << "Delete HUFFVAL FINISHED\n";
    delete (HUFFCODE);
    //std::cout << "Delete HUFFCODE FINISHED\n";
    //delete (BITS);
    //std::cout << "Delete BITS FINISHED\n";
    delete (others);
    //std::cout << "Delete others FINISHED\n";
    delete (code_size);
    //std::cout << "Delete code_size FINISHED\n";
    delete (freq);
    //std::cout << "Delete freq FINISHED\n";
    return Result;


}


void codesize(unsigned int* freq, unsigned char* code_size, int* others) {
    ///Figure K.1 – Procedure to find Huffman code sizes
    int Curlow = 257;
    int CurNextLow = 257;
    /**Find V1 for least value of
       FREQ(V1) > 0
       Find V2 for next least value
       of FREQ(V2) > 0*/
    for (int i=0; i<257; i++) {
        //std::cout << (char)i << ": " << (int)freq[i] << "\n";

        if (freq[i] > 0) {
            if (freq[i] < freq[Curlow] || Curlow == 257) {
                CurNextLow = Curlow;
                Curlow = i;
            }
            else if (freq[i] < freq[CurNextLow] || CurNextLow == 257) {
                CurNextLow = i;
            }


        }

    }

    //std::cout << "CL: " << (char)Curlow << " CNL: " << (char)CurNextLow << "\n";
    //std::cout << "_____________________\n\n";

    ///V2 EXISTS?
    if (CurNextLow == 257) {
        return; //no
    }
    //yes

    freq[Curlow] = freq[Curlow]+freq[CurNextLow];
    freq[CurNextLow] = 0;


    for (int i=0; i<1; i++) {
        code_size[Curlow] = code_size[Curlow]+1;
        if (others[Curlow] != -1) {
            Curlow = (int)others[Curlow];
            i--;
            }
    }
    others[Curlow] = CurNextLow;

    for (int i=0; i<1; i++) {
        code_size[CurNextLow] = code_size[CurNextLow]+1;
        if (others[CurNextLow] != -1) {
            CurNextLow = (int)others[CurNextLow];
            i--;
            }
    }


    codesize(freq, code_size, others);


}
void Count_BITS (unsigned char* code_size, unsigned char* BITS) {
    ///Figure K.2 – Procedure to find the number of codes of each size
    for (int i=0; i!=257; i++) {
        if (code_size[i]!=0) {
            BITS[code_size[i]]++;
        }
    }
    Adjust_BITS(BITS);
    return;
}
void Adjust_BITS (unsigned char* BITS) {
    ///Figure K.3 – Procedure for limiting code lengths to 16 bits

    int I=32;
    for (;;) {
        if (BITS[I] > 0) {
            ///TRUE
            int J = I-1;

            for (int i=0; i<1; i++) {
                J--;
                if (!(BITS[J] > 0)) {
                    i--;

                }

            }
            BITS[I] = BITS[I] - 2;
            BITS[I - 1] = BITS[I - 1] + 1;
            BITS[J + 1] = BITS[J + 1] + 2;
            BITS[J] = BITS[J] - 1;
            ///underflow?
            continue;
        }
        ///FALSE

        I--;

        if (I==16) {

            for (;BITS[I] == 0;) {
                I--;
            }
            BITS[I]--;
            break;
        }
        else {
            continue;
        }
        break;
    }
    return;



}
void Sort_input (unsigned char* code_size, unsigned char* HUFFVAL) {
    ///Figure K.4 – Sorting of input values according to code size
    int K = 0;
    for (int I = 1; !(I>32); I++) {
        for (int J = 0; !(J>255); J++) {
            if (code_size[J] == I) {
                HUFFVAL[K] = J;
                K++;
            }
        }
    }
    return;
}
void Gen_size_table (unsigned char* BITS, unsigned char* HUFFSIZE, int& LASTK) {
    ///Figure C.1 – Generation of table of Huffman code sizes
    int K = 0;
    int I = 1;
    int J = 1;
    for (;;) {
        if (!(J>BITS[I])) {
            HUFFSIZE[K] = I;
            K++;
            J++;
            continue;
        }
        I++;
        J=1;
        if (!(I > 16)) {
            continue;
        }
        HUFFSIZE[K] = 0;
        LASTK = K;
        return;
    }
}
void Gen_code_table (unsigned char* HUFFSIZE, uint32_t* HUFFCODE) {
    ///Figure C.2 – Generation of table of Huffman codes
    int K = 0;
    int CODE = 0;
    int SI = HUFFSIZE[0];
    for (;;) {
        HUFFCODE[K] = CODE;
        CODE++;
        K++;
        if (HUFFSIZE[K] == SI) {
            continue;
        }
        if (HUFFSIZE[K] == 0) {
            return;
        }
        for (;;) {
            CODE = CODE << 1;
            SI++;
            if (!(HUFFSIZE[K] == SI)) {
                continue;
            }
            break;
        }
    }




}
void Order_codes (unsigned char* HUFFVAL, unsigned char* HUFFSIZE, uint32_t* HUFFCODE, unsigned char* EHUFSI, uint32_t* EHUFCO, int& LASTK)
{
 for (int K=0; K<LASTK; K++) {
    int I = HUFFVAL[K];
    EHUFCO[I] = HUFFCODE[K];
    EHUFSI[I] = HUFFSIZE[K];
 }
 return;
}







/*
void Generate_size_table {
    if (J > BITS(I)) {
        I = I + 1;
        J = 1   ;
        if (I>16) {
            HUFFsiZZE(K) = 0
            LASTK = K
            return;
        }
        else {return;}
    }
    else {
        HUFFSIZE(K) = I
        K = K + 1;
        J = J + 1;
        return;
    }

}*/
