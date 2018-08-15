#include "PeripheralManager.h"

PeripheralManager::PeripheralManager(){
    std::fill(ports.begin(), ports.end(), nullptr);
    //temporary:
    //WstuffToOpen.push_back("./R2TERM.so");
    
    void *tempHandle;
    for(std::vector<std::string>::iterator it = stuffToOpen.begin(); it != stuffToOpen.end(); ++it){
        addPeripheral(*it);
    }
}

PeripheralManager::~PeripheralManager(){
}

void PeripheralManager::loadCfg(std::string filename){
    std::ifstream cfgstream(filename);
    if(!cfgstream){
        std::cout<<"[error] Couldn't open port 'config' file"<<std::endl;
        return;
    }
    std::string str;
    while(std::getline(cfgstream,str)){
        addPeripheral(str);
    }
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
        tickfs[portPointer] = (Func_t) dlsym(tempHandle, "tick");
        if(!tickfs[portPointer]){
            std::cout<<dlerror()<<std::endl;
            assert(tickfs[portPointer]);
        }
        Func_t tempsym = (Func_t) dlsym(tempHandle, "init");
        if(!tempsym){
            std::cout<<dlerror()<<std::endl;
            assert(tickfs[portPointer]);
        }
        tempsym();
        portPointer++;
    }
}

void PeripheralManager::tickAll(){
    for(int i=0; i<portPointer; ++i){ 
        tickfs[i]();
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
