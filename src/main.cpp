#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "gameboy.h"
#include "graphics.h"
#include "instructions.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include "input.h"
using namespace std;
using namespace std::chrono;


const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\0";

const int screen_width = 160 * 4;
const int screen_height = 144 * 4;

const double frame_rate = 59.73;
const double frame_interval = 1.0 / frame_rate; // in seconds

uint16_t z = 0;

void init(SDL_Window **screen, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        cout << "everything fail";
        exit(0);
    }

    *screen = SDL_CreateWindow("Emu", 100, 100, 160 * 4, 144 * 4, SDL_WINDOW_SHOWN);

    if (!(*screen))
    {
        cout << "screen fail";
        exit(0); 
    }

    *renderer = SDL_CreateRenderer(*screen, -1, SDL_RENDERER_ACCELERATED);
    if (!(*renderer))
    {
        cout << "renderer fail";
        exit(0);
    }
}

void update_screen(SDL_Renderer *renderer, vector<vector<Pixel>> display, uint8_t current_y)
{

    if (current_y >= 144)
        return; // VBlnak

    if (current_y == 143)
    {
        z++;
        
    }

    for (uint8_t current_x = 0; current_x < 160; current_x++)
    {
        if (display[current_x][current_y] == Pixel::DARK_GREY)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }

        SDL_Rect rect = SDL_Rect{current_x * 4, current_y * 4, 4, 4};
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_RenderPresent(renderer);
}

void update_entire_screen(SDL_Renderer *renderer, vector<vector<Pixel>> display, uint8_t current_y)
{
    if (current_y != 143)
        return;

    for (uint8_t y = 0; y < 144; y++)
    {
        for (uint8_t x = 0; x < 160; x++)
        {
            if (display[x][y] == Pixel::WHITE)
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
            else if (display[x][y] == Pixel::LIGHT_GREY)
            {
                SDL_SetRenderDrawColor(renderer, 170, 170, 170, 255);
            }
            else if (display[x][y] == Pixel::DARK_GREY)
            {
                SDL_SetRenderDrawColor(renderer, 85, 85, 85, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }

            SDL_Rect rect = SDL_Rect{x * 4, y * 4, 4, 4};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_RenderPresent(renderer);
}
int main(int argc, char *argv[])
{

    vector<uint8_t> logo = {
        0xCE,
        0xED,
        0x66,
        0x66,
        0xCC,
        0x0D,
        0x00,
        0x0B,
        0x03,
        0x73,
        0x00,
        0x83,
        0x00,
        0x0C,
        0x00,
        0x0D,
        0x00,
        0x08,
        0x11,
        0x1F,
        0x88,
        0x89,
        0x00,
        0x0E,
        0xDC,
        0xCC,
        0x6E,
        0xE6,
        0xDD,
        0xDD,
        0xD9,
        0x99,
        0xBB,
        0xBB,
        0x67,
        0x63,
        0x6E,
        0x0E,
        0xEC,
        0xCC,
        0xDD,
        0xDC,
        0x99,
        0x9F,
        0xBB,
        0xB9,
        0x33,
        0x3E,
    };

    vector<uint8_t> boot_rom;
    // FILE *file = fopen("cpu_instrs.gb", "rb");
    //FILE *file = fopen("dmg-acid2.gb", "rb");
     //FILE *file = fopen("hellogb.gb", "rb");
    //FILE *file = fopen("TESTGAME.GB", "rb");
    //FILE *file = fopen("Dr. Mario (World) (Rev 1).gb", "rb");
    //FILE *file = fopen("Tetris (World) (Rev 1).gb", "rb");
    FILE *file = fopen("Tennis (World).gb", "rb");

    uint8_t read;

    uint8_t delay = 0;

    SDL_Window *screen = nullptr;
    SDL_Renderer *renderer = nullptr;

    init(&screen, &renderer);


    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    std::vector<unsigned char> buffer(size);

    fread(buffer.data(), 1, size, file);

    CPU cpu;
    Input input;
    uint16_t counter = 0;
    for (char read : buffer)
    {
        boot_rom.push_back((uint8_t)read);
        cpu.ram[counter] = (uint8_t)read;
        counter++;
    }

    fclose(file);

    SDL_Rect clear_screen_rect = SDL_Rect{0, 0, 100, 100};


    Graphics PPU;

    bool quit = false;
    uint8_t prev = 0;
    FILE *fptr = fopen("log.txt", "w");
    // PPU.

    int ffx = 0;
    long long x = 0;
    uint32_t start = SDL_GetTicks();
    
    auto last_frame_time = high_resolution_clock::now();
    while (!quit)
    {

        auto current_time = high_resolution_clock::now();
        auto elapsed_time = duration_cast<duration<double>>(current_time - last_frame_time).count();
        int delay = 0;

        

        if (elapsed_time < frame_interval)
        {
            SDL_Delay(1);
            continue;
        }
        input.handle_inputs(&cpu);

        for (int i = 0; i < 70224; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                PPU.cycle(cpu.ram);

                if (PPU.draw_now)
                {
                    update_entire_screen(renderer, PPU.display, PPU.current_y);
                    PPU.draw_now = false;
                }
            }
            if (delay != 0)
            {
                delay--;
                continue;
            }
            Instruction result = cpu.cycle(fptr, &input);
            delay = result.cycle;
        }


        last_frame_time = current_time;

      
        if (prev == 0x86 && cpu.opcode == 0x20 && true == false)
        {
            int count = 0;
            cout << "end";

            
        }


    }

    return 0;
}
