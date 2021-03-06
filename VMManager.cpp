#include "VMManager.h"


VMManager::VMManager(int _rate){
    peripherals = new PeripheralManager();
    setPM(peripherals);
    rate = _rate;
}

VMManager::~VMManager(){
    delete peripherals;
}

void VMManager::loadPER(std::string filename){
    peripherals->loadCfg(filename);
}

void VMManager::loadROM(std::string filename){
    std::array<uint32_t, 65536> tempr;
    std::ifstream binaryfilestream(filename, std::ios::binary);
    if(!binaryfilestream){
        std::cout<<"[error] Failed to open "<<filename<<std::endl;
        return;
    }
    //std::cout<<"[info] Reading from "<<filename<<std::endl;
    uint32_t temp;
    uint16_t dp = 0;
    while(binaryfilestream.read(reinterpret_cast<char*>(&temp), sizeof(temp))){
        tempr[dp] = temp;
        dp++;
    }
    //std::cout<<"[info] Done reading from "<<filename<<std::endl;
    binaryfilestream.close();
    loaded = true;
    load(tempr);
}

void VMManager::run(bool stepmode){ //new TODO: switch to ncurses (maybe, not sure)
    if(!loaded)
        return;
    
    std::chrono::duration<double> nanosWait = std::chrono::duration<double,std::nano>(1000000000/rate);
    std::chrono::high_resolution_clock::time_point clockT1 = std::chrono::high_resolution_clock::now(); //last tick's time
    std::chrono::high_resolution_clock::time_point clockT2 = std::chrono::high_resolution_clock::now(); //current time
    std::chrono::duration<double> dura;
    
    std::chrono::time_point<std::chrono::system_clock> t1 = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> t2;
    uint64_t steps = 0;
    char temp;
    
    while(!halt){
        if(!stepmode){
            clockT2 = std::chrono::high_resolution_clock::now();
            dura = std::chrono::duration_cast<std::chrono::duration<double>>(clockT2-clockT1);
            
            if(dura.count() >= nanosWait.count()){
                clockT1 = std::chrono::high_resolution_clock::now();
            }else{
                continue;
            }
            
            if(skiptick){   //to behave like the real R216
                skiptick--;
                continue;
            }
            step();
            peripherals->tickAll();
            steps++;
            
            if(breakpoint){
                std::cout<<"[info] Breakpoint hit, press a key to continue, press 's' to enter step mode (s again to exit)"<<std::endl;
                std::cin>>temp;
                stepmode = (temp == 's');
            }
            if(halt){
                t2 = std::chrono::system_clock::now();
                std::cout<<"[Info] An HLT was caught, press 'c' to continue, anything else to end program."<<std::endl;
                std::cin>>temp;
                if(temp == 'c'){
                    halt=false;
                    continue;
                }
                auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
                std::cout<<"[info] Program took: "<<millis.count()<<"ms to run"<<std::endl;
                std::cout<<"[info] Steps taken: "<<steps<<std::endl;
                if(millis.count())
                    std::cout<<"[info] Average speed was: "<<(steps/(millis.count()))<<"KHz"<<std::endl;
                else
                    std::cout<<"[info] Program ended too quickly, no average speed could be calculated."<<std::endl;
            }
        }else{
            char temp;
            while(!halt){
                dump();
                step();
                std::cin>>temp;
                if(halt){
                    t2 = std::chrono::system_clock::now();
                    std::cout<<"[Info] An HLT was caught, press 'c' to continue, anything else to end program."<<std::endl;
                    std::cin>>temp;
                    if(temp == 'c'){
                        halt=false;
                        continue;
                    }
                }
            }
        }
    }
}
