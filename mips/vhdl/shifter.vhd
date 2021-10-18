library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity shifter is
	Port( clk, rst : in STD_LOGIC;
        regIn : in STD_LOGIC_VECTOR (31 downto 0);
        offsetReg, shamt : in STD_LOGIC_VECTOR (4 downto 0);
        direc, arith : in std_logic;    -- 0 izq, 1 derch
        selec : in std_logic_vector(1 downto 0); -- 0x nada, 10 offsetReg, 11 shamt 
        shifted : out STD_LOGIC_VECTOR (31 downto 0));
end shifter;

architecture Behavioral of shifter is

signal sh1, sh2, sh4, sh8, sh16 : std_logic_vector(31 downto 0);
signal offset : std_logic_vector(4 downto 0);
begin

offset <= offsetReg when selec(0)='0' else shamt;

shift1: process(regIn, selec, offset, direc, arith)
variable i : integer; 
variable cuatro : std_logic_vector(3 downto 0);
begin
    cuatro := selec(1) & offset(0) & direc & arith;
    if selec(1)='0' then 
        sh1 (31) <= '0';    sh1(0) <= '0'; 
    else
        case cuatro(2 downto 0) is 
        when "100" => sh1(31) <= regIn(30);     sh1(0) <= '0';
        when "101" => sh1(31) <= regIn(30);     sh1(0) <= '0';
        when "110" => sh1(31) <= '0';           sh1(0) <= regIn(1); 
        when "111" => sh1(31) <= regIn(31);     sh1(0) <= regIn(1);
        when others=> sh1(31) <= regIn(31);    sh1(0) <= regIn(0); 
        end case;
    end if;
     
    for i in 1 to 30 loop
        case cuatro(3 downto 1) is 
        when "100" => sh1(i) <= regIn(i);    -- no desp
        when "101" => sh1(i) <= regIn(i);    -- no desp
        when "110" => sh1(i) <= regIn(i-1);  -- izq
        when "111" => sh1(i) <= regIn(i+1);  -- der
        when others=> sh1(i) <= '0';  
        end case;     
    end loop;
end process; 

shift2: process(sh1, offset, direc, arith)
variable i : integer; 
variable tres : std_logic_vector(2 downto 0);
variable sale, entra : std_logic_vector(31 downto 0);
begin
    tres := offset(1) & direc & arith;
    entra := sh1; 
    
    case tres is 
    when "100" => sale(31 downto 30) := entra(29 downto 28);     sale(1 downto 0) := "00";              -- izq
    when "101" => sale(31 downto 30) := entra(29 downto 28);     sale(1 downto 0) := "00";              -- izq (imposible)
    when "110" => sale(31 downto 30) := "00";                     sale(1 downto 0) := entra(3 downto 2); -- der logico
    when "111" => sale(31 downto 30) := entra(31) & entra(31);    sale(1 downto 0) := entra(3 downto 2); -- der arith
    when others=> sale(31 downto 30) := entra(31 downto 30);     sale(1 downto 0) := entra(1 downto 0);
    end case;

    for i in 2 to 29 loop
        case tres(2 downto 1) is  
        when "10" => sale(i) := entra(i-2);  -- izq
        when "11" => sale(i) := entra(i+2);  -- der
        when others=> sale(i) := entra(i);
        end case;     
    end loop;
    
    sh2 <= sale;
end process; 


shift4: process(sh2, offset, direc, arith)
variable i : integer; 
variable tres : std_logic_vector(2 downto 0);
variable sale, entra : std_logic_vector(31 downto 0);
begin
    tres := offset(2) & direc & arith;
    entra := sh2; 
    
    case tres is 
    when "100" => sale(31 downto 28) := entra(27 downto 24);     sale(3 downto 0) := "0000";            -- izq
    when "101" => sale(31 downto 28) := entra(27 downto 24);     sale(3 downto 0) := "0000";            -- izq (imposible)
    when "110" => sale(31 downto 28) := "0000";                   sale(3 downto 0) := entra(7 downto 4); -- der logico
    when "111" => sale(31 downto 28) := (others => entra(31));    sale(3 downto 0) := entra(7 downto 4); -- der arith
    when others=> sale(31 downto 28) := entra(31 downto 28);     sale(3 downto 0) := entra(3 downto 0);
    end case;

    for i in 4 to 27 loop
        case tres(2 downto 1) is  
        when "10" => sale(i) := entra(i-4);  -- izq
        when "11" => sale(i) := entra(i+4);  -- der
        when others=> sale(i) := entra(i);
        end case;     
    end loop;
    
    sh4 <= sale;
end process; 

shift8: process(sh4, offset, direc, arith)
variable i : integer; 
variable tres : std_logic_vector(2 downto 0);
variable sale, entra : std_logic_vector(31 downto 0);
begin
    tres := offset(3) & direc & arith;
    entra := sh4; 
    
    case tres is 
    when "100" => sale(31 downto 24) := entra(23 downto 16);     sale(7 downto 0) := "00000000";        -- izq
    when "101" => sale(31 downto 24) := entra(23 downto 16);     sale(7 downto 0) := "00000000";        -- izq (imposible)
    when "110" => sale(31 downto 24) := "00000000";               sale(7 downto 0) := entra(15 downto 8); -- der logico
    when "111" => sale(31 downto 24) := (others => entra(31));    sale(7 downto 0) := entra(15 downto 8); -- der arith
    when others=> sale(31 downto 24) := entra(31 downto 24);     sale(7 downto 0) := entra(7 downto 0);
    end case;

    for i in 8 to 23 loop
        case tres(2 downto 1) is  
        when "10" => sale(i) := entra(i-8);  -- izq
        when "11" => sale(i) := entra(i+8);  -- der
        when others=> sale(i) := entra(i);
        end case;     
    end loop;
    
    sh8 <= sale;
end process; 

shift16: process(sh8, offset, direc, arith)
variable i : integer; 
variable tres : std_logic_vector(2 downto 0);
variable sale, entra : std_logic_vector(31 downto 0);
begin
    tres := offset(4) & direc & arith;
    entra := sh8; 
    
    case tres is 
    when "100" => sale(31 downto 16) := entra(15 downto 0);      sale(15 downto 0) := x"0000";        -- izq
    when "101" => sale(31 downto 16) := entra(15 downto 0);      sale(15 downto 0) := x"0000";        -- izq (imposible)
    when "110" => sale(31 downto 16) := x"0000";                  sale(15 downto 0) := entra(31 downto 16); -- der logico
    when "111" => sale(31 downto 16) := (others => entra(31));    sale(15 downto 0) := entra(31 downto 16); -- der arith
    when others=> sale(31 downto 16) := entra(31 downto 16);     sale(15 downto 0) := entra(15 downto 0);
    end case;

    sh16 <= sale;
end process; 

sync:process(clk, rst)
begin
	if rst='1' then
        shifted <= x"00000000";
    elsif (clk='1' and clk'event) then
        shifted <= sh16;
    end if;
end process;


end Behavioral;
