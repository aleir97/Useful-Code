#ifndef BRAM_H
#define BRAM_H

#include "systemc.h"

SC_MODULE (BRAM) {
public:

sc_in<bool> clk;

sc_in<sc_uint<9>>	addrA, addrB;
sc_in<sc_int<32>>	dinA, dinB;
sc_in<bool>			weA, weB;
sc_out<sc_int<32>>	doutA, doutB;

  void puertoA();
  void puertoB();

  void volcado();
  void volcado(FILE *fp);
  void cargaContenido(unsigned int, int);

  SC_CTOR(BRAM) {
	int i;
	for(i=0; i<512; ++i)	mem[i] = 0;

	cout<<"BRAM: "<<name()<<endl;
    SC_METHOD(puertoA);
	sensitive << clk.pos();

	SC_METHOD(puertoB);
	sensitive << clk.pos();
  }

private:
	sc_int<32> mem[512];	}; 


SC_MODULE(BRAM4K) {
public:

    sc_in<bool> clk;

    sc_in<sc_uint<10>>	addrA, addrB;
    sc_in<sc_int<32>>	dinA, dinB;
    sc_in<bool>			weA, weB;
    sc_out<sc_int<32>>	doutA, doutB;

    void puertoA();
    void puertoB();

    SC_CTOR(BRAM4K) {
        int i;
        for (i = 0; i < 1024; ++i)	mem[i] = 0;

        cout << "BRAM4K: " << name() << endl;
        SC_METHOD(puertoA);
        sensitive << clk.pos();

        SC_METHOD(puertoB);
        sensitive << clk.pos();
    }

private:
    sc_int<32> mem[1024];	
};




#endif