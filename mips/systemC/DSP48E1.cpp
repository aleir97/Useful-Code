#include"DSP48E1.h"

void DSP48E1::calcularALU(){
	
	sc_uint<16> conf; 
	sc_int<32> opA, opB;

	opA = C.read().range(31, 0);
	if (D.read() != 0) 
		cerr << "La entrada D a DSP48E1 deberia ser 0" << endl;
	opB(17, 0) = B.read();
	opB(31, 18) = A.read().range(13, 0);

	// versión simplificada y algo ferrea en la comprobación de los valores de configuración
	
	conf(15, 12) = ALUMODE.read();
	conf(11, 7) = INMODE.read();
	conf(6, 0) = OPMODE.read();

	switch (conf.to_uint()) {
	case 0x0033: R.write(opA + opB);	break;	// suma
	case 0x3033: R.write(opA - opB);	break;	// resta
	case 0xC033: R.write(opA & opB);	break;	// and
	case 0xC03B: R.write(opA | opB);	break;	// or
	case 0x4033: R.write(opA ^ opB);	break;	// exor
	case 0x0885: R.write(prod.read());	break;	// mult  0x0885
	default: 
		if (!rst.read()) { // si estamos en reset, entonces no pasa nada
			cerr << "Operación no valida en DSP48E1: A I O: " << hex << conf(15, 12) << " " << conf(11, 7) << " " << conf(6, 0) << endl;
			sc_stop();
		}
	};

}



void DSP48E1::registroMult(){

	sc_int<18> mB;
	sc_int<25> mA;
	sc_int<43> mult; 

	if( rst.read() ){
		prod.write(0);
	}else{
		if ((INMODE.read() == 0x11) && (OPMODE.read() == 5) && (ALUMODE.read() == 0)) {
			mB = B.read().range(17, 0);
			mA = A.read().range(24, 0);
			mult = mA * mB;
			prod.write(mult(31, 0));
		}
		else
			prod.write(0xdeaddead);
	}
}
