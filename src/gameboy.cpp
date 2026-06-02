#include <map>
#include <stack>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <stdexcept>

#include "gameboy.h"
#include "mmu.h"
#include "input.h"
using namespace std;

void CPU::constantAritmaticSetup()
{
    // for opcodes 0x80-0xbf arithmatic operations are done, and for the second argument, there is a pattern
    // this uses the lower nibble to determine what arithmatic

    arithmeticConstants[0] = &reg.b;
    arithmeticConstants[1] = &reg.c;
    arithmeticConstants[2] = &reg.d;
    arithmeticConstants[3] = &reg.e;
    arithmeticConstants[4] = &reg.h;
    arithmeticConstants[5] = &reg.l;
    // arithmeticConstants[6] = &ram[hl]; this is done on later lines of code
    arithmeticConstants[7] = &reg.a;

    arithmeticConstants[8] = &reg.b;
    arithmeticConstants[9] = &reg.c;
    arithmeticConstants[10] = &reg.d;
    arithmeticConstants[11] = &reg.e;
    arithmeticConstants[12] = &reg.h;
    arithmeticConstants[13] = &reg.l;
    // arithmeticConstants[14] = &ram[hl];
    arithmeticConstants[15] = &reg.a;
}

CPU::CPU()
{
    for (int i = 0; i <= 0xffff; i++)
    {
        ram.push_back(0);
    }
    ram[0xFF00] = 0xFF;
    ram[0xFF05] = 0x00;
    ram[0xFF06] = 0x00;
    ram[0xFF07] = 0x00;
    ram[0xFF10] = 0x80;
    ram[0xFF11] = 0xBF;
    ram[0xFF12] = 0xF3;
    ram[0xFF14] = 0xBF;
    ram[0xFF16] = 0x3F;
    ram[0xFF17] = 0x00;
    ram[0xFF19] = 0xBF;
    ram[0xFF1A] = 0x7F;
    ram[0xFF1B] = 0xFF;
    ram[0xFF1C] = 0x9F;
    ram[0xFF1E] = 0xBF;
    ram[0xFF20] = 0xFF;
    ram[0xFF21] = 0x00;
    ram[0xFF22] = 0x00;
    ram[0xFF23] = 0xBF;
    ram[0xFF24] = 0x77;
    ram[0xFF25] = 0xF3;
    ram[0xFF26] = 0xF1;
    ram[0xFF40] = 0x91;
    ram[0xFF42] = 0x00;
    ram[0xFF43] = 0x00;
    ram[0xFF45] = 0x00;
    ram[0xFF47] = 0xFC;
    ram[0xFF48] = 0xFF;
    ram[0xFF49] = 0xFF;
    ram[0xFF4A] = 0x00;
    ram[0xFF4B] = 0x00;
    ram[0xFFFF] = 0x00;
    constantAritmaticSetup();
}

Instruction CPU::cycle(FILE *fptr, Input *input)
{
    // ram[0xff44] = 0x90;
    //  handle_joypad();
    /*
    fprintf(
        fptr,
        "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
        reg.a,
        reg.f,
        reg.b,
        reg.c,
        reg.d,
        reg.e,
        reg.h,
        reg.l,
        (reg.s << 8) | reg.p, // SP
        pc,
        ram[pc],
        ram[(pc + 1) & 0xFFFF],
        ram[(pc + 2) & 0xFFFF],
        ram[(pc + 3) & 0xFFFF]);
    */
    Instruction result = process(input);
    pc += result.length;
    for (int i = 0; i < result.cycle * 4; i++)
    {
        handle_tima();
    }
    if (IME_pending)
    {
        IME = 1;
        IME_pending = false;
    }
    interrupt();
    return result;
}


void CPU::debug_print(uint8_t opcode, uint8_t arg1s, uint8_t arg2)
{\
    // this code is used to deubg the emualtor
    // if (opcode == 0x76) return;

    if (pc == 0x4b6)
    {
        debug_mode = 300;
    }
    if (debug_mode < 0)
        return;

    if (pc == 0x205f)
    {
        uint16_t addr = ram[0x2060] | ram[0x2061] << 8;
        printf("state check: [%04x] = %02x\n", addr, ram[addr]);
    }
    debug_mode--;
    printf("opcode: %02x pc: %04x sp: %04x comb: %04x D: %02x line: %d\n",
           opcode, pc, reg.s << 8 | reg.p, arg2 << 8 | arg1s, reg.d, debug_mode);

    if (pc == 0x68cd)
    {
        printf("HL=%04x val=%02x\n",
               reg.h << 8 | reg.l, ram[reg.h << 8 | reg.l]);
    }
}

// returns the delay
Instruction CPU::process(Input *input)
{
    // little endian
    // so arg 2 is bigger than arg 1
    // handle_tima();
    // interrupt();
    opcode = ram[pc];
    uint8_t arg1 = ram[pc + 1];
    uint8_t arg2 = ram[pc + 2];
    uint8_t arg3 = ram[pc + 3];

    int8_t arg1s = (int8_t)arg1;                 // this is used for the signed jumping instruction;
    uint16_t argcb = (uint16_t)arg2 << 8 | arg1; // this is used for 16 bit operations
    uint16_t sp = reg.s << 8 | reg.p;
    uint16_t hl = reg.h << 8 | reg.l;

    arithmeticConstants[6] = &ram[hl];
    arithmeticConstants[14] = &ram[hl];

    uint8_t t_nibble = opcode >> 4;
    uint8_t b_nibble = opcode & 0xf;

    uint8_t opcode_cb = arg1;

    uint8_t *SCX = &ram[0xff42];
    uint8_t *SCY = &ram[0xff43];

    uint8_t zero = 0;

    switch (opcode)
    {
    case 0x00:
        return Instruction{1, 1};
        break;
    case 0x01:
        return set_16(reg.b, reg.c, arg2 << 8 | arg1);
        break;
    case 0x02:
        (set_8(ram[reg.b << 8 | reg.c], reg.a));
        return Instruction{2, 1};
        break;
    case 0x03:
        return (inc_16(reg.b, reg.c));
        break;
    case 0x04:
        return (inc(reg.b, reg.f));
        break;
    case 0x05:
        return (dec(reg.b, reg.f));
        break;
    case 0x06:
        reg.b = arg1;
        return Instruction{2, 2};
        break;
    case 0x07:
        return (rlca(reg.a, reg.f));
        break;
    case 0x08:
        // experimental
        ram[argcb] = sp & 0xFF;
        ram[argcb + 1] = (sp >> 8);
        return Instruction{5, 3};
        break;
    case 0x09:
        return add_16(reg.h, reg.l, reg.b, reg.c, reg.f);
        break;
    case 0x0a:
        (set_8(reg.a, ram[reg.b << 8 | reg.c]));
        return Instruction{2, 1};
        break;
    case 0x0b:
        return (dec_16(reg.b, reg.c));
        break;
    case 0x0c:
        return (inc(reg.c, reg.f));
        break;
    case 0x0d:
        return (dec(reg.c, reg.f));
        break;
    case 0x0e:
        (set_8(reg.c, arg1));
        return Instruction{2, 2};
        break;
    case 0x0f:
        return rrca(reg.a, reg.f);
        break;
    case 0x10:
        return Instruction{1, 2};
        break;
    case 0x11:
        return (set_16(reg.d, reg.e, arg2 << 8 | arg1));
        break;
    case 0x12:
        (set_8(ram[reg.d << 8 | reg.e], reg.a));
        return Instruction{2, 1};
        break;
    case 0x13:
        return (inc_16(reg.d, reg.e));
        break;
    case 0x14:
        return (inc(reg.d, reg.f));
        break;
    case 0x15:
        return (dec(reg.d, reg.f)); // TO FIX !!!!!
        break;
    case 0x16:
        (set_8(reg.d, arg1));
        return Instruction{2, 2};
        break;
    case 0x17:
        return (rla(reg.a, reg.f));
        break;
    case 0x18:
        return (jr(pc, arg1s));
        break;
    case 0x19:
        return (add_16(reg.h, reg.l, reg.d, reg.e, reg.f));
        break;
    case 0x1A:
        (set_8(reg.a, ram[reg.d << 8 | reg.e]));
        return Instruction{2, 1};
        break;
    case 0x1b:
        return (dec_16(reg.d, reg.e));
        break;
    case 0x1c:
        return (inc(reg.e, reg.f));
        break;
    case 0x1d:
        return (dec(reg.e, reg.f));
        break;
    case 0x1e:
        (set_8(reg.e, arg1));
        return Instruction{2, 2};
        break;
    case 0x1f:
        return (rra(reg.a, reg.f));
        break;
    case 0x20:

        return jr_nz(pc, arg1s, reg.f);
        break;
    case 0x21:
        return set_16(reg.h, reg.l, arg2 << 8 | arg1);
        break;
    case 0x22:
        set_8(ram[reg.h << 8 | reg.l], reg.a);
        inc_16(reg.h, reg.l);
        return Instruction{2, 1};
        break;
    case 0x23:
        return inc_16(reg.h, reg.l);
        break;
    case 0x24:
        return inc(reg.h, reg.f);
        break;
    case 0x25:
        return dec(reg.h, reg.f);
        break;
    case 0x26:
        set_8(reg.h, arg1);
        return Instruction{2, 2};
        break;
    case 0x27:
        return DAA(reg.a, reg.f);
        break;
    case 0x28:
        return jr_z(pc, arg1s, reg.f);
        break;
    case 0x29:
        return add_16(reg.h, reg.l, reg.h, reg.l, reg.f);
        break;
    case 0x2a:
        set_8(reg.a, ram[reg.h << 8 | reg.l]);
        inc_16(reg.h, reg.l);
        return Instruction{2, 1};
        break;
    case 0x2b:
        return dec_16(reg.h, reg.l);
        break;
    case 0x2c:
        return inc(reg.l, reg.f);
        break;
    case 0x2d:
        return dec(reg.l, reg.f);
        break;
    case 0x2e:
        set_8(reg.l, arg1);
        return Instruction{2, 2};
        break;
    case 0x2f:
        return cpl(reg.a, reg.f);
        break;
    case 0x30:
        return jr_nc(pc, arg1s, reg.f);
        break;
    case 0x31:
        return set_16(reg.s, reg.p, argcb);
        break;
    case 0x32:
        set_8(ram[hl], reg.a);
        dec_16(reg.h, reg.l);
        return Instruction{2, 1};
        break;
    case 0x33:
        return inc_16(reg.s, reg.p);
        break;
    case 0x34:
        inc(ram[hl], reg.f);
        return Instruction{3, 1};
        break;
    case 0x35:
        dec(ram[hl], reg.f);
        return Instruction{3, 1};
        break;
    case 0x36:
        set_8(ram[hl], arg1);
        return Instruction{3, 2};
        break;
    case 0x37:
        set_reg_SUBTRACTION(reg.f, false);
        set_reg_HALF_CARRY(reg.f, false);
        return set_reg_CARRY(reg.f, true);
        break;
    case 0x38:
        return jr_c(pc, arg1s, reg.f);
        break;
    case 0x39:
        add_16(reg.h, reg.l, reg.s, reg.p, reg.f);
        return Instruction{2, 1};
        break;
    case 0x3a:
        set_8(reg.a, ram[hl]);
        dec_16(reg.h, reg.l);
        return Instruction{2, 1};
        break;
    case 0x3b:
        return dec_16(reg.s, reg.p);
        break;
    case 0x3c:
        return inc(reg.a, reg.f);
        break;
    case 0x3d:
        return dec(reg.a, reg.f);
        break;
    case 0x3e:
        set_8(reg.a, arg1);
        return Instruction{2, 2};
        break;
    case 0x3f:
        return ccf(reg.f);
        break;
    case 0x40:
        return set_8(reg.b, reg.b);
        break;
    case 0x41:
        return set_8(reg.b, reg.c);
        break;
    case 0x42:
        return set_8(reg.b, reg.d);
        break;
    case 0x43:
        return set_8(reg.b, reg.e);
        break;
    case 0x44:
        return set_8(reg.b, reg.h);
        break;
    case 0x45: // f
        return set_8(reg.b, reg.l);
        break;
    case 0x46:
        set_8(reg.b, ram[hl]);
        return Instruction{2, 1};
        break;
    case 0x47:
        return set_8(reg.b, reg.a);
        break;

    case 0x48:
        return set_8(reg.c, reg.b);
        break;
    case 0x49:
        return set_8(reg.c, reg.c);
        break;
    case 0x4a:
        return set_8(reg.c, reg.d);
        break;
    case 0x4b:
        return set_8(reg.c, reg.e);
        break;
    case 0x4c:
        return set_8(reg.c, reg.h);
        break;
    case 0x4d:
        return set_8(reg.c, reg.l);
        break;
    case 0x4e:
        set_8(reg.c, ram[hl]);
        return Instruction{2, 1};
        break;
    case 0x4f:
        return set_8(reg.c, reg.a);
        break;
    case 0x50:
        return set_8(reg.d, reg.b);
        break;
    case 0x51:
        return set_8(reg.d, reg.c);
        break;
    case 0x52:
        return set_8(reg.d, reg.d);
        break;
    case 0x53:
        return set_8(reg.d, reg.e);
        break;
    case 0x54:
        return set_8(reg.d, reg.h);
        break;
    case 0x55:
        return set_8(reg.d, reg.l);
        break;
    case 0x56:
        set_8(reg.d, ram[hl]);
        return Instruction{2, 1};
        break;
    case 0x57:
        return set_8(reg.d, reg.a);
        break;
    case 0x58:
        return set_8(reg.e, reg.b);
        break;
    case 0x59:
        return set_8(reg.e, reg.c);
        break;
    case 0x5a:
        return set_8(reg.e, reg.d);
        break;
    case 0x5b:
        return set_8(reg.e, reg.e);
        break;
    case 0x5c:
        return set_8(reg.e, reg.h);
        break;
    case 0x5d:
        return set_8(reg.e, reg.l);
        break;
    case 0x5e:
        set_8(reg.e, ram[hl]);
        return Instruction{2, 1};
        break;
    case 0x5f:
        return set_8(reg.e, reg.a);
        break;
    case 0x60:
        return set_8(reg.h, reg.b);
        break;
    case 0x61:
        return set_8(reg.h, reg.c);
        break;
    case 0x62:
        return set_8(reg.h, reg.d);
        break;
    case 0x63:
        return set_8(reg.h, reg.e);
        break;
    case 0x64:
        return set_8(reg.h, reg.h);
        break;
    case 0x65:
        return set_8(reg.h, reg.l);
        break;
    case 0x66:
        set_8(reg.h, ram[hl]);
        return Instruction{2, 1};
        break;
    case 0x67:
        return set_8(reg.h, reg.a);
        break;
    case 0x68:
        return set_8(reg.l, reg.b);
        break;
    case 0x69:
        return set_8(reg.l, reg.c);
        break;
    case 0x6a:
        return set_8(reg.l, reg.d);
        break;
    case 0x6b:
        return set_8(reg.l, reg.e);
        break;
    case 0x6c:
        return set_8(reg.l, reg.h);
        break;
    case 0x6d:
        return set_8(reg.l, reg.l);
        break;
    case 0x6e:
        set_8(reg.l, ram[hl]);
        return Instruction{2, 1};
        break;
    case 0x6f:
        return set_8(reg.l, reg.a);
        break;
    case 0x70:
        set_8(ram[hl], reg.b);
        return Instruction{2, 1};
        break;
    case 0x71:
        set_8(ram[hl], reg.c);
        return Instruction{2, 1};
        break;
    case 0x72:
        set_8(ram[hl], reg.d);
        return Instruction{2, 1};
        break;
    case 0x73:
        set_8(ram[hl], reg.e);
        return Instruction{2, 1};
        break;
    case 0x74:
        set_8(ram[hl], reg.h);
        return Instruction{2, 1};
        break;
    case 0x75:
        set_8(ram[hl], reg.l);
        return Instruction{2, 1};
        break;
    case 0x76:
        is_halted = true;
        return Instruction{1, 0};
        break;
    case 0x77:
        set_8(ram[hl], reg.a);
        return Instruction{2, 1};
        break;
    case 0x78:
        return set_8(reg.a, reg.b);
        break;
    case 0x79:
        return set_8(reg.a, reg.c);
        break;
    case 0x7a:
        return set_8(reg.a, reg.d);
        break;
    case 0x7b:
        return set_8(reg.a, reg.e);
        break;
    case 0x7c:
        return set_8(reg.a, reg.h);
        break;
    case 0x7d:
        return set_8(reg.a, reg.l);
        break;
    case 0x7e:
        set_8(reg.a, ram[hl]);
        return Instruction{2, 1};
        break;
    case 0x7f:
        return set_8(reg.a, reg.a);
        break;
    case 0xc0:
        return ret_nz(pc, reg.s, reg.p, ram, reg.f);
        break;
    case 0xc1:
        return pop(reg.b, reg.c, reg.s, reg.p, ram);
        break;
    case 0xc2:
        return jp_nz(arg2, arg1, pc, reg.f);
        break;
    case 0xc3:
        return jp(arg2, arg1, pc);
        break;
    case 0xc4:
        return call_nz(arg1, arg2, pc, reg.s, reg.p, ram, reg.f);
        break;
    case 0xc5:
        return push(reg.b, reg.c, reg.s, reg.p, ram);
        break;
    case 0xc6:
        add_8(reg.a, arg1, reg.f);
        return Instruction{2, 2};
        break;
    case 0xc7:
        return rst(0, pc, reg.s, reg.p, ram);
        break; // todo
    case 0xc8:
        return ret_z(pc, reg.s, reg.p, ram, reg.f);
        break;
    case 0xc9:
        return ret(pc, reg.s, reg.p, ram);
        break;
    case 0xca:
        return jp_z(arg2, arg1, pc, reg.f);
        break;
    case 0xcb:
        return cb_prefix(opcode_cb);
        break;
    case 0xcc:
        return call_z(arg1, arg2, pc, reg.s, reg.p, ram, reg.f);
        break;
    case 0xcd:
        return call(arg1, arg2, pc, reg.s, reg.p, ram);
        break;
    case 0xce:
        add_8_carry(reg.a, arg1, reg.f);
        return Instruction{2, 2};
        break;
    case 0xcf:
        return rst(0x8, pc, reg.s, reg.p, ram);
    case 0xd0:
        return ret_nc(pc, reg.s, reg.p, ram, reg.f);
        break;
    case 0xd1:
        return pop(reg.d, reg.e, reg.s, reg.p, ram);
        break;
    case 0xd2:
        return jp_nc(arg2, arg1, pc, reg.f);
        break;
    case 0xd3:
        return jp(arg2, arg1, pc);
        break;
    case 0xd4:
        return call_nc(arg1, arg2, pc, reg.s, reg.p, ram, reg.f);
        break;
    case 0xd5:
        return push(reg.d, reg.e, reg.s, reg.p, ram);
        break;
    case 0xd6:
        sub_8(reg.a, arg1, reg.f);
        return Instruction{2, 2};
        break;
    case 0xd7:
        return rst(0x10, pc, reg.s, reg.p, ram);
        break;
    case 0xd8:
        return ret_c(pc, reg.s, reg.p, ram, reg.f);
        break;
    case 0xd9:
        ret(pc, reg.s, reg.p, ram);

        IME = 1;
        return Instruction{4, 0};
        break;
    case 0xda:
        return jp_c(arg2, arg1, pc, reg.f);
        break;
    case 0xdc:
        return call_c(arg1, arg2, pc, reg.s, reg.p, ram, reg.f);
        break;
    case 0xde:
        sub_8_carry(reg.a, arg1, reg.f);
        return Instruction{2, 2};
        break;
    case 0xdf:
        return rst(0x18, pc, reg.s, reg.p, ram);
        break;
    case 0xe0:
        
        set_8(ram[0xff00 + arg1], reg.a);
        return Instruction{3, 2};
        break;

    case 0xe1:
        return pop(reg.h, reg.l, reg.s, reg.p, ram);
        break;
    case 0xe2:
        return set_8(ram[0xff00 + reg.c], reg.a);
        break;
    case 0xe5:
        return push(reg.h, reg.l, reg.s, reg.p, ram);
        break;
    case 0xe6:
        andf(reg.a, arg1, reg.f);
        return Instruction{2, 2};
        break;
    case 0xe7:
        return rst(0x20, pc, reg.s, reg.p, ram);
        break;

    case 0xe8:
        return add_i8(reg.s, reg.p, arg1s, reg.f);
        break;
    case 0xe9:
        pc = (reg.h << 8) | reg.l;
        return Instruction{1, 0};
        break;
    case 0xea:
        set_8(ram[argcb], reg.a);
        return {4, 3};
        break;
    case 0xee:
        xorf(reg.a, arg1, reg.f);
        return Instruction{2, 2};
        break;
    case 0xef:
        return rst(0x28, pc, reg.s, reg.p, ram);
        break;
    case 0xf0:
        if (arg1 == 0)
        {
            ram[0xff00] = input->get_input_buffer(ram[0xff00]);
        }
        set_8(reg.a, ram[0xff00 + arg1]);
        return {3, 2};
        break;
    case 0xf1:
    {
        pop(reg.a, reg.f, reg.s, reg.p, ram);
        reg.f &= 0xf0;
        return Instruction{3, 1};
        break;
    }
    case 0xf2:
        return set_8(reg.a, ram[reg.c + 0xff00]);
        break;
    case 0xf3:
        IME = 0;
        return Instruction{1, 1};
        break;
    case 0xf5:
        return push(reg.a, reg.f, reg.s, reg.p, ram);
        break;
    case 0xf6:
        orf(reg.a, arg1, reg.f);
        return Instruction{2, 2};
        break;
    case 0xf7:
        return rst(0x30, pc, reg.s, reg.p, ram);
        break;
    case 0xf8:
        return ld_hl_sp_i8(reg.h, reg.l, reg.s, reg.p, arg1s, reg.f);
        break;
    case 0xf9:
        set_16(reg.s, reg.p, reg.h << 8 | reg.l);
        return Instruction{2, 1};
        break;
    case 0xfa:
        set_8(reg.a, ram[argcb]);
        return Instruction{4, 3};
        break;
    case 0xfb:
        IME_pending = true;
        return Instruction{1, 1};
        break;
    case 0xfe:
        cp(reg.a, arg1, reg.f, 0);
        return Instruction{2, 2};
        break;
    case 0xff:
        return rst(0x38, pc, reg.s, reg.p, ram);
        break;
    }

    if (t_nibble == 0x8)
    {
        if (b_nibble >= 0x8)
        {
            Instruction solution = add_8_carry(reg.a, *arithmeticConstants[b_nibble], reg.f);
            if (b_nibble == 0xe)
            {
                return Instruction{2, 1};
            }
            return solution;
        }
        else
        {
            Instruction solution = add_8(reg.a, *arithmeticConstants[b_nibble], reg.f);

            if (b_nibble == 6)
            {
                return Instruction{2, 1};
            }
            return solution;
        }
    }

    if (t_nibble == 0x9)
    {
        if (b_nibble >= 0x8)
        {
            Instruction solution = sub_8_carry(reg.a, *arithmeticConstants[b_nibble - 8], reg.f);
            if (b_nibble == 0xe)
            {
                return Instruction{2, 1};
            }
            return solution;
        }
        else
        {
            Instruction solution = sub_8(reg.a, *arithmeticConstants[b_nibble], reg.f);
            if (b_nibble == 6)
            {
                return Instruction{2, 1};
            }
            return solution;
        }
    }

    if (t_nibble == 0xa)
    {
        if (b_nibble >= 0x8)
        {
            Instruction solution = xorf(reg.a, *arithmeticConstants[b_nibble], reg.f);
            if (b_nibble == 0xe)
            {
                return Instruction{2, 1};
            }
            // cout << "yp";
            return solution;
        }
        else
        {
            Instruction solution = andf(reg.a, *arithmeticConstants[b_nibble], reg.f);

            if (b_nibble == 6)
            {
                return Instruction{2, 1};
            }
            return solution;
        }
    }

    if (t_nibble == 0xb)
    {
        if (b_nibble >= 0x8)
        {
            
            Instruction solution = cp(reg.a, *arithmeticConstants[b_nibble], reg.f, b_nibble);
            if (b_nibble == 0xe)
                return Instruction{2, 1};
            return solution;
        }
        else
        { // cp(reg.a, *arithmeticConstants[b_nibble], reg.f, b_nibble)
            Instruction solution = orf(reg.a, *arithmeticConstants[b_nibble], reg.f);

            if (b_nibble == 6)
            {
                return Instruction{2, 1};
            }
            return solution;
        }
    }

    throw invalid_argument("cycle has passed without return");

    return Instruction{100, 100};
}

Instruction CPU::cb_prefix(uint8_t opcode_cb)
{
    uint8_t top_cb = opcode_cb >> 4;
    uint8_t bottom_cb = opcode_cb & 0xf;
    Instruction solution;
    if (top_cb == 0 && bottom_cb < 8)
    {
        solution = rlc(*arithmeticConstants[bottom_cb], reg.f);
    }
    else if (top_cb == 0 && bottom_cb >= 8)
    {
        solution = rrc(*arithmeticConstants[bottom_cb - 8], reg.f);
    }
    else if (top_cb == 1 && bottom_cb < 8)
    {
        solution = rl(*arithmeticConstants[bottom_cb], reg.f);
    }
    else if (top_cb == 1 && bottom_cb >= 8)
    {
        solution = rr(*arithmeticConstants[bottom_cb - 8], reg.f);
    }

    else if (top_cb == 2 && bottom_cb < 8)
    {
        solution = sla(*arithmeticConstants[bottom_cb], reg.f);
    }

    else if (top_cb == 2 && bottom_cb >= 8)
    {
        solution = sra(*arithmeticConstants[bottom_cb - 8], reg.f);
    }

    else if (top_cb == 3 && bottom_cb < 8)
    {
        solution = swap(*arithmeticConstants[bottom_cb], reg.f);
    }
    else if (top_cb == 3 && bottom_cb >= 8)
    {
        solution = srl(*arithmeticConstants[bottom_cb - 8], reg.f);
    }

    else if (top_cb == 4 && bottom_cb < 8)
    {
        solution = bit(*arithmeticConstants[bottom_cb], reg.f, 0, bottom_cb);
    }
    else if (top_cb == 4 && bottom_cb >= 8)
    {
        solution = bit(*arithmeticConstants[bottom_cb - 8], reg.f, 1, bottom_cb);
    }
    else if (top_cb == 5 && bottom_cb < 8)
    {
        solution = bit(*arithmeticConstants[bottom_cb], reg.f, 2, bottom_cb);
    }
    else if (top_cb == 5 && bottom_cb >= 8)
    {
        solution = bit(*arithmeticConstants[bottom_cb - 8], reg.f, 3, bottom_cb);
    }

    else if (top_cb == 6 && bottom_cb < 8)
    {
        solution = bit(*arithmeticConstants[bottom_cb], reg.f, 4, bottom_cb);
    }
    else if (top_cb == 6 && bottom_cb >= 8)
    {
        solution = bit(*arithmeticConstants[bottom_cb - 8], reg.f, 5, bottom_cb);
    }
    else if (top_cb == 7 && bottom_cb < 8)
    {
        solution = bit(*arithmeticConstants[bottom_cb], reg.f, 6, bottom_cb);
    }
    else if (top_cb == 7 && bottom_cb >= 8)
    {
        solution = bit(*arithmeticConstants[bottom_cb - 8], reg.f, 7, bottom_cb);
    }
    else if (top_cb == 8 && bottom_cb < 8)
    {
        solution = res(*arithmeticConstants[bottom_cb], reg.f, 0, bottom_cb);
    }

    else if (top_cb == 8 && bottom_cb >= 8)
    {
        solution = res(*arithmeticConstants[bottom_cb - 8], reg.f, 1, bottom_cb);
    }
    else if (top_cb == 9 && bottom_cb < 8)
    {
        solution = res(*arithmeticConstants[bottom_cb], reg.f, 2, bottom_cb);
    }
    else if (top_cb == 9 && bottom_cb >= 8)
    {
        solution = res(*arithmeticConstants[bottom_cb - 8], reg.f, 3, bottom_cb);
    }
    else if (top_cb == 10 && bottom_cb < 8)
    {
        solution = res(*arithmeticConstants[bottom_cb], reg.f, 4, bottom_cb);
    }
    else if (top_cb == 10 && bottom_cb >= 8)
    {
        solution = res(*arithmeticConstants[bottom_cb - 8], reg.f, 5, bottom_cb);
    }
    else if (top_cb == 11 && bottom_cb < 8)
    {
        solution = res(*arithmeticConstants[bottom_cb], reg.f, 6, bottom_cb);
    }
    else if (top_cb == 11 && bottom_cb >= 8)
    {
        solution = res(*arithmeticConstants[bottom_cb - 8], reg.f, 7, bottom_cb);
    }

    else if (top_cb == 12 && bottom_cb < 8)
    {
        solution = set(*arithmeticConstants[bottom_cb], reg.f, 0);
    }

    else if (top_cb == 12 && bottom_cb >= 8)
    {
        solution = set(*arithmeticConstants[bottom_cb - 8], reg.f, 1);
    }
    else if (top_cb == 13 && bottom_cb < 8)
    {
        solution = set(*arithmeticConstants[bottom_cb], reg.f, 2);
    }
    else if (top_cb == 13 && bottom_cb >= 8)
    {
        solution = set(*arithmeticConstants[bottom_cb - 8], reg.f, 3);
    }
    else if (top_cb == 14 && bottom_cb < 8)
    {
        solution = set(*arithmeticConstants[bottom_cb], reg.f, 4);
    }
    else if (top_cb == 14 && bottom_cb >= 8)
    {
        solution = set(*arithmeticConstants[bottom_cb - 8], reg.f, 5);
    }
    else if (top_cb == 15 && bottom_cb < 8)
    {
        solution = set(*arithmeticConstants[bottom_cb], reg.f, 6);
    }
    else if (top_cb == 15 && bottom_cb >= 8)
    {
        solution = set(*arithmeticConstants[bottom_cb - 8], reg.f, 7);
    }

    if (bottom_cb == 6 || bottom_cb == 0xe)
    {
        solution.cycle += 2;
    }
    return solution;
}

void CPU::interrupt()
{
    uint8_t zero = 0;
    uint8_t vBlankAddress = 0x40;

    if (checkVBlankInterrupt(ram) && checkRequestVBlankInterrupt(ram))
    {
        is_halted = false;
    }

    if (IME == 1)
    {

        if (checkVBlankInterrupt(ram) && checkRequestVBlankInterrupt(ram))
        {
            is_halted = false;
            IME = 0;
            clearVBlankInterrupt(ram);
            rst_interrupt(0x40, pc, reg.s, reg.p, ram);
        }
        else if (checkLCDInterrupt(ram) && checkRequestLCDInterrupt(ram))
        {
            is_halted = false;
            IME = 0;
            clearLCDInterrupt(ram);
            rst_interrupt(0x48, pc, reg.s, reg.p, ram);
        }
        else if (checkTimerInterrupt(ram) && checkRequestTimerInterrupt(ram))
        {
            is_halted = false;
            IME = 0;
            clearTimerInterrupt(ram);
            rst_interrupt(0x50, pc, reg.s, reg.p, ram);
        }
        else if (checkJoypadInterrupt(ram) && checkRequestJoypadInterrupt(ram))
        {
            is_halted = false;
            IME = 0;
            clearJoypadInterrupt(ram);
            rst_interrupt(0x60, pc, reg.s, reg.p, ram);
        }
    }
}

void CPU::handle_tima()
{
    t_cycle++;
    if (t_cycle % 256 == 0)
    {
        ram[0xff04]++;
    }
    if ((ram[0xff07] >> 2) & 1)
    {
        if ((ram[0xff07] & 3) == 0)
        {
            if (t_cycle % 1024 == 0)
            {
                ram[0xff05]++;
            }
        }
        else if ((ram[0xff07] & 3) == 1)
        {
            if (t_cycle % 16 == 0)
            {
                ram[0xff05]++;
            }
        }
        else if ((ram[0xff07] & 3) == 2)
        {
            if (t_cycle % 64 == 0)
            {
                ram[0xff05]++;
            }
        }
        else if ((ram[0xff07] & 3) == 3)
        {
            if (t_cycle % 256 == 0)
            {
                ram[0xff05]++;
            }
        }
        if (ram[0xff05] == 0xff)
        {
            ram[0xff05] = ram[0xff06];
            requestTimerInterrupt(ram);
        }
    }
}