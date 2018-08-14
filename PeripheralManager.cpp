#include "PeripheralManager.h"

PeripheralManager::PeripheralManager(){
    std::fill(ports.begin(), ports.end(), nullptr);
    //temporary:
    stuffToOpen.push_back("./R2TERM.so");
    
    void *tempHandle;
    for(std::vector<std::string>::iterator it = stuffToOpen.begin(); it != stuffToOpen.end(); ++it){
        addPeripheral(*it);
    }
}

PeripheralManager::~PeripheralManager(){
}

void PeripheralManager::addPeripheral(std::string filename){
    void *tempHandle = dlopen(filename.c_str(), RTLD_LAZY);
    if(!tempHandle){
        std::cerr<<"[error] failed to open shared object file "<<filename<<" for port "<<portPointer<<std::endl;
        std::cerr<<dlerror()<<std::endl;
        assert(tempHandle);
    }
    else{
        ports[portPointer] = tempHandle;
        portPointer++;
    }
}

void PeripheralManager::tickAll(){
    tickFunc_t tempsym;
    for(int i=0; i<portPointer; ++i){ //using portpointer instead of going through all ports is better
        try{ //it *is* better but just in case
            tempsym = (tickFunc_t)dlsym(ports.at(i), "tick");
        }catch(std::out_of_range *oor){
            std::cout<<oor->what()<<std::endl;
            continue;
        }
        if(!tempsym){
            std::cout<<dlerror()<<std::endl;
            continue;
        }
        tempsym();
    }
}

void PeripheralManager::sendToPort(uint8_t port, uint32_t data){
    if(data&0x20000){
        uint16_t *datatx = (uint16_t *) dlsym(ports[port], "datarx");
        *datatx = (uint16_t)(data);
           //std::cout<<"send\n";
    }else if(data&0x10000){
        bool *bump = (bool *) dlsym(ports[port], "bump");
        *bump = true;
    }
}
