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
        void tickAll();
        void sendToPort(uint8_t port, uint32_t data);
        uint32_t recvFromPort(uint8_t port);
<<<<<<< HEAD
        uint16_t highestARPort();
=======
>>>>>>> e5a3600436c7c0d7219637cbca14820ce6fbb663
        void loadCfg(std::string filename);

    protected:

    private:
        typedef void (*Func_t)();
        
        void addPeripheral(std::string filename);
        
        std::array<void*, 256> ports;
        std::array<Func_t, 256> tickfs;
        std::vector<std::string> stuffToOpen;
        uint8_t portPointer = 0;
};

#endif // PERIPHERALMANAGER_H
