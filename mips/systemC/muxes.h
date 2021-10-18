#ifndef MUXES_H
#define MUXES_H

#include "systemc.h"


SC_MODULE (muxes) {
public:

sc_in< bool >  			SelALUA;
sc_in< sc_int<32> >   	regA;
sc_in< sc_uint<32> >   	PC;
sc_out< sc_int<32> >	aluA;

sc_in< sc_uint<3> >  	SelALUB;
sc_in< sc_int<32> >   	regB;
sc_in< sc_int<16> >   	inm;
sc_out< sc_int<32> >	aluB;

sc_in< sc_uint<2> >  	IoD;
sc_in< sc_uint<32> >   	nuevoPC;
sc_in< sc_int<32> >   	resultadoALU;
sc_out< sc_uint<32> >	direccion;

sc_in< sc_uint<3> >  	selDatoReg;
sc_in< sc_int<32> >   	salidaMem, salidaALU, LOreg, shifted;
sc_out< sc_int<32> >	rdValue;


    void muxA();
    void muxB();
	void muxDir();
	void muxDato();
  
  SC_CTOR(muxes) {
	cout<<"muxes: "<<name()<<endl;

	SC_METHOD(muxA);
	sensitive << SelALUA << regA << PC;

	SC_METHOD(muxB);
	sensitive << SelALUB << regB << inm;

	SC_METHOD(muxDir);
	sensitive << IoD << PC << resultadoALU << nuevoPC;

	SC_METHOD(muxDato);
	sensitive << selDatoReg << inm << salidaMem << salidaALU << LOreg << shifted;

  }

}; 

#endif