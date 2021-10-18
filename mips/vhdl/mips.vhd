library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

--  Uncomment the following lines to use the declarations that are
--  provided for instantiating Xilinx primitive components.
--library UNISIM;
--use UNISIM.VComponents.all;

entity mips is
 Port(	clk, rst : in STD_LOGIC;
        extDataIn, io0, io2, extInput : in STD_LOGIC_VECTOR (31 downto 0);
        extDataOut, io1, io3 : out STD_LOGIC_VECTOR (31 downto 0);
        extAddrIn : in STD_LOGIC_VECTOR (9 downto 0);
        extAddrOut : out STD_LOGIC_VECTOR (9 downto 0);
        incomingW, Wio0, Wio2 : in std_logic;
        outgoingW, Wio1, Wio3 : out std_logic); 
end mips;



architecture Behavioral of mips is
component controlCableado
port (	clk, rst : in STD_LOGIC;
		opCodeIni, opCode, functCode : in STD_LOGIC_VECTOR (5 downto 0);
		ALUop : out STD_LOGIC_VECTOR (4 downto 0);
		EscrPC_Cond, EscrPC, LeerMem, EscrMem, EscrIR, SelALUA, EscrReg, RegDest, EscrLO : out STD_LOGIC; --IoD, 
		FuentePC, SelALUB, CondSel, IoD : out STD_LOGIC_VECTOR (1 downto 0);
		direc, arith, noExt : out std_logic;                 -- 0 izq, 1 derch
        shiftSel : out std_logic_vector(1 downto 0); -- 0x nada, 10 offsetReg, 11 shamt 
		selDatoReg_o : out std_logic_vector(2 downto 0));
		
end component;
component bancoRegs 
port ( 	clk, rst : in STD_LOGIC;
		 regLect1, regLect2, regEscr : in  STD_LOGIC_VECTOR (4 downto 0);
		 datoEscribir  : in  STD_LOGIC_VECTOR (31 downto 0);
		 escrReg : in STD_LOGIC;
		 datoA, datoB : out  STD_LOGIC_VECTOR (31 downto 0));
end component;
component ALU 
port ( 	clk, rst : in std_logic; 
		a, b : in STD_LOGIC_VECTOR (31 downto 0);
		op : in STD_LOGIC_VECTOR (4 downto 0);		
		noExt : in std_logic;  
		resultado : out  STD_LOGIC_VECTOR (31 downto 0);
		zero : out STD_LOGIC);
end component;

component desplazar2 
port ( 	entrada : in STD_LOGIC_VECTOR (31 downto 0);
		salida : out  STD_LOGIC_VECTOR (31 downto 0));
end component;
component extSigno 
port ( 	entrada : in STD_LOGIC_VECTOR (15 downto 0);
		salida : out  STD_LOGIC_VECTOR (31 downto 0));
end component;
component memoria 
generic ( fichero : string := "nada" );
port (	clk, rst : in STD_LOGIC; 
        direccion, dataIn : in STD_LOGIC_VECTOR (31 downto 0);
        leerMem, escrMem : in STD_LOGIC;
        dataOut : out  STD_LOGIC_VECTOR (31 downto 0);
        extDataIn, io0, io2, extInput : in STD_LOGIC_VECTOR (31 downto 0);
        extDataOut, io1, io3 : out STD_LOGIC_VECTOR (31 downto 0);
        extAddrIn : in STD_LOGIC_VECTOR (9 downto 0);
        extAddrOut : out STD_LOGIC_VECTOR (9 downto 0);
        incomingW, Wio0, Wio2 : in std_logic;
        outgoingW, Wio1, Wio3 : out std_logic);
end component;




component shifter
port (  clk, rst : in STD_LOGIC; 
        regIn : in STD_LOGIC_VECTOR (31 downto 0);
        offsetReg, shamt : in STD_LOGIC_VECTOR (4 downto 0);
        direc, arith : in std_logic;    -- 0 izq, 1 derch
        selec : in std_logic_vector(1 downto 0); -- 0x nada, 10 offsetReg, 11 shamt 
        shifted : out STD_LOGIC_VECTOR (31 downto 0));
end component;


signal instruccion, direccion, salidaMem, PC, nuevoPC, salidaALU, resultadoALU, slt, lui, LOreg : std_logic_vector(31 downto 0);
signal regA, regB, datoA, datoB, selA, selB, shifted : std_logic_vector(31 downto 0);
signal datosMem, datoEscribir, extenSigno, desplz, jumpDir : std_logic_vector(31 downto 0);
signal regEscr : std_logic_vector(4 downto 0);
signal EscrPC_Cond, EscrPC, LeerMem, EscrMem, EscrIR, SelALUA, EscrReg, RegDest : STD_LOGIC; --IoD, 
signal loadPC, zero, neg, seCumple, leftRight, arith, EscrLO, noExt : STD_LOGIC;
signal FuentePC, IoD, SelALUB, CondSel, shiftSel : STD_LOGIC_VECTOR (1 downto 0);
signal selDatoReg : STD_LOGIC_VECTOR (2 downto 0);
signal aluOp : STD_LOGIC_VECTOR (4 downto 0);

begin

control:controlCableado port map ( 
		clk => clk, 
		rst => rst, 
		opCodeIni => instruccion(31 downto 26), -- salidaMem(31 downto 26), -- opCode recién cargado 
		opCode => instruccion(31 downto 26),
		functCode => instruccion(5 downto 0),
		EscrPC_Cond => EscrPC_Cond,
		EscrPC => EscrPC,
		IoD => IoD,
		LeerMem => LeerMem,
		EscrMem => EscrMem,
		EscrIR => EscrIR,
		SelALUA => SelALUA,
		EscrReg => EscrReg,
		RegDest => RegDest,
		FuentePC => FuentePC,
		ALUop => ALUop,
		CondSel => CondSel,
        direc => leftRight,
        arith => arith, 
        noExt => noExt, 
        shiftSel => shiftSel, 	
        selDatoReg_o => selDatoReg,	
        EscrLO => EscrLO, 
		SelALUB => SelALUB);  

desplazador:shifter port map (
		clk => clk, 
        rst => rst, 
        regIn => datoB,
        offsetReg => datoA(4 downto 0),
        shamt =>  instruccion(10 downto 6),
        direc => leftRight,     -- 0 left, 1 right 
        arith => arith, 
        selec => shiftSel,      -- 0x nada, 10 offsetReg, 11 shamt 
        shifted => shifted);

banco:bancoRegs port map ( clk, rst, instruccion(25 downto 21), instruccion(20 downto 16), regEscr, datoEscribir, EscrReg, datoA, datoB); 
alu1:ALU port map ( 
        clk => clk, 
        rst => '0',  
		a => selA, 
		b => selB, 
		op => aluOp, 				
		noExt => noExt,   
		resultado => resultadoALU, 
		zero => zero);

extSigno1:extSigno	port map ( instruccion(15 downto 0), extenSigno);
desplazar2bits:desplazar2	port map ( extenSigno, desplz);
--despDir:desplazar2 port map ( instruccion
memoria1:memoria generic map(fichero => "\iniMem.mem") port map ( 
        clk => clk, 
        rst => rst, 
        direccion =>  direccion,
        dataIn => regB, 
        leerMem => LeerMem, 
        escrMem => EscrMem, 
        dataOut => salidaMem,
        extDataIn => extDataIn,
        io0 => io0,
        io2 => io2,
        extInput => extInput,
        extDataOut => extDataOut,
        io1 => io1,
        io3 => io3,
        extAddrIn => extAddrIn,
        extAddrOut => extAddrOut,
        incomingW => incomingW,
        Wio0 => Wio0,
        Wio2 => Wio2,
        outgoingW => outgoingW,
        Wio1 => Wio1,
        Wio3 => Wio3);


jumpDir <= PC(31 downto 28) & instruccion(25 downto 0) & "00";

-------------direccion <= PC when IoD='0' else salidaALU;
direccion <= PC when IoD="00" else resultadoALU when IoD="01" else nuevoPC;  --else salidaALU;


regEscr <= instruccion(20 downto 16) when RegDest='0' else instruccion(15 downto 11);

--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

slt(31 downto 1) <= (others=>'0'); slt(0) <= salidaALU(31);
lui(31 downto 16) <= instruccion(15 downto 0);  lui(15 downto 0) <= x"0000";

datoEscribir <= salidaMem   when selDatoReg = "000" else salidaALU  when selDatoReg = "001" else
                LOreg       when selDatoReg = "010" else shifted    when selDatoReg = "011" else
                slt         when selDatoReg = "100" else lui        when selDatoReg = "101" else (others=>'-');
                
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

selA <= PC when SelALUA='0' else regA;
selB <= 	regB when SelALUB="00" else
		X"00000004" when SelALUB="01" else
		extenSigno when SelALUB="10" else desplz;
		
		--esto está mal!!!
--nuevoPC <= resultadoALU when FuentePC="00" else
--		 salidaALU when FuentePC="01" else jumpDir;

nuevoPC <= jumpDir when FuentePC="10" else  salidaALU when ((FuentePC="01") and (seCumple='1')) 
            else PC when ((FuentePC="01") and (seCumple='0')) else resultadoALU;

neg <= resultadoALU(31);	 
seCumple <= zero when CondSel="00" else (not zero) when CondSel="01" else (neg or zero) when CondSel="10" else (not (neg or zero)); 	 
loadPC <= (seCumple and EscrPC_Cond) or EscrPC;



process(clk, rst, loadPc, PC, nuevoPC, EscrIR, instruccion, salidaMem, datosMem, regA, regB, datoA, datoB, salidaALU, resultadoALU)
begin
	if rst='1' then
		PC <= X"00000000";
		-- datosMem <= X"00000000";
		LOreg <= x"00000000";
	elsif clk='1' and clk'event then
		if loadPC = '1' then PC <= nuevoPC; end if;
		if EscrIR = '1' then instruccion <= salidaMem; end if; 
--		datosMem <= salidaMem;
		regA <= datoA;
		regB <= datoB;
		salidaALU <= resultadoALU;
		if EscrLO = '1' then LOreg <= resultadoALU; end if;
	end if;
end process;

end Behavioral;
