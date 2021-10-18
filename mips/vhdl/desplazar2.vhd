----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:26:27 02/05/2013 
-- Design Name: 
-- Module Name:    desplazar2 - Behavioral 
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

entity desplazar2 is
    Port ( entrada : in STD_LOGIC_VECTOR (31 downto 0);
			  salida : out  STD_LOGIC_VECTOR (31 downto 0));
end desplazar2;

architecture Behavioral of desplazar2 is

begin

	salida <= entrada(29 downto 0) & "00";

end Behavioral;

