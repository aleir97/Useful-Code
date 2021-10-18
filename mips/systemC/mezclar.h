#ifndef MEZCLAR_H
#define MEZCLAR_H

#include "systemc.h"

SC_MODULE (mezclar) {
public:

sc_in < bool > clk;
sc_in < bool > rst;

sc_in < sc_int<32> > audio_in1, audio_in2, dummy1, dummy2;
sc_in < bool > ready_in1, ready_in2; // riesgo de perderse por la diferencia en la velocidad de reloj...
sc_out < sc_uint<10> > addr_in1, addr_in2;
sc_out < sc_int<16> > audio_out;
sc_out< bool> valid;

  void procesar();


  SC_CTOR(mezclar) {
	cout<<"mezclar: "<<name()<<endl;
    SC_METHOD(procesar);
	sensitive << clk.pos();

	stEspera = true;	dir1 = 256;		dir2 = 0;	// las direcciones van de 2 en 2 porque se saltan la parte imaginaria
  }
private: 
	bool stEspera; 
	sc_uint<10> dir1, dir2; 
	sc_uint<8> factor1, factor2;	
}; 


#endif;