#include <iostream>
#include <stack>

const uint8_t colorlookupfg[16]={30,34,32,36,31,35,33,37,90,94,92,96,91,95,93,97};
const uint8_t colorlookupbg[16]={40,44,42,46,41,45,43,47,100,104,102,106,101,105,103,107};
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
    std::cout<<"\033c";
    std::cout<<"\033[0;0H";
}

extern "C" void tick(){
    if(bump){
        
    }
    if(datarx){
        if(!(datarx&0xF000)){
            if(col > 31){ //why the hell is this does what >16 is supposed to do? idk
                col = 0;
                row++;
                std::cout<<std::endl;
            }
            std::cout<<static_cast<char>(datarx);
            col++;
            //std::cout<<datarx<<std::endl;
            col++;
        }else if(datarx&0x1000){ //https://en.wikipedia.org/wiki/ANSI_escape_code
            col = datarx & 0xF0; //0000 0000 xxxx yyyy
            col >>= 4;
            row = datarx & 0xF;
            std::cout<<"\033["<<std::to_string(col+1)<<";"<<std::to_string(row+1)<<"f"; "<<std::endl;
        }else if(datarx&0x2000){
            uint8_t bg = datarx & 0xF0;
            bg >>= 4;
            uint8_t fg = datarx & 0xF;
            std::cout<<"\033["<<std::to_string(colorlookupfg[fg])<<";"<<std::to_string(colorlookupbg[bg])<<"m";
        }
    }
    ///post tick processing:
    datarx = 0;
    bump = false;
    
}
