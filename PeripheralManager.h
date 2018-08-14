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
        std::array<void*, 256> ports;
        std::vector<std::string> stuffToOpen;
        uint8_t portPointer = 0;
        typedef void (*tickFunc_t)();
};

#endif // PERIPHERALMANAGER_H
