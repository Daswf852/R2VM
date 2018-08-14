#ifndef R216_H
#define R216_H
#include "default.h"
#include "PeripheralManager.h"

class R216
{
    public:
        R216();
        ~R216();
    protected:
        void step();
        void load(std::array<uint32_t, 65536> rom);
        void dump(bool core = false);
        void setPM(PeripheralManager *PM);
        bool halt;
        uint8_t skiptick = 0;
        bool breakpoint;
    private:
        void basicUpdateFlags(uint16_t value);

        std::array<uint32_t, 65536> memory;
        uint16_t registers[16];
        uint16_t writeMask;
        bool carry, overflow, zero, sign;
        bool running;
        bool active;

        uint16_t C2OP1;
        uint16_t C2OP2;
        uint16_t prevOP1;
        bool shifted;
        bool chaining;

        uint16_t readClass1Data(uint32_t instruction);
        void writeClass1Data(uint32_t instruction, uint16_t data);

        uint16_t readClass1aData(uint32_t instruction);
        void writeClass1aData(uint32_t instruction, uint16_t data);

        uint16_t readClass2DataOP1(uint32_t instruction);
        uint16_t readClass2DataOP2(uint32_t instruction);
        void writeClass2Data(uint16_t data, bool updateFlags = true);
        void readClass2Data(uint32_t instruction);
        
        PeripheralManager *peripherals;
};

#endif // R216_H
