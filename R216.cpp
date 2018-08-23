#include "R216.h"

R216::R216(){
}

R216::~R216(){
}

void R216::setPM(PeripheralManager *PM){
    peripherals = PM;
}

void R216::load(std::array<uint32_t, 65536> rom){
    memory=rom;
}

void R216::dump(bool core){
    std::cout<<"[debug] Dumping processor information:"<<std::endl;
    std::cout<<"[debug] registers:"<<std::endl;
    for(int i=0; i<16; i++){
            std::cout<<"registers["<<i<<"] = "<<registers[i]<<std::endl;
    }
    std::cout<<"[debug] Write mask: "<<writeMask;
    std::cout<<"[debug] Flags:"<<std::endl;
    std::cout<<"Carry:    "<<carry<<std::endl
             <<"Overflow: "<<overflow<<std::endl
             <<"Sign:     "<<sign<<std::endl
             <<"Zero:     "<<zero<<std::endl;
    std::cout<<"[debug] Current instruction: "<<std::hex<<memory[registers[15]]<<std::dec<<std::endl;;
    if(core){
        std::ofstream dstream("core.dump");
        if(!dstream)
            return;
        std::cout<<"[debug] Core is dumped"<<std::endl;
        for(int i=0; i<65536; i++){
            if(memory[i])
                dstream<<"memory["<<i<<"] = "<<std::hex<<memory[i]<<std::dec<<std::endl;
        }
    }
}

void R216::step(){
    uint32_t temp32;
    uint16_t temp16;
    uint16_t temp16_2;
    uint32_t cInstruction = memory[registers[15]];
    switch((cInstruction & 0x1F000000) >> 24){
        case 0: //MOV
            readData(3);
            writeData(3, OP2);
            break;
        case 1: //AND
            readData(3);
            writeData(3, OP1&OP2);
            break;
        case 2: //OR
            readData(3);
            writeData(3, OP1|OP2);
            break;
        case 3: //XOR
            readData(3);
            writeData(3, OP1^OP2);
            break;
        case 4: //ADD
            readData(3);
            temp32 = OP1+OP2;
            writeData(3, OP1+OP2);
            carry=(temp32&0x10000)?true:false;
            overflow=false;
            if(!(OP1&0x8000) && !(OP2&0x8000) && (temp32&0x8000))
                overflow=true;
            if((OP1&0x8000) && (OP2&0x8000) && !(temp32&0x8000))
                overflow=true;
            
            break;
        case 5: //ADC
            readData(3);
            temp32 = OP1+OP2;
            if(carry){
                temp32++;
                carry = false;
            }
            writeData(3, temp32);
            carry=(temp32&0x10000)?true:false;
            overflow=false;
            if(!(OP1&0x8000) && !(OP2&0x8000) && (temp32&0x8000))
                overflow=true;
            if((OP1&0x8000) && (OP2&0x8000) && !(temp32&0x8000))
                overflow=true;
            break;
        case 6: //SUB
            readData(3);
            temp16=OP1-OP2;
            writeData(3, temp16);
            carry = (OP1<OP2)?true:false;
            overflow=false;
            if(!(OP1&0x8000) && (OP2&0x8000) && (temp16&0x8000))
                overflow=true;
            if((OP1&0x8000) && !(OP2&0x8000) && !(temp16&0x8000))
                overflow=true;
            break;
        case 7: //SBB
            readData(3);
            writeData(3, OP1-(OP2+carry));
            carry = (OP1<(OP2+carry))?true:false;
            overflow=false;
            if(!(OP1&0x8000) && (OP2&0x8000) && (temp16&0x8000))
                overflow=true;
            if((OP1&0x8000) && !(OP2&0x8000) && !(temp16&0x8000))
                overflow=true;
            break;
        case 8: //SWM
            temp16 = (readData(2) & 0x1FFF );
            writeMask = temp16;
            basicUpdateFlags(temp16);
            break;
        case 9: //ANDS
            readData(3);
            basicUpdateFlags(OP1&OP2);
            break;
        case 0xA: //ORS
            readData(3);
            basicUpdateFlags(OP1|OP2);
            break;
        case 0xB: //XORS
            readData(3);
            basicUpdateFlags(OP1^OP2);
            break;
        case 0xC: //ADDS
            readData(3);
            temp32 = OP1+OP2;
            basicUpdateFlags(OP1+OP2);
            carry=(temp32&0x10000)?true:false;
            overflow=false;
            if(!(OP1&0x8000) && !(OP2&0x8000) && (temp32&0x8000))
                overflow=true;
            if((OP1&0x8000) && (OP2&0x8000) && !(temp32&0x8000))
                overflow=true;
            break;
        case 0xD: //ADCS
            readData(3);
            temp32 = OP1+OP2;
            if(carry){
                temp32++;
                carry = false;
            }
            basicUpdateFlags(temp32);
            carry=(temp32&0x10000)?true:false;
            overflow=false;
            if(!(OP1&0x8000) && !(OP2&0x8000) && (temp32&0x8000))
                overflow=true;
            if((OP1&0x8000) && (OP2&0x8000) && !(temp32&0x8000))
                overflow=true;
            break;
        case 0xE: //SUBS
            readData(3);
            basicUpdateFlags(OP1-OP2);
            carry = (OP1<OP2)?true:false;
            overflow=false;
            if(!(OP1&0x8000) && (OP2&0x8000) && (temp16&0x8000))
                overflow=true;
            if((OP1&0x8000) && !(OP2&0x8000) && !(temp16&0x8000))
                overflow=true;
            //std::cout<<OP1<<", "<<OP2<<", "<<temp16<<std::endl;
            break;
        case 0xF: //SBBS
            readData(3);
            basicUpdateFlags(OP1-(OP2+carry));
            carry = (OP1<(OP2+carry))?true:false;
            overflow=false;
            if(!(OP1&0x8000) && (OP2&0x8000) && (temp16&0x8000))
                overflow=true;
            if((OP1&0x8000) && !(OP2&0x8000) && !(temp16&0x8000))
                overflow=true;
            break;
        case 0x10: //HLT
            halt=true;
            break;
        case 0x11: //J**
            temp16 = registers[15];
            switch(cInstruction & 0xF){
                case 0: //JMP
                    registers[15] = readData(2);
                    break;
                case 1: //JN(ever) aka NOP, will be a vm special instruction
                    temp16 = readData(2);
                    std::cout<<"[info] Special call "<<temp16<<" is made"<<std::endl;
                    switch(temp16){
                        case 1: //dump
                            dump(true);
                            break;
                        case 2: //breakpoint
                            dump(true);
                            breakpoint=true;
                            break;
                        case 3: //mul
                            temp16=memory[registers[14]++];
                            temp16_2=memory[registers[14]++];
                            registers[14]--;
                            memory[registers[14]] = (temp16*temp16_2)|0x20000000;
                            break;
                        case 4: //div
                            temp16=memory[registers[14]++];
                            temp16_2=memory[registers[14]++];
                            registers[14]--;
                            memory[registers[14]] = (temp16%temp16_2)|0x20000000;
                            registers[14]--;
                            memory[registers[14]] = (temp16/temp16_2)|0x20000000;
                            break;
                        default:
                            std::cout<<"[waning] Invalid call"<<std::endl;
                            break;
                    }
                    temp16 = ++registers[15]; //setting temp16 for safety.
                                              //a call with the equal value of
                                              //registers[15] would break the
                                              //program.
                    break;
                case 2:
                    if(carry)
                        registers[15] = readData(2);
                    break;
                case 3:
                    if(!carry)
                        registers[15] = readData(2);
                    break;
                case 4:
                    if(overflow)
                        registers[15] = readData(2);
                    break;
                case 5:
                    if(!overflow)
                        registers[15] = readData(2);
                    break;
                case 6:
                    if(sign)
                        registers[15] = readData(2);
                    break;
                case 7:
                    if(!sign)
                        registers[15] = readData(2);
                    break;
                case 8:
                    if(zero)
                        registers[15] = readData(2);
                    break;
                case 9:
                    if(!zero)
                        registers[15] = readData(2);
                    break;
                case 0xA:
                    if(zero || (sign!=overflow))
                        registers[15] = readData(2);
                    break;
                case 0xB:
                    if(!zero && (sign==overflow))
                        registers[15] = readData(2);
                    break;
                case 0xC:
                    if(sign!=overflow)
                        registers[15] = readData(2);
                    break;
                case 0xD:
                    if(sign==overflow)
                        registers[15] = readData(2);
                    break;
                case 0xE:
                    if(carry || zero)
                        registers[15] = readData(2);
                    break;
                case 0xF:
                    if(!carry && !zero)
                        registers[15] = readData(2);
                    break;
            }
            if(temp16 != registers[15])
                registers[15]--;
            break;
        case 0x12: //ROL
            readData(3);
            writeData(3, (OP1<<OP2) | (OP2 >> (16-OP2)));
            break;
        case 0x13: //ROR
            readData(3);
            writeData(3, (OP1 >> OP2) | (OP2 << (16-OP2)));
            break;
        case 0x14: //SHL
            readData(3);
            writeData(3, OP1<<(OP2&0xF));
            shifted=true;
            break;
        case 0x15: //SHR
            readData(3);
            writeData(3, OP1>>(OP2&0xF));
            shifted=true;
            break;
        case 0x16: //SCL
            readData(3);
            writeData(3, (OP1<<(OP2&0xF)) | prevOP1 >> (16-(OP2&0xF)));
            shifted=true;
            break;
        case 0x17: //SCR
            readData(3);
            writeData(3, (OP1>>(OP2&0xF)) | prevOP1 << (16-(OP2&0xF)));
            shifted=true;
            break;
        case 0x18: ///BUMP
            peripherals->sendToPort((uint8_t)readData(1), 0x30000);
            break;
        case 0x19: ///WAIT
            
            break;
        case 0x1A: ///SEND
            readData(3);
            peripherals->sendToPort( ((uint8_t)OP1) , (((uint32_t)OP2)|0x20000) );
            break;
        case 0x1B: ///RECV
            readData(3);
            writeData(3, peripherals->recvFromPort( ((uint8_t)OP1) ));
            break;
        case 0x1C: //PUSH
            registers[14]--;
            memory[registers[14]] = readData(2)|0x20000000;
            skiptick = 1;
            break;
        case 0x1D: //POP
            writeData(1, cInstruction, memory[registers[14]]);
            registers[14]++;
            skiptick = 1;
            break;
        case 0x1E: //CALL
            registers[14]--;
            memory[registers[14]] = registers[15]|0x20000000;
            registers[15] = readData(2);
            registers[15]--;
            skiptick = 1;
            break;
        case 0x1F: //RET
            registers[15] = memory[registers[14]];
            registers[14]++;
            skiptick = 1;
            break;
    }
    if(shifted){
        chaining = true;
        shifted = false;
    }
    registers[15]++;
}

void R216::basicUpdateFlags(uint16_t value){
    zero = (!value)?true:false;
    sign = (value&0x8000)?true:false;
    carry=false;
    overflow=false;
}

/*
[19:51] <LBPHacker> in the case of modes 8 and A, it just does mode 0
[19:51] <LBPHacker> and mode 2
[19:51] <LBPHacker> respectively
*/

void R216::writeData(uint8_t _class, uint16_t data, bool updateFlags){
    prevOP1 = readData(_class);
        if(_class == 1 || _class == 3){
            switch((memory[registers[15]] & 0xF00000)>>20){
                case 0:
                case 1:
                case 2:
                case 3:
                case 8:
                case 9:
                case 0xA:
                case 0xB: //kek, thats a lot of common ones
                    registers[ memory[registers[15]] & 0xF] = data;
                    break;
                case 4:
                case 6:
                    memory[ registers[ memory[registers[15]] & 0xF ] ] = (((memory[registers[15]] & 0x1F000000) >> 24)?(data|(writeMask<<16)):data)|0x20000000;
                    break;
                case 5:
                case 7:
                    memory[(memory[registers[15]] & 0xFFFF0)>>4] = (((memory[registers[15]] & 0x1F000000) >> 24)?(data|(writeMask<<16)):data)|0x20000000;
                    break;
                case 0xC:
                case 0xE:
                    if(memory[registers[15]] & 0x8000)
                        memory[registers[(memory[registers[15]]&0xF0000)>>16]-registers[memory[registers[15]]&0xF]] = (((memory[registers[15]] & 0x1F000000) >> 24)?(data|(writeMask<<16)):data)|0x20000000;
                    else
                        memory[registers[(memory[registers[15]]&0xF0000)>>16]+registers[memory[registers[15]]&0xF]] = (((memory[registers[15]] & 0x1F000000) >> 24)?(data|(writeMask<<16)):data)|0x20000000;
                    break;
                case 0xD:
                case 0xF:
                    if(memory[registers[15]] & 0x8000)
                        memory[registers[(memory[registers[15]]&0xF0000)>>16]-((memory[registers[15]]&0x7FF0)>>4)] = (!((memory[registers[15]] & 0x1F000000) >> 24)?(data|(writeMask<<16)):data)|0x20000000;
                    else
                        memory[registers[(memory[registers[15]]&0xF0000)>>16]+((memory[registers[15]]&0x7FF0)>>4)] = (!((memory[registers[15]] & 0x1F000000) >> 24)?(data|(writeMask<<16)):data)|0x20000000;
                    break;
                default:
                    break;
            }
    }/*else if(_class == 2){ ///this isnt even needed, there are no class 1* writes...
        switch((memory[registers[15]] & 0xF00000)>>20){
            case 0:
                registers[memory[registers[15]]&0xF] = data;
                break;
            case 1:
                memory[registers[(memory[registers[15]]&0xF0)>>4]] = data|0x20000000;
                break;
            case 2:
                break;
            case 3:
                memory[(memory[registers[15]]&0xFFFF)>>4] = data|0x20000000;
                break;
            case 9:
                if(memory[registers[15]] & 0x8000)
                    memory[registers[(memory[registers[15]]&0xF0000)>>16]-registers[(memory[registers[15]]&0xF0)>>4]] = data|0x20000000;
                else
                    memory[registers[(memory[registers[15]]&0xF0000)>>16]+registers[(memory[registers[15]]&0xF0)>>4]] = data|0x20000000;

                break;
            case 0xB:
                if(memory[registers[15]] & 0x8000)
                    memory[registers[(memory[registers[15]]&0xF0000)>>16]-((memory[registers[15]]&0x7FF0)>>11)] = data|0x20000000;
                else
                    memory[registers[(memory[registers[15]]&0xF0000)>>16]+((memory[registers[15]]&0x7FF0)>>11)] = data|0x20000000;
                break;
            default:
                break;
        }
    }*/
    if(updateFlags)
        basicUpdateFlags(data);
}

uint16_t R216::readData(uint8_t _class){
    uint16_t tempOP1 = [this]() -> uint16_t {
        switch((memory[registers[15]] & 0xF00000)>>20){
            case 0:
            case 1:
            case 2:
            case 3:
            case 8:
            case 9:
            case 0xA:
            case 0xB: //kek, thats a lot of common ones
                return registers[memory[registers[15]]&0xF];
                break;
            case 4:
            case 6:
                return static_cast<uint16_t>(memory[registers[memory[registers[15]]&0xF]]);
                break;
            case 5:
            case 7:
                return static_cast<uint16_t>(memory[(memory[registers[15]] & 0xFFFF0)>>4]);
                break;
            case 0xC:
            case 0xE:
                if(memory[registers[15]] & 0x8000)
                    return static_cast<uint16_t>(memory[registers[(memory[registers[15]]&0xF0000)>>16]-registers[memory[registers[15]]&0xF]]);
                else
                    return static_cast<uint16_t>(memory[registers[(memory[registers[15]]&0xF0000)>>16]+registers[memory[registers[15]]&0xF]]);
                break;
            case 0xD:
            case 0xF:
                if(memory[registers[15]] & 0x8000)
                    return static_cast<uint16_t>(memory[registers[(memory[registers[15]]&0xF0000)>>16]-((memory[registers[15]]&0x7FF0)>>4)]);
                else
                    return static_cast<uint16_t>(memory[registers[(memory[registers[15]]&0xF0000)>>16]+((memory[registers[15]]&0x7FF0)>>4)]);
                break;
        }
        return 0;
    }();
    uint16_t tempOP2 = [this]() -> uint16_t {
        switch((memory[registers[15]] & 0xF00000)>>20){
            case 0:
            case 4:
            case 8:
            case 0xC:
                return registers[(memory[registers[15]]&0xF0)>>4];
                break;
            case 1:
                return static_cast<uint16_t>(memory[registers[(memory[registers[15]]&0xF0)>>4]]);
                break;
            case 2:
            case 6:
            case 0xA:
                return (memory[registers[15]]&0xFFFF0)>>4;
                break;
            case 3:
                return static_cast<uint16_t>(memory[(memory[registers[15]]&0xFFFF0)>>4]);
                break;
            case 5:
                return registers[memory[registers[15]]&0xF];
                break;
            case 7:
            case 0xF:
                return memory[registers[15]]&0xF;
                break;
            case 9:
                if(memory[registers[15]] & 0x8000)
                    return static_cast<uint16_t>(memory[ registers[((memory[registers[15]]&0xF0000)>>16)] - registers[((memory[registers[15]]&0xF0)>>4)] ]);
                else
                    return static_cast<uint16_t>(memory[ registers[((memory[registers[15]]&0xF0000)>>16)] + registers[((memory[registers[15]]&0xF0)>>4)] ]);
                break;
            case 0xB:
                if(memory[registers[15]] & 0x8000)
                    return static_cast<uint16_t>(memory[ registers[(memory[registers[15]]&0xF0000)>>16] - ((memory[registers[15]]&0x7FF0)>>4) ]);
                else
                    return static_cast<uint16_t>(memory[ registers[(memory[registers[15]]&0xF0000)>>16] + ((memory[registers[15]]&0x7FF0)>>4) ]);
                break;
            case 0xD:
                return registers[memory[registers[15]]&0xF];
                break;
            case 0xE:
                return ((memory[registers[15]]&0x7FF0)>>4);
                break;
        }
        return 0;
    }();
    OP1 = 0;
    OP2 = 0;
    switch(_class){
        case 1:
            OP1=tempOP1;
            return OP1;
            break;
        case 2:
            OP1=tempOP2;
            return OP1;
            break;
        case 3:
            OP1=tempOP1;
            OP2=tempOP2;
            return OP1; //for quick access
            break;
        default:
            break;
    }
    return 0;
}

