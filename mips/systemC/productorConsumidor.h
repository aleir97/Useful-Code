#ifndef PRODUCTORCONSUMIDOR_H
#define PRODUCTORCONSUMIDOR_H

#include "systemc.h"
#include <stdio.h>


SC_MODULE(productor) {
public:

	sc_in<bool>			clk, rst, Winput;
	sc_out<sc_uint<10>>	extAddr;
	sc_in<sc_int<32>>	input;
	sc_out<bool>		Woutput, W;
	sc_out<sc_int<32>>	extData, output;


	SC_HAS_PROCESS(productor);

	void producir() {

		int dato, dir;

		Woutput.write(false);	output.write(0);	W.write(false);

		if (rst.read()) {
			estado = contador = 0;
			return;
		}
		switch (estado) {
		case 0:
			//if (Winput.read() && input.read()) {
			estado = 1; contador = 0;
			//}
			break;
		case 1:
			fscanf(fichero, "%d %d", &dir, &dato);
			extAddr.write(dir);	extData.write(dato);	W.write(true);
			++contador;
			if (contador == 512) {
				contador = 0;
				estado = 2;
				Woutput.write(true);	output.write(1);
			}
			break;
		case 2:
			fscanf(fichero, "%d %d", &dir, &dato);
			extAddr.write(dir);	extData.write(dato);	W.write(true);
			++contador;
			if (contador == 512) {
				contador = 0;
				estado = 3;
				Woutput.write(true);	output.write(1);
			}
			break;
		default:
			if (Winput.read() && input.read()) {
				estado = 2; contador = 0;
			}
		};
	}

	productor(sc_module_name name_, char* fileName) : sc_module(name_) {
		cout << "productor: " << name() << "  " << fileName << endl;
		fichero = fopen(fileName, "rt");
		if (!fichero) {
			fprintf(stderr, "No es posible abrir el fichero %s\n", fileName);
			exit(-1);
		}
		SC_METHOD(producir);
		sensitive << clk.pos();
	}
private:
	FILE* fichero;
	int estado, contador;
};


SC_MODULE(consumidor) {
public:
	sc_in<bool>			clk, rst, W, Winput;
	sc_in<sc_uint<10>>	extAddr;
	sc_in<sc_int<32>>	input, extData;
	sc_out<bool>		Woutput;
	sc_out<sc_int<32>>	output;

	SC_HAS_PROCESS(consumidor);

	void consumir() {
		sc_int<32> valor;

		Woutput.write(false);	output.write(0);

		if (rst.read()) {
			estado = contador = 0;
			return;
		}
		switch (estado) {
		case 0:	
			estado = 1;	 break;
		case 1: 
			if (W.read()) {
				cout << hex << "@ " << extAddr.read() << "   " << extData.read() << endl;
				++contador; 
				if (contador == 512) {
					estado = contador = 0;
					Woutput.write(true);	output.write(1);
				}
			}
			break;
		default: 
			cerr << "Estado no contemplado en consumidor: " << estado << endl;
		};

	}

	consumidor(sc_module_name name_, char* fileName) : sc_module(name_) {
		cout << "consumidor: " << name() << "  " << fileName << endl;
		/*
		fichero = fopen(fileName, "wt");			NO UTILIZO FICHERO DE SALIDA, UTILIZO LA PANTALLA
		if(!fichero){
			fprintf(stderr, "No es posible abrir el fichero %s\n", fileName);
			exit(-1);
		}*/
		SC_METHOD(consumir);
		sensitive << clk.pos();
	}
private:
	FILE* fichero;
	int estado, contador;
};



#endif


