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
    for(int i=0; i<portPointer; ++i){ 
        Func_t tempsym = (Func_t) dlsym(ports[portPointer], "deinit");
        if(!tempsym){
            std::cout<<dlerror()<<std::endl;
            assert(ports[portPointer]);
        }
        tempsym();
    }
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
<<<<<<< HEAD
    //if(port>portPointer)
    //    return;
    uint32_t *datatx = (uint32_t *) dlsym(ports[port], "datarx");
    if(!datatx){
        std::cerr<<dlerror()<<std::endl;
        std::cout<<(int)port<<std::endl;
        assert(datatx);
    }
    *datatx = data;
}

uint32_t PeripheralManager::recvFromPort(uint8_t port){
    uint32_t temp;
    //std::cout<<(int)port<<std::endl;
    temp =  *((uint32_t *) dlsym(ports[port], "datatx"));
    //std::cout<<'a'<<std::endl;
    return temp;
}

uint16_t PeripheralManager::highestARPort(){
    uint16_t retval = -1;
    uint32_t datarx;
    for(int i=0; i<portPointer; ++i){
        datarx = *((uint32_t *) dlsym(ports[i], "datatx"));
         if(datarx & 0x10000){
            retval = i;
        }
    }
    return retval;
=======
    if(data&0x20000){
        uint16_t *datatx = (uint16_t *) dlsym(ports[port], "datarx");
        *datatx = (uint16_t)(data);
           //std::cout<<"send\n";
    }else if(data&0x10000){
        bool *bump = (bool *) dlsym(ports[port], "bump");
        *bump = true;
    }
}

uint32_t PeripheralManager::recvFromPort(uint8_t port){
    return 0;
>>>>>>> e5a3600436c7c0d7219637cbca14820ce6fbb663
}
