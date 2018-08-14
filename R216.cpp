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
            readClass2Data(cInstruction);
            writeClass2Data(C2OP2);
            break;
        case 1: //AND
            readClass2Data(cInstruction);
            writeClass2Data(C2OP1&C2OP2);
            break;
        case 2: //OR
            readClass2Data(cInstruction);
            writeClass2Data(C2OP1|C2OP2);
            break;
        case 3: //XOR
            readClass2Data(cInstruction);
            writeClass2Data(C2OP1^C2OP2);
            break;
        case 4: //ADD
            readClass2Data(cInstruction);
            temp32 = C2OP1+C2OP2;
            writeClass2Data(C2OP1+C2OP2);
            carry=(temp32&0x10000)?true:false;
            overflow=false;
            if((C2OP1&0x8000)&&(C2OP2&0x8000)){
                if(!(temp32&0x8000))
                    overflow = true;
            }else if(!((C2OP1&0x8000)&&(C2OP2&0x8000))){
                if(temp32&0x8000)
                    overflow = true;
            }
            break;
        case 5: //ADC
            readClass2Data(cInstruction);
            temp32 = C2OP1+C2OP2;
            if(carry){
                temp32++;
                carry = false;
            }
            writeClass2Data(temp32);
            carry=(temp32&0x10000)?true:false;
            overflow=false;
            if((C2OP1&0x8000)&&(C2OP2&0x8000)){
                if(!(temp32&0x8000))
                    overflow = true;
            }else if(!((C2OP1&0x8000)&&(C2OP2&0x8000))){
                if(temp32&0x8000)
                    overflow = true;
            }
            break;
        case 6: //SUB
            readClass2Data(cInstruction);
            temp16=C2OP1-C2OP2;
            writeClass2Data(temp16);
            carry = (C2OP1<C2OP2)?true:false;
            overflow=false;
            if(!(C2OP1&0x8000) && (C2OP2&0x8000) && (temp16&0x8000))
                overflow=true;
            if((C2OP1&0x8000) && !(C2OP2&0x8000) && !(temp16&0x8000))
                overflow=true;
            break;
        case 7: //SBB
            readClass2Data(cInstruction);
            writeClass2Data(C2OP1-(C2OP2+carry));
            carry = (C2OP1<(C2OP2+carry))?true:false;
            overflow=false;
            if(!(C2OP1&0x8000) && (C2OP2&0x8000) && (temp16&0x8000))
                overflow=true;
            if((C2OP1&0x8000) && !(C2OP2&0x8000) && !(temp16&0x8000))
                overflow=true;
            break;
        case 8: //SWM
            temp16 = /*(*/(readClass1aData(cInstruction) & 0x1FFF ) /*| 0x2000)*/;
            writeMask = temp16;
            basicUpdateFlags(temp16);
            break;
        case 9: //ANDS
            readClass2Data(cInstruction);
            basicUpdateFlags(C2OP1&C2OP2);
            break;
        case 0xA: //ORS
            readClass2Data(cInstruction);
            basicUpdateFlags(C2OP1|C2OP2);
            break;
        case 0xB: //XORS
            readClass2Data(cInstruction);
            basicUpdateFlags(C2OP1^C2OP2);
            break;
        case 0xC: //ADDS
            readClass2Data(cInstruction);
            temp32 = C2OP1+C2OP2;
            basicUpdateFlags(C2OP1+C2OP2);
            carry=(temp32&0x10000)?true:false;
            overflow=false;
            if((C2OP1&0x8000)&&(C2OP2&0x8000)){
                if(!(temp32&0x8000))
                    overflow = true;
            }else if(!((C2OP1&0x8000)&&(C2OP2&0x8000))){
                if(temp32&0x8000)
                    overflow = true;
            }
            break;
        case 0xD: //ADCS
            readClass2Data(cInstruction);
            temp32 = C2OP1+C2OP2;
            if(carry){
                temp32++;
                carry = false;
            }
            basicUpdateFlags(temp32);
            carry=(temp32&0x10000)?true:false;
            overflow=false;
            if((C2OP1&0x8000)&&(C2OP2&0x8000)){
                if(!(temp32&0x8000))
                    overflow = true;
            }else if(!((C2OP1&0x8000)&&(C2OP2&0x8000))){
                if(temp32&0x8000)
                    overflow = true;
            }
            break;
        case 0xE: //SUBS
            readClass2Data(cInstruction);
            basicUpdateFlags(C2OP1-C2OP2);
            carry = (C2OP1<C2OP2)?true:false;
            overflow=false;
            if(!(C2OP1&0x8000) && (C2OP2&0x8000) && (temp16&0x8000))
                overflow=true;
            if((C2OP1&0x8000) && !(C2OP2&0x8000) && !(temp16&0x8000))
                overflow=true;
            //std::cout<<C2OP1<<", "<<C2OP2<<", "<<temp16<<std::endl;
            break;
        case 0xF: //SBBS
            readClass2Data(cInstruction);
            basicUpdateFlags(C2OP1-(C2OP2+carry));
            carry = (C2OP1<(C2OP2+carry))?true:false;
            overflow=false;
            if(!(C2OP1&0x8000) && (C2OP2&0x8000) && (temp16&0x8000))
                overflow=true;
            if((C2OP1&0x8000) && !(C2OP2&0x8000) && !(temp16&0x8000))
                overflow=true;
            break;
        case 0x10: //HLT
            halt=true;
            break;
        case 0x11: //J**
            temp16 = registers[15];
            switch(cInstruction & 0xF){
                case 0: //JMP
                    registers[15] = readClass1aData(cInstruction);
                    break;
                case 1: //JN(ever) aka NOP, will be a vm special instruction
                    temp16 = readClass1aData(cInstruction);
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
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 3:
                    if(!carry)
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 4:
                    if(overflow)
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 5:
                    if(!overflow)
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 6:
                    if(sign)
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 7:
                    if(!sign)
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 8:
                    if(zero)
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 9:
                    if(!zero)
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 0xA:
                    if(zero || (sign!=overflow))
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 0xB:
                    if(!zero && (sign==overflow))
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 0xC:
                    if(sign!=overflow)
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 0xD:
                    if(sign==overflow)
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 0xE:
                    if(carry || zero)
                        registers[15] = readClass1aData(cInstruction);
                    break;
                case 0xF:
                    if(!carry && !zero)
                        registers[15] = readClass1aData(cInstruction);
                    break;
            }
            if(temp16 != registers[15])
                registers[15]--;
            break;
        case 0x12: //ROL
            readClass2Data(cInstruction);
            writeClass2Data((C2OP1<<C2OP2) | (C2OP2 >> (16-C2OP2)));
            break;
        case 0x13: //ROR
            readClass2Data(cInstruction);
            writeClass2Data((C2OP1 >> C2OP2) | (C2OP2 << (16-C2OP2)));
            break;
        case 0x14: //SHL
            readClass2Data(cInstruction);
            writeClass2Data(C2OP1<<(C2OP2&0xF));
            shifted=true;
            break;
        case 0x15: //SHR
            readClass2Data(cInstruction);
            writeClass2Data(C2OP1>>(C2OP2&0xF));
            shifted=true;
            break;
        case 0x16: //SCL
            readClass2Data(cInstruction);
            writeClass2Data((C2OP1<<(C2OP2&0xF)) | prevOP1 >> (16-(C2OP2&0xF)));
            shifted=true;
            break;
        case 0x17: //SCR
            readClass2Data(cInstruction);
            writeClass2Data((C2OP1>>(C2OP2&0xF)) | prevOP1 << (16-(C2OP2&0xF)));
            shifted=true;
            break;
        case 0x18: ///BUMP
            break;
        case 0x19: ///WAIT
            break;
        case 0x1A: ///SEND --THIS IS TEMPORARY UNTIL MY LAZY ASS IMPLEMENTS THE PM CLASS
            readClass2Data(cInstruction);
            //if(!peripherals){
                peripherals->sendToPort( ((uint8_t)C2OP1) , (((uint32_t)C2OP2)|0x20000) );
            //std::cout<<"send\n";
            //}
            break;
        case 0x1B: ///RECV
            break;
        case 0x1C: //PUSH
            registers[14]--;
            memory[registers[14]] = readClass1aData(cInstruction)|0x20000000;
            skiptick = 1;
            break;
        case 0x1D: //POP
            writeClass1Data(cInstruction, memory[registers[14]]);
            registers[14]++;
            skiptick = 1;
            break;
        case 0x1E: //CALL
            registers[14]--;
            memory[registers[14]] = registers[15]|0x20000000;
            registers[15] = readClass1aData(cInstruction);
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

uint16_t R216::readClass1Data(uint32_t instruction){
    switch((instruction & 0xF00000)>>20){
        case 0:
            return registers[instruction&0xF];
            break;
        case 4:
            return static_cast<uint16_t>(memory[registers[instruction&0xF]]);
            break;
        case 5:
            return static_cast<uint16_t>(memory[(instruction&0xFFFF0)>>4]);
            break;
        case 0xC:
            if(instruction & 0x8000)
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]-registers[instruction&0xF]]);
            else
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]+registers[instruction&0xF]]);
            break;
        case 0xD:
            if(instruction & 0x8000)
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]-((instruction&0x7FF0)>>4)]);
            else
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]+((instruction&0x7FF0)>>4)]);
            break;
        default:
            std::cout<<"[error] Invalid operand mode specified at: "<<registers[15]<<". Instruction is "<<std::hex<<memory[registers[15]]<<std::dec<<std::endl;
            break;
    }
    return 0;
}

void R216::writeClass1Data(uint32_t instruction, uint16_t data){
    prevOP1 = readClass1Data(instruction);
    switch((instruction & 0xF00000)>>20){
        case 0:
            registers[instruction & 0xF] = data;
            break;
        case 4:
            memory[registers[instruction&0xF]] = data;
            break;
        case 5:
            memory[(instruction&0xFFFF0)>>4] = data;
            break;
        case 0xC:
            if(instruction & 0x8000)
                memory[registers[(instruction&0xF0000)>>16]-registers[instruction&0xF]] = data;
            else
                memory[registers[(instruction&0xF0000)>>16]+registers[instruction&0xF]] = data;
            break;
        case 0xD:
            if(instruction & 0x8000)
                memory[registers[(instruction&0xF0000)>>16]-((instruction&0x7FF0)>>4)] = data;
            else
                memory[registers[(instruction&0xF0000)>>16]+((instruction&0x7FF0)>>4)] = data;
            break;
        default:
            break;
    }
}

uint16_t R216::readClass1aData(uint32_t instruction){
    switch((instruction & 0xF00000)>>20){
        case 0:
            return registers[(instruction&0xF0)>>4];
            break;
        case 1:
            return static_cast<uint16_t>(memory[registers[(instruction&0xF0)>>4]]);
            break;
        case 2:
            return (instruction&0xFFFF0)>>4;
            break;
        case 3:
            return static_cast<uint16_t>(memory[(instruction&0xFFFF)>>4]);
            break;
        case 9:
            if(instruction & 0x8000)
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]-registers[(instruction&0xF0)>>4]]);
            else
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]+registers[(instruction&0xF0)>>4]]);
            break;
        case 0xB:
            if(instruction & 0x8000)
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]-((instruction&0x7FF0)>>11)]);
            else
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]+((instruction&0x7FF0)>>11)]);
            break;
        default:
            std::cout<<"[error] Invalid operand mode specified at: "<<registers[15]<<". Instruction is "<<std::hex<<memory[registers[15]]<<std::dec;
            break;
    }
    return 0;
}

void R216::writeClass1aData(uint32_t instruction, uint16_t data){
    prevOP1 = readClass1aData(instruction);
    switch((instruction & 0xF00000)>>20){
        case 0:
            registers[instruction&0xF] = data;
            break;
        case 1:
            memory[registers[(instruction&0xF0)>>4]] = data|0x20000000;
            break;
        case 2:
            break;
        case 3:
            memory[(instruction&0xFFFF)>>4] = data|0x20000000;
            break;
        case 9:
            if(instruction & 0x8000)
                memory[registers[(instruction&0xF0000)>>16]-registers[(instruction&0xF0)>>4]] = data|0x20000000;
            else
                memory[registers[(instruction&0xF0000)>>16]+registers[(instruction&0xF0)>>4]] = data|0x20000000;

            break;
        case 0xB:
            if(instruction & 0x8000)
                memory[registers[(instruction&0xF0000)>>16]-((instruction&0x7FF0)>>11)] = data|0x20000000;
            else
                memory[registers[(instruction&0xF0000)>>16]+((instruction&0x7FF0)>>11)] = data|0x20000000;

            break;
        default:
            break;
    }
}

/*
[19:51] <LBPHacker> in the case of modes 8 and A, it just does mode 0
[19:51] <LBPHacker> and mode 2
[19:51] <LBPHacker> respectively
*/

void R216::readClass2Data(uint32_t instruction){
    C2OP1 = readClass2DataOP1(instruction);
    C2OP2 = readClass2DataOP2(instruction);
}

uint16_t R216::readClass2DataOP1(uint32_t instruction){
    switch((instruction & 0xF00000)>>20){
        case 0:
        case 1:
        case 2:
        case 3:
        case 8:
        case 9:
        case 0xA:
        case 0xB: //kek, thats a lot of common ones
            return registers[instruction&0xF];
            break;
        case 4:
        case 6:
            return static_cast<uint16_t>(memory[registers[instruction&0xF]]);
            break;
        case 5:
        case 7:
            return static_cast<uint16_t>(memory[(instruction & 0xFFFF0)>>4]);
            break;
        case 0xC:
        case 0xE:
            if(instruction & 0x8000)
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]-registers[instruction&0xF]]);
            else
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]+registers[instruction&0xF]]);
            break;
        case 0xD:
        case 0xF:
            if(instruction & 0x8000)
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]-((instruction&0x7FF0)>>4)]);
            else
                return static_cast<uint16_t>(memory[registers[(instruction&0xF0000)>>16]+((instruction&0x7FF0)>>4)]);
            break;
    }
    return 0;
}


uint16_t R216::readClass2DataOP2(uint32_t instruction){
    switch((instruction & 0xF00000)>>20){
        case 0:
        case 4:
        case 8:
        case 0xC:
            return registers[(instruction&0xF0)>>4];
            break;
        case 1:
            return static_cast<uint16_t>(memory[registers[(instruction&0xF0)>>4]]);
            break;
        case 2:
        case 6:
        case 0xA:
            return (instruction&0xFFFF0)>>4;
            break;
        case 3:
            return static_cast<uint16_t>(memory[(instruction&0xFFFF0)>>4]);
            break;
        case 5:
            return registers[instruction&0xF];
            break;
        case 7:
        case 0xF:
            return instruction&0xF;
            break;
        case 9:
            if(instruction & 0x8000)
                return static_cast<uint16_t>(memory[ registers[((instruction&0xF0000)>>16)] - registers[((instruction&0xF0)>>4)] ]);
            else
                return static_cast<uint16_t>(memory[ registers[((instruction&0xF0000)>>16)] + registers[((instruction&0xF0)>>4)] ]);
            break;
        case 0xB:
            if(instruction & 0x8000)
                return static_cast<uint16_t>(memory[ registers[(instruction&0xF0000)>>16] - ((instruction&0x7FF0)>>4) ]);
            else
                return static_cast<uint16_t>(memory[ registers[(instruction&0xF0000)>>16] + ((instruction&0x7FF0)>>4) ]);
            break;
        case 0xD:
            return registers[instruction&0xF];
            break;
        case 0xE:
            return ((instruction&0x7FF0)>>4);
            break;
    }
    return 0;
}

void R216::writeClass2Data(uint16_t data, bool updateFlags){
    prevOP1 = readClass2DataOP1(memory[registers[15]]);
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
    if(updateFlags)
        basicUpdateFlags(data);
}


