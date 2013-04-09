/*
 * @file sd.h
 *
 * header for SD driver
 * documentation found at:
 * http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
 * brief explantion of commands, maybe.
 * http://alumni.cs.ucr.edu/~amitra/sdcard/Additional/sdcard_appnote_foust.pdf
 */

#ifndef STDDEF_H
#define STDDEF_H

/* base address */
// bus address
 unsigned int EMMC_BASE = 0x7E300000;
// Physical address
//unsigned int EMMC_BASE   = 0x20300000;

/* EMMC Address Map */
/* The following are registers of size 32 */
#define ARG2                 0x0           // ACMD23 Argument
#define BLKSIZECNT           0x4           // Block Size and count
#define ARG1                 0x8           // Argument                            ** pair
#define CMDTM                0xc           // Command and Transfer Mode
#define RESP0                0x10          // Response bits 31 : 0
#define RESP1                0x14          // Response bits 63 : 32
#define RESP2                0x18          // Response bits 95 : 64
#define RESP3                0X1C          // Response bits 127 : 96
#define DATA                 0x20          // Data                                ** pair
#define STATUS               0x24          // Status
#define CONTROL0             0x28          // Host Configuration bits
#define CONTROL1             0x2c          // Host configuration bits
#define INTERRUPT            0X30          // interupt flags                      ** pair
#define IRPT_MASK            0x34          // Interrupt Flag Enable
#define IRPT_EN              0x38          // Interrupt Generation Enable
#define CONTROL2             0x3c          // Host Configuration bits
#define FORCE_IRPT           0X50          // Force Interrupt Event
#define BOOT_TIMEOUT         0x70          // Timeout in boot mode
#define DBG_SEL              0x74          // Debug Bus Configuration
#define EXRDFIFO_CFG         0x80          // Extension FIFO Configuration        ** pair
#define EXRDFIFO_EN          0x84          // Extension FIFO Enable
#define TUNE_STEP            0x88          // Delay per card clock tuning step
#define TUNE_STEPS_STD       0x8c          // Card clock tuning steps for SDR     **pair
#define TUNE_STEPS_DDR       0x90          // Card clock tuning steps for DDR
#define SPI_INT_SPT          0xf0          // SPI Interrupt Support
#define SLOTISR_VER          0xfc          // Slot Interrupt Status and Version

/*
 * This register contains the argument for the SD card specific command
 * ACMD23 (SET_WR_BLK_ERASE_COUNT). ARG2 must be set before the ACMD23 command
 * is issued using the CMDTM register.
 *
 * i have absolutly no idea what that means
 */

// the folowing are all registers
//unsigned int argument             : 32 ;
//unsigned int resp0                : 32 ;      // bits 31:0 of the SD card response
//unsigned int resp1                : 32 ;      // bits 63:32 of the SD card response
//unsigned int resp2                : 32 ;      // bits 95:64 of the SD card response
//unsigned int resp3                : 32 ;      // bits 127:96 of the SD card response
//unsigned int data                 : 32 ;      // Data to/from the card
//unsigned int timeout              : 32 ;      // Number of card clock cycles after which
// timeout during boot mode is flagged

struct blksizecnt{
  unsigned int blkcnt             : 10 ;
  unsigned int zero               : 5  ;      // Always 0
  unsigned int blksize            : 17 ;
};

struct arg1 {
  unsigned int arg                : 32 ;
};

struct cmdtm {
  unsigned int z1                 : 1  ;      // write 0 don't care on read
  unsigned int tm_blkcnt_en       : 1  ;      // Enable the block counter for multiple block transfers
  unsigned int tm_auto_cmd_en     : 2  ;      // Select the command to be sent after completion of data transfer
  unsigned int tm_dat_dir         : 1  ;      // Direction of data transfer
  unsigned int tm_multi_block     : 1  ;      // Tpe of data transfer
  unsigned int z2                 : 10 ;      // write 0, don't care on read
  unsigned int cmd_rspns_type     : 2  ;      // Type of response from card
  unsigned int z3                 : 1  ;      // write 0, don't care on read
  unsigned int cmd_crcchk_en      : 1  ;      // Check the responses CRC
  unsigned int cmd_ixchk_en       : 1  ;      // Check that response has same index as command
  unsigned int cmd_isdata         : 1  ;      // Command involves data transfer
  unsigned int cmd_type           : 2  ;      // Type of command to be issued to card
  unsigned int cmd_index          : 6  ;      // Index of command to be issued to card
  unsigned int z4                 : 1  ;      // write 0 don't care on read
};

struct status {
  unsigned int cmd_inhibit        : 1  ;      // Command line still used by previous command
  unsigned int dat_inhibit        : 1  ;      // Data lines still used by previous data transfer
  unsigned int dat_active         : 1  ;      // At least one data line is active
  unsigned int z3                 : 5  ;      // Write as 0 don't care on read
  unsigned int write_transfer     : 1  ;      // New data can be written to EMMC: 0 no
  unsigned int read_transfer      : 1  ;      // New data can be read from EMMC: 0 no
  unsigned int z2                 : 10 ;      // Write 0 don't care on read
  unsigned int dat_level0         : 4  ;      // Value of data lines DAT3 to DAT0
  unsigned int cmd_level          : 1  ;      // Value of command line CMD
  unsigned int dat_level1         : 4  ;      // Value of data lines DAT7 to DAT4
  unsigned int z1                 : 3  ;      // write 0 don't care on read
};

struct control0 {
  unsigned int z4                 : 8  ;      // write 0 don't care on read
  unsigned int alt_boot_en        : 1  ;      // Enable alternate boot mode access
  unsigned int boot_en            : 1  ;      // Boot mode acess
  unsigned int spi_mode           : 1  ;      // SPI mode enable
  unsigned int gap_ien            : 1  ;      // enable sdio interrupt at block gap
  unsigned int readwait_en        : 1  ;      // Use DAT2 read-wait prot. for SDIO cards
  unsigned int gap_restart        : 1  ;      // Restart transtation stopped below
  unsigned int gap_stop           : 1  ;      // Stop the current transaction at the nex block gap
  unsigned int z3                 : 10 ;      // write as 0, don't care on read
  unsigned int hctl_8bit          : 1  ;      // Use 8 data lines
  unsigned int z2                 : 2  ;      // write as 0, don't care on read
  unsigned int hctl_hs_en         : 1  ;      // Select high speed mode
  unsigned int hctl_dwidth        : 1  ;      // Use data lines
  unsigned int z1                 : 1  ;      // write 0 don't care on read
};

struct contol1 {
  unsigned int z3                 : 5  ;      // write 0 don't care on read
  unsigned int srst_data          : 1  ;      // Resset the handling circuit
  unsigned int srst_cmd           : 1  ;      // Reset the data handling circuit
  unsigned int srst_hc            : 1  ;      // Reset the complete host circuit
  unsigned int z2                 : 4  ;      // write 0 don't care on read
  unsigned int data_tounit        : 4  ;      // Data timeout unit exponent x - TMCLK*2^(x+13)  1111 off
  unsigned int clk_freq8          : 8  ;      // SD clock base divider LSBs
  unsigned int clk_freq_ms2       : 2  ;      // SD clock divider MSB
  unsigned int clk_genself        : 5  ;      // Mode of clock generation
  unsigned int z1                 : 2  ;      // write 0 don't care on read
  unsigned int clk_en             : 1  ;      // SD clock enable
  unsigned int clk_stable         : 1  ;      // SD clock stable
  unsigned int clk_intlen         : 1  ;      // Clock enable for internal EMMC clock for power saving
};

struct interrupt {
  unsigned int cmd_done           : 1  ;      // Command has finished
  unsigned int data_done          : 1  ;      // Data transfer has finished
  unsigned int block_gap          : 1  ;      // Data transfer has stopped at block gap
  unsigned int z1                 : 1  ;      // write 0 don't care on read
  unsigned int write_rdy          : 1  ;      // Data can be written to data register
  unsigned int read_rdy           : 1  ;      // Data register contains data to be read
  unsigned int z2                 : 2  ;      // write 0 don't care on read
  unsigned int card               : 1  ;      // card made interrupt request
  unsigned int z3                 : 3  ;      // Clock retune request was made
  unsigned int retune             : 1  ;      // Clock retune request was made
  unsigned int bootack            : 1  ;      // Boot acknowledge has been received
  unsigned int endboot            : 1  ;      // Boot operation has terminated
  unsigned int cto_err            : 1  ;      // Timeout on command line
  unsigned int ccrc_err           : 1  ;      // Command crc error
  unsigned int cend_err           : 1  ;      // Bit on command line not 1
  unsigned int cbad_err           : 1  ;      // Incorrect command index in response
  unsigned int dto_err            : 1  ;      // Timeout on data line
  unsigned int dcrc_err           : 1  ;      // Data CRC error
  unsigned int dend_err           : 1  ;      // End bit on data line not 1
  unsigned int z4                 : 1  ;      // write 0 don't care on read
  unsigned int acmd_err           : 1  ;      // auto command error
  unsigned int z5                 : 7  ;      // write 0 don't care on read
};

struct irpt_mask {
  unsigned int  cmd_done          : 1  ;      // Set flag if command has finished
  unsigned int  data_done         : 1  ;      // Set flag data transger has finished
  unsigned int  block_gap         : 1  ;      // Set flag data transfer has stopped at block gap
  unsigned int  z1                : 1  ;      // write 0 dont' care on read
  unsigned int  write_rdy         : 1  ;      // Set flag if data can be written to data register
  unsigned int  read_rdy          : 1  ;      // Set flag if data register contains data to be read
  unsigned int  z2                : 2  ;      // write 0 don't care on read
  unsigned int  card              : 1  ;      // Set flag if card made interrupt request
  unsigned int  z3                : 2  ;      // write 0 don't care on read
  unsigned int  retune            : 1  ;      // Set flag if clock retune request was made
  unsigned int  bootack           : 1  ;      // Set flag if boot acknowledge has been recieved
  unsigned int  endboot           : 1  ;      // Set flag if boot operation has terminated
  unsigned int  z4                : 1  ;      // write 0 don't care on read
  unsigned int  cto_err           : 1  ;      // Set flag if timeout on command line
  unsigned int  ccrc_err          : 1  ;      // Set flag if command crc error
  unsigned int  cend_err          : 1  ;      // Set flag if end bit on command line not 1
  unsigned int  cbad_err          : 1  ;      // Set flag if incorrect command index in response
  unsigned int  dto_err           : 1  ;      // Set flag if timeout on data line
  unsigned int  dcrc_err          : 1  ;      // Set flag if data crc error
  unsigned int  dend_err          : 1  ;      // Set flag if end bit on data line not 1
  unsigned int  z5                : 1  ;      // write 0 don't care on read
  unsigned int  acmd_err          : 1  ;      // Set flag auto command erro
  unsigned int  z6                : 7  ;      // write 0 don't care on read
};

struct irpt_en {
  unsigned int cmd_done           : 1  ;      // Create interrupt if command has finished
  unsigned int data_done          : 1  ;      // Create interrupt if data transfer has finished
  unsigned int block_gap          : 1  ;      // Create interrupt if data transger has stopped at blcok gap
  unsigned int z1                 : 1  ;      // write 0 don't care on read
  unsigned int write_rdy          : 1  ;      // Create interrupt if data can be written to data register
  unsigned int read_rdy           : 1  ;      // Create interrupt if data register contains data to be read
  unsigned int z2                 : 2  ;      // write 0 don't care on read
  unsigned int card               : 1  ;      // Create interrupt if card made interrupt request
  unsigned int z3                 : 3  ;      // write 0 don't care on read
  unsigned int retune             : 1  ;      // Create interrupt if clock retune request was made
  unsigned int bootack            : 1  ;      // Create interrupt if boot acknowledge has been received
  unsigned int endboot            : 1  ;      // Create interrupt if boot operation has terminated
  unsigned int z5                 : 1  ;      // write 0 don't care on read
  unsigned int cto_err            : 1  ;      // Create interrupt if timeout on command line
  unsigned int ccrc_err           : 1  ;      // Create interrupt if command crc error
  unsigned int cend_err           : 1  ;      // Create interrupt if end bit on command line 1
  unsigned int cbad_err           : 1  ;      // Create interrupt if incorrect command index in response
  unsigned int dto_err            : 1  ;      // Create interrupt if timeout on data line
  unsigned int dcrc_err           : 1  ;      // Create interrupt if end bit on data line not 1
  unsigned int dend_err           : 1  ;      // Create interrupt if end bit on data line not 1
  unsigned int z6                 : 1  ;      // write 0 don't care on read
  unsigned int acmd_err           : 1  ;      // Create interrupt if auto command error
  unsigned int z7                 : 7  ;      // write 0 don't care on read
};

struct control2 {
  unsigned int acnox_err          : 1  ;      // Auto command not executed due to an error
  unsigned int acto_err           : 1  ;      // Timeout ocurred during auto command execution
  unsigned int accrc_err          : 1  ;      // Command crc error ocurred during auto command execution
  unsigned int acend_err          : 1  ;      // End bit is not 1 during auto command execution
  unsigned int acdab_err          : 1  ;      // Command index error occurred during auto command execution
  unsigned int z1                 : 2  ;      // write 0 don't care on read
  unsigned int notc12_err         : 1  ;      // Error occurred during auto command CMD12 execution
  unsigned int z2                 : 8  ;      // write 0 don't care on read
  unsigned int uhsmode            : 3  ;      // Select speed mode
  unsigned int z3                 : 2  ;      // write 0 don't care on read
  unsigned int tuneon             : 1  ;      // Start tuning the SD clock
  unsigned int tuned              : 1  ;      // Tuned clock is used for sampling data
  unsigned int z4                 : 8  ;      // write 0 don't care on read
};

struct force_irpt {
  unsigned int cmd_done           : 1  ;      // Create command has finished
  unsigned int data_done          : 1  ;      // Create data transfer has finished
  unsigned int block_gap          : 1  ;      // Create interrupt if data transfer has stopped at blcok gap
  unsigned int z1                 : 1  ;      // write 0 don't care on read
  unsigned int write_rdy          : 1  ;      // Create data can be written to data register
  unsigned int read_rdy           : 1  ;      // Create data register contains data to be read
  unsigned int z2                 : 2  ;      // write 0 and don't care on read
  unsigned int card               : 1  ;      // Create card made interrupt request
  unsigned int z3                 : 3  ;      // write 0 don't care on read
  unsigned int retune             : 1  ;      // Create clock retune request was made
  unsigned int bootack            : 1  ;      // Creat boot acknowledge has been recieved
  unsigned int endboot            : 1  ;      // Create boot operation has terminated
  unsigned int z4                 : 1  ;      // write 0 don't care on read
  unsigned int cto_err            : 1  ;      // create timeout on command line
  unsigned int ccrc_err           : 1  ;      // Create command crc error
  unsigned int cend_err           : 1  ;      // Create end bit on command line not 1
  unsigned int cbad_err           : 1  ;      // Create incorrect command index in response
  unsigned int dto_err            : 1  ;      // Create timeout on data line
  unsigned int dcrc_err           : 1  ;      // Create data crc error
  unsigned int dend_err           : 1  ;      // Create end bit on data line not 1
  unsigned int z5                 : 1  ;      // write 0 don't care on read
  unsigned int acmd_err           : 1  ;      // Create auto command error
  unsigned int z6                 : 8  ;      // write 0 don't care on read
};

struct dbg_sel {
  unsigned int select             : 1  ;      // Submodules accessed by debug bus
  unsigned int z1                 : 31 ;      // write 0 don't care on read
};

struct exrdfifo_cfg {
  unsigned int rd_thrsh           : 3  ;      // Read threshold in 32 bits words
  unsigned int z1                 : 29 ;      // write 0 don't care on read
};

struct exrdfifo_en {
  unsigned int enable             : 1  ;      // Enable the extension FIFO
  unsigned int z1                 : 31 ;      // write 0 don't care on read
};

struct tune_step {
  unsigned int delay              : 3  ;      // Sampling clock delay per step
  unsigned int z1                 : 29 ;      // write 0 don't care on read
};

struct tune_steps_std {
  unsigned int steps              : 6  ;      // Number of steps (0 to 40)
  unsigned int z1                 : 26 ;      // write 0 don't care on read
};

struct tune_steps_ddr {
  unsigned int steps              : 6  ;      // Interrupt independent of card select line
  unsigned int z1                 : 26 ;      // write 0 don't care on read
};

struct spi_int_spt {
  unsigned int select             : 8  ;      // interrupt independent of car select line
  unsigned int z1                 : 24 ;      // write 0 don't care on read
};

struct slotisr_ver {
  unsigned int slot_status        : 8  ;      // slot_status
  unsigned int z1                 : 8  ;      // write 0 don't care on read
  unsigned int sdversion          : 8  ;      // host controller specification version
  unsigned int vendor             : 8  ;      // vendor version number
};

#endif
