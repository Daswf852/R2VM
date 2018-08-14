#include <iostream>
#include <stack>

/* A sample peripheral for R2VM
 * Notes:
 * - Unlike R216, the data sent by the R216 should be cleared by the peripheral to save performance
 */


uint8_t row = 0;
uint8_t col = 0;

const uint8_t sendTickDelay = 1;    //buffer will be sent to r2 after delay is done
                                    //this should usually be set on receiving data
uint8_t waitingFor = 0;             //this is to keep track of how many ticks we've been waiting for to send data
                                    //if the data stack is empty, this counter is set to 0
std::stack<uint16_t> datatx_buffer; //writes by tick should be done to this

extern "C" {
    uint16_t datatx =     0; //data to be read by the r2
    uint16_t datarx =     0; //data that is received to be processed on tick
    bool     bump   = false; //r2 sets this to true on bumps
}

extern "C" void init(){
    std::cout<<std::endl;
    //std::cout<<"\033c";
    //std::cout<<"\033["<<0<<";"<<0<<"H";
}

extern "C" void tick(){
    if(bump){
        
    }
    if(datarx){
        if(!(datarx&0xF000)){
            if(col > 16){
                col = 0;
                row++;
                std::cout<<std::endl;
            }
            std::cout<<static_cast<char>(datarx);
            //std::cout<<datarx<<std::endl;
            col++;
        }else if(datarx&0x1000){ //https://en.wikipedia.org/wiki/ANSI_escape_code
            //row = datarx & 0x3E0; //0000 00xx xxxy yyyy
            //col = datarx & 0x1F;
            //std::cout<<"\033["<<row<<";"<<col<<"H";
            
        }
    }
    ///post tick processing:
    datarx = 0;
    bump = false;
    
}
