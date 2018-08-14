#ifndef PERIPHERALMANAGER_H
#define PERIPHERALMANAGER_H
#include "default.h"
#include <dlfcn.h>
#include <cassert>

class PeripheralManager
{
    public:
        PeripheralManager();
        ~PeripheralManager();
        void addPeripheral(std::string filename);
        void tickAll();
        void sendToPort(uint8_t port, uint32_t data);

    protected:

    private:
        typedef void (*Func_t)();
        std::array<void*, 256> ports;
        std::array<Func_t, 256> tickfs;
        std::vector<std::string> stuffToOpen;
        uint8_t portPointer = 0;
};

#endif // PERIPHERALMANAGER_H
