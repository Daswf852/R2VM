/* diagper.cpp
 * small peripheral program to diagnose the interface
 */

#include <iostream>

extern "C" {
    uint32_t datatx =     0; //data to be read by the r2
    uint32_t datarx =     0; //data that is received to be processed on tick
}

extern "C" void init(){
    std::cout<<"Intializing peripheral."<<std::endl;
}

extern "C" void deinit(){
    std::cout<<"Deinitializing peripheral."<<std::endl;
}

extern "C" void tick(){
    if(datarx){
        if(datarx & 0x10000){
            std::cout<<"Received bump."<<std::endl;
        }else if(datarx & 0x20000){
            std::cout<<"Received data: 0x"<<std::hex<<(uint16_t)datarx<<std::dec<<std::endl;
        }else{
            std::cout<<"Invalid data received: 0x"<<std::hex<<datarx<<std::dec<<std::endl;
        }
    }
    datarx=0;
}
