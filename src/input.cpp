#include "input.h"
#include <SDL2/SDL.h>
#include "mmu.h"
#include "gameboy.h"

void Input::handle_inputs(CPU *cpu)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            exit(0);
        }
        if (event.type == SDL_KEYDOWN)
        {
            if (true)
            {
                if (event.key.keysym.sym == SDLK_DOWN)
                    DOWN = true;
                else
                    DOWN = false;

                if (event.key.keysym.sym == SDLK_LEFT)
                    LEFT = true;
                else
                    LEFT = false;

                if (event.key.keysym.sym == SDLK_RIGHT)
                    RIGHT = true;
                else
                    RIGHT = false;

                if (event.key.keysym.sym == SDLK_UP)
                    UP = true;
                else
                    UP = false;
            }
            if (true)
            {
                if (event.key.keysym.sym == SDLK_j) {
                    START = true;
                }
                else
                    START = false;

                if (event.key.keysym.sym == SDLK_s)
                    B = true;
                else
                    B = false;

                if (event.key.keysym.sym == SDLK_a)
                    A = true;
                else
                    A = false;

                if (event.key.keysym.sym == SDLK_k)
                    SELECT = true;
                else
                    SELECT = false;
            }
            if (A | B | START | SELECT) {
                requestJoypadInterrupt(cpu->ram);
            }
            if (DOWN | UP | RIGHT | LEFT) {
                requestJoypadInterrupt(cpu->ram);
            }
        }
    }
}

uint8_t Input::get_input_buffer(uint8_t ff00) {
    // bits 5 and 4 select which buttons to read
    // bit 5 = select buttons (A, B, SELECT, START)
    // bit 4 = select d-pad (RIGHT, LEFT, UP, DOWN)
    // a bit is 0 when pressed, 1 when not pressed

    uint8_t result = ff00 | 0x0F; // set lower 4 bits high by default (not pressed)

    if (!(ff00 & 0x20)) // button keys selected (bit 5 low)
    {
        if (START)  {result &= ~0x08;}
        if (SELECT) result &= ~0x04;
        if (B)      result &= ~0x02;
        if (A)      result &= ~0x01;
    }
    if (!(ff00 & 0x10)) // d-pad selected (bit 4 low)
    {
        if (DOWN)  result &= ~0x08;
        if (UP)    result &= ~0x04;
        if (LEFT)  result &= ~0x02;
        if (RIGHT) result &= ~0x01;
    }

    return result;
}