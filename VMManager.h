#ifndef VMMANAGER_H
#define VMMANAGER_H
#include "R216.h"
#include "PeripheralManager.h"

class VMManager : public R216{
    public:
<<<<<<< HEAD
        VMManager(int _rate = 60);
=======
        VMManager();
>>>>>>> e5a3600436c7c0d7219637cbca14820ce6fbb663
        ~VMManager();
        void loadROM(std::string filename);
        void loadPER(std::string filename);
        void run(bool stepmode=false);
        //void _dump(bool core=false); ///DEPRECATED, USE `JN 1` TO DUMP

    protected:

    private:
        bool loaded;
        PeripheralManager *peripherals;
<<<<<<< HEAD
        uint32_t rate;
=======
        //bool debugmode;
        //std::vector<uint16_t> breakpoints;
>>>>>>> e5a3600436c7c0d7219637cbca14820ce6fbb663
};

#endif // VMMANAGER_H
