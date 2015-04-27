#include "define.h"
#include "extern_variable.h"
#include "extern_prototype.h"

// 이 함수는 H/W 변동이 없는 한 수정 없음
void cpu_setup(void)
{
	pll_setup();
	Peripheral_Clock_setup();
	interrupt_setup();
	
	// copy frash to ram
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    	
	// 반드시 ram에 올라가 있을 것
	InitFlash();
	
	//gpio 초기 설정
	GPIO_setup();
	
	// 외부버스 설정
	Xinterface_setup();
	
	// 타이머설정
	InitTimer();
	
	sci_setup();
	
	i2c_setup();
}

// 이 함수는 H/W 변동이 없는 한 수정 없음
void pll_setup(void)
{
	EALLOW;
	
	// disable watchdog
	*SysCtrlRegs_WDCR = 0x0068;
	
	// pll init.
	// Make sure the PLL is not running in limp mode
	if((*SysCtrlRegs_PLLSTS & 0x0008) != 0)
	{
		// Missing external clock has been detected
		// Replace this line with a call to an appropriate
		// SystemShutdown(); function.
		asm("        ESTOP0");
	}
	
	// DIVSEL MUST be 0 before PLLCR can be changed from
	// 0x0000. It is set to 0 by an external reset XRSn
	// This puts us in 1/4
	if((*SysCtrlRegs_PLLSTS & 0x0080) != 0)
	{
		*SysCtrlRegs_PLLSTS &= 0x007f;
	}
	
	// Change the PLLCR
	if((*SysCtrlRegs_PLLCR & 0x000f) != 10)
	{    	
		// Before setting PLLCR turn off missing clock detect logic    	
		*SysCtrlRegs_PLLSTS |= 0x0040;
		*SysCtrlRegs_PLLCR = 10;
		
		*SysCtrlRegs_WDCR = 0x0068;
		
		*SysCtrlRegs_PLLSTS &= 0xffbf;
	}
	
	// If switching to 2
	
	*SysCtrlRegs_PLLSTS |= 0x0100; //SysCtrlRegs.PLLSTS.bit.DIVSEL = divsel;
	*SysCtrlRegs_PLLSTS &= 0x017f; //SysCtrlRegs.PLLSTS.bit.DIVSEL = divsel;
	
	EDIS;
}

// 이 함수는 H/W 변동이 없는 한 수정 없음
void Peripheral_Clock_setup(void)
{
	EALLOW;
	
	// HISPCP/LOSPCP prescale register settings, normally it will be set to default values
	*SysCtrlRegs_HISPCP = 0x0001;
	*SysCtrlRegs_LOSPCP = 0x0002;
	
	// XCLKOUT to SYSCLKOUT ratio.  By default XCLKOUT = 1/4 SYSCLKOUT
	// XTIMCLK = SYSCLKOUT/2
	*XintfRegs_XINTCNF2_High = 0x0001;
	    
	// XCLKOUT = XTIMCLK/2
	*XintfRegs_XINTCNF2_Low |= 0x0004;
	
	// Enable XCLKOUT
	*XintfRegs_XINTCNF2_Low &= 0xfff7;
	
	*SysCtrlRegs_PCLKCR0 = 0x0f30;
	
	//SysCtrlRegs.PCLKCR0.bit.I2CAENCLK = 1;   // I2C
	//SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;   // SCI-A
	//SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 1;   // SCI-B
	//SysCtrlRegs.PCLKCR0.bit.SCICENCLK = 1;   // SCI-C
	//SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;   // SPI-A
	
	*SysCtrlRegs_PCLKCR1 = 0;
	
	// *SysCtrlRegs_PCLKCR3 = 0x3100;
	*SysCtrlRegs_PCLKCR3 = 0x3300;
	// *SysCtrlRegs_PCLKCR3 = 0x3700;
	
	//SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
	//SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1; // CPU Timer 1
	//SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 1; // CPU Timer 2    
	//SysCtrlRegs.PCLKCR3.bit.XINTFENCLK = 1;     // XTIMCLK
	//SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;    // GPIO input clock
	
	EDIS;
}

// 이 함수는 H/W 변동이 없는 한 수정 없음
void interrupt_setup(void)
{
	EALLOW;
	
	// interrupt vector init.
	//--- Disable interrupts
	DINT;			// Disable global interrupts
	

	//--- Initialize the PIE_RAM
	*PieCtrlRegs_PIECTRL = 0x0000;			// Disable the PIE	
	
	*PieCtrlRegs_PIEIER1 = 0x0000;
	*PieCtrlRegs_PIEIER2 = 0x0000;
	*PieCtrlRegs_PIEIER3 = 0x0000;
	*PieCtrlRegs_PIEIER4 = 0x0000;
	*PieCtrlRegs_PIEIER5 = 0x0000;
	*PieCtrlRegs_PIEIER6 = 0x0000;
	*PieCtrlRegs_PIEIER7 = 0x0000;
	*PieCtrlRegs_PIEIER8 = 0x0000;
	*PieCtrlRegs_PIEIER9 = 0x0000;
	*PieCtrlRegs_PIEIER10 = 0x0000;
	*PieCtrlRegs_PIEIER11 = 0x0000;
	*PieCtrlRegs_PIEIER12 = 0x0000;
	
	*PieCtrlRegs_PIEIFR1 = 0x0000;
	*PieCtrlRegs_PIEIFR2 = 0x0000;
	*PieCtrlRegs_PIEIFR3 = 0x0000;
	*PieCtrlRegs_PIEIFR4 = 0x0000;
	*PieCtrlRegs_PIEIFR5 = 0x0000;
	*PieCtrlRegs_PIEIFR6 = 0x0000;
	*PieCtrlRegs_PIEIFR7 = 0x0000;
	*PieCtrlRegs_PIEIFR8 = 0x0000;
	*PieCtrlRegs_PIEIFR9 = 0x0000;
	*PieCtrlRegs_PIEIFR10 = 0x0000;
	*PieCtrlRegs_PIEIFR11 = 0x0000;
	*PieCtrlRegs_PIEIFR12 = 0x0000;
	
//	*PieCtrlRegs_PIEACK = 0xffff;
	
	IER = 0x0000;
	IFR = 0x0000;
	
	//--- Enable the PIE
	*PieCtrlRegs_PIECTRL = 0x0001;			// Enable the PIE
	
	EDIS;
}

// 이 함수는 H/W 변동이 없는 한 수정 없음
void sci_setup(void)
{
	//unsigned int i;
	
	EALLOW;
	
	// GPIO22, GPIO23
	*GpioCtrlRegs_GPAMUX2_Low |= 0xf000;
	
	// GPIO62, GPIO63
	*GpioCtrlRegs_GPBMUX2_High |= 0x5000;
	
	*GpioCtrlRegs_GPAQSEL2_Low |= 0xc000;
	
	*GpioCtrlRegs_GPBQSEL2_High |= 0x3000;
	
	EDIS;
	
	// 전면 manager용
	// 최초 sci reset
	*ScibRegs_SCICTL1 = 0x0000;
	
	// baudrate set 38400
	// TMS320x2833x, 2823x Serial Communications Interface (SCI).pdf 22page
	*ScibRegs_SCIHBAUD = 0x00;
	*ScibRegs_SCILBAUD = 0x79;
	
	// 1 stop bit,  No loopback 
	// No parity,8 char bits,
	// async mode, idle-line protocol
	*ScibRegs_SCICCR = 0x0007;
	
	// tx, rx enable
	*ScibRegs_SCICTL1 = 0x0003;
	
	// rx interrupt enable
	*ScibRegs_SCICTL2 = 0x0002;
	
	// sci enable
	*ScibRegs_SCIFFTX = 0x8040;
	*ScibRegs_SCIFFRX = 0;
	
	// *ScibRegs_SCITXBUF = 0;
	
	// sci b enable
	*ScibRegs_SCICTL1 |= 0x0020;
	
	
	//himix
	// sci c
	// 최초 sci reset
	*ScicRegs_SCICTL1 = 0x0000;
	
	// baudrate set 9600
	// TMS320x2833x, 2823x Serial Communications Interface (SCI).pdf 22page
	*ScicRegs_SCIHBAUD = 0x01;
	*ScicRegs_SCILBAUD = 0xe7;
	
	// 1 stop bit,  No loopback 
	// No parity,8 char bits,
	// async mode, idle-line protocol
	*ScicRegs_SCICCR = 0x0007;
	
	// tx enable
	*ScicRegs_SCICTL1 = 0x0002;
	
	// rx interrupt enable
	// *ScicRegs_SCICTL2 = 0x0002;
	
	// sci enable
	*ScicRegs_SCIFFTX = 0x8040;
	*ScicRegs_SCIFFRX = 0;
	
	// *ScibRegs_SCITXBUF = 0;
	
	// sci c enable
	*ScicRegs_SCICTL1 |= 0x0020;
}

// 이 함수는 H/W 변동이 없는 한 수정 없음
void InitFlash(void)
{
   EALLOW;
   //Enable Flash Pipeline mode to improve performance
   //of code executed from Flash.
   //FlashRegs.FOPT.bit.ENPIPE = 1;
   *FlashRegs_FOPT = 1;

   //                CAUTION
   //Minimum waitstates required for the flash operating
   //at a given CPU rate must be characterized by TI.
   //Refer to the datasheet for the latest information.

   //Set the Paged Waitstate for the Flash
//   FlashRegs.FBANKWAIT.bit.PAGEWAIT = 5;
//
//   //Set the Random Waitstate for the Flash
//   FlashRegs.FBANKWAIT.bit.RANDWAIT = 5;
//
//   //Set the Waitstate for the OTP
//   FlashRegs.FOTPWAIT.bit.OTPWAIT = 8;
//
//   //                CAUTION
//   //ONLY THE DEFAULT VALUE FOR THESE 2 REGISTERS SHOULD BE USED
//   FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF;
//   FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF;
   
   *FlashRegs_FBANKWAIT = 0x0505;    
    *FlashRegs_FOTPWAIT = 0x0008;

    //                CAUTION
    //ONLY THE DEFAULT VALUE FOR THESE 2 REGISTERS SHOULD BE USED   
    *FlashRegs_FSTDBYWAIT = 0x01ff; // 요부분에 뭔가 있음    
    *FlashRegs_FACTIVEWAIT = 0x01FF;
   EDIS;

   //Force a pipeline flush to ensure that the write to
   //the last register configured occurs before returning.

   asm(" RPT #7 || NOP");
}

// 이 함수는 H/W 변동이 없는 한 수정 없음
void GPIO_setup(void)
{	
	EALLOW;					// Enable EALLOW protected register access
	
	// 각 pin 용도 설정
	// group A
//	*GpioCtrlRegs_GPAMUX1_Low = 0x0000;
//	*GpioCtrlRegs_GPAMUX1_High = 0x0000;
	
//	*GpioCtrlRegs_GPAMUX2_Low = 0x0000;
//	*GpioCtrlRegs_GPAMUX2_High = 0x0000;
	
	// group B
//	*GpioCtrlRegs_GPBMUX1_Low = 0x0035;
//	*GpioCtrlRegs_GPBMUX1_High = 0x0000;
	
//	*GpioCtrlRegs_GPBMUX2_Low = 0x0000;
//	*GpioCtrlRegs_GPBMUX2_High = 0x5000;
	
	// group C
//	*GpioCtrlRegs_GPCMUX1_Low = 0x0000;
//	*GpioCtrlRegs_GPCMUX1_High = 0x0000;
	
//	*GpioCtrlRegs_GPCMUX2_Low = 0x0000;
	
	
	
//	*GpioCtrlRegs_GPACTRL_Low = 0x0000;
//	*GpioCtrlRegs_GPACTRL_High = 0x0000;
	
//	*GpioCtrlRegs_GPBCTRL_Low = 0x0000;
//	*GpioCtrlRegs_GPBCTRL_High = 0x0000;	
	
//	*GpioCtrlRegs_GPAQSEL1_Low = 0x0000;
//	*GpioCtrlRegs_GPAQSEL1_High = 0x0000;
	
//	*GpioCtrlRegs_GPAQSEL2_Low = 0x0000;
//	*GpioCtrlRegs_GPAQSEL2_High = 0x0000;	
	
//	*GpioCtrlRegs_GPBQSEL1_Low = 0x000f; // i2c
//	*GpioCtrlRegs_GPBQSEL1_High = 0x0000;
	
//	*GpioCtrlRegs_GPBQSEL2_Low = 0x0000;
//	*GpioCtrlRegs_GPBQSEL2_High = 0x0000;
	
	
	// GPIO00 ~ GPIO15
	// 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	//  x  x  1  1  1  x  1  x  1  0  1  1  0  1  1  0
	*GpioCtrlRegs_GPADIR_Low = 0x3ab6;
	*GpioCtrlRegs_GPAPUD_Low = 0x3ab6;
	
	// GPIO16 ~ GPIO31
	// 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
	//  x  x  x  x  1  x  1  1  x  x  x  x  1  1  0  1
	*GpioCtrlRegs_GPADIR_High = 0x0b0d;
	*GpioCtrlRegs_GPAPUD_High = 0x0b0d;
	
	// GPIO32 ~ GPIO47
	// 47 46 45 44 43 42 41 40 39 38 37 36 35 34 33 32
	//  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x
	*GpioCtrlRegs_GPBDIR_Low = 0x0000;
	*GpioCtrlRegs_GPBPUD_Low = 0x0000;
	
	// GPIO48 ~ GPIO63
	// 63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48
	//  x  x  0  x  1  1  1  1  0  1  0  0  0  x  x  0
	*GpioCtrlRegs_GPBDIR_High = 0x0f40;
	*GpioCtrlRegs_GPBPUD_High = 0x0f40;
	
	// GPIO64 ~ GPIO79
	// 79 78 77 76 75 74 73 72 71 70 69 68 67 66 65 64
	//  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x
	*GpioCtrlRegs_GPCDIR_Low = 0x0000;
	*GpioCtrlRegs_GPCPUD_Low = 0x0000;
	
	// GPIO80 ~ GPIO87
	// 87 86 85 84 83 82 81 80
	//  x  x  x  x  x  x  x  x
	*GpioCtrlRegs_GPCDIR_High = 0x0000;
	*GpioCtrlRegs_GPCPUD_High = 0x0000;	
	

//--- External interrupt selection
	*GpioIntRegs_GPIOXINT1SEL = 0x0000;
	*GpioIntRegs_GPIOXINT2SEL = 0x0000;
	*GpioIntRegs_GPIOXINT3SEL = 16;
	*GpioIntRegs_GPIOXINT4SEL = 0x0000;
	*GpioIntRegs_GPIOXINT5SEL = 0x0000;
	*GpioIntRegs_GPIOXINT6SEL = 0x0000;
	*GpioIntRegs_GPIOXINT7SEL = 0x0000;
	
	*GpioIntRegs_GPIOXNMISEL = 0x0000;
	
	*XIntruptRegs_XINT1CR = 0x0000;
	*XIntruptRegs_XINT2CR = 0x0000;
	*XIntruptRegs_XINT3CR = 0x0001;
	*XIntruptRegs_XINT4CR = 0x0000;
	*XIntruptRegs_XINT5CR = 0x0000;
	*XIntruptRegs_XINT6CR = 0x0000;
	*XIntruptRegs_XINT7CR = 0x0000;
	
	*XIntruptRegs_XNMICR = 0x0000;
	
//--- Low-power mode selection
	*GpioIntRegs_GPIOLPMSEL = 0x0000;
	
	/////////////////////////////////////////////////////////////////////////////
	
	EDIS;
}

// 이 함수는 H/W 변동이 없는 한 수정 없음
void Xinterface_setup(void)
{
	EALLOW;
	
	// GPIO14, GPIO15
	*GpioCtrlRegs_GPAMUX1_High |= 0x5000;
	
	// GPIO30, GPIO31
	*GpioCtrlRegs_GPAMUX2_High |= 0xff00;
	
	// GPIO34, GPIO35, GPIO36, GPIO37, GPIO38, GPIO39
	*GpioCtrlRegs_GPBMUX1_Low |= 0xfff0;
	
	// GPIO40, GPIO41, GPIO42, GPIO43, GPIO44, GPIO45, GPIO46, GPIO47
	*GpioCtrlRegs_GPBMUX1_High |= 0xffff;
		
	// GPIO64, GPIO65, GPIO66, GPIO67, GPIO68, GPIO69, GPIO70, GPIO71
	*GpioCtrlRegs_GPCMUX1_Low |= 0xffff;
	
	// GPIO72, GPIO73, GPIO74, GPIO75, GPIO76, GPIO77, GPIO78, GPIO79
	*GpioCtrlRegs_GPCMUX1_High |= 0xffff;
	
	// GPIO80, GPIO81, GPIO82, GPIO83, GPIO84, GPIO85, GPIO86, GPIO87
	*GpioCtrlRegs_GPCMUX2_Low |= 0xffff;
	
	
	// All Zones---------------------------------
	// Timing for all zones based on XTIMCLK = SYSCLKOUT
	// Buffer up to 3 writes
	*XintfRegs_XINTCNF2_Low |= 0x0003;
	
	// XCLKOUT is enabled
	// XCLKOUT = XTIMCLK
	*XintfRegs_XINTCNF2_Low &= 0xfff3;
	
	*XintfRegs_XINTCNF2_High = 0x0000;
	
	
	// Zone 0(Access Time인 느린 외부 장치를 기준) ----------------
	// When using ready, ACTIVE must be 1 or greater
	// Lead must always be 1 or greater
	// Zone 0 write timing
	// 31 30 29 29 27 26 25 24 23 22 21 20 19 18 17 16
	// <--- not use ------------>  0 <-- not -->  1  1
	// 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	//  0  0  0  1  0  1  0  0  0  1  0  0  1  0  0  0
	// 31 ~ 23 : x
	// 22 : x2timing
	// 21 ~ 18 : x
	// 17 ~ 16 : xsize
	// 15 : readymode
	// 14 : useready
	// 13 ~ 12 : xrdlead
	// 11 ~ 9 : xrdactive
	// 8 ~ 7 : xrdtrail
	// 6 : xwrlead
	// 5 ~ 2 : xwractive
	// 1 ~ 0 : xwrtrail	
	
	// *XintfRegs_XTIMING0_Low = 0x1448;
	// *XintfRegs_XTIMING0_High = 0x0003;
	
	// *XintfRegs_XTIMING0_Low = 0x3edf;
	
	//원래
	 *XintfRegs_XTIMING0_Low = 0x3fdf;
	 *XintfRegs_XTIMING0_High = 0x0043;
	
	//  *XintfRegs_XTIMING0_Low = 0x16d2;
	//  *XintfRegs_XTIMING0_Low = 0x18b1;
	//  *XintfRegs_XTIMING0_High = 0x0003;
	
	
//	XintfRegs.XTIMING0.bit.XWRLEAD = 1;//max 45ns
//	XintfRegs.XTIMING0.bit.XWRACTIVE = 2;//min 25ns
//	XintfRegs.XTIMING0.bit.XWRTRAIL = 0;//min 0ns
//	// Zone 0 read timing
//	XintfRegs.XTIMING0.bit.XRDLEAD = 1;//min 0ns
//	XintfRegs.XTIMING0.bit.XRDACTIVE = 2;//min 30ns
//	XintfRegs.XTIMING0.bit.XRDTRAIL = 0;//min 0ns
//
//	// don't double Zone 0 read/write lead/active/trail timing
//	XintfRegs.XTIMING0.bit.X2TIMING = 0;
//
//	// Zone 0 will not sample XREADY signal
//	XintfRegs.XTIMING0.bit.USEREADY = 0;
//	XintfRegs.XTIMING0.bit.READYMODE = 0;
//
//	// Size must be either:
//	// 0,1 = x32 or
//	// 1,1 = x16 other values are reserved
//	XintfRegs.XTIMING0.bit.XSIZE = 3;

	// Zone 6(Access Time인 느린 외부 장치-FRAM를 기준) ----------------
	// When using ready, ACTIVE must be 1 or greater
	// Lead must always be 1 or greater
	// Zone 6 write timing
/*
	XintfRegs.XTIMING6.bit.XWRLEAD = 1;
	XintfRegs.XTIMING6.bit.XWRACTIVE = 4;
	XintfRegs.XTIMING6.bit.XWRTRAIL = 2;
	// Zone 6 read timing
	XintfRegs.XTIMING6.bit.XRDLEAD = 1;
	XintfRegs.XTIMING6.bit.XRDACTIVE = 4;
	XintfRegs.XTIMING6.bit.XRDTRAIL = 2;
*/
	// 31 30 29 29 27 26 25 24 23 22 21 20 19 18 17 16
	// <--- not use ------------>  1 <-- not -->  1  1
	// 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	//  0  0  1  1  1  1  1  1  1  1  0  1  1  1  1  1
	*XintfRegs_XTIMING6_Low = 0x3fdf;
	*XintfRegs_XTIMING6_High = 0x0043;
	
//	XintfRegs.XTIMING6.bit.XWRLEAD = 3;
//	XintfRegs.XTIMING6.bit.XWRACTIVE = 7;
//	XintfRegs.XTIMING6.bit.XWRTRAIL = 3;
//	// Zone 6 read timing
//	XintfRegs.XTIMING6.bit.XRDLEAD = 3;
//	XintfRegs.XTIMING6.bit.XRDACTIVE = 7;
//	XintfRegs.XTIMING6.bit.XRDTRAIL = 3;
//
//	// don't double Zone 6 read/write lead/active/trail timing
//	XintfRegs.XTIMING6.bit.X2TIMING = 1;
//
//	// Zone 6 will not sample XREADY signal
//	XintfRegs.XTIMING6.bit.USEREADY = 0;
//	XintfRegs.XTIMING6.bit.READYMODE = 0;
//	
//	
//
//	// Size must be either:
//	// 0,1 = x32 or
//	// 1,1 = x16 other values are reserved
//	XintfRegs.XTIMING6.bit.XSIZE = 3;

	

	// Zone 7(Wiznet W3150A+를 기준) ------------------------------
	// When using ready, ACTIVE must be 1 or greater
	// Lead must always be 1 or greater
	// Zone 7 write timing
	// 31 30 29 29 27 26 25 24 23 22 21 20 19 18 17 16
	// <--- not use ------------>  0 <-- not -->  1  1
	// 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	//  0  0  0  1  0  1  0  0  1  0  1  0  1  0  0  1
	// *XintfRegs_XTIMING7_Low = 0x3fdf;
	*XintfRegs_XTIMING7_Low = 0x14a9;
	
	*XintfRegs_XTIMING7_High = 0x0003;
	
//	XintfRegs.XTIMING7.bit.XWRLEAD = 3;
//	XintfRegs.XTIMING7.bit.XWRACTIVE = 7;
//	XintfRegs.XTIMING7.bit.XWRTRAIL = 3;
//	// Zone 7 read timing
//	XintfRegs.XTIMING7.bit.XRDLEAD = 3;
//	XintfRegs.XTIMING7.bit.XRDACTIVE = 7;
//	XintfRegs.XTIMING7.bit.XRDTRAIL = 3;
//
//	// don't double Zone 7 read/write lead/active/trail timing
//	XintfRegs.XTIMING7.bit.X2TIMING = 0;
//
//	// Zone 7 will not sample XREADY signal
//	XintfRegs.XTIMING7.bit.USEREADY = 0;
//	XintfRegs.XTIMING7.bit.READYMODE = 0;
//
//	// 1,1 = x16 data bus
//	// 0,1 = x32 data bus
//	// other values are reserved
//	XintfRegs.XTIMING7.bit.XSIZE = 3;

	// Bank switching
    // Assume Zone 7 is slow, so add additional BCYC cycles
    // when ever switching from Zone 7 to another Zone.
    // This will help avoid bus contention.
    *XintfRegs_XBANK = 0x001b;
    //XintfRegs.XBANK.bit.BANK = 3;
    //XintfRegs.XBANK.bit.BCYC = 3;

//	//--- External interrupt selection
//	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 1; 		// GPIO00 is XINT1 source 
//	GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 0; 		// GPIO00 is XINT2 source 
//	GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 16; 		// GPIO48 is XINT3 source (48-32)
//	GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL = 17; 		// GPIO49 is XINT4 source (49-32)
//	
//	{
//		GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL = 18; 		// GPIO50 is XINT5 source (50-32)
//		GpioIntRegs.GPIOXINT6SEL.bit.GPIOSEL = 19; 		// GPIO51 is XINT6 source (51-32)
//	}
//	
//	{
//		GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL = 0; 		// GPIO50 is XINT5 source (50-32)
//		GpioIntRegs.GPIOXINT6SEL.bit.GPIOSEL = 0; 		// GPIO51 is XINT6 source (51-32)
//	}
//
//	GpioIntRegs.GPIOXINT7SEL.bit.GPIOSEL = 0;		// GPIO32 is XINT7 source
//	GpioIntRegs.GPIOXNMISEL.all = 0x0000;			// GPIO0 is XNMI source
//
//	PieVectTable.XINT1 = &XINT1_ISR;
//	PieVectTable.XINT3 = &XINT3_ISR;
//	PieVectTable.XINT4 = &XINT4_ISR;
//	
//	{
//		PieVectTable.XINT5 = &XINT5_ISR;
//		PieVectTable.XINT6 = &XINT6_ISR;
//	}
//
//	XIntruptRegs.XINT1CR.bit.ENABLE = 1;		// XINT1 enabled
//	XIntruptRegs.XINT2CR.all = 0x0000;			// XINT2 disabled
//	XIntruptRegs.XINT3CR.bit.ENABLE = 1;		// XINT3 enabled
//	XIntruptRegs.XINT4CR.bit.ENABLE = 1;		// XINT4 enabled
//	
//	{
//		XIntruptRegs.XINT5CR.bit.ENABLE = 1;		// XINT5 enabled
//		XIntruptRegs.XINT6CR.bit.ENABLE = 1;		// XINT6 enabled
//	}
//	
//	{
//		XIntruptRegs.XINT5CR.all = 0x0000;		// XINT5 enabled
//		XIntruptRegs.XINT6CR.all = 0x0000;		// XINT6 enabled
//	}
//
//	XIntruptRegs.XINT7CR.all = 0x0000;			// XINT7 disabled
//	XIntruptRegs.XNMICR.all = 0x0000;			// XNMI disabled
//
//	// Set XINT Control Register(Falling Edge)
//	XIntruptRegs.XINT1CR.bit.POLARITY = 0;
//	XIntruptRegs.XINT3CR.bit.POLARITY = 0;
//	XIntruptRegs.XINT4CR.bit.POLARITY = 0;
//	
//	{
//		XIntruptRegs.XINT5CR.bit.POLARITY = 0;
//		XIntruptRegs.XINT6CR.bit.POLARITY = 0;
//	}
//	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;		// Enable the PIE block
//	PieCtrlRegs.PIEIER1.bit.INTx4 = 1;		// Enable XINT1 in the PIE: Group 12 interrupt 1
//	PieCtrlRegs.PIEIER12.bit.INTx1 = 1;		// Enable XINT3 in the PIE: Group 12 interrupt 1
//	PieCtrlRegs.PIEIER12.bit.INTx2 = 1;		// Enable XINT4 in the PIE: Group 12 interrupt 1
//	
//	{
//		PieCtrlRegs.PIEIER12.bit.INTx3 = 1;		// Enable XINT5 in the PIE: Group 12 interrupt 1  //25.8kV용 //DI 2번카드
//		PieCtrlRegs.PIEIER12.bit.INTx4 = 1;		// Enable XINT6 in the PIE: Group 12 interrupt 1 	//25.8kV용 //DI 2번카드
//	}
//	IER |= M_INT12;	// Enable CPU int12 which is connected to XINT3/4/5/6
//	IER |= M_INT1;	// Enable CPU int12 which is connected to XINT3/4/5/6
	EDIS;

	//Force a pipeline flush to ensure that the write to
	//the last register configured occurs before returning.
	asm(" RPT #7 || NOP");
}

// 이 함수는 H/W 변동이 없는 한 수정 없음
void InitTimer(void)
{
	EALLOW;  // This is needed to write to EALLOW protected registers
	
	// system clock 150MHz
	// 주기 1.389ms -> 1389us
	// 150 * 1389 = 208350
	// *CpuTimer0Regs_PRD = 0x49f0;//(unsigned int)k;
	// *CpuTimer0Regs_PRDH = 0x0002;//(unsigned int)(k >> 16);
	*CpuTimer0Regs_PRD = 0x2dde;//(unsigned int)k;
	*CpuTimer0Regs_PRDH = 0x0003;//(unsigned int)(k >> 16);
	
	// Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
	*CpuTimer0Regs_TPR = 0;
	*CpuTimer0Regs_TPRH = 0;
	
	// Make sure timer is stopped:
	//CpuTimer0Regs.TCR.bit.TSS = 1;
	// Reload all counter register with period value:
	//CpuTimer0Regs.TCR.bit.TRB = 1;
	*CpuTimer0Regs_TCR = 0x4c30;	
	*CpuTimer0Regs_TCR = 0x4001;
	
	// system clock 150MHz
	// 주기 1ms -> 1000us
	// 150 * 1000 = 150000
	*CpuTimer1Regs_PRD = 0x49f0;//(unsigned int)k;
	*CpuTimer1Regs_PRDH = 0x0002;//(unsigned int)(k >> 16);
	
	// Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
	*CpuTimer1Regs_TPR = 0;
	*CpuTimer1Regs_TPRH = 0;
	
	// Make sure timer is stopped:
	//CpuTimer0Regs.TCR.bit.TSS = 1;
	// Reload all counter register with period value:
	//CpuTimer0Regs.TCR.bit.TRB = 1;
	*CpuTimer1Regs_TCR = 0x4c30;	
	*CpuTimer1Regs_TCR = 0x4001;
	
//	// system clock 150MHz
//	// 주기 100us -> 10us
//	// 150 * 100 = 15000
//	// 150 * 30 = 4500
//	*CpuTimer2Regs_PRD = 0xbb8;//(unsigned int)k;
//	*CpuTimer2Regs_PRDH = 0x0000;//(unsigned int)(k >> 16);
//	
//	// Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
//	*CpuTimer2Regs_TPR = 0;
//	*CpuTimer2Regs_TPRH = 0;
//	
//	// Make sure timer is stopped:
//	//CpuTimer0Regs.TCR.bit.TSS = 1;
//	// Reload all counter register with period value:
//	//CpuTimer0Regs.TCR.bit.TRB = 1;
//	*CpuTimer2Regs_TCR = 0x4c30;	
//	*CpuTimer2Regs_TCR = 0x4001;
	
	
//	// system clock 150MHz
//	// 주기 1.389ms -> 1390us
//	// 150 * 1395 = 208500
//	*CpuTimer0Regs_PRD = 0x3162;//(unsigned int)k;
//	*CpuTimer0Regs_PRDH = 0x0003;//(unsigned int)(k >> 16);
//	
//	// Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
//	*CpuTimer0Regs_TPR = 0;
//	*CpuTimer0Regs_TPRH = 0;
//	
//	// Make sure timer is stopped:
//	//CpuTimer0Regs.TCR.bit.TSS = 1;
//	// Reload all counter register with period value:
//	//CpuTimer0Regs.TCR.bit.TRB = 1;
//	*CpuTimer0Regs_TCR = 0x4c30;
//	
//	
//	*CpuTimer0Regs_TCR = 0x4001;

//	*CpuTimer0Regs_PRD = 0xffff;
//	*CpuTimer0Regs_PRDH = 0xffff;
//	
//	// Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
////	CpuTimer0Regs.TPR.all  = 0;
////	CpuTimer0Regs.TPRH.all = 0;
//	*CpuTimer0Regs_TPR = 0;
//	*CpuTimer0Regs_TPRH = 0;
//	
//	// Make sure timer is stopped:
//	//CpuTimer0Regs.TCR.bit.TSS = 1;
//	// Reload all counter register with period value:
//	//CpuTimer0Regs.TCR.bit.TRB = 1;
//	*CpuTimer0Regs_TCR = 0x0030;
	
	// Reset interrupt counters:
	//CpuTimer0.InterruptCount = 0;


// CpuTimer 1 and CpuTimer2 are reserved for DSP BIOS & other RTOS
// Do not use these two timers if you ever plan on integrating
// DSP-BIOS or another realtime OS.
//
// Initialize address pointers to respective timer registers:
//	CpuTimer1.RegsAddr = &CpuTimer1Regs;
//	CpuTimer2.RegsAddr = &CpuTimer2Regs;
//	// Initialize timer period to maximum:
//	CpuTimer1Regs.PRD.all  = 0xFFFFFFFF;
//	CpuTimer2Regs.PRD.all  = 0xFFFFFFFF;
//    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
//	CpuTimer1Regs.TPR.all  = 0;
//	CpuTimer1Regs.TPRH.all = 0;
//	CpuTimer2Regs.TPR.all  = 0;
//	CpuTimer2Regs.TPRH.all = 0;
//    // Make sure timers are stopped:
//	CpuTimer1Regs.TCR.bit.TSS = 1;
//	CpuTimer2Regs.TCR.bit.TSS = 1;
//	// Reload all counter register with period value:
//	CpuTimer1Regs.TCR.bit.TRB = 1;
//	CpuTimer2Regs.TCR.bit.TRB = 1;
//	// Reset interrupt counters:
//	CpuTimer1.InterruptCount = 0;
//	CpuTimer2.InterruptCount = 0;
	
//	
//
//	// Configure CPU-Timer 0, 1, and 2 to interrupt every 1 milliseconds:
//	// 150MHz CPU Freq, 1 ms Period (uSeconds 단위)
//	ConfigCpuTimer(&CpuTimer0, 150, 1000); //1[msec]
////	ConfigCpuTimer(&CpuTimer0, 150, 2000); //1[msec]
//	//ConfigCpuTimer_1(&CpuTimer1, 150, 1000000);		//2010.11.22-2 cch
//	//ConfigCpuTimer(&CpuTimer2, 150, 10000); //10[msec]
//
//	
//	// To ensure precise timing, use write-only instructions to write to the entire register. Therefore, if any
//	// of the configuration bits are changed in ConfigCpuTimer and InitCpuTimers (in DSP2833x_CpuTimers.h), the
//	// below settings must also be updated.
//	CpuTimer0Regs.TCR.all = 0x4001; // Use write-only instruction to set TSS bit = 0
//	CpuTimer1Regs.TCR.all = 0x4001; // Use write-only instruction to set TSS bit = 0
//	CpuTimer2Regs.TCR.all = 0x4001; // Use write-only instruction to set TSS bit = 0
//
//	
//	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;		// Enable the PIE block
//	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;		// Enable timer 0 interrupt
////	PieCtrlRegs.PIEIER1.bit = 1;		// Enable timer 0 interrupt
////	IER |= (M_INT1 | M_INT14 | M_INT12);	 //Enable timer 0, timer 2 interrupt
//	IER |= (M_INT1 | M_INT14);	 //Enable timer 0, timer 2 interrupt
	EDIS;    // This is needed to disable write to EALLOW protected registers
}

// 이 함수는 H/W 변동이 없는 한 수정 없음
void i2c_setup(void)
{
	// Setting GPIO for I2C
	EALLOW;
	/* Enable internal pull-up for the selected pins */
//	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
//	GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   // Enable pull-up for GPIO33 (SCLA)
	
	/* Set qualification for selected pins to asynch only */
//	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
//	GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)
	
	/* Configure I2C pins using GPIO regs*/
	// This specifies which of the possible GPIO pins will be I2C functional pins.
//	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // Configure GPIO32 for SDAA operation
//	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // Configure GPIO33 for SCLA operation

	// GPIO32, GPIO33
	*GpioCtrlRegs_GPBMUX1_Low |= 0x0005;
	
	*GpioCtrlRegs_GPBQSEL1_Low |= 0x000f; // i2c

	EDIS;
	
	// Initialize I2C interface
	// Put I2C module in the reset state
	*I2caRegs_I2CMDR = 0;
	
	// Set Slave Address according to AT24C16 device
	// *I2caRegs_I2CSAR = 0x0050; //eeprom slave address
	*I2caRegs_I2CSAR = 0x00; //eeprom slave address

	// Set I2C module clock input
	// 시스템clock 150MHz
	// 150MHz/(PSC + 1) = 10MHz -> I2C module clock
	*I2caRegs_I2CPSC = 14; // need 7-12 Mhz on module clk (150/15 = 10MHz)
	
	// 400KHz clock speed in SCL for master mode(2.5us period)
	// Clock Pulse Width Low  : 1.5us
	// Clock Pulse Width High : 1.0us
	// F28335의 I2C User Guide에서 Master Clock 주기를 구하는 공식을 참조바람.
	// I2C module clock time * (상위시간 + d(5)) = 원하는시간
	// 0.0000001 * (상위 + 5) = 0.0000015
	// 0.0000001 * (하위 + 5) = 0.000001
	*I2caRegs_I2CCLKL = 10;
	*I2caRegs_I2CCLKH = 5;
	
	// Disable I2C interrupt
	// *I2caRegs_I2CIER = 0x0;
	*I2caRegs_I2CIER = 0x00;
	
	// Enable TX and RX FIFO in I2C module
	*I2caRegs_I2CFFTX = 0x6000; // Enable FIFO mode and TXFIFO
	*I2caRegs_I2CFFRX = 0x2040; // Enable RXFIFO, clear RXFFINT

	// Enable I2C module
	*I2caRegs_I2CMDR = 0x20; 
}

// 이 함수는 H/W 변동이 없는 한 수정 없음
void lcd_setup(void)
{
	DELAY_US(50000);
	
	// 7 6 5 4 |3 2 1 0
	// <data >    e   rs
	
	// initialize
	*LCD_CS = 0x34;
	DELAY_US(1);
	*LCD_CS = 0x30;
	DELAY_US(1);
	
	DELAY_US(50000);
	
	*LCD_CS = 0x34;
	DELAY_US(1);
	*LCD_CS = 0x30;
	DELAY_US(1);
	
	DELAY_US(500);
	
	*LCD_CS = 0x34;
	DELAY_US(1);
	*LCD_CS = 0x30;
	DELAY_US(1);
	
	DELAY_US(500);
	
	/////////////////////////
	*LCD_CS = 0x24;
	DELAY_US(1);
	*LCD_CS = 0x20;
	DELAY_US(1);
		
	*LCD_CS = 0x24;
	DELAY_US(1);
	*LCD_CS = 0x20;
	DELAY_US(1);
	
	*LCD_CS = 0x84;
	DELAY_US(1);
	*LCD_CS = 0x80;
	DELAY_US(1);
	
	DELAY_US(1000);
	
	*LCD_CS = 0x04;
	DELAY_US(1);
	*LCD_CS = 0x00;
	DELAY_US(1);
	
	*LCD_CS = 0xf4;
	DELAY_US(1);
	*LCD_CS = 0xf0;
	DELAY_US(1);
	
	DELAY_US(1000);
	
	*LCD_CS = 0x04;
	DELAY_US(1);
	*LCD_CS = 0x00;
	DELAY_US(1);
	
	*LCD_CS = 0x14;
	DELAY_US(1);
	*LCD_CS = 0x10;
	DELAY_US(1);
	
	DELAY_US(2000);
	
	*LCD_CS = 0x04;
	DELAY_US(1);
	*LCD_CS = 0x00;
	DELAY_US(1);
	
	*LCD_CS = 0x64;
	DELAY_US(1);
	*LCD_CS = 0x60;
	DELAY_US(1);
	
	DELAY_US(2000);
	
	lcd_control_write(LCD_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF);
	
	DELAY_US(3000);
	
	lcd_control_write(0x40);//Make Font, '0' Address is excepted.(+8)
	DELAY_US(100);

//UPARROW
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x0e);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x15);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x00);			/* clear display */
	DELAY_US(100);
	
//UPARROW 0x01
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x0e);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x15);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x00);			/* clear display */
	DELAY_US(100);
	
//DOWNARROW 0x02
	DELAY_US(100);
	lcd_character_write(0x00);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x15);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x0e);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);

//UPDOWNARROW 0x03
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x0e);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x15);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x15);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x0e);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);

//ENTER 0x04
	DELAY_US(100);
	lcd_character_write(0x01);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x01);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x01);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x05);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x09);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x1f);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x08);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);

//PERCENT 0x05
//	DELAY_US(100);
//	lcd_character_write(0x00);			/* clear display */
//	DELAY_US(100);
//	lcd_character_write(0x19);			/* clear display */
//	DELAY_US(100);
//	lcd_character_write(0x1a);			/* clear display */
//	DELAY_US(100);
//	lcd_character_write(0x04);			/* clear display */
//	DELAY_US(100);
//	lcd_character_write(0x0B);			/* clear display */
//	DELAY_US(100);
//	lcd_character_write(0x13);			/* clear display */
//	DELAY_US(100);
//	lcd_character_write(0x00);			/* clear display */
//	DELAY_US(100);
//	lcd_character_write(0x00);			/* clear display */
//	DELAY_US(100);
//ANGLE 0x05
	lcd_character_write(0x00);	
	DELAY_US(100);
	lcd_character_write(0x01);	
	DELAY_US(100);
	lcd_character_write(0x02);	
	DELAY_US(100);
	lcd_character_write(0x04);	
	DELAY_US(100);
	lcd_character_write(0x08);	
	DELAY_US(100);
	lcd_character_write(0x1f);	
	DELAY_US(100);
	lcd_character_write(0x00);	
	DELAY_US(100);	
	lcd_character_write(0x00);	
	DELAY_US(100);

//THETA 0x06
	DELAY_US(100);
	lcd_character_write(0x0E);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x11);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x11);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x1F);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x11);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x11);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x0E);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x00);			/* clear display */
	DELAY_US(100);
	DELAY_US(100);
	DELAY_US(100);

//THAU 0x07
	DELAY_US(100);
	lcd_character_write(0x1F);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x02);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x08);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x08);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x04);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x02);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x00);			/* clear display */
	DELAY_US(100);

//FULLDOT 0x08
	DELAY_US(100);
	lcd_character_write(0x1F);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x1F);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x1F);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x1F);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x1F);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x1F);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x1F);			/* clear display */
	DELAY_US(100);
	lcd_character_write(0x1F);			/* clear display */
	DELAY_US(100);

	lcd_control_write(0x80);
	DELAY_US(3000);
}




