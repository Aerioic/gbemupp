#pragma once

#include <vector>
#include <stack>
#include <stdint.h> 
#include <map>
#include "instructions.h"
#include "input.h"

    



class CPU {
    
public:
    std::vector<uint8_t> video;
    std::vector<uint8_t> rom;
    std::stack<uint16_t> gb_stack;

    void debug_print(uint8_t opcode, uint8_t arg1s, uint8_t arg2);

    uint8_t dma_cycles = 0;
    
    
    

    struct {
        uint8_t a = 0x01;
        uint8_t b = 0x00;
        uint8_t c = 0x13;
        uint8_t d = 0x00;
        uint8_t e = 0xd8;
        uint8_t h = 0x01;
        uint8_t l = 0x4d;
        uint8_t f = 0xb0;
        uint8_t s = 0xFFFE >> 8;
        uint8_t p = 0xFFFE & 0xff;
    } reg;

    struct {
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
    } d_pad;

    struct {
        bool a = false;
        bool b = false;
        bool SELECT = false;
        bool START = false;
    } buttons;

    int IME = 0;
    int t_cycle = 0;
    int TIMA = 0;
    int DIV = 0;
    bool IME_pending = false;

    int debug_mode = 0;

    std::map<uint8_t, uint8_t*> arithmeticConstants; 

    
/*
    struct {
        bool zero = false;
        bool subtract = false;
        bool half_carry = false;
        bool carry = false;
    } flags;
*/

    // this handles the opcode calling
    Instruction process(Input *input);

public:

    uint16_t pc = 0x0100;
    std::vector<uint8_t> ram;
    uint8_t opcode;
    bool is_halted = false;


    CPU();

    void constantAritmaticSetup();

    void handle_joypad();

    // returns the delay/time

    // the loop calls this function. This also incrementes the PC
    Instruction cycle(FILE *fptr, Input *input);

    void handle_tima();

    void interrupt();

    Instruction cb_prefix(uint8_t opcode_cb);
};