#include "mmuMem.h"

void mmuMem::direcciones() {

	sc_uint<3> modulo;

	nineBaddress	.write(addr		.read().range(10, 2)); // ojo a este 2
	writeAddress	.write(extAddrIn.read().range(8, 0));	
	extAddrOut		.write(addr		.read().range(11, 2)); // ojo a este 2 

	modulo = addr.read().range(13, 11);

	if (modulo == 1)		// solo para el bloque 1
		writeD.write(W.read());
	else
		writeD.write(false);
	
	// bloques compartidos


	writeSh0.write(incomingW.read() && (!extAddrIn.read().bit(9)));
	writeSh1.write(incomingW.read() && ( extAddrIn.read().bit(9)));

	// escribir al siguiente procesador
	extDataOut.write(dataIn.read());
	outgoingW.write(W.read() && ((modulo == 4) || (modulo == 5)));

	// escribir IO
	Wio1.write(0);		Wio3.write(0);
	if (W.read()) {
		if (addr.read() == 0x3004) {	// io1
			Wio1.write(1);
			io1.write(dataIn.read());
		}
		if (addr.read() == 0x300c) {	// io3
			Wio3.write(1);
			io3.write(dataIn.read());
		}
	}

}

void mmuMem::lectura() {		// es básicamente un MUX

	int x = 0;
	switch (regModulo.read()) {	// posiblemente, no sea el mejor código que he hecho
	case 0: dataOut.write(goodDataOut[0].read()); break;
	case 1: dataOut.write(goodDataOut[1].read()); break;
	case 2: dataOut.write(goodDataOut[2].read()); break;
	case 3: dataOut.write(goodDataOut[3].read()); break;
	case 6: // IO
		switch (whichIO.read()){
		case 0: dataOut.write(regIO0.read()); break; 
		case 2: dataOut.write(regIO2.read()); break;
		case 4: 
			x = extInput.read();
			dataOut.write(extInput.read()); break;
		default: dataOut.write(0);
		};
		break;
	default:
		dataOut.write(0);
	};
}

void mmuMem::registrar() {	// sensible sólo al reloj (no hay reset)

	if (rst.read()) {
		regIO0.write(0);		regIO2.write(0);
		regLectura.write(0);	regModulo.write(0);		whichIO.write(0);
	}
	else {

		if (regLectura.read() && (regModulo.read() == 6)) { // resetea el registro IO después de ser leído
			if (whichIO.read() == 0)
				regIO0.write(0);
			if (whichIO.read() == 2)
				regIO2.write(0);
		}	

		if (Wio0.read()) {
			if (idx == 8)
				idx = idx;
			regIO0.write(io0.read());
		}
		if (Wio2.read()) {
			regIO2.write(io2.read());
		}

		regModulo.write(addr.read().range(13, 11));
		regLectura.write(R.read());
		int a = addr.read();
		int b = R.read();
		whichIO.write(addr.read().range(4,2));	// supongo que no se intentan direcciones "ilegales"
	}
}



int  mmuMem::leeLinea(char* cad, FILE* pt) {

	int c, i = 0, pasa = 0;


	while (1) {
		c = fgetc(pt);
		if (c == '#') {
			cad[i] = 0;
			while ((c != 10) && (c != 13) && (c != -1))
				c = fgetc(pt);
			return (i > 0);
		}
		if ((c == 10) || (c == 13) || (c == -1)) {
			cad[i] = 0;
			return (i > 0);
		}
		if (c == ':') {
			cad[i] = 0;
			if (c > 0)	return 2;
			else	return 0;
		}
		if ((c == '(') || (c == ')'))
			c = ' ';
		if ((pasa == 1) && (c >= ' '))
			cad[i++] = c;
		else {
			if (c > ' ') {	// caracter válido
				pasa = 1;
				cad[i++] = c;
			}
		}
	}
}

char* mmuMem::parse(char* palabra, char* cadena) {

	int i = 0, j = 0;

	while (cadena[i] == ' ')	++i;
	while (1) {
		palabra[j] = cadena[i++];
		if (palabra[j] == ' ') {
			palabra[j] = 0;
			if (cadena[i + 1] == ',')
				++i;
		}
		if (palabra[j] == ',')
			palabra[j] = 0;
		if (palabra[j] == 0)
			return cadena + i;
		++j;
	}
}

int  mmuMem::buscaOP(char* inst) {
	//                  0       1      2       3     4      5      6      7       8       9       10      11      12     13      14     15     16     17     18     19    20    21     22      23     24    25     26,        27
	char* nombres[28] = { "add", "sub", "mult", "and", "or", "xor", "slt", "sllv", "srlv", "srav", "addi", "andi", "ori", "slti", "sll", "srl", "sra", "beq", "bne", "lui", "lw", "sw", "blez", "bgtz", "j", "mflo", "apagar", "nop" };
	int i;

	for (i = 0; i < 28; ++i) {
		if (!strcmp(nombres[i], inst))
			return i;
	}
	return 0xFFFF; // error
}

int  mmuMem::registro(char* cad) {
	char* nombres[32] = { "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra" };
	int i;
	if (cad[0] = !'$') {
		fprintf(stderr, "registro inválido %s\n", cad);
		exit(-1);
	}
	cad++;
	for (i = 0; i < 32; ++i) {
		if (!strcmp(nombres[i], cad))
			return i;
	}
	sscanf(cad, "%d", &i);
	if ((i >= 0) && (i <= 31))
		return i;

	fprintf(stderr, "registro inválido %s\n", cad - 1);
	exit(-1);
}

int  mmuMem::parseInm(char* cad) {
	int n;
	while (cad[0] == ' ') ++cad;
	if ((cad[0] == '0') && ((cad[1] == 'x') || (cad[1] == 'X')))
		sscanf(cad, "%x", &n);
	else
		sscanf(cad, "%d", &n);
	return n;
}



void mmuMem::parseProgram() {
	int i, j, NL = 0, NETQ = 0, code, inst;
	int reg1, reg2, reg3, inm;
	char programa[1000][128];
	char etiquetas[20][128];
	char* pt;
	char buffer[16];
	int target[20];
	unsigned int maq;
	char tmp[64], * cad;
	unsigned int ent;

	code = leeLinea(programa[NL], contenido);
	
	if ((programa[NL][0] >= '0') && (programa[NL][0] <= '9')) { // suponemos que le programa ya está ensamblado, lo leemos en memoria y nos vamos
		i = 0;
		cad = programa[NL];
		if ((cad[0] == '0') && ((cad[1] == 'x') || (cad[1] == 'X')))
			sscanf(cad, "%x", &ent);
		else
			sscanf(cad, "%d", &ent);
		mem[0]->cargaContenido(ent, i++);
		while (!feof(contenido)) {
			fscanf(contenido, "%s", tmp);			cad = tmp;
			while (cad[0] == ' ') ++cad;
			if ((cad[0] == '0') && ((cad[1] == 'x') || (cad[1] == 'X')))
				sscanf(cad, "%x", &ent);
			else
				sscanf(cad, "%d", &ent);
			mem[0]->cargaContenido(ent, i++);
		}
		fclose(contenido);
		return;
	}
	// si el programa no estaba ya ensamblado, continuamos

	if (code == 1)
		++NL;
	if (code == 2) {
		strcpy(etiquetas[NETQ], programa[NL]);
		target[NETQ] = NL;
		++NETQ;
	}

	while (!feof(contenido)) {
		code = leeLinea(programa[NL], contenido);
		if (code == 1)
			++NL;
		if (code == 2) {
			strcpy(etiquetas[NETQ], programa[NL]);
			target[NETQ] = NL;
			++NETQ;
		}
	}

	fclose(contenido);

	for (i = 0; i < NL; ++i) {
		reg1 = reg2 = inm = 0; // para que no se queje el depurador
		pt = parse(buffer, programa[i]);
		inst = buscaOP(buffer);
		if ((inst < 24) && (inst != 2)) {	// instrucciones con al menos 1 registro	excepto mult, que no tiene registro destino
			pt = parse(buffer, pt);
			reg1 = registro(buffer);
		}
		if (inst < 19) {	// instrucciones con al menos 2 registros
			pt = parse(buffer, pt);
			reg2 = registro(buffer);
		}
		if (inst < 10) {	// instrucciones con 3 registros
			pt = parse(buffer, pt);
			reg3 = registro(buffer);
		}
		if ((inst == 20) || (inst == 21)) {	// lw y sw
			pt = parse(buffer, pt);
			inm = parseInm(buffer);
			pt = parse(buffer, pt);
			reg2 = registro(buffer);
		}
		if (((inst >= 10) && (inst <= 16)) || (inst == 19)) {	// inm de instrucciones aritmético lógica incluidos desplazamientos y lui
			pt = parse(buffer, pt);
			inm = parseInm(buffer);
		}

		if ((inst == 17) || (inst == 18) || (inst == 22) || (inst == 23) || (inst == 24)) {		// etiqueta en las instrucciones de salto
			pt = parse(buffer, pt);
			for (j = 0; j < NETQ; ++j) {
				if (!strcmp(etiquetas[j], buffer)) {
					if (inst == 24) // j
						inm = target[j];
					else
						inm = target[j] - (i + 1);
					break;
				}
			}
			if (j == NETQ) {
				fprintf(stderr, "no se encuentra la etiqueta %s\n", buffer);
				exit(-1);
			}
		}

		if (inst == 25) {	// mflo en el último momento
			pt = parse(buffer, pt);
			reg1 = registro(buffer);
		}

		switch (inst) {
		case 0:  maq = 0x00000000 | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | 0x20;	break;	//add
		case 1:  maq = 0x00000000 | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | 0x22;	break; //sub
		case 2:  maq = 0x00000000 | (reg2 << 21) | (reg3 << 16) | 0 | 0x18;	break; //mult
		case 3:  maq = 0x00000000 | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | 0x24;	break; //and
		case 4:  maq = 0x00000000 | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | 0x25;	break; //or
		case 5:  maq = 0x00000000 | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | 0x26;	break; //xor
		case 6:  maq = 0x00000000 | (reg2 << 21) | (reg3 << 16) | (reg1 << 11) | 0x2a;	break; //slt
		case 7:  maq = 0x00000000 | (reg3 << 21) | (reg2 << 16) | (reg1 << 11) | 0x04;	break; //sllv
		case 8:  maq = 0x00000000 | (reg3 << 21) | (reg2 << 16) | (reg1 << 11) | 0x06;	break; //srlv
		case 9:  maq = 0x00000000 | (reg3 << 21) | (reg2 << 16) | (reg1 << 11) | 0x07;	break; //srav
		case 10: maq = (0x08 << 26) | (reg2 << 21) | (reg1 << 16) | (inm & 0xffff);			break; //addi
		case 11: maq = (0x0c << 26) | (reg2 << 21) | (reg1 << 16) | (inm & 0xffff);			break; //andi
		case 12: maq = (0x0d << 26) | (reg2 << 21) | (reg1 << 16) | (inm & 0xffff);			break; //ori
		case 13: maq = (0x0a << 26) | (reg2 << 21) | (reg1 << 16) | (inm & 0xffff);			break; //slti
		case 14: maq = (0x00 << 26) | (reg2 << 16) | (reg1 << 11) | ((inm & 0x1f)<<6) | 0x00;	break; //sll
		case 15: maq = (0x00 << 26) | (reg2 << 16) | (reg1 << 11) | ((inm & 0x1f)<<6) | 0x02;	break; //srl
		case 16: maq = (0x00 << 26) | (reg2 << 16) | (reg1 << 11) | ((inm & 0x1f)<<6) | 0x03;	break; //sra
		case 17: maq = (0x04 << 26) | (reg1 << 21) | (reg2 << 16) | (inm & 0xffff);			break; //beq
		case 18: maq = (0x05 << 26) | (reg1 << 21) | (reg2 << 16) | (inm & 0xffff);			break; //bne
		case 19: maq = (0x0f << 26) | (reg2 << 21) | (reg1 << 16) | (inm & 0xffff);			break; //lui
		case 20: maq = (0x23 << 26) | (reg2 << 21) | (reg1 << 16) | (inm & 0xffff);			break; //lw
		case 21: maq = (0x2b << 26) | (reg2 << 21) | (reg1 << 16) | (inm & 0xffff);			break; //sw
		case 22: maq = (0x06 << 26) | (reg1 << 21) | 0 | (inm & 0xffff);			break; //blez
		case 23: maq = (0x07 << 26) | (reg1 << 21) | 0 | (inm & 0xffff);			break; //bgtz
		case 24: maq = (0x02 << 26) | (inm & 0x3ffffff);										break; //j
		case 25: maq = maq = 0x00000000 | (reg1 << 11) | 0x12;	break; //mflo
		case 26: maq = 0xffffffff;															break; //apagar
		case 27: maq = 0x00000000;															break; //nop
		default:
			fprintf(stderr, "instrucion desconocida con opCode %d\n", inst);		
			exit(-1);
		};
		mem[0]->cargaContenido(maq, i);
	}

	for (; i < 512; ++i)
		mem[0]->cargaContenido(0, i);

}