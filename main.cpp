#include "VMManager.h"

std::string infile = "a.bin";
std::string cfgfile = "ports";

int main(int argc, char **argv){
    VMManager *VMM = new VMManager();
    if(argc > 3 || argc < 2){
        std::cout<<argv[0]<<" [binary] [peripheral config]"<<std::endl
                 <<argv[0]<<" -h"<<std::endl;
        return 1;
    }else if(argc == 2){
        std::cout<<argv[0]<<" [binary] [peripheral config]"<<std::endl
                    <<argv[0]<<" -h"<<std::endl;
        return 1;
    }else if(argc == 3){
        infile = std::string(argv[1]);
        cfgfile = std::string(argv[2]);
        VMM->loadROM(infile);
        VMM->loadPER(cfgfile);
    }
    VMM->run(false);
    delete VMM;
    return 0;
}
