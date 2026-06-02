#include <vector>
#include <stack>
#include <stdint.h>

enum Mode {MODE_0, MODE_1, MODE_2, MODE_3};

enum Pixel {WHITE, DARK_GREY, LIGHT_GREY, BLACK};

struct Tile {
    std::vector<std::vector<Pixel>> data;
    uint8_t x;
    uint8_t y;

};

class Graphics {
    private:
    uint16_t dot = 0;
    Mode current_mode = Mode::MODE_2;
    Mode prev_mode = Mode::MODE_1;  

// these are the current pixels that are being rendered. updated every dot
    

    uint8_t SCX = 0; // regs
    uint8_t SCY;

    uint8_t end_x; // this is the furthert left pixel that will be rendered
    uint8_t end_y; // this is the furthest bottom pixel that will be rendered

    bool is_line_drawed = false;


    int dot_per_line = 0; // this is used for debug ONLY


    std::vector<std::vector<Pixel>> background;
    std::vector<std::vector<Pixel>> sprites;

    uint16_t tile_location(std::vector<uint8_t> &ram, uint8_t tile_index, bool is_sprite);

    void init_mode_3(std::vector<uint8_t>& ram);

    // this loop will never draw beyond the Y buffers of teh screen(159), because it will automatically be switched to the next mode
    void draw_line(std::vector<uint8_t>& ram);

    

    
    // title
    void load_background(std::vector<uint8_t>& ram);

    void load_sprites(std::vector<uint8_t>& ram);

    // this is used to add a tile to the background
    void add_tile(std::vector<std::vector<Pixel>> tile, uint8_t x_pixel, uint8_t y_pixel);

    // this loads a tile from memory. Can be used for sprites or background.
    std::vector<std::vector<Pixel>> load_tile(std::vector<uint8_t> &ram, uint8_t tile_index, bool is_sprite);

    // this loads a line of pixels and updated 0xff44
    std::vector<Pixel> load_line(uint8_t byte1, uint8_t byte2, uint8_t row, std::vector<std::vector<Pixel>>& tile);

    void OAM_transfer(std::vector<uint8_t> &ram);

    void add_sprite(std::vector<std::vector<Pixel>> tile, uint8_t x_pixel, uint8_t y_pixel);




    void manage_dot_clock(std::vector<uint8_t>& ram);
    void vertical_blank();
    void horizontal_blank();

        public:

    bool draw_now = false;
    std::vector<std::vector<Pixel>> display;
    uint8_t current_y = 0;

    Pixel screen[160* 144];

    Graphics()
        : display(160, std::vector<Pixel>(144, BLACK)),
        background(256, std::vector<Pixel>(256, BLACK)),
        sprites(160, std::vector<Pixel>(144, BLACK)){}

    void cycle(std::vector<uint8_t>& ram);


    


};
