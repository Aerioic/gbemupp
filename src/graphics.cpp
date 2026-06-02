#include "graphics.h"
#include <iostream>
#include <stack>
#include <stdint.h>
#include <vector>
#include "mmu.h"
#include <cstring>
using namespace std;

int oz = 0;

// LYC == LY (bit 2, read-only but we set it internally)
bool get_lyc_eq_ly(uint8_t stat)
{
    return (stat >> 2) & 1;
}
void set_lyc_eq_ly(uint8_t &stat, bool value)
{
    if (value)
        stat |= 0x04;
    else
        stat &= ~0x04;
}

// Mode 0 int select (bit 3)
bool get_mode0_int_select(uint8_t stat)
{
    return (stat >> 3) & 1;
}
void set_mode0_int_select(uint8_t &stat, bool value)
{
    if (value)
        stat |= 0x08;
    else
        stat &= ~0x08;
}

// Mode 1 int select (bit 4)
bool get_mode1_int_select(uint8_t stat)
{
    return (stat >> 4) & 1;
}
void set_mode1_int_select(uint8_t &stat, bool value)
{
    if (value)
        stat |= 0x10;
    else
        stat &= ~0x10;
}

// Mode 2 int select (bit 5)
bool get_mode2_int_select(uint8_t stat)
{
    return (stat >> 5) & 1;
}
void set_mode2_int_select(uint8_t &stat, bool value)
{
    if (value)
        stat |= 0x20;
    else
        stat &= ~0x20;
}

// LYC int select (bit 6)
bool get_lyc_int_select(uint8_t stat)
{
    return (stat >> 6) & 1;
}
void set_lyc_int_select(uint8_t &stat, bool value)
{
    if (value)
        stat |= 0x40;
    else
        stat &= ~0x40;
}

void Graphics::init_mode_3(vector<uint8_t> &ram)
{

    current_y = 0;

    load_background(ram);
    if (ram[0xff46] != 0) OAM_transfer(ram);
    load_sprites(ram);

}
// this loop will never draw beyond the Y buffers of teh screen(159), because it will automatically be switched to the next mode
void Graphics::draw_line(vector<uint8_t> &ram)
{
    if (is_line_drawed)
        return;
    is_line_drawed = true;

    vector<Pixel> line(160);

    // this loop uses SCX and SCY to check what part of the background should be drawn
    // this loop never had values greter than
    uint8_t counter = 0;
    for (uint16_t x = SCX; x <= SCX + 159; x++)
    {

        uint8_t current_x = (x) % 256; // this code loops around to the back, if x goes over the edge
        uint8_t bg_y = (SCY + current_y == 0) ? 0 : (SCY + current_y) % 256;

        display[counter][current_y] = background[current_x][bg_y];
        if (sprites[counter][current_y] != Pixel::BLACK) {
            display[counter][current_y] = sprites[counter][current_y];
        }
        counter++;
    }

    draw_now = true;
}

void Graphics::load_sprites(vector<uint8_t> &ram)
{
    uint16_t sprite_map = 0xFE00;

    for (int x = 0; x < 160; x++) {
        for (int y = 0; y < 144; y++) {
            sprites[x][y] = Pixel::BLACK;
        }
    }

    for (int i = 0xFE00; i <= 0xFE9F; i += 4)
    {
        uint8_t position_y = ram[i];
        uint8_t position_x = ram[i + 1];
        uint8_t tile_index = ram[i + 2];
        uint8_t attributes = ram[i + 3];


        vector<vector<Pixel>> tile = load_tile(ram, tile_index, true);
        add_sprite(tile, position_x, position_y);
    }
}

void Graphics::load_background(vector<uint8_t> &ram)
{
    uint16_t tilemap = (ram[0xFF40] >> 3) & 1 ? 0x9C00 : 0x9800;
    SCX = ram[0xff43];
    SCY = ram[0xff42];

    end_x = (SCX + 159) % 256;
    end_y = (SCY + 143) % 256;


    for (uint8_t y = 0; y < 32; y++)
    { 
        for (uint8_t x = 0; x < 32; x++)
        { 

            uint8_t tile_location = ram[tilemap + y * 32 + x];



            vector<vector<Pixel>> tile = load_tile(ram, tile_location, false);
            add_tile(tile, x * 8, y * 8);
        }
    }
    oz++;
}

void Graphics::add_tile(vector<vector<Pixel>> tile, uint8_t x_pixel, uint8_t y_pixel)
{
    uint8_t x_tile_pixel = 0;
    uint8_t y_tile_pixel = 0;

    for (uint16_t y = y_pixel; y < y_pixel + 8; y++)
    {
        for (uint16_t x = x_pixel; x < x_pixel + 8; x++)
        {
            background[x][y] = tile[x_tile_pixel][y_tile_pixel];
            x_tile_pixel++;
        }
        x_tile_pixel = 0;
        y_tile_pixel++;
    }
}

void Graphics::add_sprite(vector<vector<Pixel>> tile, uint8_t x_pixel, uint8_t y_pixel)
{
    uint8_t x_tile_pixel = 0;
    uint8_t y_tile_pixel = 0;

    for (uint16_t y = y_pixel - 16; y < y_pixel - 8; y++)
    {
        for (uint16_t x = x_pixel - 8; x < x_pixel; x++)
        {
            if (x > 0 && y > 0 && x < 160 && y < 144 && tile[x_tile_pixel][y_tile_pixel] != Pixel::BLACK)
            {
                sprites[x % 160][y % 144] = tile[x_tile_pixel][y_tile_pixel];
            }
            x_tile_pixel++;
        }
        x_tile_pixel = 0;
        y_tile_pixel++;
    }
}

vector<vector<Pixel>> Graphics::load_tile(vector<uint8_t> &ram, uint8_t tile_index, bool is_sprite)
{
    vector<vector<Pixel>> tile(8, vector<Pixel>(8));
    uint8_t row = 0;
    uint16_t location = tile_location(ram, tile_index, is_sprite);

    for (uint16_t i = location; i < location + 16; i += 2)
    {
        load_line(ram[i], ram[i + 1], row, tile); 
        row++;
    }
    return tile;
}

uint16_t Graphics::tile_location(vector<uint8_t> &ram, uint8_t tile_index, bool is_sprite)
{
    int8_t signed_tile_index = (int8_t)tile_index;
    if (is_sprite)
        return (uint16_t)(0x8000 + tile_index * 16);
    return (ram[0xFF40] >> 4) & 1 ? (uint16_t)(0x8000 + tile_index * 16) : (uint16_t)(0x9000 + signed_tile_index * 16);
}

vector<Pixel> Graphics::load_line(uint8_t byte1, uint8_t byte2, uint8_t row, vector<vector<Pixel>> &tile)
{
    vector<Pixel> line(8);

    for (uint8_t i = 0; i <= 7; i++)
    {
        bool dark_grey = (((1 << i) & byte1) >> i);
        bool light_grey = (((1 << i) & byte2) >> i);

        if (dark_grey == false && light_grey == false)
            line[i] = Pixel::BLACK;
        else if (dark_grey == false && light_grey == true)
            line[i] = Pixel::LIGHT_GREY;
        else if (dark_grey == true && light_grey == false)
            line[i] = Pixel::DARK_GREY;
        else if (dark_grey == true && light_grey == true)
            line[i] = Pixel::WHITE;
        tile[7 - i][row] = line[i];
    }

    return line;
}

void Graphics::OAM_transfer(vector<uint8_t> &ram)
{
    if (ram[0xff46] != 0)
    {
        uint16_t source = ram[0xff46] << 8;
        for (uint8_t i = 0; i < 0xa0; i++)
        {
            ram[0xfe00 + i] = ram[source + i];
        }
        ram[0xff46] = 0;
    }
}

void Graphics::manage_dot_clock(vector<uint8_t> &ram)
{
    if (current_mode == MODE_1 && dot == 456)
    {

        if (current_y == 153)
        {
            current_mode = MODE_2;
            setPPUMode2(ram);
            if (get_mode2_int_select(ram[0xff41]))
            {
                requestLCDInterrupt(ram);
            }
        }

        dot = 0;
        is_line_drawed = false;
        current_y++;
    }

    else if (current_mode == MODE_2 && dot == 80)
    {
        if (current_y >= 144)
            init_mode_3(ram);
        current_mode = MODE_3;
        setPPUMode3(ram);
        dot = 0;
        is_line_drawed = false;
    }
    else if (current_mode == MODE_3 && dot == 172)
    {
        if (get_mode0_int_select(ram[0xff41]))
        {
            requestLCDInterrupt(ram);
        }
        current_mode = MODE_0;
        setPPUMode0(ram);
        dot = 0;
        is_line_drawed = false;
    }
    else if (current_mode == MODE_0 && dot == 204)
    {
        current_y += 1;
        if (current_y == 144)
        {
            if (get_mode1_int_select(ram[0xff41]))
            {
                requestLCDInterrupt(ram);
            }
            current_mode = MODE_1;
            setPPUMode1(ram);
            requestVBlankInterrupt(ram);


        }
        else
        {
            if (get_mode2_int_select(ram[0xff41]))
            {
                requestLCDInterrupt(ram);
            }
            current_mode = MODE_2;
            setPPUMode2(ram);
            dot_per_line = 0;
        }
        is_line_drawed = false;
        dot = 0;
    }

    dot++;
    dot_per_line++;
}

bool lcd_check(vector<uint8_t> &ram)
{
    return ram[0xff40] >> 7;
}

void Graphics::cycle(vector<uint8_t> &ram)
{
    uint8_t lcd_status = ram[0xff41];

    if (!lcd_check(ram))
    {
        dot = 0;
        current_mode = MODE_0;
        ram[0xff44] = 0;
        current_y = 0;
        setPPUMode0(ram);

        return;
    }

    if (get_lyc_eq_ly(ram[0xff41]) && ram[0xff44] == ram[0xff45])
    {
        set_lyc_eq_ly(ram[0xff41], true);
        requestLCDInterrupt(ram);
    }
    else {
        set_lyc_eq_ly(ram[0xff41], false);
    }


    if (current_mode == MODE_3)
    {
        draw_line(ram);
    }

    uint16_t prevdot = dot;
    manage_dot_clock(ram);

    ram[0xff44] = current_y;
}
