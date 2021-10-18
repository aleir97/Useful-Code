----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:23:56 02/05/2013 
-- Design Name: 
-- Module Name:    extSigno - Behavioral 
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
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

---- Uncomment the following library declaration if instantiating
---- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity extSigno is
    Port ( entrada : in STD_LOGIC_VECTOR (15 downto 0);
			  salida : out  STD_LOGIC_VECTOR (31 downto 0));
end extSigno;

architecture Behavioral of extSigno is
	signal signo4 : STD_LOGIC_VECTOR (3 downto 0);
begin

	signo4 <= entrada(15) & entrada(15) & entrada(15) & entrada(15);
	salida <= signo4 & signo4 & signo4 & signo4 & entrada;

end Behavioral;

