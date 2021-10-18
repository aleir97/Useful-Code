#ifndef SHIFTER_H
#define SHIFTER_H

#include "systemc.h"


SC_MODULE (shifter) {
public:

sc_in<bool> clk, rst;

sc_in< sc_uint<5> >  	shamt;
sc_in< sc_int<32> >   	regIn, offsetReg;
sc_in<bool>				direc, arith;		// izq, 1 der
sc_in< sc_uint<2> >  	selec;				// 0- nada, 10 offsetReg, 11 shamt 
sc_out< sc_int<32> >	shifted;

  void shift();
  void registro();
  void setIdx(int id) { idx = id; };
  
  SC_CTOR(shifter) {
	cout<<"shifter: "<<name()<<endl;

	SC_METHOD(shift);
	sensitive << shamt << regIn << offsetReg << direc << arith << selec;
	SC_METHOD(registro);
	sensitive << clk.pos();

  }
private:
	int idx;
	sc_int<32> sh1, sh2, sh4, sh8, sh16;
}; 

#endif