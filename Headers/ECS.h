#ifndef ECS_H
#define ECS_H
#include <cstdint>
class ECSData {
    public:
        std::vector<int> data;
        std::vector<int> s;
        ECSData(int DataLength, int SizeLength) {
            data.resize(DataLength);
            s.resize(SizeLength);
        }
};

std::vector<unsigned char> CreateECS (ECSData data);
unsigned char write_char (unsigned char s, unsigned char MSB, uint32_t read, unsigned char byte);

#endif // ECS_H
