library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity controlCableado is
	Port( clk, rst : in STD_LOGIC;
			opCodeIni, opCode, functCode : in STD_LOGIC_VECTOR (5 downto 0);
			ALUop : out STD_LOGIC_VECTOR (4 downto 0);
			EscrPC_Cond, EscrPC, LeerMem, EscrMem, EscrIR, SelALUA, EscrReg, RegDest, EscrLO : out STD_LOGIC; -- IoD, 
			FuentePC, IoD, SelALUB, CondSel : out STD_LOGIC_VECTOR (1 downto 0);
			direc, arith, noExt : out std_logic;                 -- 0 izq, 1 derch
            shiftSel : out std_logic_vector(1 downto 0);-- 0x nada, 10 offsetReg, 11 shamt
            selDatoReg_o : out std_logic_vector(2 downto 0));  
end controlCableado;

architecture Behavioral of controlCableado is
	type estados is (inicial, segundo, buff, lw1, lw2, sw, arith1, arith2, inme1, inme2, mult2, branch, jump);
	signal estado, proximoEstado : estados;
	signal selDatoReg : std_logic_vector(2 downto 0);
begin


--add	00000
--sub	00001
--and	00110
--or	01110
--xor	00010
--mult	10000

FSM:process(opCode, opCodeIni, functCode, estado)
begin

    CondSel <= opCode(1 downto 0);
    shiftSel <= "00";
    direc <= '0';
    arith <= '0';     
    selDatoReg <= "000";
    LeerMem	<= '0';
	IoD        <= "00";
    EscrIR     <= '0';
    EscrPC     <= '0';
    FuentePC   <= "00";
    EscrPC_Cond <= '0';
    EscrMem <= '0';
    EscrReg <= '0';
    RegDest <= '0';   
    EscrLO <= '0';
    noExt <= '0';
         
	case estado is
	when buff =>
		LeerMem	<= '1';
		SelALUA	<= '0';
		selALUB <= "01";
		ALUOp 	<= "00000";
		proximoEstado <= inicial;
	when inicial =>
		SelALUA	<= '0';
		EscrIR	<= '1';
		EscrPC	<= '1';
		FuentePC	<= "00";
		selALUB 	<= "01";
		ALUOp 	<= "00000";
		proximoEstado <= segundo;
	when segundo =>
		SelALUA	<= '0';
		selALUB 	<= "11";
		ALUOp 	<= "00000";
		proximoEstado <= segundo;
		if opCodeIni="100011" then
			proximoEstado <= lw1;
		elsif opCodeIni="101011" then
			proximoEstado <= sw;
		elsif opCodeIni="000000" then
			proximoEstado <= arith1;
		elsif opCodeIni(5 downto 3)="001" then 
		    proximoEstado <= inme1;
		elsif opCodeIni(5 downto 2)="0001" then
			proximoEstado <= branch;
		else
			proximoEstado <= jump;
		end if;
	when lw1 => 
 		LeerMem	<= '1';
		SelALUA	<= '1';
		IoD 		<= "01";
		selALUB 	<= "10";
		ALUOp 	<= "00000";
		selDatoReg <= "000";
		proximoEstado <= lw2;
	when lw2 =>
 		LeerMem	<= '1';-- para la siguiente instrucción
		SelALUA	<= '0';
		selALUB 	<= "00";
		ALUOp 	<= "00000";
		EscrReg <= '1';   -- seleccionado en lw1
		RegDest <= '0';
		proximoEstado <= inicial;
	when sw =>
		SelALUA	<= '1';
		IoD 		<= "01";
		selALUB 	<= "10";
		ALUOp 	<= "00000";
		EscrMem <= '1';
		proximoEstado <= buff;
	when arith1 =>
		SelALUA	<= '1';
		selALUB 	<= "00";
		ALUOp 	<= "01110";	      -- or por defecto
		selDatoReg <= "001";      -- salida de la ALU por defecto 
		proximoEstado <= arith2;  -- por defecto
		
		case functCode is
		when "100000" => 	ALUOp 	<= "00000";	-- sumar
		when "100010" => 	ALUOp 	<= "00001";	-- restar
		when "100100" => 	ALUOp 	<= "00110";	-- and
		when "100101" => 	ALUOp 	<= "01110";	-- or
		when "100110" => 	ALUOp 	<= "00010";	-- exor
		when "101010" => 	ALUOp 	<= "00001";     selDatoReg <= "100";  -- slt
		when "011000" => 	ALUOp 	<= "10000";	    proximoEstado <= mult2;  -- mult
		when "000000" =>    direc   <= '0';  arith  <= '0';  shiftSel <= "11";    selDatoReg <= "011";     -- sll 
		when "000100" =>    direc   <= '0';  arith  <= '0';  shiftSel <= "10";    selDatoReg <= "011";     -- sllv
		when "000010" =>    direc   <= '1';  arith  <= '0';  shiftSel <= "11";    selDatoReg <= "011";     -- srl 
		when "000110" =>    direc   <= '1';  arith  <= '0';  shiftSel <= "10";    selDatoReg <= "011";     -- srlv
		when "000011" =>    direc   <= '1';  arith  <= '1';  shiftSel <= "11";    selDatoReg <= "011";     -- sra 
		when "000111" =>    direc   <= '1';  arith  <= '1';  shiftSel <= "10";    selDatoReg <= "011";     -- srav
		when "010010" =>    selDatoReg <= "010";      -- LO
        when others =>      proximoEstado <= buff;    -- nunca deberíamos llegar aquí
        end case;
		--add	00000
        --sub    00001
        --and    00110
        --or    01110
        --xor    00010
        --mult    10000
    when mult2 =>
 		LeerMem	<= '1';
        SelALUA    <= '0';
        selALUB     <= "00";
        ALUOp     <= "10000"; -- no hace nada,en realidad
        EscrLO <= '1';
        proximoEstado <= buff; -- inicial;    
    when arith2 =>
 		LeerMem	<= '1';
		SelALUA	<= '0';
		selALUB 	<= "00";
		ALUOp 	<= "00000";
		EscrReg <= '1';   -- seleccionado en arith1
		RegDest <= '1';
		proximoEstado <= inicial;
    when inme1 => 
        SelALUA    <= '1';
        selALUB     <= "10";     -- inmediato extendido en signo
        proximoEstado <= inme2;
        selDatoReg <= "001";    -- por defecto
        case opCode(2 downto 0) is
        when "000" =>     ALUOp     <= "00000";    -- addi
        when "010" =>     ALUOp     <= "00001";   selDatoReg <= "100";  -- slti/restar
        when "100" =>     ALUOp     <= "00110";   noExt <= '1';         -- andi
        when "101" =>     ALUOp     <= "01110";   noExt <= '1';         -- ori
        when "111" =>     ALUOp     <= "01110";   selDatoReg <= "101";  -- lui
        when others =>    ALUOp     <= "01110";   proximoEstado <= buff;  -- no debería suceder nunca
        end case;
    when inme2 =>         
 		LeerMem	<= '1';
        SelALUA    <= '0';
        selALUB     <= "00";
        ALUOp     <= "00000";
        EscrReg <= '1'; -- seleccionado en inme1
        RegDest <= '0'; -- esta es la única diferencia con arith2
        proximoEstado <= inicial;        			
	when branch =>
 		LeerMem	<= '1';
		SelALUA	<= '1';
		IoD 		<= "10";
		FuentePC	<= "01";
		selALUB 	<= "00";
		ALUOp 	<= "00001";
		EscrPC_Cond <= '1';   -- la condicion se mira fuera
		proximoEstado <= inicial;
	when jump => -- jump 
 		LeerMem	<= '1';
        SelALUA    <= '0';
        IoD         <= "10";
        FuentePC    <= "10";
        selALUB     <= "00";
        ALUOp     <= "00000";
        EscrPC  <= '1'; 
        proximoEstado <= inicial;
	when others => -- una copia de buff
		LeerMem	<= '1';
        SelALUA    <= '0';
        selALUB <= "01";
        ALUOp     <= "00000";
        proximoEstado <= inicial;	   
    end case;
end process;

sync:process(clk, rst, proximoEstado, selDatoReg)
begin
	if rst='1' then
		estado <= buff; -- ojo a esto
		selDatoReg_o <= "000";
	elsif (clk='1' and clk'event) then
		estado <= proximoEstado;
		selDatoReg_o <= selDatoReg;
	end if;
	
	
end process;

end Behavioral;

