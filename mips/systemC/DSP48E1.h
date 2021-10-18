#ifndef DSP48E1_H
#define DSP48E1_H

#include "systemc.h"

// versión simplificada con la configuración más sencilla y directa
SC_MODULE (DSP48E1) {
public:

sc_in<bool> clk, rst;

sc_in< sc_int<30> > A;
sc_in< sc_int<18> > B;
sc_in< sc_int<48> > C;
sc_in< sc_int<25> > D;
sc_in< sc_uint<4> > ALUMODE;
sc_in< sc_uint<5> > INMODE;
sc_in< sc_uint<7> > OPMODE;
sc_out< sc_int<48> > R;

void calcularALU();
void registroMult();

  SC_CTOR(DSP48E1) {
	cout<<"DSP48E1: "<<name()<<endl;

	SC_METHOD(calcularALU);
	sensitive << A << B << C << D << ALUMODE << INMODE << OPMODE << prod; 
	SC_METHOD(registroMult);
	sensitive << clk.pos(); 

  }

private: 
	sc_signal<sc_int<32>> prod; 
}; 

#endif