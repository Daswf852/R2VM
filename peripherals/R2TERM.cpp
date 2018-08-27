/* R2TERM.cpp
 * a terminal peripheral based off of the real thing made in TPT
 */

#ifdef WIN32
    #include <pdcurses.h>
#else
    #include <ncurses.h>
#endif

uint8_t row, col;
char inputchar = ERR;
char chartosend;
bool readytosend, sent;

extern "C" {
    uint32_t datatx =     0; //data to be read by the r2
    uint32_t datarx =     0; //data that is received to be processed on tick
}

extern "C" void init(){
    initscr();
    noecho();
    nodelay(stdscr, 1);
    refresh();
}

extern "C" void deinit(){
    endwin();
}

extern "C" void tick(){
    if(sent){
        sent = false;
        readytosend = false;
    }
    inputchar = getch();
    if(inputchar != ERR){
        if(!readytosend){
            if(inputchar == '\n'){
                chartosend = 0xD;
            }else{
                chartosend = inputchar;  
            }
        }
        readytosend = true;
    }
    if(datarx==0x10000 && readytosend){
        datatx=chartosend;
        readytosend = false;
        sent=true;
    }else if(readytosend && !(datarx==0x10000)){
        datatx=0x10000;
    }
    if(datarx){
        if(datarx & 0x10000){
        }else if(datarx & 0x20000){
            uint16_t data = (uint16_t)(datarx&0xFFFF);
            if(data&0x1000){
                row = data & 0xF;
                col = (data & 0xF0)>>4;
                move(col,row);
            }else if(data&0x2000){
                //color later
            }else{
                addch((data>33 && data<127)?data:' ');
                col++;
                if(col>15){
                    col = 0;
                    row++;
                    move(col,row);
                }
            }
            
            refresh();
        }else{
        }
    }
    datarx=0;
}
