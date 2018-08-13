#include "VMManager.h"

std::string infile = "a.bin";

int main(int argc, char **argv){
    VMManager *VMM = new VMManager();
    if(argc > 3){
        std::cout<<argv[0]<<"-b [binary]"<<std::endl
                 <<argv[0]<<"-d [dumpfile]"<<std::endl
                 <<argv[0]<<"-h"<<std::endl;
        return 1;
    }else if(argc == 2){
        if(std::string(argv[1]) == std::string("--help")){
            std::cout<<argv[0]<<"-b [binary]"<<std::endl
                 <<argv[0]<<"-d [dumpfile]"<<std::endl
                 <<argv[0]<<"-h"<<std::endl;
            return 1;
        }
    }else if(argc == 3){
        if(std::string(argv[1]) == std::string("-b")){
            infile = std::string(argv[2]);
            VMM->loadROM(infile, true);
        }else if(std::string(argv[1]) == std::string("-d")){
            infile = std::string(argv[2]);
            VMM->loadROM(infile, false);
        }
    }
    VMM->run(false);
    delete VMM;
    return 0;
}
