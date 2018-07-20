/*
Contains definitions for driver and board related objects, including programmed IO register bits offsets and
shadow registers structures, driver I/O control definitions etc. Structures defined in this file should be
declared in the DLL, not the user program. Making all these structures directly accessable unecessarly 
complicates the software.
*/

#ifndef AD83000x_DEFINES_H
#define AD83000x_DEFINES_H


//Channels
/** @brief Defines __Channels__ values for AD16-250 */
#define IN0		1
/** @brief Defines __Channels__ values for AD16-250 */
#define IN1		2
/** @brief Defines __Channels__ values for AD16-250 */
#define IN2		4
/** @brief Defines __Channels__ values for AD16-250 */
#define IN3		8

// TriggerTypes
#define NO_TRIGGER					0
#define WAVEFORM_TRIGGER			1
#define SYNC_SELECTIVE_RECORDING	2
#define HETERODYNE					3
#define TTL_TRIGGER_EDGE			4

// Slope
#define FALLING_EDGE	0
#define RISING_EDGE		1

#ifndef _WINDOWS

    #define MAX_UVDMA                    8        // number of uvdma boards this driver supports
    #define UVDMA_CSR_INDEX              0x80     // offset in config space to OUR control register 
    #define PCI_VENDOR_ID_ULTRAVIEW      0xfebd   // Ultraview vendor ID
    #define PCI_DEVICE_ID_ULTRA_ADDA     0x0065   // AD8-3000 Device ID

struct uvdma_reg_values
{
    // 123010, changed to unsigned int from u_int32_t
    unsigned int changed_bits;        // requested bits to be changed
    unsigned int new_bit_value;       // requested new bit values
    unsigned int data;                // used to transfer various data into/out of the driver
    unsigned int pio_offset;          // offset for user PIO access using address multiplexers to reduce true PIO size
    unsigned int spiAddress;
    unsigned int mask_info;
};


    // Linux device driver IO control definitions
    #define UVDMA_IOC_MAGIC 'L'

#define IOCTL_GET_NUM_DEVICES                _IOWR(UVDMA_IOC_MAGIC, 0x01, struct uvdma_reg_values)
#define IOCTL_WRITE_CONFIG_SPACE             _IOWR(UVDMA_IOC_MAGIC, 0x02, struct uvdma_reg_values)
#define IOCTL_SET_ARMED                      _IOWR(UVDMA_IOC_MAGIC, 0x03, struct uvdma_reg_values)
#define IOCTL_SET_USER_PIO_REG               _IOWR(UVDMA_IOC_MAGIC, 0x04, struct uvdma_reg_values)
#define IOCTL_GET_USER_PIO_REG               _IOWR(UVDMA_IOC_MAGIC, 0x05, struct uvdma_reg_values)
#define IOCTL_SET_PREVENT_UNDEROVER_RUNS     _IOWR(UVDMA_IOC_MAGIC, 0x06, struct uvdma_reg_values)
#define IOCTL_GET_BLOCKS_SYNTHESIZED         _IOWR(UVDMA_IOC_MAGIC, 0x07, struct uvdma_reg_values)
#define IOCTL_SET_NUM_BLOCKS_PER_BOARD       _IOWR(UVDMA_IOC_MAGIC, 0x08, struct uvdma_reg_values)
#define IOCTL_GET_OVERRUNS                   _IOWR(UVDMA_IOC_MAGIC, 0x09, struct uvdma_reg_values)
#define IOCTL_GET_STATUS                     _IOWR(UVDMA_IOC_MAGIC, 0x0A, struct uvdma_reg_values)
#define IOCTL_SET_SHADOW_REG				 _IOWR(UVDMA_IOC_MAGIC, 0x0B, struct uvdma_reg_values)		// For Driver version > 1.0.0; Added to keep shadow registers of all SET_USER_PIO_REG; AL 071213
#define IOCTL_GET_SHADOW_REG				 _IOWR(UVDMA_IOC_MAGIC, 0x0C, struct uvdma_reg_values)		// For Driver version > 1.0.0; Added to keep shadow registers of all SET_USER_PIO_REG; AL 071213

    // Linux Interrupt Masks
    #define AD_DMA_INTERRUPT    0x40000000
    #define AD_INTERRUPT        0x20000000
    #define ALL_AD_INTERRUPT    0x10000000
    #define DA_DMA_INTERRUPT	0x08000000
    #define DA_INTERRUPT        0x04000000
    #define ALL_DA_INTERRUPT    0x02000000
    #define ERROR_INTERRUPT     0x00800000

    // If another interrput is added make sure it is added to IS_OUR_INTERRUPT
    #define IS_OUR_INTERRUPT (AD_DMA_INTERRUPT | AD_INTERRUPT | ALL_AD_INTERRUPT | DA_DMA_INTERRUPT | DA_INTERRUPT | ALL_DA_INTERRUPT | ERROR_INTERRUPT)

#else

    // Windows device driver IO control definitions
    #define IOCTL_WRITE_CONFIG_SPACE            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_GET_VERSION		            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)		// Driver version > 1.0.0 support shadow registers; AL 071212
    #define IOCTL_SET_ARMED                     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
    #define IOCTL_SET_USER_PIO_REG              CTL_CODE(FILE_DEVICE_UNKNOWN, 0x820, METHOD_BUFFERED, FILE_ANY_ACCESS)
    #define IOCTL_GET_USER_PIO_REG              CTL_CODE(FILE_DEVICE_UNKNOWN, 0x830, METHOD_BUFFERED, FILE_ANY_ACCESS)
    #define IOCTL_SET_PREVENT_UNDEROVERRUNS     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x840, METHOD_BUFFERED, FILE_ANY_ACCESS)
    #define IOCTL_GET_BLOCKS_SYNTHESIZED        CTL_CODE(FILE_DEVICE_UNKNOWN, 0x850, METHOD_BUFFERED, FILE_ANY_ACCESS)
    #define IOCTL_SET_NUM_BLOCKS_PER_BOARD      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x860, METHOD_BUFFERED, FILE_ANY_ACCESS)
    #define IOCTL_GET_OVERRUNS                  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x870, METHOD_BUFFERED, FILE_ANY_ACCESS)
    #define IOCTL_GET_STATUS                    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x880, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_SET_SHADOW_REG				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x890, METHOD_BUFFERED, FILE_ANY_ACCESS)		// For Driver version > 1.0.0; Added to keep shadow registers of all SET_USER_PIO_REG; AL 071213
	#define IOCTL_GET_SHADOW_REG				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8A0, METHOD_BUFFERED, FILE_ANY_ACCESS)		// For Driver version > 1.0.0; Added to keep shadow registers of all SET_USER_PIO_REG; AL 071213

    // Windows Interrupt Masks
    #define  ALL_A2D_COMPLETE        0x10000000
    #define  A2D_D2A_COMPLETE        0x20000000
    #define  DMA_COMPLETE            0x40000000

	// Driver struct	// Driver version >= 1.0.0 support shadow registers; AL 071212
	typedef struct _DRV_VERSION {
		unsigned major;
		unsigned minor;
		unsigned internal;
	} DRV_VERSION, *PDRV_VERSION;

#endif







// The offsets in the actual memory-map
#define PIO_DMA_CONTROL    0	// read/write	
#define PIO_DMA_AD_LOW     1	// write only, cannot read from PIO_DMA_AD_LOW, instead user PIO read data is read
#define PIO_DMA_DA_LOW     2	// write only
#define PIO_DMA_AD_HIGH    3	// write only
#define PIO_DMA_DA_HIGH    4	// write only
#define PIO_WR_ADR         5	// write only	
#define PIO_WR_DATA        6	// write only	
#define PIO_RD_ADR         7	// write only	
#define PIO_RD_DATA        1	// read only, cannot write PIO_RD_DATA, this would write to PIO_DMA_AD_LOW
#define	NUM_PIO_REGS       8


// Offsets in the user PIO space of the PIO registers. These are the addresses to set the address multiplexers to:
#define	PIO_OFFSET_CONTROL                0x00
#define	PIO_OFFSET_BLOCKSIZE              0x01
#define	PIO_OFFSET_NUMBLOCKS              0x02
#define	PIO_OFFSET_PCIE_RD_START_BLOCK    0x03
#define	PIO_OFFSET_ADCSERIAL              0x04
#define	PIO_OFFSET_PROMPROG               0x05
#define	PIO_OFFSET_CAPTUREADJ             0x06
#define PIO_OFFSET_DACSERIAL              0x07
#define PIO_OFFSET_ADC_DAC_SERIAL         0x08
#define PIO_OFFSET_DAC_RD_START_BLOCK     0x09
#define PIO_OFFSET_PCIE_RD_END_BLOCK      0x0A
#define PIO_OFFSET_DAC_RD_END_BLOCK       0x0B
#define PIO_OFFSET_RS_SERIAL_WR           0x0C // 123109
#define PIO_OFFSET_RS_SERIAL_RD           0x07 // 123109
#define PIO_OFFSET_ADC_CHECKSUM           0x08 // 012610
#define PIO_OFFSET_ADC_UNDEROVER          0x09 // 012610
#define PIO_OFFSET_DRAM_ADJUST            0x0D // 122810
#define PIO_OFFSET_DAC_WR_START_BLOCK     0x0E // 122910
#define PIO_OFFSET_DAC_WR_END_BLOCK       0x0F // 122910
#define PIO_OFFSET_ADC_DATA_SETUP         0x10 // 122910
#define PIO_OFFSET_PERSISTENT_STORAGE     0x12 // 122910
#define PIO_OFFSET_AVERAGER_CYCLES        0x13 // 122910
#define PIO_OFFSET_TRIGGER_THRESHOLD      0x14 // 122910
#define PIO_OFFSET_PRE_TRIGGER            0x15 // 122910
#define PIO_OFFSET_ISLA_CAPTURE           0x16 // 122910
#define PIO_OFFSET_AVERAGER_LEN			  0x17 // 110912
#define PIO_OFFSET_MOTOR_0				  0x18 // 110912
#define PIO_OFFSET_MOTOR_1				  0x19 // 110912
#define PIO_OFFSET_ECL_TRIGGER_DELAY	  0x1A // 060313
#define PIO_OFFSET_12BIT_GLOBAL_IDELAY	  0x1B // 062313
#define	PIO_OFFSET_CAPTURE_DEPTH		  0x1C // 062513
#define	PIO_OFFSET_CAPTURE_COUNT		  0x1D // 062513
#define PIO_OFFSET_SYNTH_REG			  0x1E // 062613
#define PIO_OFFSET_FIRMWARE_VERSION_RD	  0x1F // 060513






// Mask info 
#define SPI_DEVICE_BIT_POS		20
    #define AD16				0	// used only for shadow registers

    #define SYNTHA			0	// used only for shadow registers
	#define SYNTHB			1	// used only for shadow registers
	#define IO_EXP			2	// used only for shadow registers
	#define AD12			3	// used only for shadow registers
#define DATA_WORD_BIT_POS		16
	#define WORD_0	0
	#define WORD_1	1
#define DATA_LENGTH_BIT_POS		0
	#define SYNTH_DATA_LENGTH	0xffff
	#define IO_EXP_DATA_LENGTH	0xff
	#define AD12_DATA_LENGTH	0xffff
    #define AD16_DATA_LENGTH	0xff

#define AD_IO_SYNTH_bit	24		// Bits 24 and 25 determine AD12, IO_EXP, or SYNTH SPI communication
								// "00" = AD12, "01" = IO_EXP, "10" = SYNTH


//PIO_OFFSET_AVERAGER_CYCLES bit positions
#define AVERAGER_CYCLES     0

//PIO_OFFSET_TRIGGER_THRESHOLD
#define THRESH_A 0
#define THRESH_B 16

//PIO_OFFSET_PRE_TRIGGER bit positions
#define PRE_TRIGGER          0
#define TRIGGER_MODE	    11
//#define TRIGGER_SLOPE       13		//Unused now that TTL Invert uses this
#define ANALOG_DIGITAL_BIT  13







// DMA Control Register Offsets (This register is not accessable at the user level)
#define CLEAR_AD_DMA_INT    31
#define CLEAR_DA_DMA_INT    30
#define CLEAR_AD_INT        29 
#define CLEAR_ALL_AD_INT    28 
#define CLEAR_DA_INT        27
#define CLEAR_ALL_DA_INT    26
#define	AWAITING_AD_INT     25 
#define AWAITING_DA_INT     24
#define CLEAR_ERROR_INT     23
#define INTERRUPT_ENABLE    22

// If another interrput clear is added make sure it is added to CLEAR_ALL_INTS
#define CLEAR_ALL_INTS	((1<<CLEAR_AD_DMA_INT) | (1<<CLEAR_DA_DMA_INT) | (1<<CLEAR_AD_INT) | (1<<CLEAR_ALL_AD_INT) | (1<<CLEAR_DA_INT) | (1<<CLEAR_ALL_DA_INT) | (1<<CLEAR_ERROR_INT))



// Control Register bit offsets
#define SETUP_DONE			  31
#define DAC_GO                29
#define DAC_RESET             28

#define SOFTWARE_RUN_BIT      27
#define	SOFTWARE_STOP         26
#define	INTERLEAVED_RW        25
#define READ_WHOLE_ROW        24

#define	LED_FUNCTION1         21
#define	LED_FUNCTION0         20
#define	INT_CLOCK_EN          19
#define WRITE_WHOLE_ROW       18



// DRAM Adjustment Register Offsets
#define DRAM_RD_TIME          8       // bits 10,9,8

#define DRAM_IDELAY_DQ_INC    6
#define DRAM_IDELAY_DQ_CE     5
#define DRAM_IDELAY_DQ_RST    4

#define DRAM_IDELAY_DQS_INC   2
#define DRAM_IDELAY_DQS_CE    1
#define DRAM_IDELAY_DQS_RST   0



// Capture Adjustment Register Offsets
#define ADC_RST_P             31
#define ADC_RST_N             30
#define ADC_HOLD_TRIGGER      29
#define DCM_PS_POL            28    // 1 Bit, the DCM phase shift polarity     120809

#define DCM_PS_VAL            20    // 7 Bits, the DCM phase shift value   120809

#define IDELAY_RST            19
#define IDELAY_CE             18
#define IDELAY_INC            17
#define IDELAY_CTRL_RST       16

#define ACQUIRE_DISABLE_EDGE  15
#define ACQUIRE_DISABLE_RESET 14
#define ACQUIRE_DISABLE_INV   13	// 100909 trigger polarity select
#define CONTINUOUS_ARF        12

#define ADC_DEBUG_SYNC        10 
#define SINGLE_CHAN_SELECT    9
#define MEZZ_OSC_POWER_DOWN   8
#define ADC_CLK_DIV           4 // bits 6,5,4 are 0->no decimation 1->div2 2->div4 3->div8 4->div16 others are illegal
#define DUAL_ADC_SNGL_CHAN    2
#define DIGITAL_SHIFT         0



// ADC Serial Register Offsets
#define ADC_DCM_RESET        28
#define DISABLE_ADC2_WR      25
#define DISABLE_ADC3_WR      24

#define ADC_CAL              21
#define ADC_SERIAL_WE        20
// Bits 19:16 are 8-bit ADC serial address, Bits 15:0 are 8-bit ADC serial data



// ADC Data Setup Register Offsets
#define AD12_ADC_ECEBAR		   18
#define AD12_ADC_NDM		   17
#define ADC_USER_OFFSET        13       // 4 bits 16:13
#define ADC_DECI_OFFSET         9       // 4 bits 12:9
#define NUM_ADC_CHAN_OFFSET     5       // 4 bits 8:5
#define ADC_RESOLUTION_OFFSET   0       // 5 bits 4:0








// ADC blocksize is fixed at 32MB (this is not the DMA blocksize)
#define ADC_BLOCKSIZE           (1024*1024)	

#ifndef DIG_BLOCKSIZE
#define DIG_BLOCKSIZE           (1024*1024)	// bytes per DMA block default=(1024*1024). If this is change rebuild Linux archive in /lib !!!
#endif

#define MAX_DEVICES             5
#define MAX_CHAN                8

#define CHECKMEM_NUM_BLOCKS     64	        // 64   - 101510 was MIN_BLOCKS_TO_ACQUIRE
#define RUN_ADC_DCM_PS_CAL      false       // false
#define ADC_DCM_PS_ITER         20          // 20   50
#define MAX_OFFSET_RATIO        1.50        // 1.50




// Enable print statements that point out work to be done
#define PRINT_TODO  false

// RevC 14-bit mezzanines use ADC DAC's reference, should be backwards compatible with pre RevC mezzanines
#define SEL_ADC_DAC_REF 0x0001	// else would be 0x0000

// When true driver will not return read requests unless board has acquired enough data to ensure the data is new
// When false driver will immediately ask board for data regardless of how much data has been acquired
#define PREVENT_UNDER_OVER_RUNS true    // true

// When true firmware will hold 8-bit ADCs unreset after trigger event 
// When false a falling edge on exernal sync/trig input will reset the ADCs
#define SEL_ADC_HOLD_TRIGGER    true    // true

// Clear INTERLEAVE_RW if you want device to finish the entire acquire before initiating reverse reads.             
#define SEL_INTERLEAVED_RW      true    // true		

// During reverse reads, read 16KB (DRAM row) at a time increases chances of overruns ('0' -> 4KB reads)
#define SEL_READ_WHOLE_ROW      false    // true

// During PCIe D/A we read a PAGE of data across the bus at a time. These FIFOs
// are not as deep as the A/D FIFOs and do not store a whole DRAM of data (16KB). 
// true -> increase DRAM bandwidth for A/D
// false -> required for PCIe D/A operations !!! Fix This !!!
#define SEL_WRITE_WHOLE_ROW     false	// true requires: sel_interleaved_rw = false	

// Enable/Disable excessive readback in the DAC serial setup. Used for debugging.
#define EX_DAC_SERIAL_READBACK  false    // false   





#define DAC_RZ_MODE     0x00    // 0x00 -> Normal Mode, 0x01 -> Double RZ Mode
// DAC Full Scale Current [9:0], 0x000 -> 10mA, 0x200 -> 20mA, 0x3FF -> 30mA
#define DAC_FSC1        0x00    // FSC[7:0]
#define DAC_FSC2        0x02    // 0x02     Bits 1:0 are FSC[9:8], other bits should be zero

// Specifies if the DACs should be run in Master/Slave mode. DAC0 is the Master. Only applicable if two DACs are currently being used.
#define DA_MASTER_SLAVE     true    // true
#define MASTER_DAC_INVERT   true    // 100510 On original DAC mezzanine DAC0 was the master, on PCIeDAQ_ADC08_DA14_Mezz DAC2 is master, DA1 is slave




	


// Strobe delay values for capturing 12/14-bit data
// 27 works for 270-400Mhz swept
#define IDELAY_HIGH_FREQ        370
#define IDELAY_HIGH             8      // 8 // 9  
#define IDELAY_LOW              1  


// 250MHz-> 4ns 4000ps/78ps = 51.3 IDELAY taps per clock cycle
// 1/4 cycle at 250MHz is ~12 taps
//#define DRAM_DQS_IDELAY_VAL     2       // 0 @ 300MHz
//#define DRAM_DQ_IDELAY_VAL      8		// 8 @ 300MHz, 8 @333


// 250MHz->4ns. 2ns per bit cell. 1ns delay should be good
// 400MHz->2.5ns. 1.25ns per bit cell. 612.5ns delay should be good
#define SEL_DRAM_RD_TIME 	        4    // (5 for CAS=4, CAS=6), (4 for CAS=5)
// 250MHz works with 3, after some change ??? Used to be 5!? 
// 333MHz works with 4, this must be due to fixed resync clock phase/fixed delays


// @ 333MHz DQS delay of 0-,3 are best, DQ delays of 6,7,8,9 are best
#define MIN_DQS_DELAY           0   // 0   // less than 2 has lots of errors, 5-9 are all about the same
#define MAX_DQS_DELAY           9   // 1   // 64 max,
#define MIN_DQ_DELAY            0   // 8   // higher than 18 has lots of errors, 7-14 seem to be the best
#define MAX_DQ_DELAY            15   // 9  // 64 max, 

#define NUM_DRAM_DQ             128





#define ADC_RESET_TIME          100     // 100     // 093010
#define ADC_CAL_WAIT            10     // 100
#define ADC_SERIAL_WAIT         5      // 10
#define DAC_SERIAL_RD_DELAY     10      // 10
#define DAC_SERIAL_WR_DELAY     10      // 10
#define DAC_DLL_LOCK_WAIT       100     // 100

// 14-bit Mezz Mappings
// VoutA- Ch1 Gain Adj.
// VoutC- Ch0 Gain Adj.
// VoutE- Ch1 Offset Adj.
// VoutG- Ch0 Offset Adj.
// VoutB- N/C
// VoutD- N/C
// VoutF- Ch0 Bias Trim
// VoutH- Ch1 Bias Trim

#define ADC_DAC_CH1GAIN 0
#define ADC_DAC_NC0     1
#define ADC_DAC_CH0GAIN 2
#define ADC_DAC_NC1     3
#define ADC_DAC_CH0OFF  4
#define ADC_DAC_CH0BIAS 5
#define ADC_DAC_CH1OFF  6
#define ADC_DAC_CH1BIAS 7
#define ADC_DAC_ALL     0xF


// Board dependent configuration data read from config file
typedef struct
{
    unsigned int serial_number;
    unsigned int config_file_found;
    unsigned short board_type;
    unsigned short board_rev;           // 120110 Allow software to read board revision (8-bit capture timing)
    unsigned int fpga_type;


    // Board characteristics (Always Required)
    unsigned int adc_res;               // Read from config file (ADC bit resolution)
    unsigned int dac_res;               // Read from config file (DAC bit resolution)
    unsigned int adc_chan;              // Read from config file (number of ADC channels)
    unsigned int dac_chan;              // Read from config file (number of DAC channels)

    unsigned int dram_dqs;              // Read from config file (number of ADC channels)
    unsigned int dram_dq;              // Read from config file (number of DAC channels)

    // Settings required for boards with National ADC08 series converters
    unsigned int is_adc08d1520;

    // Settings required for boards with National ADC08 series converters
    unsigned int is_adc12d2000;

    unsigned int ADC12D2000_desi_q_offset;
    unsigned int ADC12D2000_desi_q_offset_neg;
    unsigned int ADC12D2000_desq_i_offset;
    unsigned int ADC12D2000_desq_i_offset_neg;

    unsigned int ADC12D2000_desiq_q_offset;
    unsigned int ADC12D2000_desiq_q_offset_neg;
    unsigned int ADC12D2000_desiq_i_offset;
    unsigned int ADC12D2000_desiq_i_offset_neg;

    unsigned int ADC12D2000_desclkiq_q_offset;
    unsigned int ADC12D2000_desclkiq_q_offset_neg;
    unsigned int ADC12D2000_desclkiq_i_offset;
    unsigned int ADC12D2000_desclkiq_i_offset_neg;

    unsigned int ADC12D2000_desclkiq_q_fsr;
    unsigned int ADC12D2000_desclkiq_i_fsr;

    unsigned int ADC12D2000_CALIBRATION_VALUES;
    unsigned int ADC12D2000_BIAS_ADJUST;
    unsigned int ADC12D2000_DES_TIMING_ADJUST;
    unsigned int ADC12D2000_APERTURE_DELAY_COARSE;
    unsigned int ADC12D2000_APERTURE_DELAY_FINE;

    // Settings required to differentiate between AD14-400s with old firmware and upgraded triggering firmware
    unsigned int is_AD5474;

    //Settings required for boards with ISLA214P converters
    unsigned int is_ISLA214P;

    //Settings required for boards with ISLA216P converters
    unsigned int is_ISLA216P;

    // Board dependent converter calibration settings (for 8-bit boards)
    unsigned int adc2_off;              // Read from config file
    unsigned int adc2_off_Q;            // Read from config file

    unsigned int adc3_off;              // Read from config file
    unsigned int adc3_off_Q;            // Read from config file

    unsigned int adc2_fsr;              // Read from config file
    unsigned int adc2_fsr_Q;            // Read from config file
    unsigned int adc3_fsr;              // Read from config file
    unsigned int adc3_fsr_Q;            // Read from config file

    unsigned int adc2_off_neg;          // Read from config file
    unsigned int adc2_off_neg_Q;        // Read from config file

    unsigned int adc3_off_neg;          // Read from config file
    unsigned int adc3_off_neg_Q;        // Read from config file

    unsigned int clk500_phs;            // Read from config file
    unsigned int clk1000_phs;           // Read from config file
    unsigned int shift_adc2;            // Read from config file

    // Board dependent converter calibration settings (for non 8-bit boards)
    unsigned int adc_gain0;             // Read from config file
    unsigned int adc_gain1;             // Read from config file

    unsigned int adc_offs0;             // Read from config file
    unsigned int adc_offs1;             // Read from config file
    unsigned int adc_offs2;             // Read from config file
    unsigned int adc_offs3;             // Read from config file

    unsigned int adc_bias0;             // Read from config file
    unsigned int adc_bias1;             // Read from config file
    unsigned int adc_bias2;             // Read from config file
    unsigned int adc_bias3;             // Read from config file

    //ISLA216P 16-bit/ISLA214P 14-bit converter gain calibration settings
    unsigned int ISLA_ADC0_GAIN_COARSE0;
    unsigned int ISLA_ADC0_GAIN_MEDIUM0;
    unsigned int ISLA_ADC0_GAIN_FINE0;
    unsigned int ISLA_ADC0_OFFSET_FINE0;
    unsigned int ISLA_ADC0_OFFSET_COARSE0;

    unsigned int ISLA_ADC1_GAIN_COARSE0;
    unsigned int ISLA_ADC1_GAIN_MEDIUM0;
    unsigned int ISLA_ADC1_GAIN_FINE0;
    unsigned int ISLA_ADC1_OFFSET_FINE0;
    unsigned int ISLA_ADC1_OFFSET_COARSE0;

    unsigned int ISLA_ADC2_GAIN_COARSE0;
    unsigned int ISLA_ADC2_GAIN_MEDIUM0;
    unsigned int ISLA_ADC2_GAIN_FINE0;
    unsigned int ISLA_ADC2_OFFSET_FINE0;
    unsigned int ISLA_ADC2_OFFSET_COARSE0;

    unsigned int ISLA_ADC3_GAIN_COARSE0;
    unsigned int ISLA_ADC3_GAIN_MEDIUM0;
    unsigned int ISLA_ADC3_GAIN_FINE0;
    unsigned int ISLA_ADC3_OFFSET_FINE0;
    unsigned int ISLA_ADC3_OFFSET_COARSE0;

    unsigned int ISLA_ADC0_GAIN_COARSE1;
    unsigned int ISLA_ADC0_GAIN_MEDIUM1;
    unsigned int ISLA_ADC0_GAIN_FINE1;
    unsigned int ISLA_ADC0_OFFSET_FINE1;
    unsigned int ISLA_ADC0_OFFSET_COARSE1;

    unsigned int ISLA_ADC1_GAIN_COARSE1;
    unsigned int ISLA_ADC1_GAIN_MEDIUM1;
    unsigned int ISLA_ADC1_GAIN_FINE1;
    unsigned int ISLA_ADC1_OFFSET_FINE1;
    unsigned int ISLA_ADC1_OFFSET_COARSE1;

    unsigned int ISLA_ADC2_GAIN_COARSE1;
    unsigned int ISLA_ADC2_GAIN_MEDIUM1;
    unsigned int ISLA_ADC2_GAIN_FINE1;
    unsigned int ISLA_ADC2_OFFSET_FINE1;
    unsigned int ISLA_ADC2_OFFSET_COARSE1;

    unsigned int ISLA_ADC3_GAIN_COARSE1;
    unsigned int ISLA_ADC3_GAIN_MEDIUM1;
    unsigned int ISLA_ADC3_GAIN_FINE1;
    unsigned int ISLA_ADC3_OFFSET_FINE1;
    unsigned int ISLA_ADC3_OFFSET_COARSE1;

    unsigned int ISLA_ADC0_IDELAY_OFFSET;
    unsigned int ISLA_ADC1_IDELAY_OFFSET;
    unsigned int ISLA_ADC2_IDELAY_OFFSET;
    unsigned int ISLA_ADC3_IDELAY_OFFSET;

    unsigned int ISLA_2X_CLK;

    unsigned int ecl_trigger_delay;

    // Board dependent variables that are not directly read from config file
    unsigned int adc_chan_used;         // Passed in from setup structure
    unsigned long samples_per_block;    // Determined from adc_res, adc_chan
    unsigned long max_adc_value;        // Determined from adc_res
    unsigned long max_dac_value;        // Determined from dac_res

    unsigned short strobe_count_mult;   // Determined from adc_res
    int adc_error_value;                // Determined from adc_res, make sure this is a signed number
    bool continuous_arf;      // Determined from adc_res, adc_chan
    unsigned int pre_trigger_len;

	unsigned int MICROSYNTH;
	unsigned int MICROSYNTH_OSC_FREQ;

} config_struct;






// Board dependent calibration data used to calibrate boards for mid-scale offsets and check for glitches.
typedef struct
{
    int glitches[MAX_CHAN];
    int num_7F[MAX_CHAN];
    int num_80[MAX_CHAN];
    int num_less7F[MAX_CHAN];
    int num_more80[MAX_CHAN];
    int glitches_ps[MAX_CHAN][ADC_DCM_PS_ITER+1];

    unsigned short adc_offset_neg[MAX_CHAN];
    unsigned char adc_offset[MAX_CHAN];
    int inc_adc_offset[MAX_CHAN];
} cal_struct;





#endif














