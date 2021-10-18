#include"muxes.h"

void muxes::muxA() {

	if (SelALUA.read() == 0)
		aluA.write(PC.read().to_int());
	else
		aluA.write(regA.read());
}


void muxes::muxB(){
	
	switch (SelALUB.read()) {
	case 0:		aluB.write(regB.read());				break;
	case 1:		aluB.write(4);							break;
	case 2:		aluB.write(inm.read().to_int());		break;
	case 3:		aluB.write(inm.read().to_int() << 2);	break;
	default:	aluB.write(inm.read().to_uint() & 0xffff);
	};
}

void muxes::muxDir() {
	int a = PC.read();
	int b = resultadoALU.read();
	int c = nuevoPC.read();

	switch (IoD.read()) {
	case 0:		direccion.write(PC.read());						break;
	case 1:		direccion.write(resultadoALU.read().to_uint());	break;
	default:	direccion.write(nuevoPC.read());
	};
}

void muxes::muxDato() {

	sc_int<32> slt = 0;

	switch (selDatoReg.read()) {
	case 0:		rdValue.write(salidaMem.read());			break;
	case 1:		rdValue.write(salidaALU.read());			break;
	case 2:		rdValue.write(LOreg.read());				break;
	case 3:		rdValue.write(shifted.read());				break;
	case 4:		slt.bit(0) = salidaALU.read().bit(31);
				rdValue.write(slt);							break;
	default:	rdValue.write(inm.read() << 16);			// lui
	};
}



