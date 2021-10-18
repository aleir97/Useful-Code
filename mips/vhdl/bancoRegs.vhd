----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    11:17:48 02/05/2013 
-- Design Name: 
-- Module Name:    bancoRegs - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
--use IEEE.STD_LOGIC_ARITH.ALL;
--use IEEE.STD_LOGIC_UNSIGNED.ALL;
USE ieee.numeric_std.ALL;

---- Uncomment the following library declaration if instantiating
---- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity bancoRegs is
    Port ( clk, rst : in STD_LOGIC;
			  regLect1, regLect2, regEscr : in  STD_LOGIC_VECTOR (4 downto 0);
			  datoEscribir  : in  STD_LOGIC_VECTOR (31 downto 0);
			  escrReg : in STD_LOGIC;
			  datoA, datoB : out  STD_LOGIC_VECTOR (31 downto 0));
end bancoRegs;

architecture Behavioral of bancoRegs is
	type arrayRegistros is array (0 to 31) of std_logic_vector(31 downto 0);
	signal banco : arrayRegistros;
begin

datoA <= banco(TO_INTEGER(unsigned(regLect1)));
datoB <= banco(TO_INTEGER(unsigned(regLect2)));

Escritura:process(clk, rst, regEscr)
   variable dir1, dir2 : integer;
begin
	if rst='1' then
		for i in 0 to 31 loop
			banco(i) <= X"00000000";
		end loop;
	elsif (clk='1' and clk'event) then
		if (escrReg='1' and (regEscr/="00000")) then
			banco(TO_INTEGER(unsigned(regEscr))) <= datoEscribir;
		end if;
	end if;
end process;

end Behavioral;

