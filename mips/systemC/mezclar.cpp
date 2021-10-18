#include"mezclar.h"

void mezclar::procesar(){

	sc_int<32> audio1, audio2;
	sc_int<16> escal1, escal2;
	sc_int<24> mult1, mult2, suma;


	addr_in1.write( dir1 );			addr_in2.write( dir2 ); 
	valid.write(false);

	if( rst.read() ){
		stEspera = true;	dir1 = 256;		dir2 = 0; 
		factor1 = 0;		factor2 = 255; 
		return;
	}

	if(stEspera){					// lo que ocurra antes no nos importa
		if( ready_in2.read() )
			stEspera = false; 
	}else{
		dir1 += 2;						dir2 += 2; 
		audio1 = audio_in1.read();		audio2 = audio_in2.read(); 
		escal1 = audio1(23, 8);			escal2 = audio2(23, 8);
		mult1 = escal1 * factor1;		mult2 = escal2 * factor2; 
		suma = mult1 + mult2; 			audio_out->write( suma(23, 8) );	
		++factor1;						--factor2;
		valid.write(true);

		// espero que el factor de escala (23, 8) siga siendo correcto
	}

	
}
