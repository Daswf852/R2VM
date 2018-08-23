#include <iostream>
#include <stdio.h>
#include <stack>
#ifdef WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    struct termios oldt, newt;
#endif

const uint8_t colorlookupfg[16]={30,34,32,36,31,35,33,37,90,94,92,96,91,95,93,97};
const uint8_t colorlookupbg[16]={40,44,42,46,41,45,43,47,100,104,102,106,101,105,103,107};
uint8_t row = 0;
uint8_t col = 0;
char temp;

extern "C" {
    uint16_t datatx =     0; //data to be read by the r2
    uint16_t datarx =     0; //data that is received to be processed on tick
    bool     bump   = false; //r2 sets this to true on bumps
}

extern "C" void init(){
    std::cout<<std::endl;
    std::cout<<"\033c";
    std::cout<<"\033[1;1H";
    #ifdef WIN32
    #else
        //disable blocking:
        fcntl(0, F_SETFL, (fcntl(0, F_GETFL, 0) | O_NONBLOCK));
        //disable echo:
        tcgetattr(STDIN_FILENO, &oldt);
        newt=oldt;
        newt.c_lflag &= (~ICANON & ~ECHO);
        tcsetattr(STDIN_FILENO,TCSANOW,&newt);
    #endif
}

extern "C" void deinit(){
    std::cout<<"\033[13;17H";
    #ifdef WIN32
    #else
        tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
    #endif
}

extern "C" void tick(){
    #ifdef WIN32
        temp = getch();
    #else
        temp = getchar();
    #endif
    if (temp){
        datatx = 0x30000;
    }
    if(bump){
        
    }
    if(datarx){
        if(!(datarx&0xF000)){
            if(col > 15){
                col = 0;
                row++;
                std::cout<<std::endl;
            }
            std::cout<<static_cast<char>(datarx);
            col++;
        }else if(datarx&0x1000){
            col = datarx & 0xF0;
            col >>= 4;
            row = datarx & 0xF;
            std::cout<<"\033["<<std::to_string(col+1)<<";"<<std::to_string(row+1)<<"f";
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
