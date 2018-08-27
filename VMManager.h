#ifndef VMMANAGER_H
#define VMMANAGER_H
#include "R216.h"
#include "PeripheralManager.h"

class VMManager : public R216{
    public:
        VMManager(int _rate = 60);
        ~VMManager();
        void loadROM(std::string filename);
        void loadPER(std::string filename);
        void run(bool stepmode=false);
        //void _dump(bool core=false); ///DEPRECATED, USE `JN 1` TO DUMP

    protected:

    private:
        bool loaded;
        PeripheralManager *peripherals;
        uint32_t rate;
};

#endif // VMMANAGER_H
