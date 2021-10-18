#include"gestionPC.h"

void gestionPC::mux(){
	
	bool neg; 
	neg = resultadoALU.read().bit(31); // bit de signo

	switch (CondSel.read()) {
	case 0: condi = zero.read();				break;
	case 1: condi = !zero.read();				break;
	case 2: condi = neg|| zero.read();	break;
	default: condi = !(neg || zero.read());
	};

	switch (FuentePC.read()) {
	case 2: 
		tmpPC(31, 28) = PC.read().range(31, 28);		tmpPC(27, 2) = jumpDir.read();		tmpPC(1, 0) = 0;	break;
	case 1: 
		if (condi)
			tmpPC = salidaALU.read();
		else
			tmpPC = PC.read();
		break;
	default:
		tmpPC = resultadoALU.read();
	};

	nuevoPC.write(tmpPC);

}


void gestionPC::registro() {

	if (rst.read()) {
		PC.write(0);
		regPC.write(0);
	}
	else {
		if (EscrPC.read() || (EscrPC_Cond.read() && condi)) {
			PC.write(tmpPC);
			regPC.write(tmpPC);
		}
	}

}
