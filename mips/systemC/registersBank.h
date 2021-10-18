#ifndef REGISTERSBANK_H
#define REGISTERSBANK_H

#include "systemc.h"


SC_MODULE(registersBank) {
public:

	sc_in<bool> clk;
	sc_in<bool> rst;

	sc_in< sc_uint<5> >  	rs, rt, rd;
	sc_in< sc_int<32> >   	rdValue;
	sc_in<bool>				EscrReg;
	sc_out< sc_int<32> >	regA, regB;


	void readProcA();
	void readProcB();
	void writeProc();
	void setIdx(int id) { idx = id; };

	SC_CTOR(registersBank) {
		cout << "registersBank: " << name() << endl;

		cambio.write(false);
		for (int i = 0; i < 32; ++i)	regs[i] = 0;
		SC_METHOD(readProcA);					// regA y regB no están en realidad registrados, ya que la ALU de la FPGA ya lo hace
		sensitive << rs << cambio;
		SC_METHOD(readProcB);
		sensitive << rt << cambio;
		SC_METHOD(writeProc);
		sensitive << clk.pos();

	}

	~registersBank() {					// esta función vuelca el contenido del banco al terminar el programa
		for (int i = 0; i < 8; ++i)		// sólo 8 registros, pero se puede cambiar
			printf("$%s: %08x    $%s: %08x    $%s: %08x    $%s: %08x\n", nom[i], regs[i].to_int(), nom[i + 8], regs[i + 8].to_int(), nom[i + 16], regs[i + 16].to_int(), nom[i + 24], regs[i + 24].to_int());
	}


private:
	int idx;
	sc_int<32> regs[32];
	sc_signal<bool> cambio;
	char* nom[32] = { "0 ", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra" };

}; 

#endif