#include <vector>
#include <cmath>
#include <iostream>
#include "ECS.h"
#include <bitset>

std::vector<unsigned char> CreateECS (ECSData PreECS) {
    std::vector<unsigned char> ECS;
    unsigned char res = 0;
    unsigned char bits_written = 0;
    unsigned char pos = 0;
    int bits_read = 0;
    int s = 0;
    int T = 0; int eob=0;
    if (PreECS.data[0] < 0) {PreECS.data[0] = ~(PreECS.data[0]*(-1));} //Negative numbers, first number
    for (int ECSbyte=0; ECSbyte < PreECS.data.size();) {

        //int bits_read = 0;
        for (;;) {

            if (bits_read == PreECS.s[ECSbyte]) {
                //std::cout << "BREAK bits_read\n";
                ECSbyte++;
                //if (PreECS.data[ECSbyte] == -54) {
                //        std::cout << "S\n";}
                //std::cout << "CurData: " << PreECS.data[ECSbyte] << " bits:  " << std::bitset<16>(PreECS.data[ECSbyte]*-1) << std::endl;
                if (ECSbyte == PreECS.data.size()) {break;}
                if (PreECS.data[ECSbyte] < 0) {PreECS.data[ECSbyte] = ~(PreECS.data[ECSbyte]*(-1));} //Negative numbers

                bits_read = 0;
                break;
            }
            if (bits_written == 8) {
                //std::cout << "BREAK bits_written\n";
                break;
            }

            T = PreECS.data[ECSbyte];
            s = PreECS.s[ECSbyte]-bits_read;
            pos = s;

            if (s > 8-bits_written) {s=8-bits_written;}
            res = write_char (s, pos, T, res);
            bits_read += s;
            bits_written += s;
        }

        if (bits_written == 8) {
            ECS.push_back(res);
            if (res == 255) {
                ECS.push_back(0);
            }
            res = 0;
            bits_written = 0;
            //std::cout << "RESET\n";
            //std::cout << std::bitset<8>(ECS[ECS.size()-1]) << std::endl;
            //std::cout << ECSbyte << " / " << PreECS.data.size() << std::endl;
        }
    }
    if (bits_written != 0) {
        ECS.push_back(res << 8-bits_written);
    }
    return ECS;
}
unsigned char write_char (unsigned char s, unsigned char MSB, uint32_t read, unsigned char byte) {
    //std::cout << "READ: " << read << "   s: " << (int)s << "\n";
    //std::cout << "MSB: " << (int)MSB << "\n";
    uint32_t mask = 255;
    mask = std::pow(2, s)-1;
    read >>= MSB-s;
    byte <<= s;
    byte += mask & read;

    //std::cout << "CurMask:" << std::bitset<16>(mask) << std::endl;
    //std::cout << "Curbyte:" << std::bitset<8>(byte) << std::endl;
    //std::cout << "Curread:" << std::bitset<8>(read) << std::endl;



    return byte;
}
