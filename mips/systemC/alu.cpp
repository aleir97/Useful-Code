#include"alu.h"

void alu::entradas(){
	
	sc_uint<5> op;
	sc_uint<4> Am;
	sc_uint<5> Im;
	sc_uint<7> Om;

	op = aluOp.read();

	Im = 0;		Im.bit(4) = Im.bit(0) = op.bit(4);
	Om = 1;		Om.bit(2) = op.bit(4);
				Om.bit(5) = Om.bit(4) = Om.bit(1) = !op.bit(4);
				Om.bit(3) = op.bit(3);
				Am.bit(1) = Am.bit(0) = op.bit(0);		Am(3, 2) = op(2, 1);

	INMODE.write(Im);
	OPMODE.write(Om); 
	ALUMODE.write(Am);

	D.write(0);
	B.write(opB.read().range(17, 0));

	if (op.bit(4)) {	// multiplicacion
		C.write(0);
		A.write(opA.read().range(29, 0));
	}
	else {				// alu normal
		C.write(opA.read().to_int());					// nos aseguraramos de que se extiende el signo
		A.write(opB.read().range(31, 18).to_int());
	}
	   
}

void alu::resultados(){
	
	sc_int<32> tmp; 
	tmp = R.read().range(31, 0);
	resultadoALU.write( tmp );
	zero.write(tmp == 0);
	if (idx == 7)
		idx = idx;
}

void alu::registro(){

	if( rst.read() ){
		salidaALU.write(0);
		salidaLO.write(0);
	}else{
		salidaALU.write(R.read().range(31, 0));
		if (EscrLO.read())
			salidaLO.write(R.read()); // comprobar si funciona
	}
}
