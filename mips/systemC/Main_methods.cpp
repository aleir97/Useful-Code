#include "systemc.h"

#include"mipsCA.h"
#include "productorConsumidor.h"

#define Nproc 17

SC_MODULE(fakeExtInputs) {
public:

	sc_out< sc_int<32> >	extInputs[Nproc];
	sc_in< sc_int<32> >		mask;

	void asignar() {
		for (int i = 0; i < Nproc; ++i)
			extInputs[i].write(0x00dead00);	// solo 1 procesador la utiliza
		extInputs[8].write(mask);			// máscara binaria para el filtro
	}

	SC_CTOR(fakeExtInputs) {
		cout << "fakeExtInputs: " << name() << endl;
		SC_METHOD(asignar);
		sensitive << mask;
	}
};


char* conIndex(char* orig, int n) {
	static char cadena[256];
	sprintf(cadena, "%s_%d", orig, n);
	return cadena;
}


class top : public sc_module
{
public:
	sc_in < bool > clk, rst;

productor* instProd;
consumidor *instCons;
mipsCA* instMIPS[Nproc];
fakeExtInputs* instFEI;

sc_signal<sc_int<32>>	extData[Nproc + 1], pescadilla;
sc_signal<sc_uint<10>>	extAddr[Nproc + 1];
sc_signal<bool>			W[Nproc + 1], Wpescadilla;

sc_signal<sc_int<32>>	LR[Nproc + 1], RL[Nproc + 1], extInput[Nproc], mask;
sc_signal<bool>			WLR[Nproc + 1], WRL[Nproc + 1];



	SC_CTOR(top){
	
		int i;
		char* codigos[] = { "programaL0.txt", "programaL1.txt", "programaL2.txt", "programaL3.txt", "programaL4.txt", "programaL5.txt", "programaL6.txt", "programaL7.txt", "programaFiltrar.txt", "programaL0_inv.txt", "programaL1_inv.txt", "programaL2_inv.txt", "programaL3_inv.txt", "programaL4_inv.txt", "programaL5_inv.txt", "programaL6_inv.txt", "programaL7_inv.txt" };
		char* datos[] = { "data_Dummy.txt", "data_L1_fwd.txt", "data_L2_fwd.txt", "data_L3_fwd.txt", "data_L4_fwd.txt", "data_L5_fwd.txt", "data_L6_fwd.txt", "data_L7_fwd.txt", "data_Dummy.txt", "data_Dummy.txt", "data_L1_inv.txt", "data_L2_inv.txt", "data_L3_inv.txt", "data_L4_inv.txt", "data_L5_inv.txt", "data_L6_inv.txt", "data_L7_inv.txt" };

		for (i = 0; i < Nproc; ++i)
			instMIPS[i] = new mipsCA(conIndex("mips", i), codigos[i], datos[i], i);

		instFEI = new fakeExtInputs("instFEI");
		instProd = new productor("instProd", "datosEntradaL0.txt");
		instCons = new consumidor("instCons", "");


		for (i = 0; i < Nproc; ++i) {
			instMIPS[i]->extDataIn(extData[i]);
			instMIPS[i]->extAddrIn(extAddr[i]);
			instMIPS[i]->incomingW(W[i]);
			instMIPS[i]->extDataOut(extData[i + 1]);
			instMIPS[i]->extAddrOut(extAddr[i + 1]);
			instMIPS[i]->outgoingW(W[i + 1]);
			instMIPS[i]->io0( LR[i]);
			instMIPS[i]->io2( RL[i + 1]); 
			instMIPS[i]->io1( LR[i + 1]);
			instMIPS[i]->io3( RL[i]); 
			instMIPS[i]->Wio0( WLR[i]);
			instMIPS[i]->Wio2(WRL[i + 1]);
			instMIPS[i]->Wio1( WLR[i + 1]);
			instMIPS[i]->Wio3( WRL[i]);
			instMIPS[i]->clk(clk);
			instMIPS[i]->rst(rst);

			instMIPS[i]->extInput(extInput[i]);
			instFEI->extInputs[i](extInput[i]);
		}

		instFEI->mask(mask);

		mask.write(255);		// aquí se especifica qué frecuencias se filtran

		instProd->clk(clk);
		instProd->rst(rst);
		instProd->Winput(WRL[0]);
		instProd->W(W[0]);
		instProd->extAddr(extAddr[0]);
		instProd->input(RL[0]);
		instProd->Woutput(WLR[0]);
		instProd->extData(extData[0]);
		instProd->output(LR[0]);

		instCons->clk(clk);
		instCons->rst(rst);
		instCons->Winput(WLR[Nproc]);
		instCons->W(W[Nproc]);
		instCons->extAddr(extAddr[Nproc]);
		instCons->extData(extData[Nproc]);
		instCons->input(LR[Nproc]);
		instCons->Woutput(WRL[Nproc]);
		instCons->output(RL[Nproc]);

	}

	~top() {
		for(int i=0; i<Nproc; ++i) delete instMIPS[i];
	}

};

int sc_main(int nargs, char* vargs[]){

	sc_clock clk ("clk", 1, SC_NS);
	sc_signal <bool> rst;

	top principal("top");
	principal.clk( clk );
	principal.rst( rst );


	rst.write(true);
	sc_start(2, SC_NS);				

	rst.write(false);
	sc_start();

	if (! sc_end_of_simulation_invoked()) {
		cerr << "Final de la simulacion invocado" << endl; 
		sc_stop();
	}

	return 0;

}
