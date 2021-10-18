#include"shifter.h"

void shifter::shift(){
	
	int size; 

	// implementación simplificada para aumentar la velocidad de simulación

	switch (selec.read()) {
	case 2: 
		size = offsetReg.read().range(4, 0); break;
	case 3: 
		size = shamt.read(); break;
	default: sh16 = 0; return;
	};

	if (direc.read()) {	// derecha
		if(arith.read())
			sh16 = regIn.read().to_int() >> size;	// arit
		else
			sh16 = regIn.read().to_uint() >> size;	// logico
	} else { // izquierda
		sh16 = regIn.read().to_int() << size;
	}

}


void shifter::registro(){

	if( rst.read() ){
		shifted.write(0);
	}else{
		shifted.write(sh16);
	}
}
