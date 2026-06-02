#include <stdint.h>
#include <vector>
#pragma once

class CPU;

class Input {
private:

    bool START = false;
    bool SELECT = false;
    bool A = false;
    bool B = false;

    bool UP = false;
    bool DOWN = false;
    bool LEFT =  false;
    bool RIGHT = false;

public:
    void handle_inputs(CPU *cpu);
    uint8_t get_input_buffer(uint8_t ff00);
};