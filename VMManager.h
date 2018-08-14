#ifndef VMMANAGER_H
#define VMMANAGER_H
#include "R216.h"

class VMManager : public R216{
    public:
        VMManager();
        ~VMManager();
        void loadROM(std::string filename);
        void run(bool stepmode=false);
        //void _dump(bool core=false); ///DEPRECATED, USE `JN 1` TO DUMP

    protected:

    private:
        bool loaded;
        //bool debugmode;
        //std::vector<uint16_t> breakpoints;
};

#endif // VMMANAGER_H
