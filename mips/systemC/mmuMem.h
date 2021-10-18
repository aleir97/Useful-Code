#ifndef MMUMEM_H
#define MMUMEM_H

#include "systemc.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"BRAM.h"


SC_MODULE (mmuMem) {
public:

	sc_in<sc_int<32>>	dataIn;
	sc_in<sc_uint<32>>	addr;
	sc_out<sc_int<32>>	dataOut;
	sc_in<bool>			R, W;

	sc_in<sc_int<32>>	extDataIn;
	sc_in<sc_uint<10>>	extAddrIn;
	sc_in<bool>			incomingW;

	sc_out<sc_int<32>>	extDataOut;
	sc_out<sc_uint<10>>	extAddrOut;
	sc_out<bool>		outgoingW;

	sc_in<sc_int<32>>	io0, io2, extInput;
	sc_out<sc_int<32>>	io1, io3;
	sc_in<bool>			Wio0, Wio2;
	sc_out<bool>		Wio1, Wio3;

	sc_in<bool>			clk, rst;

	void direcciones();
	void lectura();
	void registrar();
	void setIdx(int id) {
		idx = id;
	};
	

	SC_HAS_PROCESS(mmuMem);

	mmuMem(sc_module_name name_, char* codeFileName, char* dataFileName) : sc_module(name_) {
		unsigned int ent;
		cout << "mmuMem: " << name() << "  " << codeFileName << " " << dataFileName << endl;
		char tmp[64], *cad ;
		
		noWrite.write(0);

		mem[0] = new BRAM("code");
		mem[1] = new BRAM("data");
		mem[2] = new BRAM("interna0");
		mem[3] = new BRAM("interna1");

		mem[0]->addrA(nineBaddress);
		mem[0]->addrB(nullAddress);
		mem[0]->dinA(nullDataIn);
		mem[0]->dinB(nullDataIn);
		mem[0]->weA(noWrite);
		mem[0]->weB(noWrite);
		mem[0]->doutA(goodDataOut[0]);
		mem[0]->doutB(nullDataOut[0]);
		mem[0]->clk(clk);

		mem[1]->addrA(nineBaddress);
		mem[1]->addrB(nullAddress);
		mem[1]->dinA(dataIn);
		mem[1]->dinB(nullDataIn);
		mem[1]->weA(writeD);
		mem[1]->weB(noWrite);
		mem[1]->doutA(goodDataOut[1]);
		mem[1]->doutB(nullDataOut[1]);
		mem[1]->clk(clk);

		// estos 2 bloques son de memoria compartida. 
		// Se escriben desde fuera y se leen desde el procesador propio		 
		mem[2]->addrA(nineBaddress);		// lectura
		mem[2]->addrB(writeAddress);		// escritura
		mem[2]->dinA(nullDataIn);
		mem[2]->dinB(extDataIn);
		mem[2]->weA(noWrite);
		mem[2]->weB(writeSh0);
		mem[2]->doutA(goodDataOut[2]);
		mem[2]->doutB(nullDataOut[2]);
		mem[2]->clk(clk);

		mem[3]->addrA(nineBaddress);		// lectura
		mem[3]->addrB(writeAddress);		// escritura
		mem[3]->dinA(nullDataIn);
		mem[3]->dinB(extDataIn);
		mem[3]->weA(noWrite);
		mem[3]->weB(writeSh1);
		mem[3]->doutA(goodDataOut[3]);
		mem[3]->doutB(nullDataOut[3]);
		mem[3]->clk(clk);


		contenido = fopen(codeFileName, "rt");
		if (!contenido) {
			fprintf(stderr, "Error abriendo %s\n", codeFileName);
			exit(-1);
		}
		parseProgram();

		contenido = fopen(dataFileName, "rt");
		if (!contenido) {
			fprintf(stderr, "Error abriendo %s\n", dataFileName);
			exit(-1);
		}
		for (int i = 0; i < 512; ++i) {
			fscanf(contenido, "%s", tmp);			cad = tmp;
			while(cad[0]==' ') ++cad;
			if( (cad[0]=='0') && ((cad[1]=='x') || (cad[1]=='X')) )
				sscanf(cad, "%x", &ent);
			else
				sscanf(cad, "%d", &ent);
			mem[1]->cargaContenido(ent, i);
		}
		fclose(contenido);

/*		char contMem[128];			// solo para volcar la memoria para poder usarla en VHDL
		sprintf(contMem, "%s.mem", codeFileName);
		FILE* todoMem;
		todoMem = fopen(contMem, "wt");
		mem[0]->volcado(todoMem);
		mem[1]->volcado(todoMem);
		fclose(todoMem);	*/

		SC_METHOD(direcciones);
		sensitive << addr << extAddrIn << R << W << incomingW << dataIn;
		SC_METHOD(lectura);
		sensitive << regModulo << goodDataOut[0] << goodDataOut[1] << goodDataOut[2] << goodDataOut[3] << whichIO << regIO0 << regIO2 << regLectura;
		SC_METHOD(registrar);
		sensitive << clk.pos();
	}

	~mmuMem() {
		mem[1]->volcado();
		delete mem[0];
		delete mem[1];
		delete mem[2];
		delete mem[3];
	}

private:
	FILE *contenido;
	BRAM *mem[4];
	int idx;
	sc_signal<sc_uint<9>> nullAddress, nineBaddress, writeAddress;
	sc_signal<sc_int<32>> nullDataIn;
	sc_signal<sc_int<32>> nullDataOut[4], goodDataOut[4];
	sc_signal<bool>		  noWrite, writeD, writeSh0, writeSh1;
	sc_signal<sc_uint<3>> regModulo, whichIO;
	sc_signal < sc_int<32>>	regIO0, regIO2;
	sc_signal <bool> regLectura;

	void parseProgram();
	int leeLinea(char* cad, FILE* pt);
	char* parse(char* palabra, char* cadena);
	int buscaOP(char *inst);
	int registro(char *cad);
	int parseInm(char *cad);

};


#endif