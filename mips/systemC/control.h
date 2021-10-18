#ifndef CONTROL_H
#define CONTROL_H

#include "systemc.h"

SC_MODULE (control) {
public:

sc_in<bool> clk;
sc_in<bool> rst;

sc_in< sc_int<32> >	instruction;
sc_out< sc_uint<5> >  	rs, rt, rd, shamt;
sc_out< sc_uint<2> >  	FuentePC, IoD, CondSel, shiftSel;
sc_out< sc_uint<3> >  	SelALUB, selDatoReg_o;
sc_out< bool >  		EscrPC_Cond, EscrPC, LeerMem, EscrMem, EscrIR, SelALUA, EscrReg, EscrLO;
sc_out<bool>            direc, arith;
sc_out< sc_uint<5> >  	ALUop;
sc_out< sc_uint<26> >  	inmJ;
sc_out< sc_int<16> >  	inm; 


  void controlProc();
  void registros();

  void setIdx(int id) { idx = id; };
  
  SC_CTOR(control) {
	cout<<"control: "<<name()<<endl;

  SC_METHOD(controlProc);
  sensitive << instruction << estado;
  SC_METHOD(registros);
  sensitive << clk.pos(); 
  }

private:
    int idx;
sc_uint<32> IR;
sc_signal<sc_uint<4> > estado; 
sc_uint<4> state;
sc_uint<3> selDatoReg;
enum st { inicial, segundo, buff, lw1, lw2, sw, arith1, arith2, inme1, inme2, mult2, branch, jump};

}; 

#endif