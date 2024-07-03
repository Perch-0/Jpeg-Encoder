#include <iostream>
#include <fstream>
#include <iomanip>
#include "matrix.h"
#include "huffman.h"
#include "RLE.h"
#include <math.h>
#include <vector>
#include <algorithm>
#include "ECS.h"

using namespace std;

matrixClass YCbCrtoRGB(matrixClass YCbCr, matrixClass Correction);
matrixClass RGBtoYCbCr(int r, int g, int b, matrixClass Correction);
matrixClass ColorCoefficients();
vector<matrixClass> Convert(int ImgSize, int width, int height, unsigned char* data);
matrixClass ResReduction(matrixClass A);
unsigned char*RGBMatrixtoBMPArray(matrixClass RedPixelMatrix, matrixClass GreenPixelMatrix, matrixClass BluePixelMatrix);
void FlipBits(int ImgSize, int width, unsigned char* data);

template <typename T>
T ReadFileheader(unsigned char* fileheader, T location, int offset = 0);
template <typename T>
void ModifyFileheader (unsigned char* fileheader, T location, int value, int offset = 0);

matrixClass RepeatEdges(matrixClass A, int SubSampleFac_H, int SubSampleFac_V);
matrixClass ShiftRange(matrixClass A);
matrixClass DiscreteCosineTransform(matrixClass A);
matrixClass DCT(matrixClass A);

matrixClass Quantization(matrixClass matrix, matrixClass OutputQuantizationMatrix);
matrixClass ZigZag (matrixClass A);
int LenOfData (matrixClass matrix);

void createJpeg (matrixClass QMatrix, int height, int width,
                 huffmanTables HuffTable_Lum_AC,
                 huffmanTables HuffTable_Chrom_AC,
                 huffmanTables HuffTable_Lum_DC,
                 huffmanTables HuffTable_Chrom_DC, vector<unsigned char> ECS,
                 string filename = "Jpeg.jpg");

vector <matrixClass> SplitBlocks(matrixClass A, int SubSampleFac_H, int SubSampleFac_V);
namespace BMP
{
    typedef uint32_t DWORD;
    typedef uint16_t WORD;
    typedef uint8_t BYTE;
    ///CONSTANTS
    const WORD kFileSignature = 0; //WORD 2 bytes
    const DWORD kFileSize = 2; //DWORD 4 bytes
    const DWORD kOffset = 10; //DWORD 4 bytes
    const DWORD kInfoHeaderSize = 14; //DWORD 4 bytes
    const DWORD kWidth = 18; //DWORD 4 bytes
    const WORD kHeight = 22; //DWORD 4 bytes
    const DWORD kPlanes =   26; //WORD 2 bytes
    const WORD kBitsPerPixel = 28; //WORD 2 bytes
    const DWORD kImageSize = 34; //DWORD 4 bytes
}

void createImg(unsigned char* Pixeldata, int width, int height, int length, string filename = "CreateFile.bmp")
{
    using namespace BMP;
    unsigned char* Bitmap = new unsigned char[54+length]{0};
    ModifyFileheader(Bitmap, kFileSignature, 19778);
    ModifyFileheader(Bitmap, kFileSize, 54+48);
    ModifyFileheader(Bitmap, kOffset, 54);
    ModifyFileheader(Bitmap, kInfoHeaderSize, 40);
    ModifyFileheader(Bitmap, kWidth, width);
    ModifyFileheader(Bitmap, kHeight, height);
    ModifyFileheader(Bitmap, kPlanes, 1);
    ModifyFileheader(Bitmap, kBitsPerPixel, 24);
    ModifyFileheader(Bitmap, kImageSize, LenOfData(matrixClass(height, width)));
    cout << hex << ReadFileheader(Bitmap, kFileSignature) << dec << endl;
    for (int i=0; i<length; i++) {
        Bitmap[i+54] = Pixeldata[i];
    }
    FILE * pFile;

    pFile = fopen (filename.c_str(), "wb");
    fwrite (*&Bitmap , sizeof(unsigned char), length+54, pFile);
    //for (int i=0; i<ImgSize; i++) {/*cout << hex << i+54 << " : " << (int)*&data[i] << endl;*/}
    fclose (pFile);

} bool DEBUG = false;
///------------------------------------------------------------------------------------------------------------------------
///MAIN_START
///------------------------------------------------------------------------------------------------------------------------
int main()
{
    FILE* image = fopen("C:/Users/elev/Documents/256x128SingleDots.bmp", "rb");
    cout << fixed << setprecision(0);
    unsigned char fileheader[14];
    int width = 0;
    int height = 0;
    int ImgSize = 0;
    int offset = 0;

    fread(fileheader, sizeof(unsigned char), 14, image);

    offset = ReadFileheader(fileheader, BMP::kOffset);
    unsigned char info[offset-14];
    fread(info, sizeof(unsigned char), offset-14, image);
    width = ReadFileheader(info, BMP::kWidth, -14);
    height = ReadFileheader(info, BMP::kHeight, -14);
    ImgSize = ReadFileheader(info, BMP::kImageSize, -14);
    unsigned char* data = new unsigned char[ImgSize];
    fread(data, sizeof(unsigned char), ImgSize, image);

    FlipBits(ImgSize, width, data);
    //for (int i=0; i<ImgSize; i++) {cout << hex << i+54 << " : " << (int)*&data[i] << endl;}

    vector<matrixClass> PixelMatrices = Convert(ImgSize, width, height, data);
    matrixClass ImgY = PixelMatrices[0];
    matrixClass ImgCb = PixelMatrices[1];
    matrixClass ImgCr = PixelMatrices[2];
    ImgY = FlipMatrix(ImgY, 'x');
    ImgCb = FlipMatrix(ImgCb, 'x');
    ImgCr = FlipMatrix(ImgCr, 'x');
    ImgCb = ResReduction(ImgCb);
    ImgCr = ResReduction(ImgCr);


    cout << "ImgY\n";
    if (DEBUG) {Print_matrix(ImgY);}
    cout << "ImgCb\n";
    if (DEBUG){Print_matrix(ImgCb);}
    cout << "ImgCr\n";
    if (DEBUG){Print_matrix(ImgCr);}

    matrixClass temp = RepeatEdges(ImgY, 16, 16);

    cout << "Rows: " << temp.rows() << "Cols: " << temp.columns() << endl;

    int VSamplingFac_Y = 2;
    int HSamplingFac_Y = 2;
    int VSamplingFac_Cb = 1;
    int HSamplingFac_Cb = 1;
    int VSamplingFac_Cr = 1;
    int HSamplingFac_Cr = 1;



    vector<matrixClass> ChannelY = SplitBlocks(ImgY, HSamplingFac_Y, VSamplingFac_Y);
    vector<matrixClass> ChannelCb = SplitBlocks(ImgCb, HSamplingFac_Cb, VSamplingFac_Cb);
    vector<matrixClass> ChannelCr = SplitBlocks(ImgCr, HSamplingFac_Cr, VSamplingFac_Cr);

    height += (16-(height%16))%16;
    width += (16-(width%16))%16;

    cout << "ChannelY\n";
    for (int i=0; i<ChannelY.size()&&DEBUG; i++) {
        Print_matrix(ChannelY[i]);
    }






    int QualityFactor = 80;
    cout << "DCT" << "\n";
    for (int i=0; i<ChannelY.size(); i++) {
        ChannelY[i] = DCT(ChannelY[i]);
    }
    for (int i=0; i<ChannelCb.size(); i++) {
        ChannelCb[i] = DCT(ChannelCb[i]);
    }
    for (int i=0; i<ChannelCr.size(); i++) {
        ChannelCr[i] = DCT(ChannelCr[i]);
    }
    cout << "ChannelY_DCT\n";
    for (int i=0; i<ChannelY.size()&&DEBUG; i++) {
        Print_matrix(ChannelY[i]);
    }
    cout << "ChannelCB_DCT\n";
    for (int i=0; i<ChannelCb.size()&&DEBUG; i++) {
        Print_matrix(ChannelCb[i]);
    }
    cout << "ChannelCR_DCT\n";
    for (int i=0; i<ChannelCr.size()&&DEBUG; i++) {
        Print_matrix(ChannelCr[i]);
    }
    cout << "Quantization" << "\n";
    matrixClass OutputQuantizationMatrix(8, 8);
    {
    matrixClass BaseQuantizationMatrix(8,8);
    //https://stackoverflow.com/questions/29215879/how-can-i-generalize-the-quantization-matrix-in-jpeg-compression
    BaseQuantizationMatrix.matrix =
      {{16,  11,  10,  16,  24,  40,  51,  61},
        {12,  12,  14,  19,  26,  58,  60,  55},
        {14,  13,  16,  24,  40,  57,  69,  56},
        {14,  17,  22,  29,  51,  87,  80,  62},
        {18,  22,  37,  56,  68, 109, 103,  77},
        {24,  35,  55,  64,  81, 104, 113,  92,},
        {49,  64,  78,  87, 103, 121, 120, 101},
        {72,  92,  95,  98, 112, 100, 103,  99}};
    double S = 0;
    if (QualityFactor < 50) {
        S = 5000/QualityFactor;
    }
    else {
        S = 200 - 2*QualityFactor;
    }

    OutputQuantizationMatrix.matrix = BaseQuantizationMatrix.matrix;
    OutputQuantizationMatrix = ScalarMultiply(OutputQuantizationMatrix, S);
    OutputQuantizationMatrix = ScalarAdd(OutputQuantizationMatrix, 50);
    OutputQuantizationMatrix = ScalarDivide(OutputQuantizationMatrix, 100);

    for (int i=0; i<OutputQuantizationMatrix.rows(); i++) {
        for (int j=0; j<OutputQuantizationMatrix.columns(); j++) {
                OutputQuantizationMatrix[i][j] = floor(OutputQuantizationMatrix[i][j]);
            if (OutputQuantizationMatrix[i][j] == 0) {
                OutputQuantizationMatrix[i][j] = 1;
            }
        }
    }

    }
    for (int i=0; i<ChannelY.size(); i++) {
        ChannelY[i] = Quantization(ChannelY[i], OutputQuantizationMatrix);
    }
    for (int i=0; i<ChannelCb.size(); i++) {
        ChannelCb[i] = Quantization(ChannelCb[i], OutputQuantizationMatrix);
    }
    for (int i=0; i<ChannelCr.size(); i++) {
        ChannelCr[i] = Quantization(ChannelCr[i], OutputQuantizationMatrix);
    }
    cout << "ChannelYAFterQunants\n";
    for (int i=0; i<ChannelY.size()&&DEBUG; i++) {
        Print_matrix(ChannelY[i]);
    }

    OutputQuantizationMatrix = ZigZag(OutputQuantizationMatrix); //???
    cout << "QuantizationTable\n"; Print_matrix(OutputQuantizationMatrix);
    cout << "ZigZag" << "\n";
    for (int i=0; i<ChannelY.size(); i++) {
        ChannelY[i] = ZigZag(ChannelY[i]);
    }
    for (int i=0; i<ChannelCb.size(); i++) {
        ChannelCb[i] = ZigZag(ChannelCb[i]);
    }
    for (int i=0; i<ChannelCr.size(); i++) {
        ChannelCr[i] = ZigZag(ChannelCr[i]);
    }


    cout << "RunSize" << "\n";



    vector<vector<unsigned char>> RunSize_Y;
    RunSize_Y.resize(ChannelY.size());
    for (int i=0; i<ChannelY.size(); i++) {
        vector<unsigned char> tmp = GetRunlengthSize(ChannelY[i]);
        if (tmp.size() == 1) {

            RunSize_Y[i].insert(RunSize_Y[i].begin(), tmp.begin(), tmp.end());
        }
        else {

            RunSize_Y[i].insert(RunSize_Y[i].begin(), tmp.begin()+1, tmp.end());
        }

    }
    vector<vector<unsigned char>> RunSize_Cb;
    RunSize_Cb.resize(ChannelCb.size());
    for (int i=0; i<ChannelCb.size(); i++) {
        vector<unsigned char> tmp = GetRunlengthSize(ChannelCb[i]);
        if (tmp.size() == 1) {

            RunSize_Cb[i].insert(RunSize_Cb[i].begin(), tmp.begin(), tmp.end());
        }
        else {

            RunSize_Cb[i].insert(RunSize_Cb[i].begin(), tmp.begin()+1, tmp.end());
        }

    }

    vector<vector<unsigned char>> RunSize_Cr;
    RunSize_Cr.resize(ChannelCr.size());
    for (int i=0; i<ChannelCr.size(); i++) {
        vector<unsigned char> tmp = GetRunlengthSize(ChannelCr[i]);
        if (tmp.size() == 1) {

            RunSize_Cr[i].insert(RunSize_Cr[i].begin(), tmp.begin(), tmp.end());
        }
        else {

            RunSize_Cr[i].insert(RunSize_Cr[i].begin(), tmp.begin()+1, tmp.end());
        }

    }

    for (int i=0; i<RunSize_Y.size()&&DEBUG; i++) {
        for (int j=0; j<RunSize_Y[i].size(); j++) {
            cout << (int)RunSize_Y[i][j] << endl;
        }
    }

    //AC vector


    vector<vector<int>> VAC_Y;
    VAC_Y.resize(RunSize_Y.size());
    ///-----------------------
    for (int block=0; block<ChannelY.size(); block++) {
    int RunLen = 0;
    int ZeroBuf = 0;

        for (int i=1; i<ChannelY[block].columns(); i++) {

            if (ChannelY[block][0][i] == 0 && RunLen < 15) {
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
                VAC_Y[block].push_back(0);
                ZeroBuf = 0;
            }
            VAC_Y[block].push_back(ChannelY[block][0][i]);

            //std::cout << "Pushback: " << RunLen*16 << "\n";
            RunLen = 0;

        }
    }
    ///--------------------

    vector<vector<int>> VAC_Cb;
    VAC_Cb.resize(RunSize_Cb.size());
    ///-----------------------
    for (int block=0; block<ChannelCb.size(); block++) {
    int RunLen = 0;
    int ZeroBuf = 0;

        for (int i=1; i<ChannelCb[block].columns(); i++) {

            if (ChannelCb[block][0][i] == 0 && RunLen < 15) {
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
                VAC_Cb[block].push_back(0);
                ZeroBuf = 0;
            }
            VAC_Cb[block].push_back(ChannelCb[block][0][i]);

            //std::cout << "Pushback: " << RunLen*16 << "\n";
            RunLen = 0;

        }
    }
    ///--------------------

    vector<vector<int>> VAC_Cr;
    VAC_Cr.resize(RunSize_Cr.size());
    ///-----------------------
    for (int block=0; block<ChannelCr.size(); block++) {
    int RunLen = 0;
    int ZeroBuf = 0;

        for (int i=1; i<ChannelCr[block].columns(); i++) {

            if (ChannelCr[block][0][i] == 0 && RunLen < 15) {
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
                VAC_Cr[block].push_back(0);
                ZeroBuf = 0;
            }
            VAC_Cr[block].push_back(ChannelCr[block][0][i]);
            //std::cout << "Pushback: " << RunLen*16 << "\n";
            RunLen = 0;

        }
    }
    ///--------------------
    cout << "VACY\n" << endl;
    for (int i=0; i<VAC_Y.size(); i++) {
            cout << endl;
        for (int j=0; j<VAC_Y[i].size(); j++) {
        //cout << (int)VAC_Y[i][j] << ",  ";
        }

    }
    if (DEBUG) {
    cout << "\n\nCHANNEL_Y\n" << endl;
    for (int block=0; block<ChannelY.size(); block++) {

        Print_matrix(ChannelY[block]);
    }

    cout << "\n\nCHANNEL_Cb\n" << endl;
    for (int block=0; block<ChannelCb.size(); block++) {

        Print_matrix(ChannelCb[block]);
    }

    cout << "\n\nCHANNEL_Cr\n" << endl;
    for (int block=0; block<ChannelCr.size(); block++) {

        Print_matrix(ChannelCr[block]);
    }
    }

    ///AC matrices
    cout << "AC" << "\n";
    matrixClass AC_Y(1, 63*ChannelY.size());
    for (int i=0; i<ChannelY.size(); i++) {
        for (int j=0; j<63; j++) {
        AC_Y[0][63*i+j] = ChannelY[i][0][j+1];}
    }
    matrixClass AC_Cb(1, 63*ChannelCb.size());
    for (int i=0; i<ChannelCb.size(); i++) {
        for (int j=0; j<63; j++) {
        AC_Cb[0][63*i+j] = ChannelCb[i][0][j+1];}
    }
    matrixClass AC_Cr(1, 63*ChannelCr.size());
    for (int i=0; i<ChannelCr.size(); i++) {
        for (int j=0; j<63; j++) {
        AC_Cr[0][63*i+j] = ChannelCr[i][0][j+1];}
    }
    ///DC matrices
    cout << "DC" << "\n";
    vector<int> DC_Y(ChannelY.size(), 0);
    for (int i=0; i<ChannelY.size(); i++) {
        DC_Y[i] = ChannelY[i][0][0];
    }
    vector<int> DC_Cb(ChannelCb.size(), 0);
    for (int i=0; i<ChannelCb.size(); i++) {
        DC_Cb[i] = ChannelCb[i][0][0];
    }
    vector<int> DC_Cr(ChannelCr.size(), 0);
    for (int i=0; i<ChannelCr.size(); i++) {
        DC_Cr[i] = ChannelCr[i][0][0];
    }


    DC_Y = DCDiff(DC_Y);
    DC_Cb = DCDiff(DC_Cb);
    DC_Cr = DCDiff(DC_Cr);

    const int LUMINANCE = 1;
    const int CHROMINANCE_CB = 2;
    const int CHROMINANCE_CR = 3;

    vector<int> order = {LUMINANCE, LUMINANCE, LUMINANCE, LUMINANCE, CHROMINANCE_CB, CHROMINANCE_CR};
    /*int sum=DC_Y.size()+DC_Cb.size()+DC_Cr.size();
    for (int i=0; i<sum; i++) {
        static int y = 0;
        static int cb = 0;
        static int cr = 0;
        static int PrevVal = 0;

        int j = order[i%order.size()];

        if (j == LUMINANCE) { cout << "LUMINANCE\n";
            DC_Y[y] = -1*DC_Y[y] + PrevVal;
            PrevVal = DC_Y[y];
            y++;
        }
        if (j == CHROMINANCE_CB) { cout << "CHROMINANCE_CB\n";
            DC_Cb[cb] = -1*DC_Cb[cb] + PrevVal;
            PrevVal = DC_Cb[cb];
            cb++;
        }
        if (j == CHROMINANCE_CR) { cout << "CHROMINANCE_CR\n";
            DC_Cr[cr] = -1*DC_Cr[cr] + PrevVal;
            PrevVal = DC_Cr[cr];
            cr++;
        }
    }*/



    ///Size Lum
    vector<unsigned char> Size_Lum;
    for (int i=0; i<DC_Y.size(); i++) {
        Size_Lum.push_back(calcSize(DC_Y[i]));
    }
    ///Size Chrom
    vector<unsigned char> Size_Cb;
    vector<unsigned char> Size_Cr;
    for (int i=0; i<DC_Cb.size(); i++) {
        Size_Cb.push_back(calcSize(DC_Cb[i]));
    }
    for (int i=0; i<DC_Cr.size(); i++) {
        Size_Cr.push_back(calcSize(DC_Cr[i]));
    }
    vector<unsigned char> Size_Chrom;
    Size_Chrom.insert(Size_Chrom.end(), Size_Cb.begin(), Size_Cb.end());
    Size_Chrom.insert(Size_Chrom.end(), Size_Cr.begin(), Size_Cr.end());






    vector<unsigned char> RunSize_Chrom;
    for (int i=0; i<RunSize_Cb.size(); i++) {
        RunSize_Chrom.insert(RunSize_Chrom.end(), RunSize_Cb[i].begin(), RunSize_Cb[i].end());
    }

    for (int i=0; i<RunSize_Cr.size(); i++) {
        RunSize_Chrom.insert(RunSize_Chrom.end(), RunSize_Cr[i].begin(), RunSize_Cr[i].end());
    }




    vector<unsigned char> RunSize_Lum;
    for (int i=0; i<RunSize_Y.size(); i++) {
        RunSize_Lum.insert(RunSize_Lum.end(), RunSize_Y[i].begin(), RunSize_Y[i].end());
    }


    ///HUFFMAN TABLES
    huffmanTables HuffTable_Lum_AC = GenHuffTables(RunSize_Lum);
    huffmanTables HuffTable_Chrom_AC = GenHuffTables(RunSize_Chrom);
    huffmanTables HuffTable_Lum_DC = GenHuffTables(Size_Lum);
    huffmanTables HuffTable_Chrom_DC = GenHuffTables(Size_Chrom);
    //Print_matrix(AC_Y);

    /*cout << "HUFFMAN TABLES\n";
    for (int i=0; i<256; i++) {
        if(HuffTable_Lum_DC.EHUFSI[i] !=0) {
        cout << "EHUFCO" << i << ":"<<bitset<16>(HuffTable_Lum_DC.EHUFCO[i]) << "    "<< "EHUFSI" << i << ":"<<bitset<16>(HuffTable_Lum_DC.EHUFSI[i]) << endl;

        }
    }
    for (int i=0; i<256; i++) {
        //if(HuffTable_Lum_DC.EHUFSI[i] !=0) {
        cout << "VAL" << i << ":"<<bitset<16>(HuffTable_Lum_DC.HUFFVAL[i]) << "    "<< "EHUFSI" << i << ":"<<bitset<16>(HuffTable_Lum_DC.EHUFSI[i]) << endl;

        //}
    }*/
    if (DEBUG) {
    cout << "RunSize_Lum:\n";
    for (int i=0; i<RunSize_Lum.size(); i++) {
        cout << (int)RunSize_Lum[i] << endl;
    }
    cout << "RunSize_Cb:\n";
    for (int i=0; i<RunSize_Cb.size(); i++) {
        cout << "Block: " << i << endl;
        for (int j=0; j<RunSize_Cb[i].size(); j++) {

            cout << (int)RunSize_Cb[i][j] << endl;
        }
    }
    cout << "RunSize_Cr:\n";
    for (int i=0; i<RunSize_Cr.size(); i++) {

        for (int j=0; j<RunSize_Cr[i].size(); j++) {

            cout << (int)RunSize_Cr[i][j] << endl;
        }
    }
    }

    cout << "END\n\n";
    int blocks = 0;
    ///interleaved
    ECSData EntropyData(0, 0);

    for (int i=0; i<RunSize_Cr.size(); i++) {
        int coeffs = 0;
        for (int j=0; j<RunSize_Cr[i].size(); j++) {
            if (RunSize_Cr[i][j] == 0) {
                continue;
            }
            int watch = RunSize_Cr[i][j];
            unsigned char zero = (RunSize_Cr[i][j]>>4);
            coeffs += 1+zero;
        }
        if (coeffs > 63) {system("pause");}
        cout << coeffs << endl;
    }

    auto PushBlock = [&](int component){
        static int block_lum = 0;
        static int block_cb = 0;
        static int block_cr = 0;

        if (component == LUMINANCE) {
            ///Size (VLC)
            EntropyData.data.push_back(HuffTable_Lum_DC.EHUFCO[Size_Lum[block_lum]]);
            EntropyData.s.push_back(HuffTable_Lum_DC.EHUFSI[Size_Lum[block_lum]]); //cout << "VAL DC:\n";
            ///DC value (VLI)
            EntropyData.data.push_back(DC_Y[block_lum]);
            EntropyData.s.push_back(calcSize(DC_Y[block_lum]));



            for (int AC=0; AC<RunSize_Y[block_lum].size(); AC++) { //cout << "AC:\n";
            ///RunSize (VLC)
            EntropyData.data.push_back(HuffTable_Lum_AC.EHUFCO[RunSize_Y[block_lum][AC]]);
            EntropyData.s.push_back(HuffTable_Lum_AC.EHUFSI[RunSize_Y[block_lum][AC]]);
            ///AC value (VLI)
            if (RunSize_Y[block_lum][AC] != 0) {
            EntropyData.data.push_back(VAC_Y[block_lum][AC]);
            EntropyData.s.push_back(calcSize(VAC_Y[block_lum][AC]));
            }
            }
        block_lum++;
        }
        if (component == CHROMINANCE_CB) {
            ///Size (VLC)
            EntropyData.data.push_back(HuffTable_Chrom_DC.EHUFCO[Size_Cb[block_cb]]);
            EntropyData.s.push_back(HuffTable_Chrom_DC.EHUFSI[Size_Cb[block_cb]]);
            ///DC value (VLI)
            EntropyData.data.push_back(DC_Cb[block_cb]);
            EntropyData.s.push_back(calcSize(DC_Cb[block_cb]));



            for (int AC=0; AC<RunSize_Cb[block_cb].size(); AC++) {
            ///RunSize (VLC)
            EntropyData.data.push_back(HuffTable_Chrom_AC.EHUFCO[RunSize_Cb[block_cb][AC]]);
            EntropyData.s.push_back(HuffTable_Chrom_AC.EHUFSI[RunSize_Cb[block_cb][AC]]);
            ///AC value (VLI)
            if (RunSize_Cb[block_cb][AC] != 0) {
            EntropyData.data.push_back(VAC_Cb[block_cb][AC]);
            EntropyData.s.push_back(calcSize(VAC_Cb[block_cb][AC]));
            }
            }
        block_cb++;
        }
        if (component == CHROMINANCE_CR) {
            ///Size (VLC)
            EntropyData.data.push_back(HuffTable_Chrom_DC.EHUFCO[Size_Cr[block_cr]]);
            EntropyData.s.push_back(HuffTable_Chrom_DC.EHUFSI[Size_Cr[block_cr]]);
            ///DC value (VLI)
            EntropyData.data.push_back(DC_Cr[block_cr]);
            EntropyData.s.push_back(calcSize(DC_Cr[block_cr]));



            for (int AC=0; AC<RunSize_Cr[block_cr].size(); AC++) {
            ///RunSize (VLC)
            EntropyData.data.push_back(HuffTable_Chrom_AC.EHUFCO[RunSize_Cr[block_cr][AC]]);
            EntropyData.s.push_back(HuffTable_Chrom_AC.EHUFSI[RunSize_Cr[block_cr][AC]]);
            ///AC value (VLI)
            if (RunSize_Cr[block_cr][AC] != 0) {
            EntropyData.data.push_back(VAC_Cr[block_cr][AC]);
            EntropyData.s.push_back(calcSize(VAC_Cr[block_cr][AC]));
            }
            }
            block_cr++;
        }
        return 0;
    };


    int blocksum = ChannelY.size()+ChannelCb.size()+ChannelCr.size();

    for (int block=0; block<blocksum; block++) {
        PushBlock(order[block%order.size()]);
        //cout << order[block%order.size()] << "push" << endl;

    }



    cout << "Size_Lum:\n";
    for (int i=0; i<Size_Lum.size()&&DEBUG; i++) {
        cout << (int)Size_Lum[i] << endl;
    }
    cout << "Size_Chrom:\n";
    for (int i=0; i<Size_Chrom.size()&&DEBUG; i++) {
        cout << (int)Size_Chrom[i] << endl;
    }



    cout << "BLOCKS: " << blocks << endl;
    for (int i=0; i<DC_Y.size()&&DEBUG; i++) {
        cout << DC_Y[i] << endl;
    }
    cout << endl << endl;

    cout << "ECS:       S:\n";
    for (int i=0; i<EntropyData.data.size()&&DEBUG; i++) {
        cout << setfill(' ') << setw(4) << EntropyData.data[i] << "       " << EntropyData.s[i] << endl;
    }
    cout << endl << endl << endl;
    cout << "CreateECS\n";
    vector<unsigned char> ECS = CreateECS(EntropyData);




    FILE * pFile;

    pFile = fopen ("JpegBin.txt", "wb");
    fwrite (static_cast<void*>(ECS.data()) , sizeof(unsigned char), ECS.size(), pFile);
    //for (int i=0; i<ImgSize; i++) {/*cout << hex << i+54 << " : " << (int)*&data[i] << endl;*/}
    fclose (pFile);



    createJpeg(OutputQuantizationMatrix, height, width, HuffTable_Lum_AC, HuffTable_Chrom_AC, HuffTable_Lum_DC, HuffTable_Chrom_DC, ECS);
    //
    /*for(;;) {



    }*/

    /*matrixClass MTest = RepeatEdges(Test[0]);
    //matrixClass MTest = ResReduction(Test[0]);
    data = RGBMatrixtoBMPArray(MTest, MTest, MTest);
    FlipBits(LenOfData(MTest), MTest.columns(), data);
    createImg(data, MTest.columns(), MTest.rows(), LenOfData(MTest), "ConvertTest.bmp");
    cout << "Return";*/
    //matrixClass Mtest(height, width);
    //Mtest = PixelMatrices[0];
    //Mtest = FlipMatrix(Mtest, 'x');
    /*
    Mtest = ShiftRange(Mtest);
    //Print_matrix(Mtest);
    //Quantization(Mtest, 80);
    //Quantization(Mtest, 50);
    Print_matrix(Entropycoding(Quantization(DCT(Mtest), 100)));
    Entropycoding(Mtest);*/
    /*matrixClass Ntest(8, 8);
    Ntest.matrix =
              {{-76, -73, -67, -62, -58, -67, -64, -55},
               {-65, -69, -73, -38, -19, -43, -59, -56},
               {-66, -69, -60, -15, +16, -24, -62, -55},
               {-65, -70, -57, - 6, +26, -22, -58, -59},
               {-61, -67, -60, -24, - 2, -40, -60, -58},
               {-49, -63, -68, -58, -51, -60, -70, -53},
               {-43, -57, -64, -69, -73, -67, -63, -45},
               {-41, -49, -59, -60, -63, -52, -50, -34}};
    //Ntest = ShiftRange(Ntest);
    Print_matrix(Ntest);
    Print_matrix(DCT(Ntest));*/
    //Print_matrix(Mtest);
    /*cout << "-------------------------------------------------------------------\n";
    vector<matrixClass> test = SplitBlocks(Mtest);
    for (int i=0; i<test.size(); i++) {
        Print_matrix(test[i]);
    }*/

    cout << endl;
    for (int i=0; i<RunSize_Y[0].size(); i++) {
        cout << dec << (int)RunSize_Y[0][i] << "  ";
    }
    cout << "\n";
    Print_matrix(ChannelY[0]);
    return 0;
}
///------------------------------------------------------------------------------------------------------------------------
///MAIN_END
///------------------------------------------------------------------------------------------------------------------------
void createJpeg (matrixClass QMatrix, int height, int width,
                 huffmanTables HuffTable_Lum_AC,
                 huffmanTables HuffTable_Chrom_AC,
                 huffmanTables HuffTable_Lum_DC,
                 huffmanTables HuffTable_Chrom_DC, vector<unsigned char> ECS,
                 string filename) {
    std::vector<unsigned char> Jpeg;
    Jpeg.insert(Jpeg.end(), {0xFF, 0xD8}); ///Start Of Image
    Jpeg.insert(Jpeg.end(), {0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x01, 0x00, 0x48, 0x00, 0x48, 0x00, 0x00}); ///APP0
    Jpeg.insert(Jpeg.end(), {0xFF, 0xDB, 0x00, 0x43, 0x00}); ///DQT Lum
    for (int i=0; i<QMatrix.rows(); i++) {
        for (int j=0; j<QMatrix.columns(); j++) {
            Jpeg.push_back(QMatrix[i][j]);
        }
    }
    Jpeg.insert(Jpeg.end(), {0xFF, 0xDB, 0x00, 0x43, 0x01}); ///DQT Chrom
    for (int i=0; i<QMatrix.rows(); i++) {
        for (int j=0; j<QMatrix.columns(); j++) {
            Jpeg.push_back(QMatrix[i][j]);
        }
    }
    vector<unsigned char> height_width;
    for (;width > 0;) {
        height_width.insert(height_width.begin(), width % 256);
        width = width / 256;
    }
        if (height_width.size() == 1) {
            height_width.insert(height_width.begin(), 0);
        }
    for (;height > 0;) {
        height_width.insert(height_width.begin(), height % 256);
        height = height / 256;
    }
        if (height_width.size() == 3) {
            height_width.insert(height_width.begin(), 0);
        }
    vector<unsigned char> SOF_length;
    {
        int length = height_width.size() + 16;
        for (;length > 0;) {
        SOF_length.insert(SOF_length.begin(), length % 256);
        length = length / 256;
        }

    }
    Jpeg.insert(Jpeg.end(), {0xFF, 0xC0, 0x00, 0x11, 0x08}); ///SOF
    Jpeg.insert(Jpeg.end(), height_width.begin(), height_width.end());
    Jpeg.insert(Jpeg.end(), {0x03}); ///Components
    ///Components (Component, Horizontal sampling factor, Vertical sampling factor, table id)
    Jpeg.insert(Jpeg.end(), {0x01, 0x22, 0x00}); //Lum
    Jpeg.insert(Jpeg.end(), {0x02, 0x11, 0x01}); //Cb
    Jpeg.insert(Jpeg.end(), {0x03, 0x11, 0x01}); //Cr

    ///DHT

    ///DHT Lum_DC
    {
        Jpeg.insert(Jpeg.end(), {0xFF, 0xC4});
        vector<unsigned char> DHT;
        vector<unsigned char> DHT_length;
        int length = 0;
        for (int i=1;i<17;i++) {
            length += HuffTable_Lum_DC.BITS[i];
            DHT.insert(DHT.end(), HuffTable_Lum_DC.BITS[i]);
        }
        for(int i=0;i<length;i++) {
            DHT.insert(DHT.end(), HuffTable_Lum_DC.HUFFVAL[i]);
        }
            length += 19;
            for (;length > 0;) {
            DHT_length.insert(DHT_length.begin(), length % 256);
            length = length / 256;
            }
            if (DHT_length.size() == 1) {
                DHT_length.insert(DHT_length.begin(), 0);
            }

        Jpeg.insert(Jpeg.end(), DHT_length.begin(), DHT_length.end());
        Jpeg.insert(Jpeg.end(), {0x00});
        Jpeg.insert(Jpeg.end(), DHT.begin(), DHT.end());
    }

    ///DHT Lum_AC
    {
        Jpeg.insert(Jpeg.end(), {0xFF, 0xC4});
        vector<unsigned char> DHT;
        vector<unsigned char> DHT_length;
        int length = 0;
        for (int i=1;i<17;i++) {
            length += HuffTable_Lum_AC.BITS[i];
            DHT.insert(DHT.end(), HuffTable_Lum_AC.BITS[i]);
        }
         for(int i=0;i<length;i++) {
            DHT.insert(DHT.end(), HuffTable_Lum_AC.HUFFVAL[i]);
        }

            length += 19;
            for (;length > 0;) {
            DHT_length.insert(DHT_length.begin(), length % 256);
            length = length / 256;
            }
            if (DHT_length.size() == 1) {
                DHT_length.insert(DHT_length.begin(), 0);
            }

        Jpeg.insert(Jpeg.end(), DHT_length.begin(), DHT_length.end());
        Jpeg.insert(Jpeg.end(), {0x10});
        Jpeg.insert(Jpeg.end(), DHT.begin(), DHT.end());
    }

    ///DHT Chrom_DC
    {
        Jpeg.insert(Jpeg.end(), {0xFF, 0xC4});
        vector<unsigned char> DHT;
        vector<unsigned char> DHT_length;
        int length = 0;
        for (int i=1;i<17;i++) {
            length += HuffTable_Chrom_DC.BITS[i];
            DHT.insert(DHT.end(), HuffTable_Chrom_DC.BITS[i]);
        }
            for(int i=0;i<length;i++) {

            DHT.insert(DHT.end(), HuffTable_Chrom_DC.HUFFVAL[i]);
        }
            length += 19;
            for (;length > 0;) {
            DHT_length.insert(DHT_length.begin(), length % 256);
            length = length / 256;
            }
            if (DHT_length.size() == 1) {
                DHT_length.insert(DHT_length.begin(), 0);
            }

        Jpeg.insert(Jpeg.end(), DHT_length.begin(), DHT_length.end());
        Jpeg.insert(Jpeg.end(), {0x01});
        Jpeg.insert(Jpeg.end(), DHT.begin(), DHT.end());
    }

    ///DHT Chrom_AC
    {
        Jpeg.insert(Jpeg.end(), {0xFF, 0xC4});
        vector<unsigned char> DHT;
        vector<unsigned char> DHT_length;
        int length = 0;
        for (int i=1;i<17;i++) {
            length += HuffTable_Chrom_AC.BITS[i];
            DHT.insert(DHT.end(), HuffTable_Chrom_AC.BITS[i]);
        }
            for(int i=0;i<length;i++) {

            DHT.insert(DHT.end(), HuffTable_Chrom_AC.HUFFVAL[i]);
        }
            length += 19;
            for (;length > 0;) {
            DHT_length.insert(DHT_length.begin(), length % 256);
            length = length / 256;
            }
            if (DHT_length.size() == 1) {
                DHT_length.insert(DHT_length.begin(), 0);
            }

        Jpeg.insert(Jpeg.end(), DHT_length.begin(), DHT_length.end());
        Jpeg.insert(Jpeg.end(), {0x11});
        Jpeg.insert(Jpeg.end(), DHT.begin(), DHT.end());
    }

    ///SOS
    Jpeg.insert(Jpeg.end(), {0xFF, 0xDA, 0x00, 0x0C, 0x03});
    Jpeg.insert(Jpeg.end(), {0x01, 0x00}); //Lum Lum_DC Lum_AC
    Jpeg.insert(Jpeg.end(), {0x02, 0x11}); //Cb Chrom_DC Chrom_AC
    Jpeg.insert(Jpeg.end(), {0x03, 0x11}); //Cr Chrom_DC Chrom_AC
    Jpeg.insert(Jpeg.end(), {0x00, 0x3F}); //Spectral Selection
    Jpeg.insert(Jpeg.end(), {0x00}); // Successive Approximation
    ///ECS
    Jpeg.insert(Jpeg.end(), ECS.begin(), ECS.end());
    Jpeg.insert(Jpeg.end(), {0xFF, 0xD9});



    cout << "HuffTable_Lum_DC\n";
    for (int i=0; i<256; i++) {
        if (HuffTable_Lum_DC.EHUFSI[i] != 0) {
        cout << " EHUFCO" << bitset<8>(i) << ": " << bitset<16>(HuffTable_Lum_DC.EHUFCO[i]) << " EHUFSI" << i << ": " << (int)HuffTable_Lum_DC.EHUFSI[i];
        cout << endl;

        }

    }
    cout << "HuffTable_Lum_AC\n";
    for (int i=0; i<256; i++) {
        if (HuffTable_Lum_AC.EHUFSI[i] != 0) {
        cout << " EHUFCO" << bitset<8>(i) << ": " << bitset<16>(HuffTable_Lum_AC.EHUFCO[i]) << " EHUFSI" << i << ": " << (int)HuffTable_Lum_AC.EHUFSI[i];
        cout << endl;

        }

    }
    cout << "HuffTable_Chrom_DC\n";
    for (int i=0; i<256; i++) {
        if (HuffTable_Chrom_DC.EHUFSI[i] != 0) {
        cout << " EHUFCO" << bitset<8>(i) << ": " << bitset<16>(HuffTable_Chrom_DC.EHUFCO[i]) << " EHUFSI" << i << ": " << (int)HuffTable_Chrom_DC.EHUFSI[i];
        cout << endl;

        }

    }cout << "HuffTable_Chrom_AC\n";
    for (int i=0; i<256; i++) {
        if (HuffTable_Chrom_AC.EHUFSI[i] != 0) {
        cout << " EHUFCO" << bitset<8>(i) << ": " << bitset<16>(HuffTable_Chrom_AC.EHUFCO[i]) << " EHUFSI" << i << ": " << (int)HuffTable_Chrom_AC.EHUFSI[i];
        cout << endl;

        }

    }


    for(int i=0, j=0, e = 3, s=0; i<Jpeg.size() ; i++, j++, e--) {
        if (Jpeg[i] == 255 && Jpeg[i+1] != 216) {
            e += 2+(Jpeg[i+2]*256)+Jpeg[i+3];
            cout << endl;
            cout << j;
            cout << endl << endl;
            j = -2;
        }
        /*if (e == 1 && Jpeg[i+1] != 255) {
            cout << "\nECS:\n";
            s=1;
        }
        if (s==1) {
            cout << bitset<8>(Jpeg[i]) << endl;
            continue;
        }*/
        cout << uppercase << hex << setfill('0') << setw(2) << (int)Jpeg[i] << "  ";
    }

    ///FWRITE_JPEG
    FILE * pFile;

    pFile = fopen (filename.c_str(), "wb");
    fwrite (static_cast<void*>(Jpeg.data()) , sizeof(unsigned char), Jpeg.size(), pFile);
    //for (int i=0; i<ImgSize; i++) {/*cout << hex << i+54 << " : " << (int)*&data[i] << endl;*/}
    fclose (pFile);


}

vector <matrixClass> SplitBlocks(matrixClass A, int SubSampleFac_H, int SubSampleFac_V) {
    matrixClass ImageMatrix = RepeatEdges(A, SubSampleFac_H, SubSampleFac_V);
    vector<matrixClass> BlockVector;
    cout << "rows: " << ImageMatrix.rows() << "  columns: " << ImageMatrix.columns() << endl;
    int NofBlocks = (ImageMatrix.rows()*ImageMatrix.columns())/64;


    for (int i=0; i<NofBlocks; i++) {
        BlockVector.push_back(matrixClass(8, 8));
    }

    for (int r=0; r<ImageMatrix.rows(); r++) {
        for (int c=0; c<ImageMatrix.columns(); c++) {
            BlockVector[floor(c/8)+floor(r/8)*(ImageMatrix.columns()/8)][r%8][c%8] = ImageMatrix[r][c];
            //cout << ImageMatrix[r][c] << endl;
        }
    }

    vector<matrixClass> Result;
    for (int i=0; i<NofBlocks; i++) {
        Result.push_back(matrixClass(8, 8));
    }
    int rows = ImageMatrix.columns()/8;
    for (int i=0, c=0, r=0; i<Result.size(); i++) {


        //\operatorname{mod}\left(\operatorname{floor}\left(x\right),H\right)+\left(\operatorname{mod}\left(\operatorname{floor}\left(\frac{x}{H\cdot V}\right),r_{1}\right)\cdot H\right)

        //\operatorname{mod}\left(\operatorname{floor}\left(\frac{x}{H}\right),V\right)+\operatorname{floor}\left(\frac{x}{H\cdot V\cdot r_{1}}\right)\cdot V
        int H = SubSampleFac_H;
        int V = SubSampleFac_V;
        int g = (int)floor(i)%H+((int)floor(i/(H*V))%rows)*H;

        int f = ((int)floor(i/H)%V)*rows;

        int k = floor(i/(V*rows)*rows*V);

        int index = (g + f + k);



        //int g = (int)floor(i)%H+((int)floor(i/(H*V))%rows)*H; int f = ((int)floor(i/H)%V)*V*rows; int k = floor(i/(H*V*rows)*rows*V*H);

        if (!(index < BlockVector.size())) {
            cout << "Error: " << i << "  index: " << index << "BlockVectorSize: " << BlockVector.size(); system("pause");
        }
        cout << "i: " << i << "  index: " << index << "  BlockVectorSize: " << BlockVector.size() << endl;
        Result[i] = BlockVector[index];

       /*
        f=\operatorname{mod}\left(\operatorname{floor}\left(\frac{x}{H}\right),V\right)\cdot r_{1}
        g=\operatorname{mod}\left(\operatorname{floor}\left(x\right),H\right)+\left(\operatorname{mod}\left(\operatorname{floor}\left(\frac{x}{H\cdot V}\right),r_{1}\right)\cdot H\right)
        k=\operatorname{floor}\left(\frac{x}{r_{1}\cdot V}\right)\cdot r_{1}
        */
    }


    return Result;

}
matrixClass ZigZag (matrixClass A){
    matrixClass ZigZag(1, A.columns()*A.rows());
    //y=-x\ +f\left\{0<x<j\right\}\left\{0<y<i\right\} DESMOS
    int rows = A.rows();
    int columns = A.columns();
    vector<double> Final;
    int x = 0;
    int y = 2;
    int k = 1;
    int f = y-x;
    for (int j=0; j<rows+columns-1; j++) {
    vector<double> Temp;
    for (int i = 0; i<f-1; i++) {

        x += k;
        y -= k;
        Temp.push_back(A[y-1][x-1]);
    }
    if (j%2 != 0){reverse(Temp.begin(), Temp.end());}
    Final.insert(Final.end(), Temp.begin(), Temp.end());
    x += k;
    y -= k;
    y = x;
    x = 0;

    if (y > rows) {
        x = 2+j-rows;
        y = rows;
    }
    y += 1;
    f = abs(y-x);
    }
    for (int i=0; i<Final.size(); i++) {
        ZigZag[0][i] = Final[i];
    }

    return ZigZag;
    /*for (int i=(-1)*(A.rows()+1)*(A.columns()+1); i<(A.rows()+1)*(A.columns()+1); i++) {
            int y = i%(rows+1);
            int x = floor(i/(rows+1))+y;
            cout << x << "|" << y << endl;
            if(0<=y && y<A.rows() && 0<=x && x<A.columns()) {
                //ZigZag[i] = A[i][i]

                }

    }*/
}

matrixClass Quantization (matrixClass DCTcoeff, matrixClass OutputQuantizationMatrix) {

    matrixClass output(8, 8);
    for (int i=0; i<output.rows(); i++){
        for (int j=0; j<output.columns(); j++) {
            output[i][j] = round(DCTcoeff[i][j]/OutputQuantizationMatrix[i][j]);
        }
    }

    return output;
}

matrixClass ShiftRange(matrixClass A) {
    int rows = A.rows();
    int columns = A.columns();
    for (int i=0; i<rows; i++) {
        for (int j=0; j<columns; j++) {
            A[i][j] = A[i][j]-128;
        }
    }
    return A;
}

matrixClass DCT(matrixClass A) {return DiscreteCosineTransform(A);}
matrixClass DiscreteCosineTransform(matrixClass A) {
    int rows = A.rows();
    int columns = A.columns();
    const double PI = 3.14159265358979323846;
    auto lambda = [] (int a) -> double {if (a == 0) {return (1/sqrt(2));} else {return 1;}};
    matrixClass F(rows, columns);
    for (int u=0; u<rows; u++) {
    for (int v=0; v<columns; v++) {
    double temp = 0;
    for (int r = 0; r<rows; r++) {
        for (int c = 0; c<columns; c++) {
            temp+=cos( ((PI*u)/(2*rows))*(2*r+1)   )*cos( ((PI*v)/(2*columns))*(2*c+1)   )*A[r][c];

        }
    }

    temp *= lambda(u)*lambda(v);
    temp*=sqrt(2.0/rows)*sqrt(2.0/columns);
    F[u][v]=temp;}}
    return F;

}

unsigned char* RGBMatrixtoBMPArray(matrixClass RedPixelMatrix, matrixClass GreenPixelMatrix, matrixClass BluePixelMatrix) {
    int rows = RedPixelMatrix.rows();
    int columns = RedPixelMatrix.columns();
    int counter = 0;
    //cout << rows << "\n";
    //cout << columns << "\n";
    //cout << 3*rows*columns+(columns%4)*rows << endl;
    int length = 3*rows*columns+(3 - (columns * 3 - 1) % 4)*rows;
    unsigned char* BMPArray = new unsigned char[3*rows*columns+(3 - (columns * 3 - 1) % 4)*rows];
    for (int i = 0; i < rows; i++) {
        for (int j=0; j<columns; j++) {
            BMPArray[counter]=RedPixelMatrix.matrix[i][j];
            BMPArray[counter+1]=GreenPixelMatrix.matrix[i][j];
            BMPArray[counter+2]=BluePixelMatrix.matrix[i][j];
            counter+=3;
            //cout << "row: " << i << "  | column: " << j << "row: " << rows << "  | column: " << columns << endl;

            if (j==columns-1) {
                for (int pad=0; pad < 3 - (columns * 3 - 1) % 4; pad++)
                {
                BMPArray[counter]=0;
                counter++;
                cout << "S";

                }
            }
        }

    }

    return BMPArray;
}


matrixClass ResReduction(matrixClass A) {
    int rows = A.rows();
    int columns = A.columns();


    if (columns % 2 != 0) {
        for (int i=0; i<rows; i++) {
            A[i].resize(columns+1);
            A[i][columns] = A[i][columns-1];
        }
    }
    if (rows % 2 != 0) {
        A.matrix.resize(rows+1);
        A[rows] = A[rows-1];
    }

    rows = A.rows();
    columns = A.columns();
    matrixClass B(ceil(rows/2), ceil(columns/2));
    cout << "ResReduction | " << A.rows() << endl;
    cout << "ResReduction | " << A.columns() << endl;
    cout << "ResReduction | " << B.rows() << endl;
    cout << "ResReduction | " << B.columns() << endl;
    for (int r=0; r<rows; r++) {
        for (int c=0; c<columns; c++) {
            if (c%2==0 && r%2==0) {
                    /*if (c<B.columns() && r<B.rows())
                    {
                    B.matrix[r/2][c/2] = A.matrix[r][c];
                    }
                if (c<5) {cout << "row: " << r << "  | Frow: " << floor(r/2) << "  | column: " << c << "  | Fcolumn: " << floor(r/2) << endl;}*/

            B[r/2][c/2] = (A[r][c] + A[r+1][c] + A[r][c+1] + A[r+1][c+1]) / 4;

            }

            }
        }
    cout << "Success \n";
    return B;
    }


vector<matrixClass> Convert(int ImgSize, int width, int height, unsigned char* data) {

    int pad = 0;
    matrixClass Y(height, width);
    matrixClass Cb(height, width);
    matrixClass Cr(height, width);
    cout << "\n width" << width;
    cout << "\n height" << height;
    cout << "\n Imgsize" << ImgSize;
    cout << endl;
    int j = 0;
    for (int i=0; i<ImgSize; i+=3) {
        if (((i-pad)/3) % width == 0 && (i-1)/3 > 0) {
            i += 3 - (width * 3 - 1) % 4;
            //https://stackoverflow.com/questions/76277082/how-to-handle-padding-when-reading-in-a-bmp#comment134514269_76277221
            pad += 3 - (width * 3 - 1) % 4;

        }
        if (!(i<ImgSize)) {break;}
        matrixClass YCbCr = RGBtoYCbCr(data[i],data[i+1],data[i+2],ColorCoefficients());
        //YCbCr = YCbCrtoRGB(YCbCr,ColorCoefficients());
        Y.matrix[floor((j)/width)][(j)%width] = YCbCr.matrix[0][0];
        Cb.matrix[floor((j)/width)][(j)%width] = YCbCr.matrix[1][0];
        Cr.matrix[floor((j)/width)][(j)%width] = YCbCr.matrix[2][0];

        j++;

    }
    //cout << endl << j << endl;
    //cout << "row: " << Y.rows() << endl;
    //cout << "col: " << Y.columns() << endl;
    vector<matrixClass> PixelMatrices;
    PixelMatrices = {{Y},{Cb},{Cr}};
    return PixelMatrices;


}
matrixClass ColorCoefficients () {
    //https://stackoverflow.com/questions/53952959/why-were-the-constants-in-yuv-rgb-chosen-to-be-the-values-they-are
    //https://en.wikipedia.org/wiki/YCbCr#
    float Wx = 0.310; float Wy = 0.3160;
    float Rx = 0.670;  float Ry = 0.330;  // Red primary
    float Gx = 0.210;  float Gy = 0.710;  // Green primary
    float Bx = 0.140;  float By = 0.080;  // Blue primary
    float Wz = 1-(Wx+Wy);
    float Rz = 1-(Rx+Ry);
    float Gz = 1-(Gx+Gy);
    float Bz = 1-(Bx+By);
    int Y = 1.0; // maximum luminance
    //x + y + z = 1;
    float Xw = Wx / Wy; // from X = Y * x / y
    float Zw = Wz / Wy; // from Z = Y * z / y

    matrixClass A(3, 3);
    matrixClass B (1, 3);
    A.matrix = {{Rx, Gx, Bx},{Ry, Gy, By},{Rz, Gz, Bz}};
    B.matrix = {{Xw},{Y},{Zw}};
    matrixClass C(1,3);
    C = Matrix_product(Invert(A), B);
    //Print_matrix(C);
    //Print_matrix(Matrix_product(A, C));
    matrixClass D(3, 3);
    for (int r=0; r<A.rows(); r++)
        {

            for (int c=0; c<C.columns(); c++)
            {

                  for (int j=0; j<A.columns(); j++) {
                    D.matrix[r][j] = A.matrix[r][j]*C.matrix[j][c];

                  }


            }
        }
    return D;
}

matrixClass RGBtoYCbCr(int r, int g, int b, matrixClass Correction) {
    float Kr = Correction.matrix[1][0];
    float Kg = Correction.matrix[1][1];
    float Kb = Correction.matrix[1][2];

    matrixClass colormatrix(3,3);
    colormatrix.matrix = {{(Kr),            (Kg),            (Kb)},
                         {(Kr/(2*(1-Kb)))*(-1),  (Kg/(2*(1-Kb)))*(-1), 0.5},
                         {0.5,            (Kg/(2*(1-Kr)))*(-1), (Kb/(2*(1-Kr)))*(-1)}};
    matrixClass rgb(3,1);
    rgb.matrix = {{r},{g},{b}};
    matrixClass YCbCr = Matrix_product(colormatrix, rgb);

    //YCbCr.matrix[1][0] += 128;
    //YCbCr.matrix[2][0] += 128;

    return YCbCr;

}

matrixClass YCbCrtoRGB(matrixClass YCbCr, matrixClass Correction) {

    float Kr = Correction.matrix[1][0];
    float Kg = Correction.matrix[1][1];
    float Kb = Correction.matrix[1][2];
    YCbCr.matrix[1][0] -= 128;
    YCbCr.matrix[2][0] -= 128;

    matrixClass colormatrix(3,3);
    colormatrix.matrix = {{(Kr),            (Kg),            (Kb)},
                         {(Kr/(2*(1-Kb)))*(-1),  (Kg/(2*(1-Kb)))*(-1), 0.5},
                         {0.5,            (Kg/(2*(1-Kr)))*(-1), (Kb/(2*(1-Kr)))*(-1)}};
    return Matrix_product(Invert(colormatrix), YCbCr);

}
void FlipBits(int ImgSize, int width, unsigned char* data)
{
    int pad = 0;
    for (int i=0; i<ImgSize; i+=3) {
        if ((i-pad)%width==0 && i!=0) {
            i += 3 - (width * 3 - 1) % 4;
            pad += 3 - (width * 3 - 1) % 4;
        }
        if (!(i<ImgSize)) {break;}
        unsigned char tmp = data[i];
        data[i] = data[i+2];
        data[i+2] = tmp;

    }
    cout << ImgSize << endl;
    cout << width << endl;
    cout << pad << endl;
}
template <typename T>
T ReadFileheader(unsigned char* fileheader, T location, int offset)
{
    //cout << sizeof(T) << endl;
    int value = 0;
    for (int i=0; i<sizeof(location); i++) {
        value += (int)*&fileheader[location+i+offset]*pow(256, i);
    }
    return value;
}
template <typename T>
void ModifyFileheader (unsigned char* fileheader, T location, int value, int offset) {
    for (int i=0; i<sizeof(location); i++) {
        fileheader[location+i+offset] = value % 256;
        value = value / 256;
    }
}
int LenOfData (matrixClass matrix) {
    return 3*matrix.rows()*matrix.columns()+(3 - (matrix.columns() * 3 - 1) % 4)*matrix.rows();
}
matrixClass RepeatEdges(matrixClass A, int SubSampleFac_H, int SubSampleFac_V) {
    int rows = A.rows();
    int columns = A.columns();
    SubSampleFac_H *= 8;
    SubSampleFac_V *= 8;
    int rowMod = (SubSampleFac_V-(rows%SubSampleFac_V))%SubSampleFac_V;
    int colMod = (SubSampleFac_H-(columns%SubSampleFac_H))%SubSampleFac_H;

    A.matrix.resize(rows+rowMod);
    for (int i=0; i<rowMod; i++) {
        A[rows+i] = A[rows-1];
    }
    rows = A.rows();
    for (int r=0; r<rows; r++) {
        A[r].resize(columns+colMod);
        for (int j=columns; j<columns+colMod; j++) {
            A[r][j] = A[r][columns-1];
        }
    }
    columns = A.columns();
    if (columns%SubSampleFac_H!=0 || rows%SubSampleFac_V!=0) {cout << "RepeatEdges error!\n" << "\n|rows: "<< rows << "\n|columns:  " << columns << endl;}
    return A;

}
/*for (int i=0; i<54; i++) {

    cout << to_string(i) + " : " << hex << (int)*&info[i] << endl;
    }*/
