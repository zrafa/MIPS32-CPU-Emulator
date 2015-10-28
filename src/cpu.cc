#include "cpu.h"

void CPU::exe_sll() {
    registers_[rd()] = registers_[rt()] << shift();
    pc_ += 4;
}

void CPU::exe_srl() {
    registers_[rd()] = registers_[rt()] >> shift();
    pc_ += 4;
}

void CPU::exe_sra() {
    int32_t val = registers_[rt()];
    val >>= shift();
    registers_[rd()] = val;
    pc_ += 4;
}

void CPU::exe_sllv() {
    registers_[rd()] = registers_[rt()] >> registers_[rs()];
    pc_ += 4;
}

void CPU::exe_srav() {
    int32_t val = registers_[rt()];
    val >>= registers_[rs()];
    registers_[rd()] = val;
    pc_ += 4;
}

void CPU::exe_jr() {
    pc_ = registers_[rs()];
}

void CPU::exe_jalr() {
    registers_[rd()] = pc_ + 4;
    pc_ = registers_[rs()];
}

void CPU::exe_syscall() {
    // TODO: syscall
}

void CPU::exe_mfhi() {
    registers_[rd()] = hi_;
    pc_ += 4;
}

void CPU::exe_mthi() {
    hi_ = registers_[rs()];
    pc_ += 4;
}

void CPU::exe_mflo() {
    registers_[rd()] = lo_;
    pc_ += 4;
}

void CPU::exe_mtlo() {
    lo_ = registers_[rs()];
    pc_ += 4;
}

void CPU::exe_mult() {
    int64_t a = registers_[rs()];
    int64_t b = registers_[rt()];
    a *= b;
    hi_ = uint32_t(a >> 32);
    lo_ = uint32_t(a);
    pc_ += 4;
}

void CPU::exe_addu() {
    registers_[rd()] = registers_[rs()] + registers_[rd()];
    pc_ += 4;
}

void CPU::exe_subu() {
    registers_[rd()] = registers_[rs()] - registers_[rt()];
    pc_ += 4;
}

void CPU::exe_and() {
    registers_[rd()] = registers_[rs()] & registers_[rt()];
    pc_ += 4;
}

void CPU::exe_or() {
    registers_[rd()] = registers_[rs()] | registers_[rt()];
    pc_ += 4;
}

void CPU::exe_xor() {
    registers_[rd()] = registers_[rs()] ^ registers_[rt()];
    pc_ += 4;
}

void CPU::exe_nor() {
    registers_[rd()] = ~(registers_[rs()] | registers_[rt()]);
    pc_ += 4;
}

void CPU::exe_slt() {
    registers_[rd()] = int32_t(registers_[rs()]) < int32_t(registers_[rt()])? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_sltu() {
    registers_[rd()] = uint32_t(registers_[rs()]) < uint32_t(registers_[rt()])? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_bltz() {
    if (int32_t(registers_[rs()]) < 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_bgez() {
    if (int32_t(registers_[rs()]) >= 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_j() {
    pc_ = (pc_ & 0xf0000000) | jump_target() * 4;
}

void CPU::exe_jal() {
    registers_[REG_RA] = pc_ + 4;
    pc_ = (pc_ & 0xf0000000) | jump_target() * 4;
}

void CPU::exe_beq() {
    if (registers_[rs()] == registers_[rt()])
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_bne() {
    if (registers_[rs()] != registers_[rt()])
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_blez() {
    if (int32_t(registers_[rs()]) <= 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_bgtz() {
    if (int32_t(registers_[rs()]) > 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_addiu() {
    registers_[rt()] = registers_[rs()] + signed_immediate();
    pc_ += 4;
}

void CPU::exe_slti() {
    registers_[rt()] = int32_t(registers_[rs()]) < signed_immediate()? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_sltiu() {
    registers_[rt()] = registers_[rs()] < unsigned_immediate()? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_andi() {
    registers_[rt()] = registers_[rs()] & unsigned_immediate();
    pc_ += 4;
}

void CPU::exe_ori() {
    registers_[rt()] = registers_[rs()] | unsigned_immediate();
    pc_ += 4;
}

void CPU::exe_xori() {
    registers_[rt()] = registers_[rs()] ^ unsigned_immediate();
    pc_ += 4;
}

void CPU::exe_lui() {
    registers_[rt()] = uint32_t(unsigned_immediate()) << 16;
    pc_ += 4;
}

void CPU::exe_mfc0() {
    registers_[rt()] = cp0_.registers_[rd()];
    pc_ += 4;
}

void CPU::exe_mtc0() {
    cp0_.registers_[rd()] = registers_[rt()];
    pc_ += 4;
}

void CPU::exe_tlbwi() {
    mmu_.tlb_key_[cp0_.registers_[cp0_.Index]] = cp0_.registers_[cp0_.EntryHi];
    mmu_.tlb_data_[0][cp0_.registers_[cp0_.Index]] = cp0_.registers_[cp0_.EntryLo0];
    mmu_.tlb_data_[1][cp0_.registers_[cp0_.Index]] = cp0_.registers_[cp0_.EntryLo1];
    pc_ += 4;
}

void CPU::exe_eret() {
    pc_ = cp0_.registers_[cp0_.EPC];
    // TODO: who should be responsible for restoring EXL?
    //cp0_.registers_[cp0_.SR] &= 0xfffffffd;
}

void CPU::exe_lb() {
    int8_t val = mmu_.read_byte(registers_[rs()] + signed_immediate());
    registers_[rt()] = int32_t(val); 
    pc_ += 4;
}

void CPU::exe_lw() {
    uint32_t val = mmu_.read_word(registers_[rs()] + signed_immediate());
    registers_[rt()] = val;
    pc_ += 4;
}

void CPU::exe_lbu() {
    uint8_t val = mmu_.read_byte(registers_[rs()] + signed_immediate());
    registers_[rt()] = val; 
    pc_ += 4;
}

void CPU::exe_lhu() {
    uint16_t val = mmu_.read_half_word(registers_[rs()] + signed_immediate());
    registers_[rt()] = val;
    pc_ += 4;
}

void CPU::exe_sb() {
    mmu_.write_byte(registers_[rs()] + signed_immediate(), registers_[rt()] & 0xff);
    pc_ += 4;
}

void CPU::exe_sw() {
    mmu_.write_word(registers_[rs()] + signed_immediate(), registers_[rt()]);
    pc_ += 4;
}

void CPU::exe_cache() {
    pc_ += 4;
}

void CPU::instruction_decode() {
    switch (main_opcode()) {
        case 0b000000: 
            switch (sub_opcode()) {
                // sll
                case 0b000000:
                    return exe_sll();
                // srl
                case 0b000010:
                    return exe_srl();
                // sra
                case 0b000011:
                    return exe_sra();
                // sllv
                case 0b000100:
                    return exe_sllv();
                // srav
                case 0b000110:
                    return exe_srav();
                // srav
                case 0b000111:
                    return exe_srav();
                // jr
                case 0b001000:
                    return exe_jr();
                // jalr
                case 0b001001:
                    return exe_jalr();
                // syscall
                case 0b001100:
                    return exe_syscall();
                // mfhi
                case 0b010000:
                    return exe_mfhi();
                // mthi
                case 0b010001:
                    return exe_mthi();
                // mflo
                case 0b010010:
                    return exe_mflo();
                // mtlo
                case 0b010011:
                    return exe_mtlo();
                // mult
                case 0b011000:
                    return exe_mult();
                // addu
                case 0b100001:
                    return exe_addu();
                // subu
                case 0b100011:
                    return exe_subu();
                // and
                case 0b100100:
                    return exe_and();
                // or
                case 0b100101:
                    return exe_or();
                // xor
                case 0b100110:
                    return exe_xor();
                // nor
                case 0b100111:
                    return exe_nor();
                // slt
                case 0b101010:
                    return exe_slt();
                // sltu
                case 0b101011:
                    return exe_sltu();
                // TODO: exception
                default:
                    ;
            }
        case 0b000001: 
            switch (rt()) {
                // bltz
                case 0b00000:
                    return exe_bltz();
                // bgez
                case 0b00001:
                    return exe_bgez();
                // TODO: exception
                default:
                    ;
            }
        // j
        case 0b000010:
            return exe_j();
        // jal
        case 0b000011:
            return exe_jal();
        // beq
        case 0b000100:
            return exe_beq();
        // bne
        case 0b000101:
            return exe_bne();
        // blez
        case 0b000110:
            return exe_blez();
        // bgtz
        case 0b000111:
            return exe_bgtz();
        // addiu
        case 0b001001:
            return exe_addiu();
        // slti
        case 0b001010:
            return exe_slti();
        // sltiu
        case 0b001011:
            return exe_sltiu();
        // andi
        case 0b001100:
            return exe_andi();
        // ori
        case 0b001101:
            return exe_ori();
        // xori
        case 0b001110:
            return exe_xori();
        // lui
        case 0b001111:
            return exe_lui();
        case 0b010000:
            switch (sub_opcode()) {
                case 0b000000:
                    switch (rs()) {
                        // mfc0
                        case 0b00000:
                            return exe_mfc0();
                        // mtc0
                        case 0b00100:
                            return exe_mtc0();
                        // TODO: exception
                        default:
                            ; 
                    }
                // tlbwi
                case 0b000010:
                    return exe_tlbwi();
                // eret
                case 0b011000:
                    return exe_eret();
                // TODO: exception
                default:
                    ;
            }
        // lb
        case 0b100000:
            return exe_lb();
        // lw
        case 0b100011:
            return exe_lw();
        // lbu
        case 0b100100:
            return exe_lbu();
        // lhu
        case 0b100101:
            return exe_lhu();
        // sb 
        case 0b101000:
            return exe_sb();
        // sw
        case 0b101011:
            return exe_sw();
        // cache
        case 0b101111:
            return exe_cache();
            ;
    }
}
