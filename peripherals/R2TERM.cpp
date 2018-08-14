#include <iostream>
#include <stack>

/* A sample peripheral for R2VM
 * Notes:
 * - Unlike R216, the data sent by the R216 should be cleared by the peripheral to save performance
 */

extern "C" {
    uint16_t datatx =     0; //data to be read by the r2
    uint16_t datarx =     0; //data that is received to be processed on tick
    bool     bump   = false; //r2 sets this to true on bumps
}

extern "C" void init(){
}

extern "C" void tick(){
    if(datarx){
        if(!(datarx&0xF000)){
            std::cout<<static_cast<char>(datarx);
        }
    }
    ///post tick processing:
    datarx = 0;
    bump = false;
}

const uint8_t sendTickDelay = 1;    //buffer will be sent to r2 after delay is done
                                    //this should usually be set on receiving data
uint8_t waitingFor = 0;             //this is to keep track of how many ticks we've been waiting for to send data
                                    //if the data stack is empty, this counter is set to 0
std::stack<uint16_t> datatx_buffer; //writes by tick should be done to this
