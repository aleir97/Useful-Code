#ifndef GESTIONPC_H
#define GESTIONPC_H

#include "systemc.h"


SC_MODULE (gestionPC) {
public:

sc_in<bool> clk;
sc_in<bool> rst;

sc_in<bool>             EscrPC, EscrPC_Cond, zero;
sc_in< sc_uint<2> >  	FuentePC, CondSel;
sc_in< sc_int<32> >   	salidaALU, resultadoALU;
sc_in< sc_uint<26> >   	jumpDir;
sc_out< sc_uint<32> >	regPC, nuevoPC;


  void mux();
  void registro();
  
  SC_CTOR(gestionPC) {
	cout<<"gestionPC: "<<name()<<endl;

	SC_METHOD(mux);
    sensitive << PC << zero << FuentePC << CondSel << salidaALU << resultadoALU << jumpDir;
    SC_METHOD(registro);
    sensitive << clk.pos();
  }

private:
    sc_uint<32> tmpPC;
    sc_signal<sc_uint<32>> PC;
    bool condi;
};
#endif