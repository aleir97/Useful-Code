----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:28:05 02/05/2013 
-- Design Name: 
-- Module Name:    memoria - Behavioral 
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
library UNISIM;
use UNISIM.VComponents.all;

entity memoria is
    generic ( fichero : string := "\iniMem.mem" );
    Port (	clk, rst : in STD_LOGIC; 
				direccion, dataIn : in STD_LOGIC_VECTOR (31 downto 0);
				leerMem, escrMem : in STD_LOGIC;
				dataOut : out  STD_LOGIC_VECTOR (31 downto 0);
				extDataIn, io0, io2, extInput : in STD_LOGIC_VECTOR (31 downto 0);
				extDataOut, io1, io3 : out STD_LOGIC_VECTOR (31 downto 0);
				extAddrIn : in STD_LOGIC_VECTOR (9 downto 0);
				extAddrOut : out STD_LOGIC_VECTOR (9 downto 0);
				incomingW, Wio0, Wio2 : in std_logic;
				outgoingW, Wio1, Wio3 : out std_logic);
				
end memoria;

architecture Behavioral of memoria is

--	type arrayMemoria is array (0 to 255) of std_logic_vector(31 downto 0);
--	signal memoriaRAM : arrayMemoria;
	
--	signal activoA : std_logic;
--	signal RSTRAMB, CASCADEOUTA, CASCADEOUTB, DBITERR, SBITERR, CASCADEINA, CASCADEINB, INJECTDBITERR, INJECTSBITERR, REGCEAREGCE, RSTRAMARSTRAM, RSTREGARSTREG : std_logic;
 	
 	--, DOPADOP, DOPBDOP, DIPADIP, DIPBDIP : std_logic_vector(3 downto 0);
--	signal ECCPARITY, WEBWE : std_logic_vector(7 downto 0);
--	signal RDADDRECC : std_logic_vector(8 downto 0);
--	signal DIR : std_logic_vector(15 downto 0);
--	signal DOBDO, DIBDI : std_logic_vector(31 downto 0);

	
	--signal nullAddress : std_logic_vector( 15 downto 0);             innecesario, puedo poner open           
	signal sixteenBaddress : std_logic_vector( 15 downto 0);           --innecesario? no sé...
	signal writeAddress : std_logic_vector( 15 downto 0);              -- se usa en el puerto B de los viejos módulos 2 y 3
	signal nullDataIn : std_logic_vector( 31 downto 0);                -- innecesario
	signal dataOut_01, dataOut_23 : std_logic_vector( 31 downto 0); --OK...                           -- nullDataOut[4], 
	signal writeD : std_logic;                  -- se usa sólo en el módulo 1 (ahora 01)
--	signal writeSh0, writeSh1 : std_logic;                  -- hay que unificarlos. Son el módulo 23

--	signal  : std_logic_vector( 2 downto 0);          -- ahora sólo hay 3 casos: 00x, 01x y 110 habrá que hacer cambios... 
	signal regIO0, regIO2 : std_logic_vector( 31 downto 0); 
	signal regLectura : std_logic; 
	signal modulo, regModulo : std_logic_vector( 1 downto 0);
	signal whichIO : std_logic_vector( 2 downto 0);  
	
	signal escr01 : std_logic_vector(3 downto 0);
	signal escr23 : std_logic_vector(7 downto 0);
	
begin

    sixteenBaddress    <= "0" & direccion(11 downto 0) & "000";   -- antiguo DIR
    writeAddress       <= "0" & extAddrIn(9 downto 0) & "00000"; -- pasar a bytes y pasar a bits        ???  vv.write(extAddrIn.read().range(8, 0));	
    extAddrOut         <= direccion(11 downto 2); 	
    modulo             <= direccion(13 downto 12);
    writeD             <= escrMem when modulo="00" else '0';
    
    escr01             <= writeD & writeD & writeD & writeD;
    escr23             <= incomingW & incomingW & incomingW & incomingW & incomingW & incomingW & incomingW & incomingW;
    
    extDataOut          <= dataIn; 
    outgoingW           <= '1' when modulo="10" else '0';
    
    io1 <= dataIn;      Wio1 <= escrMem when direccion(15 downto 0) = x"3004" else '0';
    io3 <= dataIn;      Wio3 <= escrMem when direccion(15 downto 0) = x"300c" else '0';
    
    

lectura:process(regModulo, dataOut_01, dataOut_23, whichIO, regIO0, regIO2, regLectura, extInput)		-- es básicamente un MUX
begin

    case regModulo is
    when "00" => dataOut <= dataOut_01;
    when "01" => dataOut <= dataOut_23;
    when "11" => 
        case whichIO is
        when "000" => dataOut <= regIO0;
        when "010" => dataOut <= regIO2;
        when others => dataOut <= extInput;
        end case; 
    when others => dataOut <= (others=>'0');
    end case; 
end process; 


registrar:process(clk, rst, direccion, regLectura, regModulo, whichIO, Wio0, Wio2, io0, io2, leerMem)	
begin
    
    if rising_edge(clk) then 
        if rst = '1' then 
            regIO0 <= (others=>'0');   regIO2 <= (others=>'0');
            regLectura <= '0';         regModulo <= "00";          whichIO <= "000";
        else
            if (regLectura = '1') and (regModulo = "11") then 
                if whichIO = "000" then regIO0 <= (others=>'0'); end if;
                if whichIO = "010" then regIO2 <= (others=>'0'); end if;
            end if;
            
            if Wio0 = '1' then regIO0 <= io0;   end if;  
            if Wio2 = '1' then regIO2 <= io2;   end if;  
            
            regModulo <= modulo;
            regLectura <= leerMem;
            
            whichIO <= direccion(4 downto 2);
        end if;
    end if;
end process; 



   CODE_DATA : RAMB36E1
   generic map (     
      RDADDR_COLLISION_HWCONFIG => "PERFORMANCE",       -- Address Collision Mode: "PERFORMANCE" or "DELAYED_WRITE"       
      SIM_COLLISION_CHECK => "NONE",                    -- Collision check: Values ("ALL", "WARNING_ONLY", "GENERATE_X_ONLY" or "NONE")   
      DOA_REG => 0,                                     -- DOA_REG, DOB_REG: Optional output register (0 or 1)
      DOB_REG => 0,
      EN_ECC_READ => FALSE,                             -- Enable ECC decoder,   -- FALSE, TRUE
      EN_ECC_WRITE => FALSE,                            -- Enable ECC encoder,   -- FALSE, TRUE     
      INIT_A => X"000000000",                           -- INIT_A, INIT_B: Initial values on output ports
      INIT_B => X"000000000",      
      INIT_FILE => fichero,                             -- Initialization File: RAM initialization file      
      RAM_MODE => "TDP",                                -- RAM Mode: "SDP" or "TDP"       
      RAM_EXTENSION_A => "NONE",                        -- RAM_EXTENSION_A, RAM_EXTENSION_B: Selects cascade mode ("UPPER", "LOWER", or "NONE")
      RAM_EXTENSION_B => "NONE",    
      READ_WIDTH_A => 36,                               -- 0-72     -- READ_WIDTH_A/B, WRITE_WIDTH_A/B: Read/write width per port
      READ_WIDTH_B => 36,                               -- 0-36
      WRITE_WIDTH_A => 36,                              -- 0-36
      WRITE_WIDTH_B => 36,                              -- 0-72     
      RSTREG_PRIORITY_A => "RSTREG",                    -- RSTREG_PRIORITY_A, RSTREG_PRIORITY_B: Reset or enable priority ("RSTREG" or "REGCE")
      RSTREG_PRIORITY_B => "RSTREG",     
      SRVAL_A => X"000000000",                          -- SRVAL_A, SRVAL_B: Set/reset value for output
      SRVAL_B => X"000000000",      
      SIM_DEVICE => "7SERIES",                          -- Simulation Device: Must be set to "7SERIES" for simulation behavior
      WRITE_MODE_A => "WRITE_FIRST",                    -- WriteMode: Value on output upon a write ("WRITE_FIRST", "READ_FIRST", or "NO_CHANGE")
      WRITE_MODE_B => "WRITE_FIRST" 
   )
   port map (
          CASCADEOUTA => open,     -- 1-bit output: A port cascade          -- Cascade Signals: 1-bit (each) output: BRAM cascade ports (to create 64kx1)
          CASCADEOUTB => open,     -- 1-bit output: B port cascade        
          DBITERR => open,         -- 1-bit output: Double bit error status     -- ECC Signals: 1-bit (each) output: Error Correction Circuitry ports
          ECCPARITY => open,       -- 8-bit output: Generated error correction parity
          RDADDRECC => open,       -- 9-bit output: ECC read address
          SBITERR => open,         -- 1-bit output: Single bit error status
      -- Port A Data: 32-bit (each) output: Port A data
      DOADO => dataOut_01,                 -- 32-bit output: A port data/LSB data
      DOPADOP => open,             -- 4-bit output: A port parity/LSB parity
      -- Port B Data: 32-bit (each) output: Port B data
      DOBDO => open,                 -- 32-bit output: B port data/MSB data
      DOPBDOP => open,             -- 4-bit output: B port parity/MSB parity
          -- Cascade Signals: 1-bit (each) input: BRAM cascade ports (to create 64kx1)
          CASCADEINA => '0',       -- 1-bit input: A port cascade
          CASCADEINB => '0',       -- 1-bit input: B port cascade
          -- ECC Signals: 1-bit (each) input: Error Correction Circuitry ports
          INJECTDBITERR => '0', -- 1-bit input: Inject a double bit error
          INJECTSBITERR => '0', -- 1-bit input: Inject a single bit error
            -- Port A Address/Control Signals: 16-bit (each) input: Port A address and control signals (read port
            -- when RAM_MODE="SDP")
      ADDRARDADDR => sixteenBaddress,     -- 16-bit input: A port address/Read address
         CLKARDCLK => clk,         -- 1-bit input: A port clock/Read clock
         ENARDEN => '1',             -- 1-bit input: A port enable/Read enable
         REGCEAREGCE => '0',     -- OJO  1-bit input: A port register enable/Register enable
         RSTRAMARSTRAM => '0', -- 1-bit input: A port set/reset
         RSTREGARSTREG => '0', -- 1-bit input: A port register set/reset
      WEA => escr01,                     -- 4-bit input: A port write enable
      -- Port A Data: 32-bit (each) input: Port A data
      DIADI => dataIn,                 -- 32-bit input: A port data/LSB data
           DIPADIP => "0000",             -- 4-bit input: A port parity/LSB parity
      -- Port B Address/Control Signals: 16-bit (each) input: Port B address and control signals (write port
      -- when RAM_MODE="SDP")
      ADDRBWRADDR => x"0000",     -- 16-bit input: B port address/Write address
      CLKBWRCLK => CLK,         -- 1-bit input: B port clock/Write clock
      ENBWREN => '0',             -- 1-bit input: B port enable/Write enable
         REGCEB => '0',               -- 1-bit input: B port register enable
          RSTRAMB => '0',             -- 1-bit input: B port set/reset
         RSTREGB => '0',             -- 1-bit input: B port register set/reset
      WEBWE => x"00",                 -- 8-bit input: B port write enable/Write enable
         -- Port B Data: 32-bit (each) input: Port B data
          DIBDI => x"00000000",                 -- 32-bit input: B port data/MSB data
         DIPBDIP => "0000"              -- 4-bit input: B port parity/MSB parity
   );





   COMPARTIDA : RAMB36E1
   generic map (
      RDADDR_COLLISION_HWCONFIG => "PERFORMANCE",       -- Address Collision Mode: "PERFORMANCE" or "DELAYED_WRITE"       
      SIM_COLLISION_CHECK => "NONE",                    -- Collision check: Values ("ALL", "WARNING_ONLY", "GENERATE_X_ONLY" or "NONE")   
      DOA_REG => 0,                                     -- DOA_REG, DOB_REG: Optional output register (0 or 1)
      DOB_REG => 0,
      EN_ECC_READ => FALSE,                             -- Enable ECC decoder,   -- FALSE, TRUE
      EN_ECC_WRITE => FALSE,                            -- Enable ECC encoder,   -- FALSE, TRUE     
      INIT_A => X"000000000",                           -- INIT_A, INIT_B: Initial values on output ports
      INIT_B => X"000000000",      
      INIT_FILE => "NONE",                             -- Initialization File: RAM initialization file      
      RAM_MODE => "TDP",                                -- RAM Mode: "SDP" or "TDP"       
      RAM_EXTENSION_A => "NONE",                        -- RAM_EXTENSION_A, RAM_EXTENSION_B: Selects cascade mode ("UPPER", "LOWER", or "NONE")
      RAM_EXTENSION_B => "NONE",    
      READ_WIDTH_A => 36,                               -- 0-72     -- READ_WIDTH_A/B, WRITE_WIDTH_A/B: Read/write width per port
      READ_WIDTH_B => 36,                               -- 0-36
      WRITE_WIDTH_A => 36,                              -- 0-36
      WRITE_WIDTH_B => 36,                              -- 0-72     
      RSTREG_PRIORITY_A => "RSTREG",                    -- RSTREG_PRIORITY_A, RSTREG_PRIORITY_B: Reset or enable priority ("RSTREG" or "REGCE")
      RSTREG_PRIORITY_B => "RSTREG",     
      SRVAL_A => X"000000000",                          -- SRVAL_A, SRVAL_B: Set/reset value for output
      SRVAL_B => X"000000000",      
      SIM_DEVICE => "7SERIES",                          -- Simulation Device: Must be set to "7SERIES" for simulation behavior
      WRITE_MODE_A => "WRITE_FIRST",                    -- WriteMode: Value on output upon a write ("WRITE_FIRST", "READ_FIRST", or "NO_CHANGE")
      WRITE_MODE_B => "WRITE_FIRST" 
   )
   port map (
          CASCADEOUTA => open,     -- 1-bit output: A port cascade          -- Cascade Signals: 1-bit (each) output: BRAM cascade ports (to create 64kx1)
          CASCADEOUTB => open,     -- 1-bit output: B port cascade        
          DBITERR => open,         -- 1-bit output: Double bit error status     -- ECC Signals: 1-bit (each) output: Error Correction Circuitry ports
          ECCPARITY => open,       -- 8-bit output: Generated error correction parity
          RDADDRECC => open,       -- 9-bit output: ECC read address
          SBITERR => open,         -- 1-bit output: Single bit error status
      -- Port A Data: 32-bit (each) output: Port A data
      DOADO => dataOut_23,                 -- 32-bit output: A port data/LSB data
      DOPADOP => open,             -- 4-bit output: A port parity/LSB parity
      -- Port B Data: 32-bit (each) output: Port B data
      DOBDO => open,                 -- 32-bit output: B port data/MSB data
      DOPBDOP => open,             -- 4-bit output: B port parity/MSB parity
          -- Cascade Signals: 1-bit (each) input: BRAM cascade ports (to create 64kx1)
          CASCADEINA => '0',       -- 1-bit input: A port cascade
          CASCADEINB => '0',       -- 1-bit input: B port cascade
          -- ECC Signals: 1-bit (each) input: Error Correction Circuitry ports
          INJECTDBITERR => '0', -- 1-bit input: Inject a double bit error
          INJECTSBITERR => '0', -- 1-bit input: Inject a single bit error
            -- Port A Address/Control Signals: 16-bit (each) input: Port A address and control signals (read port
            -- when RAM_MODE="SDP")
      ADDRARDADDR => sixteenBaddress,     -- 16-bit input: A port address/Read address
         CLKARDCLK => clk,         -- 1-bit input: A port clock/Read clock
         ENARDEN => '1',             -- 1-bit input: A port enable/Read enable
         REGCEAREGCE => '0',     -- OJO  1-bit input: A port register enable/Register enable
         RSTRAMARSTRAM => '0', -- 1-bit input: A port set/reset
         RSTREGARSTREG => '0', -- 1-bit input: A port register set/reset
      WEA => "0000",                     -- 4-bit input: A port write enable
      -- Port A Data: 32-bit (each) input: Port A data
      DIADI => x"00000000",                 -- 32-bit input: A port data/LSB data
           DIPADIP => "0000",             -- 4-bit input: A port parity/LSB parity
      -- Port B Address/Control Signals: 16-bit (each) input: Port B address and control signals (write port
      -- when RAM_MODE="SDP")
      ADDRBWRADDR => writeAddress,     -- 16-bit input: B port address/Write address
      CLKBWRCLK => CLK,         -- 1-bit input: B port clock/Write clock
      ENBWREN => incomingW,             -- 1-bit input: B port enable/Write enable
         REGCEB => '0',               -- 1-bit input: B port register enable
          RSTRAMB => '0',             -- 1-bit input: B port set/reset
         RSTREGB => '0',             -- 1-bit input: B port register set/reset
      WEBWE => escr23,                 -- 8-bit input: B port write enable/Write enable
         -- Port B Data: 32-bit (each) input: Port B data
      DIBDI => extDataIn,                 -- 32-bit input: B port data/MSB data
      DIPBDIP => "0000"              -- 4-bit input: B port parity/MSB parity
   );

end Behavioral;

