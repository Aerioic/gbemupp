#include <math.h>
#include <stdint.h>
#include <vector>
#include "mmu.h"
#include <iostream>
using namespace std;
// These functions set the individual bits on the l register

bool checkVBlankInterrupt(vector<uint8_t>& ram)
{
    return ram[0xffff] & 1;
}

bool checkLCDInterrupt(vector<uint8_t>& ram)
{
    return (ram[0xffff] >> 1) & 1;
}

bool checkTimerInterrupt(vector<uint8_t>& ram)
{
    return (ram[0xffff] >> 2) & 1;
}

bool checkSerialnterrupt(vector<uint8_t>& ram)
{
    return (ram[0xffff] >> 3) & 1;
}
bool checkJoypadInterrupt(vector<uint8_t>& ram)
{
    return (ram[0xffff] >> 4) & 1;
}

bool checkRequestVBlankInterrupt(vector<uint8_t>& ram)
{
    return ram[0xff0f] & 1;
}

bool checkRequestLCDInterrupt(vector<uint8_t>& ram)
{
    return (ram[0xff0f] >> 1) & 1;
}

bool checkRequestTimerInterrupt(vector<uint8_t>& ram)
{
    return (ram[0xff0f] >> 2) & 1;
}

bool checkRequestSerialnterrupt(vector<uint8_t>& ram)
{
    return (ram[0xff0f] >> 3) & 1;
}
bool checkRequestJoypadInterrupt(vector<uint8_t>& ram)
{
    return (ram[0xff0f] >> 4) & 1;
}

void requestVBlankInterrupt(vector<uint8_t> &ram)
{
    ram[0xff0f] |= (1 << 0);
}
void requestLCDInterrupt(vector<uint8_t> &ram)
{
    ram[0xff0f] |= (1 << 1);
}
void requestTimerInterrupt(vector<uint8_t> &ram)
{
    ram[0xff0f] |= (1 << 2);
}
void requestSerialInterrupt(vector<uint8_t> &ram)
{
    ram[0xff0f] |= (1 << 3);
}
void requestJoypadInterrupt(vector<uint8_t> &ram)
{
    ram[0xff0f] |= (1 << 4);
}

void clearVBlankInterrupt(vector<uint8_t> &ram)
{
    ram[0xff0f] &= ~(1 << 0);
}
void clearLCDInterrupt(vector<uint8_t> &ram)
{
    ram[0xff0f] &= ~(1 << 1);
}
void clearTimerInterrupt(vector<uint8_t> &ram)
{
    ram[0xff0f] &= ~(1 << 2);
}
void clearSerialInterrupt(vector<uint8_t> &ram)
{
    ram[0xff0f] &= ~(1 << 3);
}
void clearJoypadInterrupt(vector<uint8_t> &ram)
{
    ram[0xff0f] &= ~(1 << 4);
}

void setPPUMode0(vector<uint8_t> &ram)
{
    ram[0xFF41] &= ~(1 << 0);
    ram[0xFF41] &= ~(1 << 1);
}

void setPPUMode1(vector<uint8_t> &ram)
{
    ram[0xFF41] |= (1 << 0);
    ram[0xFF41] &= ~(1 << 1);
}

void setPPUMode2(vector<uint8_t> &ram)
{
    ram[0xFF41] &= ~(1 << 0);
    ram[0xFF41] |= (1 << 1);
}

void setPPUMode3(vector<uint8_t> &ram)
{
    ram[0xFF41] |= (1 << 0);
    ram[0xFF41] |= (1 << 1);
}

void set_reg_zero(uint8_t &flag, bool value)
{
    if (value)
        flag |= 1 << 7;
    else
        flag &= ~(1 << 7);
    // return Instruction {0, 0};
}

void set_reg_SUBTRACTION(uint8_t &flag, bool value)
{
    if (value)
        flag |= 1 << 6;
    else
        flag &= ~(1 << 6);
}

void set_reg_HALF_CARRY(uint8_t &flag, bool value)
{
    if (value)
        flag |= 1 << 5;
    else
        flag &= ~(1 << 5);
}

bool check_joypad_selected(vector<uint8_t> &ram)
{
    return !((ram[0xff00] >> 5) & 1);
}
bool check_dpad_selected(vector<uint8_t> &ram)
{
    return !((ram[0xff00] >> 4) & 1);
}

bool check_start_down_selected(vector<uint8_t> &ram)
{
    return !((ram[0xff00] >> 3) & 1);
}
bool check_select_up_selected(vector<uint8_t> &ram)
{
    return !((ram[0xff00] >> 2) & 1);
}
bool check_b_left_selected(vector<uint8_t> &ram)
{
    return !((ram[0xff00] >> 1) & 1);
}
bool check_a_right_selected(vector<uint8_t> &ram)
{
    return !((ram[0xff00] >> 0) & 1);
}

// Press/release start or down (bit 3)
void press_start_down(vector<uint8_t> &ram)
{
    ram[0xff00] &= ~(1 << 3);
}
void release_start_down(vector<uint8_t> &ram)
{
    ram[0xff00] |= (1 << 3);
}

// Press/release select or up (bit 2)
void press_select_up(vector<uint8_t> &ram)
{
    ram[0xff00] &= ~(1 << 2);
}
void release_select_up(vector<uint8_t> &ram)
{
    ram[0xff00] |= (1 << 2);
}

// Press/release B or left (bit 1)
void press_b_left(vector<uint8_t> &ram)
{
    ram[0xff00] &= ~(1 << 1);
}
void release_b_left(vector<uint8_t> &ram)
{
    ram[0xff00] |= (1 << 1);
}

// Press/release A or right (bit 0)
void press_a_right(vector<uint8_t> &ram)
{
    ram[0xff00] &= ~(1 << 0);
}
void release_a_right(vector<uint8_t> &ram)
{
    ram[0xff00] |= (1 << 0);
}


Instruction set_reg_CARRY(uint8_t &flag, bool value)
{
    if (value)
        flag |= 1 << 4;
    else
        flag &= ~(1 << 4);
    return Instruction{1, 1};
}

// These functions get the individual bits on the l register

uint8_t get_reg_zero(uint8_t &flag)
{
    return flag >> 7;
}

uint8_t get_reg_SUBTRACTION(uint8_t &flag)
{
    pow(3, 4);
    return (flag & (uint8_t)pow(2, 6)) >> 6;
}

uint8_t get_reg_HALF_CARRY(uint8_t &flag)
{
    return (flag & 1 << 5) >> 5;
}

uint8_t get_reg_CARRY(uint8_t &flag)
{
    return (flag & 1 << 4) >> 4;
}

// these functions get and load from registers

uint16_t get_16(uint8_t upper, uint8_t lower)
{
    return upper << 8 | lower;
}
Instruction set_16(uint8_t &upper, uint8_t &lower, uint16_t to_set)
{
    upper = to_set >> 8;
    lower = 0xff & to_set;
    return Instruction{3, 3};
}

Instruction set_Hlp(uint8_t &upper, uint8_t &lower)
{
    uint16_t x = 0;
    uint16_t hl = (uint16_t)upper << 8 | lower;
    hl++;
    upper = hl >> 8;
    lower = hl & 0xff;
    return Instruction{69, 69};
}

Instruction set_8(uint8_t &to_set, uint8_t value)
{
    to_set = value;
    return Instruction{1, 1};
}

Instruction add_8(uint8_t &dest, uint8_t &value, uint8_t &l)
{
    uint16_t result = dest + value;

    set_reg_CARRY(l, result > 0xFF);
    set_reg_HALF_CARRY(l, ((dest & 0xF) + (value & 0xF)) > 0xF);
    set_reg_zero(l, (uint8_t)result == 0);
    set_reg_SUBTRACTION(l, false);

    dest = (uint8_t)result;

    return {1, 1};
}

Instruction add_8_carry(uint8_t &dest, uint8_t value, uint8_t &l)
{
    uint8_t carry = get_reg_CARRY(l);
    uint8_t result = dest + value + carry;

    set_reg_HALF_CARRY(l, ((dest & 0xF) + (value & 0xF) + carry) > 0xF);
    set_reg_CARRY(l, ((uint16_t)dest + value + carry) > 0xFF);
    set_reg_SUBTRACTION(l, false);
    set_reg_zero(l, result == 0);

    dest = result;
    return Instruction{1, 1};
}

Instruction add_16(uint8_t &dest1, uint8_t &dest2, uint8_t value1, uint8_t value2, uint8_t &l)
{
    uint16_t dest = (uint16_t)dest1 << 8 | dest2;
    uint16_t value = (uint16_t)value1 << 8 | value2;
    uint16_t result = dest + value;

    if (dest + value > 0xffff)
    {
        set_reg_CARRY(l, true);
    }
    else
        set_reg_CARRY(l, false);
    set_reg_HALF_CARRY(l,
                       ((dest & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF);
    set_reg_SUBTRACTION(l, false);

    dest1 = result >> 8;
    dest2 = result & 0xff;

    return {2, 1};
}

Instruction sub_8(uint8_t &dest, uint8_t &value, uint8_t &l)
{

    set_reg_CARRY(l, value > dest);

    set_reg_zero(l, value == dest);
    set_reg_HALF_CARRY(l, (dest & 0xF) < (value & 0xF)); // fix
    set_reg_SUBTRACTION(l, true);

    dest -= value;

    return Instruction{1, 1};
}

Instruction sub_8_carry(uint8_t &dest, uint8_t value, uint8_t &l)
{
    uint8_t carry = get_reg_CARRY(l);
    uint8_t result = dest - value - carry;

    set_reg_HALF_CARRY(l, ((dest & 0xF) < (value & 0xF) + carry));
    set_reg_CARRY(l, ((uint16_t)dest < (uint16_t)value + carry));
    set_reg_SUBTRACTION(l, true);
    set_reg_zero(l, result == 0);

    dest = result;
    return {1, 1};
}

Instruction andf(uint8_t &dest, uint8_t &value, uint8_t &l)
{
    dest &= value;

    set_reg_CARRY(l, false);
    set_reg_zero(l, dest == 0);
    set_reg_HALF_CARRY(l, true);
    set_reg_SUBTRACTION(l, false);

    return Instruction{1, 1};
}

Instruction orf(uint8_t &dest, uint8_t &value, uint8_t &l)
{
    dest |= value;

    set_reg_CARRY(l, false);
    set_reg_zero(l, dest == 0);
    set_reg_HALF_CARRY(l, false); // fix
    set_reg_SUBTRACTION(l, false);
    return Instruction{1, 1};
}

Instruction inc(uint8_t &dest, uint8_t &l)
{
    set_reg_HALF_CARRY(l, (dest & 0x0F) == 0x0F);
    dest += 1;

    set_reg_zero(l, dest == 0);
    set_reg_SUBTRACTION(l, false);

    return Instruction{1, 1};
}

Instruction inc_16(uint8_t &dest1, uint8_t &dest2)
{
    uint16_t sum = (uint16_t)dest1 << 8 | dest2;
    sum++;
    dest1 = sum >> 8;
    dest2 = sum & 0xff;
    return Instruction{2, 1};
}

Instruction dec(uint8_t &dest, uint8_t &l)
{

    set_reg_HALF_CARRY(l, (dest & 0x0F) == 0x00);
    dest -= 1;
    set_reg_zero(l, dest == 0);
    set_reg_SUBTRACTION(l, true);
    return Instruction{1, 1};
}

Instruction dec_16(uint8_t &dest1, uint8_t &dest2)
{
    uint16_t temp = (uint16_t)dest1 << 8 | dest2;
    temp--;
    dest1 = temp >> 8;
    dest2 = temp & 255;
    return Instruction{2, 1};
}

Instruction cp(uint8_t dest, uint8_t value, uint8_t &l, uint8_t _byte)
{

    uint8_t result = dest - value;

    set_reg_zero(l, result == 0);
    set_reg_SUBTRACTION(l, true);
    set_reg_CARRY(l, dest < value);
    set_reg_HALF_CARRY(l, (dest & 0xF) < (value & 0xF));
    if (_byte == 6 || _byte == 6 + 9)
        return Instruction{2, 1};

    return Instruction{1, 1};
}

Instruction xorf(uint8_t &dest, uint8_t &value, uint8_t &l)
{
    dest ^= value;

    set_reg_CARRY(l, false);
    set_reg_zero(l, dest == 0);
    set_reg_HALF_CARRY(l, false); // fix
    set_reg_SUBTRACTION(l, false);

    return Instruction{1, 1};
}

Instruction ccf(uint8_t &l)
{
    set_reg_CARRY(l, !get_reg_CARRY(l));
    set_reg_HALF_CARRY(l, false); // fix
    set_reg_SUBTRACTION(l, false);
    return Instruction{1, 1};
}

Instruction scf(uint8_t &dest, uint8_t &l)
{
    set_reg_CARRY(l, true);
    set_reg_HALF_CARRY(l, false); // fix
    set_reg_SUBTRACTION(l, false);
    return Instruction{1, 1};
}

Instruction rra(uint8_t &dest, uint8_t &l)
{
    uint8_t carry = get_reg_CARRY(l);
    bool b0 = dest & 1;

    dest >>= 1;
    dest |= (carry << 7);
    set_reg_CARRY(l, b0);
    set_reg_zero(l, false);
    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);

    return Instruction{1, 1};

    ///// 1000001 1
    // 110000 1
}

Instruction rla(uint8_t &dest, uint8_t &l)
{
    uint8_t carry = get_reg_CARRY(l);
    bool b7 = dest & 0x80;
    dest <<= 1;
    dest = carry | dest;
    set_reg_CARRY(l, b7);
    set_reg_zero(l, false);
    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);
    return Instruction{1, 1};
}

Instruction rrca(uint8_t &dest, uint8_t &l)
{
    bool b0 = dest & 1;
    dest >>= 1;
    dest = (b0 << 7) | dest;
    set_reg_CARRY(l, b0);
    set_reg_zero(l, false);
    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);

    return Instruction{1, 1};
}

Instruction rlca(uint8_t &dest, uint8_t &l)
{
    uint8_t carry = dest >> 7;

    dest = (dest << 1) | carry;

    set_reg_CARRY(l, carry);
    set_reg_zero(l, false);
    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);

    return Instruction{1, 1};
}

Instruction bit(uint8_t &dest, uint8_t &l, uint8_t _bit, uint8_t _byte)
{
    if ((dest >> _bit) & 1)
    {
        set_reg_zero(l, false);
    }
    else
        set_reg_zero(l, true);

    set_reg_HALF_CARRY(l, true);
    set_reg_SUBTRACTION(l, false);

    if (_byte == 6 || _byte == 0xe)
    {
        return Instruction{3, 2};
    }
    return Instruction{2, 2};
}

Instruction res(uint8_t &dest, uint8_t &l, uint8_t _bit, uint8_t _byte)
{
    dest &= ~(1 << _bit);
    if (_byte == 6 || _byte == 0xe)
    {
        return Instruction{4, 2};
    }
    return Instruction{2, 2};
}

Instruction set(uint8_t &dest, uint8_t &l, uint8_t _bit)
{
    dest |= 1 << _bit;
    // set_reg_zero(l, dest);
    return Instruction{2, 2};
}

Instruction srl(uint8_t &dest, uint8_t &l)
{
    set_reg_CARRY(l, dest & 1);
    dest = dest >> 1;
    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);
    set_reg_zero(l, dest == 0);

    return {2, 2};

    // set_reg_zero(l, dest);
}

Instruction rr(uint8_t &dest, uint8_t &l)
{
    bool old_carry_bit = get_reg_CARRY(l);
    set_reg_CARRY(l, dest & 1);

    dest >>= 1;
    dest = dest | old_carry_bit << 7;

    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);
    set_reg_zero(l, dest == 0);

    return Instruction{2, 2};
}

Instruction rl(uint8_t &dest, uint8_t &l)
{
    bool old_carry_bit = get_reg_CARRY(l);
    set_reg_CARRY(l, dest >> 7);

    dest <<= 1;
    dest = dest | old_carry_bit;

    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);
    set_reg_zero(l, dest == 0);

    return Instruction{2, 2};
}

Instruction rrc(uint8_t &dest, uint8_t &l)
{
    uint8_t old_bit0 = dest & 1;
    set_reg_CARRY(l, old_bit0);
    dest = (dest >> 1) | (old_bit0 << 7);  // rotate old bit 0 into bit 7
    
    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);
    set_reg_zero(l, dest == 0);
    return Instruction{2, 2};
}

Instruction rlc(uint8_t &dest, uint8_t &l)
{
    uint8_t carry = dest >> 7;
    set_reg_CARRY(l, (bool)carry);
    dest <<= 1;

    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);
    if (carry == 0)
    {
        dest = dest & ~(1);
    }
    else
    {
        dest = dest | 1;
    }
    set_reg_zero(l, dest == 0);
    return Instruction{2, 2};
}

Instruction sra(uint8_t &dest, uint8_t &l)
{
    set_reg_CARRY(l, dest & 1);
    uint8_t most_sig_bit = dest >> 7;
    dest >>= 1;
    dest = dest | (most_sig_bit << 7);

    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);
    set_reg_zero(l, dest == 0);
    return Instruction{2, 2};
}

Instruction sla(uint8_t &dest, uint8_t &l)
{
    set_reg_CARRY(l, dest >> 7);
    dest <<= 1;

    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);
    set_reg_zero(l, dest == 0);

    return Instruction{2, 2};
}

Instruction swap(uint8_t &dest, uint8_t &l)
{
    dest = (dest << 4) | (dest >> 4);

    set_reg_SUBTRACTION(l, false);
    set_reg_HALF_CARRY(l, false);
    set_reg_zero(l, dest == 0);
    set_reg_CARRY(l, false);

    return Instruction{2, 2};
}

Instruction jr(uint16_t &dest, int8_t &modifier)
{
    dest += modifier;
    return {2, 2};
}

Instruction jr_nz(uint16_t &dest, int8_t &modifier, uint8_t &l)
{

    if (!get_reg_zero(l))
        return jr(dest, modifier);
    return {2, 2};
}

Instruction jr_z(uint16_t &dest, int8_t &modifier, uint8_t &l)
{
    if (get_reg_zero(l))
        return jr(dest, modifier);
    return {2, 2};
}

Instruction jr_nc(uint16_t &dest, int8_t &modifier, uint8_t &l)
{
    if (!get_reg_CARRY(l))
        return jr(dest, modifier);
    return {2, 2};
}

Instruction jr_c(uint16_t &dest, int8_t &modifier, uint8_t &l)
{
    if (get_reg_CARRY(l))
        return jr(dest, modifier);
    return {2, 2};
}

Instruction cpl(uint8_t &dest, uint8_t &l)
{
    set_reg_HALF_CARRY(l, true);
    set_reg_SUBTRACTION(l, true);
    dest = ~dest;
    return Instruction{1, 1};
}
// value1 is msb
// value2 is lsb
Instruction push(uint8_t &value1, uint8_t &value2, uint8_t &s, uint8_t &p, vector<uint8_t> &ram)
{

    dec_16(s, p);
    uint16_t SP = (uint16_t)s << 8 | p;
    ram[SP] = value1; // high byte at new SP
    dec_16(s, p);
    SP = (uint16_t)s << 8 | p;
    ram[SP] = value2; // low byte at new SP-1
    return Instruction{4, 1};
}

Instruction pop(uint8_t &set1, uint8_t &set2, uint8_t &s, uint8_t &p, vector<uint8_t> &ram)
{
    uint16_t SP = (uint16_t)s << 8 | p;
    set2 = ram[SP]; // read low byte at current SP
    inc_16(s, p);
    SP = (uint16_t)s << 8 | p; // re-read SP after increment
    set1 = ram[SP];            // read high byte at new SP
    inc_16(s, p);
    return Instruction{3, 1};
}


Instruction ret(uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram)
{
    uint16_t SP = (uint16_t)s << 8 | p;
    PC = (uint16_t)ram[SP + 1] << 8 | ram[SP];
    SP += 2;
    s = SP >> 8;   // high byte
    p = SP & 0xFF; // low byte
    return Instruction{4, 0};
}

Instruction ret_nz(uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram, uint8_t &l)
{
    if (!get_reg_zero(l))
    {
        ret(PC, s, p, ram);
        return Instruction{5, 0};
    }
    return Instruction{2, 1};
}

Instruction ret_z(uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram, uint8_t &l)
{
    if (get_reg_zero(l))
    {
        ret(PC, s, p, ram);
        return Instruction{5, 0};
    }
    return Instruction{2, 1};
}

Instruction ret_nc(uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram, uint8_t &l)
{
    if (!get_reg_CARRY(l))
    {
        ret(PC, s, p, ram);
        return Instruction{5, 0};
    }
    return Instruction{2, 1};
}

Instruction ret_c(uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram, uint8_t &l)
{
    if (get_reg_CARRY(l))
    {
        ret(PC, s, p, ram);
        return Instruction{5, 0};
    }
    return Instruction{2, 1};
}

Instruction jp(uint8_t &value1, uint8_t &value2, uint16_t &PC)
{
    PC = (uint16_t)value1 << 8 | value2;
    return Instruction{4, 0};
}

Instruction jp_nz(uint8_t &value1, uint8_t &value2, uint16_t &PC, uint8_t &l)
{
    if (!get_reg_zero(l))
        return jp(value1, value2, PC);
    return Instruction{3, 3};
}

Instruction jp_nc(uint8_t &value1, uint8_t &value2, uint16_t &PC, uint8_t &l)
{
    if (!get_reg_CARRY(l))
        return jp(value1, value2, PC);
    return Instruction{3, 3};
}

Instruction jp_c(uint8_t &value1, uint8_t &value2, uint16_t &PC, uint8_t &l)
{
    if (get_reg_CARRY(l))
        return jp(value1, value2, PC);
    return Instruction{3, 3};
}

Instruction jp_z(uint8_t &value1, uint8_t &value2, uint16_t &PC, uint8_t &l)
{
    if (get_reg_zero(l))
        return jp(value1, value2, PC);
    return Instruction{3, 3};
}

Instruction call(uint8_t value1, uint8_t value2, uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram)
{
    uint16_t nextInstructionAddress = PC + 3;
    uint8_t nextInstructionAddressUpper = nextInstructionAddress >> 8;
    uint8_t nextInstructionAddressLower = nextInstructionAddress & 0xff;
    push(nextInstructionAddressUpper, nextInstructionAddressLower, s, p, ram);
    PC = (uint16_t)value2 << 8 | value1;

    return Instruction{6, 0};
}

Instruction rst(uint8_t addr, uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram)
{
    uint16_t ret = PC + 1; // RST is 1 byte, return to next instruction
    uint8_t upper = ret >> 8;
    uint8_t lower = ret & 0xff;
    push(upper, lower, s, p, ram);
    PC = addr;
    return Instruction{4, 0};
}

Instruction rst_interrupt(uint8_t addr, uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram)
{
    uint16_t ret = PC; // We want to return to the same location
    uint8_t upper = ret >> 8;
    uint8_t lower = ret & 0xff;
    push(upper, lower, s, p, ram);
    PC = addr;
    return Instruction{4, 0};
}

Instruction call_nz(uint8_t &value1, uint8_t &value2, uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram, uint8_t &l)
{
    if (!get_reg_zero(l))
    {
        return call(value1, value2, PC, s, p, ram);
    }
    return Instruction{3, 3};
}

Instruction call_nc(uint8_t &value1, uint8_t &value2, uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram, uint8_t &l)
{
    if (!get_reg_CARRY(l))
    {
        return call(value1, value2, PC, s, p, ram);
    }
    return Instruction{3, 3};
}

Instruction call_z(uint8_t &value1, uint8_t &value2, uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram, uint8_t &l)
{
    if (get_reg_zero(l))
        return call(value1, value2, PC, s, p, ram);
    return Instruction{3, 3};
}

Instruction call_c(uint8_t &value1, uint8_t &value2, uint16_t &PC, uint8_t &s, uint8_t &p, vector<uint8_t> &ram, uint8_t &l)
{
    if (get_reg_CARRY(l))
        return call(value1, value2, PC, s, p, ram);
    return Instruction{3, 3};
}

Instruction add_i8(uint8_t &dest1, uint8_t &dest2, int8_t &value1, uint8_t &l)
{
    uint16_t SP = (uint16_t)dest1 << 8 | dest2;
    uint8_t uval = (uint8_t)value1;

    set_reg_CARRY(l, (SP & 0xFF) + uval > 0xFF);
    set_reg_HALF_CARRY(l, (SP & 0x0F) + (uval & 0x0F) > 0x0F);
    set_reg_zero(l, false);
    set_reg_SUBTRACTION(l, false);

    SP += value1;
    dest1 = SP >> 8;
    dest2 = SP & 0xff;

    return Instruction{4, 2};
}

Instruction ld_hl_sp_i8(uint8_t &h, uint8_t &l, uint8_t &s, uint8_t &p, int8_t value, uint8_t &f)
{
    uint16_t SP = (uint16_t)s << 8 | p;

    set_reg_zero(f, false);
    set_reg_SUBTRACTION(f, false);

    if ((SP & 0xff) + (uint8_t)value > 0xff)
        set_reg_CARRY(f, true);
    else
        set_reg_CARRY(f, false);

    if ((SP & 0xf) + (value & 0xf) > 0xf)
        set_reg_HALF_CARRY(f, true);
    else
        set_reg_HALF_CARRY(f, false);

    uint16_t result = (uint16_t)SP + value;
    h = result >> 8;
    l = result & 0xff;
    return Instruction{3, 2};
}

Instruction DAA(uint8_t &a, uint8_t &f)
{
    if (!get_reg_SUBTRACTION(f)) {  // after addition
        if (get_reg_CARRY(f) || a > 0x99) {
            a += 0x60;
            set_reg_CARRY(f, true);
        }
        if (get_reg_HALF_CARRY(f) || (a & 0x0F) > 9)
            a += 0x06;
    } else {  // after subtraction
        if (get_reg_HALF_CARRY(f))
            a -= 0x06;
        if (get_reg_CARRY(f))
            a -= 0x60;
    }

    set_reg_HALF_CARRY(f, false);
    set_reg_zero(f, false);
    if (a == 0)
        set_reg_zero(f, true);

    return Instruction{1, 1};
}