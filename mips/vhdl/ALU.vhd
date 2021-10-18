library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library UNISIM;
use UNISIM.VComponents.all;

--add	00000
--sub	00001
--and	00110
--or	01110
--xor	00010
--mult	10000

entity ALU is
    Port ( clk, rst : in STD_LOGIC;
    		a, b : in STD_LOGIC_VECTOR (31 downto 0);
			  op : in STD_LOGIC_VECTOR (4 downto 0);
			  noExt : in std_logic;		  
			  resultado : out  STD_LOGIC_VECTOR (31 downto 0);
			  zero : out STD_LOGIC);
end ALU;




architecture rtl of alu is

signal aa : std_logic_vector(29 downto 0);
signal bb : std_logic_vector(17 downto 0);
signal cc : std_logic_vector(47 downto 0);
signal dd : std_logic_vector(24 downto 0);
signal rr : std_logic_vector(47 downto 0);

-- rollo 
signal ALUMODE : std_logic_vector(3 downto 0);               -- 4-bit input: ALU control input
signal CARRYINSEL : std_logic_vector(2 downto 0);         -- 3-bit input: Carry select input
signal INMODE : std_logic_vector(4 downto 0);                 -- 5-bit input: INMODE control input
signal OPMODE : std_logic_vector(6 downto 0);                 -- 7-bit input: Operation mode input  	


-- salidas a ignorar

signal ACOUT : std_logic_vector(29 downto 0);   -- 30-bit output: A port cascade output
signal BCOUT : std_logic_vector(17 downto 0);   -- 18-bit output: B port cascade output
signal CARRYCASCOUT : std_logic;				-- 1-bit output: Cascade carry output
signal MULTSIGNOUT : std_logic;       			-- 1-bit output: Multiplier sign cascade output
signal PCOUT : std_logic_vector(47 downto 0);   -- 48-bit output: Cascade output
signal OVERFLOW : std_logic;             		-- 1-bit output: Overflow in add/acc output
signal UNDERFLOW : std_logic;           		-- 1-bit output: Underflow in add/acc output
signal CARRYOUT : std_logic_vector(3 downto 0);	-- 4-bit output: Carry output

--salidas para luego
signal PATTERNBDETECT : std_logic;  -- 1-bit output: Pattern bar detect output
signal PATTERNDETECT : std_logic;   -- 1-bit output: Pattern detect output



begin

	CARRYINSEL <= "000";
	INMODE <= op(4) & "000" & op(4);
	OPMODE <= "0" & (not op(4)) & (not op(4)) & op(3) & op(4) & (not op(4)) & "1";
	ALUMODE <= op(2 downto 1) & op(0) & op(0);

--    código que funciona para ALU, pero no para MULT
--    cc(31 downto 0) <= A; 
--    cc(47 downto 32) <= (others=>A(31));
--    dd <= (others=>'0');
--    bb <= B(17 downto 0);
--    aa(13 downto 0) <= B(31 downto 18);
--    aa(29 downto 14) <= (others=>B(31));
    
--  código que también soporta MULT y suprime la extesión de signo
    cc(31 downto 0) <= A when op(4)='0' else (others=>'0'); 
    cc(47 downto 32) <= (others=>A(31)) when op(4)='0' else (others=>'0'); 
    dd <= (others=>'0');
    bb <= B(17 downto 0) when noExt='0' else ("00" & B(15 downto 0)) ;
    aa(13 downto 0) <= A(13 downto 0) when op(4)='1' else  B(31 downto 18) when noExt='0' else (others=>'0'); 
    aa(15 downto 14) <= A(15 downto 14) when op(4)='1' else (others=>B(31));
    aa(29 downto 16) <= A(29 downto 16) when op(4) = '1' else (others=>'0') when noExt='1' else  (others=>B(31));     

	resultado <= rr(31 downto 0);

   DSP48E1_inst : DSP48E1
   generic map (
      -- Feature Control Attributes: Data Path Selection
      A_INPUT => "DIRECT",               -- Selects A input source, "DIRECT" (A port) or "CASCADE" (ACIN port)
      B_INPUT => "DIRECT",               -- Selects B input source, "DIRECT" (B port) or "CASCADE" (BCIN port)
      USE_DPORT => false,                -- Select D port usage (TRUE or FALSE)      no voy a usar este puerto
      USE_MULT => "DYNAMIC",            -- Select multiplier usage ("MULTIPLY", "DYNAMIC", or "NONE")
      USE_SIMD => "ONE48",               -- SIMD selection ("ONE48", "TWO24", "FOUR12")
      -- Pattern Detector Attributes: Pattern Detection Configuration
      AUTORESET_PATDET => "NO_RESET",    -- "NO_RESET", "RESET_MATCH", "RESET_NOT_MATCH" 
--      MASK => X"3fffffffffff",           -- 48-bit mask value for pattern detect (1=ignore)
        MASK => X"ffff00000000",
      PATTERN => X"000000000000",        -- 48-bit pattern match for pattern detect
      SEL_MASK => "MASK",                -- "C", "MASK", "ROUNDING_MODE1", "ROUNDING_MODE2" 
      SEL_PATTERN => "PATTERN",          -- Select pattern value ("PATTERN" or "C")
      USE_PATTERN_DETECT => "PATDET", -- Enable pattern detect ("PATDET" or "NO_PATDET")
      -- Register Control Attributes: Pipeline Register Configuration
      ACASCREG => 0,                     -- Number of pipeline stages between A/ACIN and ACOUT (0, 1 or 2)
      ADREG => 0,                        -- Number of pipeline stages for pre-adder (0 or 1)
      ALUMODEREG => 0,                   -- Number of pipeline stages for ALUMODE (0 or 1)
      AREG => 0,                         -- Number of pipeline stages for A (0, 1 or 2)
      BCASCREG => 0,                     -- Number of pipeline stages between B/BCIN and BCOUT (0, 1 or 2)
      BREG => 0,                         -- Number of pipeline stages for B (0, 1 or 2)
      CARRYINREG => 1,                   -- Number of pipeline stages for CARRYIN (0 or 1)
      CARRYINSELREG => 1,                -- Number of pipeline stages for CARRYINSEL (0 or 1)
      CREG => 0,                         -- Number of pipeline stages for C (0 or 1)
      DREG => 0,                         -- Number of pipeline stages for D (0 or 1)
      INMODEREG => 1,                    -- Number of pipeline stages for INMODE (0 or 1)
      MREG => 1,                         -- Number of multiplier pipeline stages (0 or 1)
      OPMODEREG => 0,                    -- Number of pipeline stages for OPMODE (0 or 1)
      PREG => 0                          -- Number of pipeline stages for P (0 or 1)
   )
   port map (
      -- Cascade: 30-bit (each) output: Cascade Ports
      ACOUT => ACOUT,                   -- 30-bit output: A port cascade output
      BCOUT => BCOUT,                   -- 18-bit output: B port cascade output
      CARRYCASCOUT => CARRYCASCOUT,     -- 1-bit output: Cascade carry output
      MULTSIGNOUT => MULTSIGNOUT,       -- 1-bit output: Multiplier sign cascade output
      PCOUT => PCOUT,                   -- 48-bit output: Cascade output
      -- Control: 1-bit (each) output: Control Inputs/Status Bits
      OVERFLOW => OVERFLOW,             -- 1-bit output: Overflow in add/acc output
      PATTERNBDETECT => PATTERNBDETECT, -- 1-bit output: Pattern bar detect output
      PATTERNDETECT => zero,   -- 1-bit output: Pattern detect output
      UNDERFLOW => UNDERFLOW,           -- 1-bit output: Underflow in add/acc output
      -- Data: 4-bit (each) output: Data Ports
      CARRYOUT => CARRYOUT,             -- 4-bit output: Carry output
      P => rr,                           -- 48-bit output: Primary data output
      -- Cascade: 30-bit (each) input: Cascade Ports
      ACIN => (others=>'0'),			-- 30-bit input: A cascade data input
      BCIN => (others=>'0'),            -- 18-bit input: B cascade input
      CARRYCASCIN => '0',       		-- 1-bit input: Cascade carry input
      MULTSIGNIN => '0',         -- 1-bit input: Multiplier sign input
      PCIN => (others=>'0'),                     -- 48-bit input: P cascade input
      -- Control: 4-bit (each) input: Control Inputs/Status Bits
      ALUMODE => ALUMODE,               -- 4-bit input: ALU control input
      CARRYINSEL => CARRYINSEL,         -- 3-bit input: Carry select input
      CLK => clk,                       -- 1-bit input: Clock input
      INMODE => INMODE,                 -- 5-bit input: INMODE control input
      OPMODE => OPMODE,                 -- 7-bit input: Operation mode input
      -- Data: 30-bit (each) input: Data Ports
      A => aa,                           -- 30-bit input: A data input
      B => bb,                           -- 18-bit input: B data input
      C => cc,                           -- 48-bit input: C data input
      CARRYIN => '0',               -- 1-bit input: Carry input signal
      D => dd,                           -- 25-bit input: D data input
      -- Reset/Clock Enable: 1-bit (each) input: Reset/Clock Enable Inputs
      CEA1 => '0',                     --x 1-bit input: Clock enable input for 1st stage AREG
      CEA2 => '0',                     --x 1-bit input: Clock enable input for 2nd stage AREG
      CEAD => '0',                     --x 1-bit input: Clock enable input for ADREG
      CEALUMODE => '1',           -- 1-bit input: Clock enable input for ALUMODE
      CEB1 => '0',                     --x 1-bit input: Clock enable input for 1st stage BREG
      CEB2 => '0',                     --x 1-bit input: Clock enable input for 2nd stage BREG
      CEC => '1',                       -- 1-bit input: Clock enable input for CREG
      CECARRYIN => '1',           -- 1-bit input: Clock enable input for CARRYINREG
      CECTRL => '1',                 -- 1-bit input: Clock enable input for OPMODEREG and CARRYINSELREG
      CED => '0',                       --x 1-bit input: Clock enable input for DREG
      CEINMODE => '0',             --x 1-bit input: Clock enable input for INMODEREG
      CEM => '1',                       -- 1-bit input: Clock enable input for MREG
      CEP => '1',                       -- 1-bit input: Clock enable input for PREG
      RSTA => rst,                     -- 1-bit input: Reset input for AREG
      RSTALLCARRYIN => rst,   -- 1-bit input: Reset input for CARRYINREG
      RSTALUMODE => rst,         -- 1-bit input: Reset input for ALUMODEREG
      RSTB => rst,                     -- 1-bit input: Reset input for BREG
      RSTC => rst,                     -- 1-bit input: Reset input for CREG
      RSTCTRL => rst,               -- 1-bit input: Reset input for OPMODEREG and CARRYINSELREG
      RSTD => rst,                     -- 1-bit input: Reset input for DREG and ADREG
      RSTINMODE => rst,           -- 1-bit input: Reset input for INMODEREG
      RSTM => rst,                     -- 1-bit input: Reset input for MREG
      RSTP => rst                      -- 1-bit input: Reset input for PREG
   );





end rtl;