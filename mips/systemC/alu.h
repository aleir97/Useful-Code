#ifndef ALU_H
#define ALU_H

#include "systemc.h"
#include "DSP48E1.h"


SC_MODULE (alu) {
public:

sc_in<bool> clk, rst;

sc_in< sc_uint<5> >  	aluOp;
sc_in< sc_int<32> >   	opA, opB;
sc_in< bool>			EscrLO;
sc_out<bool>			zero;
sc_out< sc_int<32> >	resultadoALU, salidaALU, salidaLO;

  void entradas();
  void resultados();
  void registro();
  void setIdx(int id) { idx = id; };

  SC_CTOR(alu) {
	cout<<"alu: "<<name()<<endl;

	dsp = new DSP48E1("dsp48e1");

	dsp->clk(clk);
	dsp->rst(rst);
	dsp->A(A);
	dsp->B(B);
	dsp->C(C);
	dsp->D(D);
	dsp->ALUMODE(ALUMODE);
	dsp->INMODE(INMODE);
	dsp->OPMODE(OPMODE);
	dsp->R(R);

	SC_METHOD(entradas);
	sensitive << opA << opB << aluOp;
	SC_METHOD(resultados);
	sensitive << R; 
	SC_METHOD(registro);
	sensitive << clk.pos(); 

  }
private:
	int idx;
	sc_int<32> registrada;
	DSP48E1* dsp;

	sc_signal< sc_int<30> > A;
	sc_signal< sc_int<18> > B;
	sc_signal< sc_int<48> > C;
	sc_signal< sc_int<25> > D;
	sc_signal< sc_uint<4> > ALUMODE;
	sc_signal< sc_uint<5> > INMODE;
	sc_signal< sc_uint<7> > OPMODE;
	sc_signal< sc_int<48> > R;

}; 

#endif