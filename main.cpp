#include "VMManager.h"

std::string infile = "a.bin";
std::string cfgfile = "ports";

int main(int argc, char **argv){
<<<<<<< HEAD
    VMManager *VMM = new VMManager(180);
=======
    VMManager *VMM = new VMManager();
>>>>>>> e5a3600436c7c0d7219637cbca14820ce6fbb663
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
