#include "cpu.h"

void CPU::exe_sll(bool&) {
    registers_[rd()] = registers_[rt()] << shift();
    pc_ += 4;
}

void CPU::exe_srl(bool&) {
    registers_[rd()] = registers_[rt()] >> shift();
    pc_ += 4;
}

void CPU::exe_sra(bool&) {
    int32_t val = registers_[rt()];
    val >>= shift();
    registers_[rd()] = val;
    pc_ += 4;
}

void CPU::exe_sllv(bool&) {
    registers_[rd()] = registers_[rt()] << registers_[rs()];
    pc_ += 4;
}

void CPU::exe_srlv(bool&) {
    registers_[rd()] = registers_[rt()] >> registers_[rs()];
    pc_ += 4;
}

void CPU::exe_movn(bool&) {
    if (registers_[rt()] != 0) 
    	registers_[rd()] = registers_[rs()];
    pc_ += 4;
}

void CPU::exe_movz(bool&) {
    if (registers_[rt()] == 0) 
    	registers_[rd()] = registers_[rs()];
    pc_ += 4;
}

void CPU::exe_srav(bool&) {
    int32_t val = registers_[rt()];
    val >>= registers_[rs()];
    registers_[rd()] = val;
    pc_ += 4;
}

void CPU::exe_jr(bool&) {
    pc_ = registers_[rs()];
    #ifdef DEBUG
    if (registers_[rs()] == registers_[rt()])
	  printf("%x\n", pc_);
    #endif
}

void CPU::exe_jalr(bool&) {
    uint32_t npc = registers_[rs()];
    registers_[rd()] = pc_ + 4;
    pc_ = npc;
    #ifdef DEBUG
    if (registers_[rs()] == registers_[rt()])
	  printf("%x\n", pc_);
    #endif
}

void CPU::exe_syscall(bool& exception) {
    cp0_.set_exception_code(cp0_.Exc_Syscall);
    exception = true;
    return;
}

void CPU::exe_tne(bool& exception) {
    if (registers_[rs()] != registers_[rt()])
	exception = true;
    pc_ += 4;
    return;
}


void CPU::exe_mfhi(bool&) {
    registers_[rd()] = hi_;
    pc_ += 4;
}

void CPU::exe_mthi(bool&) {
    hi_ = registers_[rs()];
    pc_ += 4;
}

void CPU::exe_mflo(bool&) {
    registers_[rd()] = lo_;
    pc_ += 4;
}

void CPU::exe_mtlo(bool&) {
    lo_ = registers_[rs()];
    pc_ += 4;
}

void CPU::exe_madd(bool&) {
    int64_t a = registers_[rs()];
    int64_t b = registers_[rt()];
    a *= b;
    hi_ = hi_ + ( uint32_t(a >> 32) );
    lo_ = lo_ + ( uint32_t(a) );
    pc_ += 4;
}

void CPU::exe_mult(bool&) {
    int64_t a = registers_[rs()];
    int64_t b = registers_[rt()];
    a *= b;
    hi_ = uint32_t(a >> 32);
    lo_ = uint32_t(a);
    pc_ += 4;
}

void CPU::exe_div(bool&) {
    int32_t a = registers_[rs()];
    int32_t b = registers_[rt()];
    a *= b;
    if (b != 0)
    	hi_ = uint32_t(a / b);
    lo_ = uint32_t(a % b);
    pc_ += 4;
}

void CPU::exe_mul(bool&) {
    int64_t m = registers_[rs()] * registers_[rt()];
    registers_[rd()] = int32_t (m);
    pc_ += 4;
}

void CPU::exe_multu(bool&) {
    uint64_t a = uint64_t (registers_[rs()]);
    uint64_t b = uint64_t (registers_[rt()]);
    a *= b;
    hi_ = uint32_t(a >> 32);
    lo_ = uint32_t(a);
    pc_ += 4;
}


void CPU::exe_addu(bool&) {
    registers_[rd()] = registers_[rs()] + registers_[rt()];
    pc_ += 4;
}

void CPU::exe_subu(bool&) {
    registers_[rd()] = registers_[rs()] - registers_[rt()];
    pc_ += 4;
}

void CPU::exe_and(bool&) {
    registers_[rd()] = registers_[rs()] & registers_[rt()];
    pc_ += 4;
}

void CPU::exe_or(bool&) {
    registers_[rd()] = registers_[rs()] | registers_[rt()];
    pc_ += 4;
}

void CPU::exe_xor(bool&) {
    registers_[rd()] = registers_[rs()] ^ registers_[rt()];
    pc_ += 4;
}

void CPU::exe_nor(bool&) {
    registers_[rd()] = ~(registers_[rs()] | registers_[rt()]);
    pc_ += 4;
}

void CPU::exe_slt(bool&) {
    registers_[rd()] = int32_t(registers_[rs()]) < int32_t(registers_[rt()])? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_sltu(bool&) {
    registers_[rd()] = uint32_t(registers_[rs()]) < uint32_t(registers_[rt()])? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_bltz(bool&) {
    if (int32_t(registers_[rs()]) < 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
    #ifdef DEBUG
    if (registers_[rs()] < 0)
	  printf("%x\n", pc_);
    #endif
}

void CPU::exe_bgez(bool&) {
    if (int32_t(registers_[rs()]) >= 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
    #ifdef DEBUG
    if (registers_[rs()] >= 0)
	  printf("%x\n", pc_);
    #endif
}

void CPU::exe_j(bool&) {
    pc_ = (pc_ & 0xf0000000) | jump_target() * 4;
    #ifdef DEBUG
    if (registers_[rs()] == registers_[rt()])
	  printf("%x\n", pc_);
    #endif
}

void CPU::exe_jal(bool&) {
    registers_[REG_RA] = pc_ + 4;
    pc_ = (pc_ & 0xf0000000) | jump_target() * 4;
    #ifdef DEBUG
	  printf("%x\n", pc_);
    #endif

}

void CPU::exe_beq(bool&) {
    if (registers_[rs()] == registers_[rt()])
        pc_ += branch_offset() * 4;
    pc_ += 4;
    #ifdef DEBUG
    if (registers_[rs()] == registers_[rt()])
	  printf("%x\n", pc_);
    #endif
}

void CPU::exe_bne(bool&) {
    if (registers_[rs()] != registers_[rt()])
        pc_ += branch_offset() * 4;
    pc_ += 4;
    #ifdef DEBUG
    if (registers_[rs()] != registers_[rt()])
	  printf("%x\n", pc_);
    #endif
}

void CPU::exe_blez(bool&) {
    if (int32_t(registers_[rs()]) <= 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
    #ifdef DEBUG
    if (registers_[rs()] <= 0)
	  printf("%x\n", pc_);
    #endif
}

void CPU::exe_bnezl(bool&) {
    if (int32_t(registers_[rs()]) != 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
    #ifdef DEBUG
    if (registers_[rs()] != 0)
	  printf("%x\n", pc_);
    #endif
}

void CPU::exe_bgtz(bool&) {
    if (int32_t(registers_[rs()]) > 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
    #ifdef DEBUG
    if (registers_[rs()] > 0)
	  printf("%x\n", pc_);
    #endif
}

void CPU::exe_addiu(bool&) {
    registers_[rt()] = registers_[rs()] + signed_immediate();
    pc_ += 4;
}

void CPU::exe_slti(bool&) {
    registers_[rt()] = int32_t(registers_[rs()]) < signed_immediate()? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_sltiu(bool&) {
    registers_[rt()] = registers_[rs()] < unsigned_immediate()? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_andi(bool&) {
    registers_[rt()] = registers_[rs()] & unsigned_immediate();
    pc_ += 4;
}

void CPU::exe_ori(bool&) {
    registers_[rt()] = registers_[rs()] | unsigned_immediate();
    pc_ += 4;
}

void CPU::exe_xori(bool&) {
    registers_[rt()] = registers_[rs()] ^ unsigned_immediate();
    pc_ += 4;
}

void CPU::exe_lui(bool&) {
    registers_[rt()] = uint32_t(unsigned_immediate()) << 16;
    pc_ += 4;
}

void CPU::exe_mfc0(bool&) {
    registers_[rt()] = cp0_.registers_[rd()];
    pc_ += 4;
}

void CPU::exe_cfc1(bool&) {
    registers_[rt()] = 0x00; // TODO: because not implemented (several registers) 
    pc_ += 4;
}

void CPU::exe_mtc0(bool&) {
    cp0_.registers_[rd()] = registers_[rt()];
    pc_ += 4;
}

void CPU::exe_tlbwi(bool&) {
    mmu_.tlb_key_[cp0_.registers_[cp0_.Index]] = cp0_.registers_[cp0_.EntryHi];
    mmu_.tlb_data_[0][cp0_.registers_[cp0_.Index]] = cp0_.registers_[cp0_.EntryLo0];
    mmu_.tlb_data_[1][cp0_.registers_[cp0_.Index]] = cp0_.registers_[cp0_.EntryLo1];
    pc_ += 4;
}

void CPU::exe_eret(bool&) {
    pc_ = cp0_.registers_[cp0_.EPC];
    //cp0_.registers_[cp0_.SR] &= 0xfffffffd;
    cp0_.unset_Status_EXL();
}

void CPU::exe_lb(bool& exception) {
    int8_t val = mmu_.read_byte(registers_[rs()] + signed_immediate(), exception);
    if (exception) return;
    registers_[rt()] = int32_t(val); 
    pc_ += 4;
}

void CPU::exe_lh(bool& exception) {
    int8_t val;
    int16_t r; 
    val = mmu_.read_byte(registers_[rs()] + signed_immediate(), exception);
    if (exception) return;
	r = int16_t (val) << 8;
    val = mmu_.read_byte(registers_[rs()] + signed_immediate() + 1, exception);
    if (exception) return;
	r |= int16_t(val);
    registers_[rt()] = int32_t(r); 
    pc_ += 4;
}

void CPU::exe_sh(bool& exception) {
    int8_t val;
    int16_t r; 
    val = int8_t (registers_[rt()] & 0x000000ff);
    mmu_.write_byte(registers_[rs()] + signed_immediate() + 1, val, exception);
    if (exception) return;

    val = int8_t ((registers_[rt()] & 0x0000ff00) >> 8);
    mmu_.write_byte(registers_[rs()] + signed_immediate(), val, exception);
    if (exception) return;
    pc_ += 4;
}

void CPU::exe_lw(bool& exception) {
    uint32_t val = mmu_.read_word(registers_[rs()] + signed_immediate(), exception);
    if (exception) return;
    registers_[rt()] = val;
    pc_ += 4;
}

void CPU::exe_lwr(bool& exception) {
    uint32_t val = 0;
    int r = (registers_[rs()] + signed_immediate()) % 4;
    switch (r) {
	case 0:
		val = mmu_.read_word(registers_[rs()] + signed_immediate(), exception);
		break;
	case 1:
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate() + 2, exception) << 16);
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate() + 1, exception) << 8);
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate(), exception));
		break;
	case 2:
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate() + 1, exception) << 8);
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate(), exception));
		break;
	case 3:
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate(), exception));
		break;
    }
    if (exception) return;
    registers_[rt()] = registers_[rt()] | val;
    pc_ += 4;
}

void CPU::exe_lwl(bool& exception) {
    uint32_t val = 0;
    int r = (registers_[rs()] + signed_immediate()) % 4;
    switch (r) {
	case 0:
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate(), exception) << 24);
		break;
	case 1:
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate(), exception) << 24);
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate() - 1, exception) << 16);
		break;
	case 2:
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate(), exception) << 24);
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate() - 1, exception) << 16);
		val = val | (mmu_.read_byte(registers_[rs()] + signed_immediate() - 2, exception) << 8);
		break;
	case 3:
		val = mmu_.read_word(registers_[rs()] + signed_immediate() - 3, exception);
		break;
    }
    if (exception) return;
    registers_[rt()] = registers_[rt()] | val;
    pc_ += 4;
}

void CPU::exe_ll(bool& exception) {
    uint32_t val = mmu_.read_word(registers_[rs()] + signed_immediate(), exception);
    if (exception) return;
    registers_[rt()] = val;
    pc_ += 4;
}

void CPU::exe_lbu(bool& exception) {
    uint8_t val = mmu_.read_byte(registers_[rs()] + signed_immediate(), exception);
    if (exception) return;
    registers_[rt()] = val; 
    pc_ += 4;
}

void CPU::exe_lhu(bool& exception) {
    uint16_t val = mmu_.read_half_word(registers_[rs()] + signed_immediate(), exception);
    if (exception) return;
    registers_[rt()] = val;
    pc_ += 4;
}

void CPU::exe_sb(bool& exception) {
    mmu_.write_byte(registers_[rs()] + signed_immediate(), registers_[rt()] & 0xff, exception);
    if (exception) return;
    pc_ += 4;
}

void CPU::exe_swl(bool& exception) {
    uint32_t val;
    int r = (registers_[rs()] + signed_immediate()) % 4;
    switch (r) {
	case 0:
   		mmu_.write_byte(registers_[rs()] + signed_immediate() + 3, (registers_[rt()] >> 24), exception);
		break;
	case 1:
   		mmu_.write_byte(registers_[rs()] + signed_immediate() + 1, (registers_[rt()] >> 24), exception);
   		mmu_.write_byte(registers_[rs()] + signed_immediate() + 2, (registers_[rt()] >> 16), exception);
		break;
	case 2:
   		mmu_.write_byte(registers_[rs()] + signed_immediate() - 1, (registers_[rt()] >> 24), exception);
   		mmu_.write_byte(registers_[rs()] + signed_immediate(), (registers_[rt()] >> 16), exception);
   		mmu_.write_byte(registers_[rs()] + signed_immediate() + 1, (registers_[rt()] >> 8), exception);
		break;
	case 3:
   		mmu_.write_byte(registers_[rs()] + signed_immediate() - 1, (registers_[rt()] >> 24), exception);
   		mmu_.write_byte(registers_[rs()] + signed_immediate(), (registers_[rt()] >> 16), exception);
		break;
    }
    if (exception) return;
    pc_ += 4;
}

void CPU::exe_swr(bool& exception) {
    uint32_t val;
    int r = (registers_[rs()] + signed_immediate()) % 4;
    switch (r) {
	case 0:
   		mmu_.write_word(registers_[rs()] + signed_immediate(), registers_[rt()], exception);
		break;
	case 1:
   		mmu_.write_byte(registers_[rs()] + signed_immediate() - 1, (registers_[rt()] >> 16), exception);
   		mmu_.write_byte(registers_[rs()] + signed_immediate(), (registers_[rt()] >> 8), exception);
   		mmu_.write_byte(registers_[rs()] + signed_immediate() + 1, registers_[rt()], exception);
		break;
	case 2:
   		mmu_.write_byte(registers_[rs()] + signed_immediate() - 2, (registers_[rt()] >> 8), exception);
   		mmu_.write_byte(registers_[rs()] + signed_immediate() - 1, registers_[rt()], exception);
		break;
	case 3:
   		mmu_.write_byte(registers_[rs()] + signed_immediate() - 1, (registers_[rt()] >> 16), exception);
   		mmu_.write_byte(registers_[rs()] + signed_immediate(), (registers_[rt()] >> 8), exception);
   		mmu_.write_byte(registers_[rs()] + signed_immediate() + 1, registers_[rt()], exception);
		break;
    }
    if (exception) return;
    pc_ += 4;
}

void CPU::exe_sw(bool& exception) {
    mmu_.write_word(registers_[rs()] + signed_immediate(), registers_[rt()], exception);
    if (exception) return;
    pc_ += 4;
}

void CPU::exe_sc(bool& exception) {
    mmu_.write_word(registers_[rs()] + signed_immediate(), registers_[rt()], exception);
    registers_[rt()] = 0x1;
    if (exception) return;
    pc_ += 4;
}

void CPU::exe_cache(bool&) {
    pc_ += 4;
}
