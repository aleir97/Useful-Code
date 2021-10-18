#ifndef MIPSCA_H
#define MIPSCA_H

#include "systemc.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"alu.h"
#include"control.h"
#include"gestionPC.h"
#include"mmuMem.h"
#include"muxes.h"
#include"shifter.h"
#include"registersBank.h"

SC_MODULE (mipsCA) {
public:

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



	SC_HAS_PROCESS(mipsCA);

	mipsCA(sc_module_name name_, char* codeFileName, char* dataFileName, int idx) : sc_module(name_) {
		unsigned int ent;
		cout << "mipsCA: " << name() << endl;

		this->idx = idx;

		instalu = new alu("instalu");
		instcontrol = new control("instcontrol");
		instgestionPC = new gestionPC("instgestionPC");
		instmmuMem = new mmuMem("instmmuMem", codeFileName, dataFileName);
		instmuxes = new muxes("instmuxes");
		instshifter = new shifter("instshifter");
		instregistersBank = new registersBank("instregistersBank");

		instalu->setIdx(idx);
		instcontrol->setIdx(idx);
		instregistersBank->setIdx(idx);
		instmmuMem->setIdx(idx);
		instshifter->setIdx(idx);

		instalu->clk(clk);
		instalu->rst(rst);
		instalu->aluOp(ALUop);
		instalu->opA(opA);
		instalu->opB(opB);
		instalu->EscrLO(EscrLO);
		instalu->zero(zero);
		instalu->resultadoALU(resultadoALU);
		instalu->salidaALU(salidaALU);
		instalu->salidaLO(LOreg);

		instcontrol->clk(clk);
		instcontrol->rst(rst);
		instcontrol->instruction(salidaMem);
		instcontrol->rs(rs);
		instcontrol->rt(rt);
		instcontrol->rd(rd);
		instcontrol->shamt(shamt);
		instcontrol->FuentePC(FuentePC);
		instcontrol->IoD(IoD);
		instcontrol->CondSel(CondSel);
		instcontrol->shiftSel(shiftSel);
		instcontrol->selDatoReg_o(selDatoReg);
		instcontrol->SelALUB(SelALUB);
		instcontrol->EscrPC_Cond(EscrPC_Cond);
		instcontrol->EscrPC(EscrPC);
		instcontrol->LeerMem(LeerMem);
		instcontrol->EscrMem(EscrMem);
		instcontrol->EscrIR(EscrIR);
		instcontrol->SelALUA(SelALUA);
		instcontrol->EscrReg(EscrReg);
		instcontrol->EscrLO(EscrLO);
		instcontrol->direc(direc);
		instcontrol->arith(arith);
		instcontrol->ALUop(ALUop);
		instcontrol->inmJ(inmJ);
		instcontrol->inm(inm);

		instregistersBank->clk(clk);
		instregistersBank->rst(rst);
		instregistersBank->rs(rs);
		instregistersBank->rt(rt);
		instregistersBank->rd(rd);
		instregistersBank->rdValue(rdValue);
		instregistersBank->EscrReg(EscrReg);
		instregistersBank->regA(regA);
		instregistersBank->regB(regB);

		instmuxes->SelALUA(SelALUA);
		instmuxes->regA(regA);
		instmuxes->PC(PC);
		instmuxes->aluA(opA);
		instmuxes->SelALUB(SelALUB);
		instmuxes->regB(regB);
		instmuxes->inm(inm);
		instmuxes->aluB(opB);
		instmuxes->IoD(IoD);
		instmuxes->nuevoPC(nuevoPC);
		instmuxes->resultadoALU(resultadoALU);
		instmuxes->direccion(direccion);
		instmuxes->selDatoReg(selDatoReg);
		instmuxes->salidaMem(salidaMem);
		instmuxes->salidaALU(salidaALU);
		instmuxes->LOreg(LOreg);
		instmuxes->shifted(shifted);
		instmuxes->rdValue(rdValue);

		instmmuMem->dataIn(regB);
		instmmuMem->addr(direccion);
		instmmuMem->dataOut(salidaMem);
		instmmuMem->R(LeerMem);
		instmmuMem->W(EscrMem);
		instmmuMem->extDataIn(extDataIn);
		instmmuMem->extAddrIn(extAddrIn);
		instmmuMem->incomingW(incomingW);
		instmmuMem->extDataOut(extDataOut);
		instmmuMem->extAddrOut(extAddrOut);
		instmmuMem->outgoingW(outgoingW);
		instmmuMem->io0(io0);
		instmmuMem->io2(io2);
		instmmuMem->io1(io1);
		instmmuMem->io3(io3);
		instmmuMem->extInput(extInput);
		instmmuMem->Wio0(Wio0);
		instmmuMem->Wio2(Wio2);
		instmmuMem->Wio1(Wio1);
		instmmuMem->Wio3(Wio3);
		instmmuMem->clk(clk);
		instmmuMem->rst(rst);

		instgestionPC->clk(clk);
		instgestionPC->rst(rst);
		instgestionPC->EscrPC(EscrPC);
		instgestionPC->EscrPC_Cond(EscrPC_Cond);
		instgestionPC->zero(zero);
		instgestionPC->FuentePC(FuentePC);
		instgestionPC->CondSel(CondSel);
		instgestionPC->salidaALU(salidaALU);
		instgestionPC->resultadoALU(resultadoALU);
		instgestionPC->jumpDir(inmJ);
		instgestionPC->regPC(PC);
		instgestionPC->nuevoPC(nuevoPC);

		instshifter->clk(clk);
		instshifter->rst(rst);
		instshifter->shamt(shamt);
		instshifter->regIn(regB);
		instshifter->offsetReg(regA);
		instshifter->direc(direc);
		instshifter->arith(arith);
		instshifter->selec(shiftSel);
		instshifter->shifted(shifted);

	}

	~mipsCA() {
		delete instalu;
		delete instcontrol;
		delete instgestionPC;
		delete instmmuMem;
		delete instmuxes;
		delete instshifter;
		delete instregistersBank;
	}


private:
	int idx;
	alu* instalu;
	control* instcontrol;
	gestionPC* instgestionPC;
	mmuMem* instmmuMem;
	muxes* instmuxes;
	shifter* instshifter;
	registersBank* instregistersBank;

	sc_signal< sc_uint<32> >   	PC, nuevoPC;
	sc_signal< sc_uint<32> >	direccion;
	sc_signal< sc_int<32> >   	salidaMem, LOreg, shifted;

	sc_signal< sc_int<32> >   	opA, opB;
	sc_signal<bool>				zero;
	sc_signal< sc_int<32> >		resultadoALU, salidaALU;

	//sc_signal< sc_uint<32> >	instruction;
	sc_signal< sc_uint<5> >  	rs, rt, rd, shamt;
	sc_signal< sc_uint<2> >  	FuentePC, IoD, CondSel, shiftSel;
	sc_signal< sc_uint<3> >  	SelALUB, selDatoReg;
	sc_signal< bool >  			EscrPC_Cond, EscrPC, LeerMem, EscrMem, EscrIR, SelALUA, EscrReg, EscrLO;
	sc_signal<bool>				direc, arith;
	sc_signal< sc_uint<5> >  	ALUop;
	sc_signal< sc_uint<26> >  	inmJ;
	sc_signal< sc_int<16> >  	inm;

	sc_signal< sc_int<32> >		rdValue;
	sc_signal< sc_int<32> >		regA, regB;

};


#endif