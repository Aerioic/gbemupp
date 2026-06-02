#pragma once

#include <stdint.h>
#include <vector>

using namespace std;

struct Instruction {
    int cycle;
    int length;
};


bool checkVBlankInterrupt(std::vector<uint8_t> &ram);
bool checkLCDInterrupt(std::vector<uint8_t> &ram);
bool checkTimerInterrupt(std::vector<uint8_t> &ram);
bool checkSerialInterrupt(std::vector<uint8_t> &ram);
bool checkJoypadInterrupt(std::vector<uint8_t> &ram);

bool checkRequestVBlankInterrupt(std::vector<uint8_t> &ram);
bool checkRequestLCDInterrupt(std::vector<uint8_t> &ram);
bool checkRequestTimerInterrupt(std::vector<uint8_t> &ram);
bool checkRequestSerialInterrupt(std::vector<uint8_t> &ram);
bool checkRequestJoypadInterrupt(std::vector<uint8_t> &ram);

void requestVBlankInterrupt(std::vector<uint8_t> &ram);
void requestLCDInterrupt(std::vector<uint8_t> &ram);
void requestTimerInterrupt(std::vector<uint8_t> &ram);
void requestSerialInterrupt(std::vector<uint8_t> &ram);
void requestJoypadInterrupt(std::vector<uint8_t> &ram);

void clearVBlankInterrupt(std::vector<uint8_t> &ram);
void clearLCDInterrupt(std::vector<uint8_t> &ram);
void clearTimerInterrupt(std::vector<uint8_t> &ram);
void clearSerialInterrupt(std::vector<uint8_t> &ram);
void clearJoypadInterrupt(std::vector<uint8_t> &ram);

Instruction set_16(uint8_t& upper, uint8_t& lower, uint16_t to_set);
Instruction set_Hlp(uint8_t& upper, uint8_t& lower);
Instruction set_8(uint8_t& to_set, uint8_t value);

Instruction DAA(uint8_t &a, uint8_t &f);

Instruction inc(uint8_t& dest, uint8_t& l);
Instruction inc_16(uint8_t& dest1, uint8_t& dest2);

Instruction dec(uint8_t& dest, uint8_t& l);
Instruction dec_16(uint8_t& dest1, uint8_t& dest2);

Instruction rlca(uint8_t& dest, uint8_t& l);
Instruction rla(uint8_t& dest, uint8_t& l);
Instruction rra(uint8_t& dest, uint8_t& l);
Instruction rrca(uint8_t& dest, uint8_t& l);

Instruction rlc(uint8_t& dest, uint8_t& l);
Instruction rrc(uint8_t& dest, uint8_t& l);
Instruction rr(uint8_t& dest, uint8_t& l);
Instruction rl(uint8_t& dest, uint8_t& l);

Instruction sla(uint8_t& dest, uint8_t& l);
Instruction sra(uint8_t& dest, uint8_t& l);
Instruction srl(uint8_t& dest, uint8_t& l);

Instruction bit(uint8_t& dest, uint8_t& l, uint8_t _bit, uint8_t _byte);
Instruction res(uint8_t& dest, uint8_t& l, uint8_t _bit, uint8_t _byte);
Instruction set(uint8_t& dest, uint8_t& l, uint8_t _bit);

Instruction swap(uint8_t& dest, uint8_t& l);



Instruction jr(uint16_t& dest, int8_t& modifier);
Instruction jr_nz(uint16_t& dest, int8_t& modifier, uint8_t& l);
Instruction jr_z(uint16_t& dest, int8_t& modifier, uint8_t& l);
Instruction jr_nc(uint16_t& dest, int8_t& modifier, uint8_t& l);
Instruction jr_c(uint16_t& dest, int8_t& modifier, uint8_t& l);

Instruction add_16(uint8_t& dest1, uint8_t& dest2, uint8_t value1, uint8_t value2, uint8_t& l);

Instruction cpl(uint8_t& dest, uint8_t& l);
Instruction ccf(uint8_t& l);

Instruction set_reg_CARRY(uint8_t& flag, bool value);
void set_reg_zero(uint8_t &flag, bool value);
void set_reg_SUBTRACTION(uint8_t &flag, bool value);
void set_reg_HALF_CARRY(uint8_t &flag, bool value);

Instruction add_8_carry(uint8_t& dest, uint8_t value, uint8_t& l);
Instruction add_8(uint8_t& value, uint8_t& dest, uint8_t& l);
Instruction ld_hl_sp_i8(uint8_t &h, uint8_t &l, uint8_t &s, uint8_t &p, int8_t value, uint8_t &f);

Instruction sub_8(uint8_t& dest, uint8_t& value, uint8_t& l);
Instruction sub_8_carry(uint8_t& dest, uint8_t value, uint8_t& l);

Instruction andf(uint8_t& dest, uint8_t& value, uint8_t& l);
Instruction orf(uint8_t& dest, uint8_t& value, uint8_t& l);

Instruction cp(uint8_t dest, uint8_t value, uint8_t& l, uint8_t _byte);
Instruction xorf(uint8_t& dest, uint8_t& value, uint8_t& l);

Instruction ret(uint16_t& PC, uint8_t& s, uint8_t& p, vector<uint8_t>& ram);
Instruction ret_nz(uint16_t& PC, uint8_t& s, uint8_t& p, vector<uint8_t>& ram, uint8_t& l);
Instruction ret_z(uint16_t& PC, uint8_t& s, uint8_t& p, vector<uint8_t>& ram, uint8_t& l);
Instruction ret_nc(uint16_t& PC, uint8_t& s, uint8_t& p, vector<uint8_t>& ram, uint8_t& l);
Instruction ret_c(uint16_t& PC, uint8_t& s, uint8_t& p, vector<uint8_t>& ram, uint8_t& l);

Instruction pop(uint8_t& set1, uint8_t& set2, uint8_t& s, uint8_t& p, vector<uint8_t>& ram);
Instruction push(uint8_t& value1, uint8_t& value2, uint8_t& s, uint8_t& p, vector<uint8_t>& ram);

Instruction jp(uint8_t& value1, uint8_t& value2, uint16_t& PC);
Instruction jp_nz(uint8_t& value1, uint8_t& value2, uint16_t& PC, uint8_t& l);
Instruction jp_z(uint8_t& value1, uint8_t& value2, uint16_t& PC, uint8_t& l);
Instruction jp_nc(uint8_t& value1, uint8_t& value2, uint16_t& PC, uint8_t& l);
Instruction jp_c(uint8_t& value1, uint8_t& value2, uint16_t& PC, uint8_t& l);

Instruction call(uint8_t value1, uint8_t value2, uint16_t& PC, uint8_t& s, uint8_t& p, vector<uint8_t>& ram);
Instruction call_nz(uint8_t& value1, uint8_t& value2, uint16_t& PC, uint8_t& s, uint8_t& p, vector<uint8_t>& ram, uint8_t& l);
Instruction call_z(uint8_t& value1, uint8_t& value2, uint16_t& PC, uint8_t& s, uint8_t& p, vector<uint8_t>& ram, uint8_t& l);
Instruction call_nc(uint8_t& value1, uint8_t& value2, uint16_t& PC, uint8_t& s, uint8_t& p, vector<uint8_t>& ram, uint8_t& l);
Instruction call_c(uint8_t& value1, uint8_t& value2, uint16_t& PC, uint8_t& s, uint8_t& p, vector<uint8_t>& ram, uint8_t& l);
Instruction rst(uint8_t addr, uint16_t &PC, uint8_t &s, uint8_t &p, std::vector<uint8_t>& ram);
Instruction rst_interrupt(uint8_t addr, uint16_t &PC, uint8_t &s, uint8_t &p, std::vector<uint8_t> &ram);

Instruction add_i8(uint8_t& dest1, uint8_t& dest2, int8_t& value1, uint8_t& l);


uint8_t get_reg_zero(uint8_t &flag);

bool check_joypad_selected(std::vector<uint8_t> &ram);
bool check_dpad_selected(std::vector<uint8_t> &ram);
bool check_start_down_selected(std::vector<uint8_t> &ram);
bool check_select_up_selected(std::vector<uint8_t> &ram);
bool check_b_left_selected(std::vector<uint8_t> &ram);
bool check_a_right_selected(std::vector<uint8_t> &ram);

void select_dpad(std::vector<uint8_t> &ram);
void deselect_dpad(vector<uint8_t> &ram);

// Press/release start or down (bit 3)
void press_start_down(std::vector<uint8_t> &ram);
void release_start_down(std::vector<uint8_t> &ram);

// Press/release select or up (bit 2)
void press_select_up(std::vector<uint8_t> &ram);
void release_select_up(std::vector<uint8_t> &ram);

// Press/release B or left (bit 1)
void press_b_left(std::vector<uint8_t> &ram);
void release_b_left(std::vector<uint8_t> &ram);

// Press/release A or right (bit 0)
void press_a_right(std::vector<uint8_t> &ram);
void release_a_right(std::vector<uint8_t> &ram);

void setPPUMode0(std::vector<uint8_t> &ram);
void setPPUMode1(std::vector<uint8_t> &ram);
void setPPUMode2(std::vector<uint8_t> &ram);
void setPPUMode3(std::vector<uint8_t> &ram);
