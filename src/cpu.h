#ifndef CPU_
#define CPU_
#include "cp0.h"
#include "mmu.h"

class CPU {
public:
    CPU(std::istream& rom, std::istream& flash, uint32_t entry): 
        // RAFA cp0_(), mmu_(cp0_, rom, flash), pc_(entry) { }
        cp0_(), mmu_(cp0_, rom, flash), pc_(PC_INITIAL) { }

    void run() {
	unsigned int tick=0; // RAFA
        while (true) {
            bool exception = false;
            next(exception);
            
	
		if (tick>=1000000) {
		  tick=0;
	          printf("instruccion, pc=%x\n", pc_);
		}
		tick++;

            if (exception) {
                // store return pc only in normal level
                if (!cp0_.Status_EXL())
                    cp0_.registers_[cp0_.EPC] = pc_;
                // jump to EBase
                pc_ = cp0_.registers_[cp0_.EBase];
                // set EXL to 1
                cp0_.set_Status_EXL();
            }
        }
    }
    
private:
    void next(bool& exception) {

        // zero registers is forever return 0
        registers_[REG_ZERO] = 0x00;
        
        // increment Count register
        cp0_.registers_[cp0_.Count]++;


        // instruction fetch
        instruction_ = mmu_.read_word(pc_, exception);

	// #define DEBUG 1
	#ifdef DEBUG
		printf("instruccion=%x, pc=%x\n", instruction_, pc_);
	#endif

	if (pc_ == 0x80189fa8)
		printf("registro a0 =%x, a1=%x, a2=%x \n", registers_[REG_A0], registers_[REG_A1], registers_[REG_A2]);
	// SIMPLE SERIAL
	if (pc_ == 0x801009f4)
		printf("%c", registers_[REG_V1]);
	if (pc_ == 0x80167e98) {
		printf("registro a0 =%x, \n", registers_[REG_A0]);
	} else if (pc_ == 0x80167eac) {
		int excep;
		 uint32_t val = mmu_.read_byte(registers_[REG_V0], exception);
		printf("memoria =%x, \n", val);
	};
	if ((pc_ >= 0x80181fe8) && (pc_ <= 0x801822bc)) {
	//	printf("pc=%X, registro a0 =%X, a1=%X, a2=%X, t0=%x, t1=%X, t2=%X, t3=%X, t4=%X, t5=%X, t6=%X, t7=%X, t8=%X \n", pc_, registers_[REG_A0], registers_[REG_A1], registers_[REG_A2], registers_[REG_T0], registers_[REG_T1], registers_[REG_T2], registers_[REG_T3], registers_[REG_T4], registers_[REG_T5], registers_[REG_T6], registers_[REG_T7], registers_[REG_T8]);
	};

        if (exception) return;
        
        // instruction decode
        instruction_decode(exception);
        if (exception) return;
	// DEBUG printf("INSTR decodificada. Clock:%X \n", cp0_.registers_[cp0_.Count]);

        // clock interrupt
        // interrupt will be handled after the current instruction
        if (cp0_.registers_[cp0_.Count] >= cp0_.registers_[cp0_.Compare]) {
            cp0_.registers_[cp0_.Count] = 0x00;

            if (cp0_.interrupt_enabled() && /* IE and EXL */
                cp0_.interrupt_enabled(cp0_.IP_7) /* clock interrupt is enabled */) {
                cp0_.set_interrupt_code(cp0_.IP_7);
                cp0_.set_exception_code(cp0_.Exc_Int);
		printf("Desconocida\n");
                exception = true;
                return;
            }
        }
    }

    void instruction_decode(bool& exception);

    void exe_sll(bool& exception); 
    void exe_srl(bool& exception); 
    void exe_sra(bool& exception); 
    void exe_sllv(bool& exception); 
    void exe_srlv(bool& exception);
    void exe_srav(bool& exception); 
    void exe_jr(bool& exception);
    void exe_jalr(bool& exception);
    void exe_syscall(bool& exception);
    void exe_mfhi(bool& exception);
    void exe_mthi(bool& exception);
    void exe_mflo(bool& exception);
    void exe_mtlo(bool& exception);
    void exe_mult(bool& exception);
    void exe_addu(bool& exception);
    void exe_subu(bool& exception);
    void exe_and(bool& exception);
    void exe_or(bool& exception);
    void exe_xor(bool& exception);
    void exe_nor(bool& exception);
    void exe_slt(bool& exception);
    void exe_sltu(bool& exception);
    void exe_bltz(bool& exception);
    void exe_bgez(bool& exception);
    void exe_j(bool& exception);
    void exe_jal(bool& exception);
    void exe_beq(bool& exception);
    void exe_bne(bool& exception);
    void exe_blez(bool& exception);
    void exe_bgtz(bool& exception);
    void exe_addiu(bool& exception);
    void exe_slti(bool& exception);
    void exe_sltiu(bool& exception);
    void exe_andi(bool& exception);
    void exe_ori(bool& exception);
    void exe_xori(bool& exception);
    void exe_lui(bool& exception);
    void exe_mfc0(bool& exception);
    void exe_mtc0(bool& exception);
    void exe_tlbwi(bool& exception);
    void exe_eret(bool& exception);
    void exe_lb(bool& exception);
    void exe_lw(bool& exception);
    void exe_lbu(bool& exception);
    void exe_lhu(bool& exception);
    void exe_sb(bool& exception);
    void exe_sw(bool& exception);
    void exe_cache(bool& exception);

//    void exe_lwl(bool& exception);
    void exe_swl(bool& exception);
    void exe_swr(bool& exception);
    void exe_ll(bool& exception);
    void exe_sc(bool& exception);
    void exe_bnezl(bool& exception);
    void exe_movn(bool& exception);
    void exe_lh(bool& exception);
    void exe_movz(bool& exception);
    void exe_sh(bool& exception);
    void exe_multu(bool& exception);
    void exe_mul(bool& exception);
    void exe_cfc1(bool& exception);
    void exe_tne(bool& exception);
    void exe_div(bool& exception);
    void exe_lwr(bool& exception);
    void exe_lwl(bool& exception);
    // pref: esta instruccion no hace nada (en un hw real copia a la cache)

    // fields of instruction
    uint32_t main_opcode() const { return instruction_ >> 26; }
    uint32_t sub_opcode() const { return instruction_ & 0x3f; }
    uint32_t sub_extra_opcode() const { return instruction_ & 0x7ff; }
    uint32_t sub_extra_opcode_2() const { return instruction_ >> 6 & 0x3ff; }
    uint32_t sel() const { return instruction_ & 0x7; }
    uint32_t sub_extra_opcode_3() const { return instruction_ >> 3 & 0xff; }
    uint32_t rs() const { return instruction_ >> 21 & 0x1f; }
    uint32_t rt() const { return instruction_ >> 16 & 0x1f; }
    uint32_t rd() const { return instruction_ >> 11 & 0x1f; }
    uint32_t shift() const { return instruction_ >> 6 & 0x1f; }
    int16_t branch_offset() const { return int16_t(instruction_ & 0xffff); }
    uint32_t jump_target() const { return instruction_ & 0x3ffffff; }
    int16_t signed_immediate() const { return int16_t(instruction_ & 0xffff); }
    uint16_t unsigned_immediate() const { return uint16_t(instruction_ & 0xffff); }

    // virtual address of initial PC
    constexpr static uint32_t PC_INITIAL = 0xbfc00000;
    // RAFA static uint32_t PC_INITIAL;
    // RAFA void entry_point(uint32_t entry) { PC_INITIAL = entry; }

    CP0 cp0_;
    MMU mmu_;

    // general-purpose registers alias
    enum { 
        REG_ZERO = 0, 
        REG_AT = 1, 
        REG_V0 = 2, REG_V1, 
        REG_A0 = 4, REG_A1, REG_A2, REG_A3,
        REG_T0 = 8, REG_T1, REG_T2, REG_T3, REG_T4, REG_T5, REG_T6, REG_T7, 
        REG_T8 = 24, REG_T9, 
        REG_S0 = 16, REG_S1, REG_S2, REG_S3, REG_S4, REG_S5, REG_S6, REG_S7,
        REG_K0 = 26, REG_K1, 
        REG_GP = 28,
        REG_SP = 29, 
        REG_S8 = 30, REG_FP = 30,
        REG_RA = 31
    };

    // general-purpose registers
    uint32_t registers_[32];
    // pc registers
    uint32_t pc_;
    // current instruction
    uint32_t instruction_;
    // registers for multiplication and division
    uint32_t hi_, lo_;

};

#endif /* CPU_ */
