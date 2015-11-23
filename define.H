//#ifndef _DEFINE_H_
//#define _DEFINE_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_GlobalPrototypes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ram으로 올라가는 code 선언
#pragma CODE_SECTION(InitFlash, "ramfuncs");
#pragma CODE_SECTION(TINT1_ISR, "ramfuncs");
#pragma CODE_SECTION(XINT3_ISR, "ramfuncs");
#pragma CODE_SECTION(SCIRXB_ISR, "ramfuncs");
#pragma CODE_SECTION(SCITXINTB_ISR, "ramfuncs");
#pragma CODE_SECTION(SCITXINTC_ISR, "ramfuncs");

#pragma CODE_SECTION(real_main, "ramfuncs");
//#pragma CODE_SECTION(manager_handling, "ramfuncs");
#pragma CODE_SECTION(make_crc_send, "ramfuncs");
#pragma CODE_SECTION(serial_ok_nak_send, "ramfuncs");
#pragma CODE_SECTION(i2c_read, "ramfuncs");
#pragma CODE_SECTION(i2c_write, "ramfuncs");
#pragma CODE_SECTION(key_drive, "ramfuncs");
#pragma CODE_SECTION(menu_drive, "ramfuncs");
//#pragma CODE_SECTION(lcd_setup, "ramfuncs");
#pragma CODE_SECTION(lcd_control_write, "ramfuncs");
#pragma CODE_SECTION(lcd_character_write, "ramfuncs");
#pragma CODE_SECTION(power_update, "ramfuncs");
#pragma CODE_SECTION(delay_us, "ramfuncs");
//#pragma CODE_SECTION(display_time, "ramfuncs");
#pragma CODE_SECTION(rtc_handling, "ramfuncs");
#pragma CODE_SECTION(PROTECTIVE_RELAY, "ramfuncs");
#pragma CODE_SECTION(Inverse_GetDelayTime, "ramfuncs");
#pragma CODE_SECTION(Check_47P_10V, "ramfuncs");
#pragma CODE_SECTION(event_direct_save, "ramfuncs");
#pragma CODE_SECTION(Cal_RHour, "ramfuncs");

#pragma CODE_SECTION(RELAY_OCR50_1, "ramfuncs");
#pragma CODE_SECTION(RELAY_OCR50_2, "ramfuncs");
#pragma CODE_SECTION(RELAY_OCR51_1, "ramfuncs");
#pragma CODE_SECTION(RELAY_OCR51_2, "ramfuncs");
#pragma CODE_SECTION(RELAY_OCGR50, "ramfuncs");

#pragma CODE_SECTION(RELAY_OCGR51, "ramfuncs");
#pragma CODE_SECTION(RELAY_UVR_1, "ramfuncs");
#pragma CODE_SECTION(RELAY_UVR_2, "ramfuncs");
#pragma CODE_SECTION(RELAY_UVR_3, "ramfuncs");
#pragma CODE_SECTION(RELAY_P47, "ramfuncs");

#pragma CODE_SECTION(RELAY_N47, "ramfuncs");
#pragma CODE_SECTION(RELAY_OVR, "ramfuncs");
#pragma CODE_SECTION(RELAY_OVGR, "ramfuncs");
#pragma CODE_SECTION(RELAY_DGR, "ramfuncs");
#pragma CODE_SECTION(RELAY_SGR, "ramfuncs");

#pragma CODE_SECTION(Save_Relay_Event, "ramfuncs");
#pragma CODE_SECTION(relay_dropout_to_normal, "ramfuncs");

//#pragma CODE_SECTION(supervision_relay, "ramfuncs");

#pragma CODE_SECTION(float_to_8bit_fram, "ramfuncs");
#pragma CODE_SECTION(wave_dump_serial_sram, "ramfuncs");

#pragma CODE_SECTION(Relay_On, "ramfuncs");
#pragma CODE_SECTION(Relay_Off, "ramfuncs");

#pragma CODE_SECTION(COMM_CRC, "ramfuncs");

//#define FS	0x1234 //FI
//#define MS	0x5678 //MS //2015.02.25

#define CT_1A  0x1234 //CT 1A
#define CT_5A  0x5678	//CT 5A

#define Hz50	0x1234	//50Hz
#define Hz60	0x5678	//60Hz

#define NCT_SELECT	0x1234 //NCT 사용
#define ZCT_SELECT	0x5678 //ZCT 사용

#define OCGR_SELECT  0x1234	//OCGR 사용
#define DGR_SELECT   0x5678	//DGR 사용

#define ENABLE  0xaaaa
#define DISABLE 0xbbbb

#define OCR_NORMAL  0xaaaa
#define OCR_SELECT  0xbbbb

#define  HIMIX_SEND  *GpioIntRegs_GPBSET_High |= 0x0400
#define  HIMIX_STOP  *GpioIntRegs_GPBCLEAR_High |= 0x0400

#define F_OCR50_1         0x00000001L
#define F_OCR50_2         0x00000002L
#define F_OCR51_1         0x00000004L
#define F_OCR51_2         0x00000008L
#define F_OCGR50          0x00000010L
#define F_OCGR51          0x00000020L
#define F_UVR_1           0x00000040L
#define F_UVR_2           0x00000080L
#define F_UVR_3           0x00000100L
#define F_P47             0x00000200L
#define F_N47             0x00000400L
#define F_OVR             0x00000800L
#define F_OVGR            0x00001000L
#define F_DGR             0x00002000L
#define F_SGR             0x00004000L
#define INT_F_OCR50_1         0x00000001L
#define INT_F_OCR50_2         0x00000002L
#define INT_F_OCR51_1         0x00000003L
#define INT_F_OCR51_2         0x00000004L
#define INT_F_OCGR50          0x00000005L
#define INT_F_OCGR51          0x00000006L
#define INT_F_UVR_1           0x00000007L
#define INT_F_UVR_2           0x00000008L
#define INT_F_UVR_3           0x00000009L
#define INT_F_P47             0x0000000AL
#define INT_F_N47             0x0000000BL
#define INT_F_OVR             0x0000000CL
#define INT_F_OVGR            0x0000000DL
#define INT_F_DGR             0x0000000EL
#define INT_F_SGR             0x0000000FL
//#define CBM             0x00008000
//#define CBM2            0x00010000
//#define SYSCH           0x00020000
//#define CAI             0x00040000
//#define CAV             0x00080000
//#define CAS             0x00100000
//#define DGR2            0x00200004
//#define SGR2            0x00200008
//#define RLYSET          0x00400000
//#define SYSSET          0x00800000
//#define COMFLAG         0x01000000
//#define TIMESET         0x02000000
//#define PASSSET         0x04000000
//#define CBSET           0x08000000
//#define SYSASET         0x10000000
//#define SETUVR          0x20000000

// relay 동작 상태
#define RELAY_NORMAL      0
#define RELAY_DETECT      1
#define RELAY_PICKUP      2
#define RELAY_TRIP   			3

#define AUX_RELAY_MAX      0x0ff

#define INSTANT     0
#define DEFINITE    1
#define INVERSE     2
#define V_INVERSE   3
#define E_INVERSE   4

#define AUTO		0x1234
#define MANUAL	0x5678

#define ON		0x1234
#define OFF		0x5678

#define CB_CONTROL_TIME_MIN   1
#define CB_CONTROL_TIME_MAX   5

#define COMM_ADDR_MIN   1
#define COMM_ADDR_MAX   230
#define COMM_BAUD_MIN   3
#define COMM_BAUD_MAX   7

#define DISPLAY_CURRENT_MIN 0.03	//30[mA]
#define DISPLAY_VOLTAGE_MIN 1.9 	//1.9[V]
#define DISPLAY_Io_MIN 			0.2		//0.2[mA]

#define ADDRESS_MIN        1
#define ADDRESS_MAX        254
#define LOCAL_CONTROL_OFF    1
#define LOCAL_CONTROL_ON     0
#define DGR_ANGLE_MIN      -100
#define DGR_ANGLE_MAX      100
#define SGR_ANGLE_MIN      -100
#define SGR_ANGLE_MAX      100
#define UVR_AUTO         0
#define UVR_MANUAL       1

#define SUPERVISION_OFF    1
#define SUPERVISION_ON     0
#define CCS_MIN      0
#define CCS_MAX      200
#define TCS_MIN      0
#define TCS_MAX      200
#define ALRAM_OFF    1
#define ALRAM_ON     0


// 반한시 IEC60255-3 수식
// t = T/L * ((k / (G/Gs)^a) + c)
// 계전요소 설정 후 상수들
// 계전기 특성 : k, c, G
// 특성 곡선 지수 : a
// 설정값 : Gs
// 레버 : T/L
//
// 변수
// 입력값 : G

#define IEC_INV_k    0.14
#define IEC_INV_a    0.02
#define IEC_INV_c    0

#define IEC_VINV_k    13.5
#define IEC_VINV_a    1
#define IEC_VINV_c    0

#define IEC_EINV_k    80
#define IEC_EINV_a    2
#define IEC_EINV_c    0

#define INSTANT_PICKUP_LIMIT   10
#define DEFINITE_PICKUP_LIMIT  5
#define INVERSE_PICKUP_LIMIT   5

#define TOTAL_DELAY_50	       20 //50-1, 50-2, 50G, INST, DEF 확인 완료
																	// 총 계전기 동작 시간 = RMS_CAL_TIME + ALGORITHM_TIME(LOGIC_DELAY_TIME + PICKUP_DELAY + WAITING DELAY) + DO_CONTACT_TIME
																	// 여기에서 TOTAL_DELAY = 상수 변수인 RMS_CAL_TIME + DO_CONTACT_TIME + LOGIC_DELAY_TIME;
//#define DO_CONTACT_TIME      10 //MAX 15msec 
//#define RMS_CAL_TIME				 12 //MIN 6~7msec, MAX18~20msec	
//#define LOGIC_DELAY_TIME		 3	//OCR(inst, def의 경우) 3msec

#define TOTAL_DELAY_51	       20 // 51-1, 51-2 확인 완료
                                  // 총 계전기 동작 시간 = RMS_CAL_TIME + ALGORITHM_TIME(LOGIC_DELAY_TIME + PICKUP_DELAY + WAITING DELAY) + DO_CONTACT_TIME
#define TOTAL_DELAY_51G				 14 // 51G 확인 완료
#define TOTAL_DELAY_27	       18
#define TOTAL_DELAY_47P	       23 
#define TOTAL_DELAY_47N	       23
#define TOTAL_DELAY_59	       25 
#define TOTAL_DELAY_64	       25
#define TOTAL_DELAY_67GD       22
#define TOTAL_DELAY_67GS       25 

#define Ia   0
#define Ib   1
#define Ic   2
#define In   3
#define In2  4
#define Is   5
#define Va   6
#define Vb   7
#define Vc   8
#define Vn   9

#define KEPCO_TYPE  0x1234

#define CALIBRATION_NOT  0x0001
#define SRAM_FAIL        0x0002
#define MRAM_FAIL        0x0004
#define ADC_FAIL         0x0008
#define COMM_IF_FAIL     0x0010
#define FLASH_FAIL       0x0020

// 샘플값 기반
// true rms 및 전력용
// adc 해상도 10/32767 = 0.000305185095
// CT비 2000/1
// 상CT 부담저항 150옴
// NCT 부담저항 1500옴
// ZCT비 1150/1
// ZCT 부담저항 2000옴
// 증폭 281배

// 상CT factor
// 0.000305185095 / (150 * (1/2000)) = 0.0040691346
// 0.000305185095 / (124 * (1/2000)) = 0.004922340242
// NCT factor
// 0.000305185095 / (1500 * (1/2000)) = 0.00040691346
// ZCT factor
// 0.000305185095 / (2000 * (1/1150)) = 0.00017548143

//#define INTERNAL_CT_RATIO   0.0040691346
#define INTERNAL_CT_RATIO   0.004922340242
#define INTERNAL_NCT_RATIO  0.00040691346
#define INTERNAL_ZCT_RATIO  0.00017548143

// PT비 380/3
// 상전압 부담저항 없음
// 영상전압 2배로 증폭해줌
// 상PT factor
// 0.000305185095 * (380/3) = 0.0386567787
// GPT factor
// 0.000305185095 * (1/2) * (380/3) = 0.01932838935
// 상pt 2.31
// 영상pt 4.07(최종)
//        4.62(기존)
#define INTERNAL_PT_RATIO		0.0386567787
#define INTERNAL_GPT_RATIO	0.01932838935
#define INTERNAL_CPT_RATIO	0.00074717327

//display tag
#define DT_50_1         0
#define DT_50_2         1
#define DT_51_1         2
#define DT_51_2         3
#define DT_50G          4
#define DT_51G          5
#define DT_67N          6
#define DT_67G          7
#define DT_59           8
#define DT_64           9
#define DT_27_1         10
#define DT_27_2         11
#define DT_27_3         12
#define DT_47P          13
#define DT_47N          14
#define DT_25           15
#define DT_50M          16
#define DT_37           17
#define DT_46           18
#define DT_27M          19
#define DT_49           20
#define DT_50H          21
#define DT_SL           22
#define DT_66           23
#define DT_51LR         24
#define DT_48           25

#define WAVE_OCR50_1_BIT  0x0001
#define WAVE_OCR50_2_BIT  0x0002
#define WAVE_OCR51_1_BIT  0x0004
#define WAVE_OCR51_2_BIT  0x0008
#define WAVE_OCGR50_BIT   0x0010
#define WAVE_OCGR51_BIT   0x0020
#define WAVE_DGR_BIT      0x0040
#define WAVE_SGR_BIT      0x0080
#define WAVE_OVR_BIT      0x0100
#define WAVE_OVGR_BIT     0x0200
#define WAVE_UVR_1_BIT    0x0400
#define WAVE_UVR_2_BIT    0x0800
#define WAVE_UVR_3_BIT    0x1000
#define WAVE_47P_BIT      0x2000
#define WAVE_47N_BIT      0x4000
#define WAVE_25_BIT       0x8000

#define WAVE_UCR_BIT      0x0200
#define WAVE_NSOCR_BIT    0x0200
#define WAVE_UVR_M_BIT    0x0400
#define WAVE_THR_BIT      0x0800
#define WAVE_50H_BIT      0x1000
#define WAVE_SL_BIT       0x2000
#define WAVE_NCH_BIT      0x4000

#define OCR50_1_SET_EVENT   0x00010000L
#define OCR50_2_SET_EVENT   0x00020000L
#define OCR51_1_SET_EVENT   0x00030000L
#define OCR51_2_SET_EVENT   0x00040000L
#define OCGR50_SET_EVENT    0x00050000L
#define OCGR51_SET_EVENT    0x00060000L
#define DGR_SET_EVENT       0x00070000L
#define SGR_SET_EVENT       0x00080000L
#define OVR_SET_EVENT       0x00090000L
#define OVGR_SET_EVENT      0x000a0000L
#define UVR_1_SET_EVENT     0x000b0000L
#define UVR_2_SET_EVENT     0x000c0000L
#define UVR_3_SET_EVENT     0x000d0000L
#define P47_SET_EVENT       0x000e0000L
#define N47_SET_EVENT       0x000f0000L
#define SYNCRO_SET_EVENT    0x00100000L
#define M_50_SET_EVENT      0x00110000L
#define UCR_SET_EVENT       0x00120000L
#define NSOCR_SET_EVENT     0x00130000L
#define M_27_SET_EVENT      0x00140000L
#define THR_SET_EVENT       0x00150000L
#define H50_SET_EVENT       0x00160000L
#define SL_SET_EVENT        0x00170000L
#define NCH_SET_EVENT       0x00180000L
#define RELAY_ONOFF_EVENT   0x00200000L

#define CT_PT_SET_EVENT       0x00000000L
#define DI_DEBOUNCE_EVENT     0x00010000L
#define DO_PROPERTY_EVENT     0x00020000L
#define SUPERVISION_SET_EVENT 0x00030000L
#define PASSWORD_SET_EVENT    0x00040000L
#define TIME_SET_EVENT        0x00050000L
#define COMM_SET_EVENT        0x00060000L
#define L_CONTROL_SET_EVENT   0x00070000L
#define GR_SET_EVENT          0x00080000L
#define ZCT_ANGLE_SET_EVENT   0x00090000L
#define OCR_MODE_SET_EVENT    0x000a0000L
#define CAPTUR_EVENT          0x000b0000L
#define DO_TEST_EVENT         0x000c0000L

#define EVENT_RESET_EVENT   0x00000000L
#define ENERGY_RESET_EVENT  0x00010000L
#define Vo_RESET_EVENT      0x00020000L
#define Io_RESET_EVENT      0x00030000L
#define RHOUR_RESET_EVENT   0x00040000L

#define ACK_KEY_EVENT   0x00000000L
#define REMOTE_EVENT    0x00010000L
#define LOCAL_EVENT     0x00020000L

#define L_CB_OFF_EVENT   0x00000000L
#define L_CB_ON_EVENT    0x00010000L
#define R_CB_OFF_EVENT   0x00020000L
#define R_CB_ON_EVENT    0x00020000L

///////////////////////////////////////////////////////////////////
//  key, led, lcd 운용 값들
///////////////////////////////////////////////////////////////////
#define DISPLAY_KEY   0x001
#define SET_KEY       0x002
#define ACK_KEY       0x004
#define LEFT_KEY      0x008
#define RIGHT_KEY     0x010
#define UP_KEY        0x020
#define DOWN_KEY      0x040
#define ENTER_KEY     0x080
#define CB_ON_KEY     0x100
#define CB_OFF_KEY    0x200
#define REMOTE_KEY    0x400
#define LOCAL_KEY     0x800
#define FACTORY_KEY		0x007	//2015.02.17

#define FAULT_LED     0x001
#define OP_LED        0x002
#define SYS_FAIL_LED  0x004
#define CB_CLOSE_LED  0x008
#define CB_OPEN_LED   0x010
#define REMOTE_LED    0x020
#define LOCAL_LED     0x040


#define LCD_CLEAR     0x01
#define LCD_ON        0x0c
#define LCD_OFF       0x08
#define LCD_CURSOR_ON 0x0a
#define LCD_CURSOR_OFF 0x08
#define LCD_BLINK_ON  0x09
#define LCD_BLINK_OFF 0x08

#define LCD_L1_00  0x80
#define LCD_L1_01  0x81
#define LCD_L1_02  0x82
#define LCD_L1_03  0x83
#define LCD_L1_04  0x84
#define LCD_L1_05  0x85
#define LCD_L1_06  0x86
#define LCD_L1_07  0x87
#define LCD_L1_08  0x88
#define LCD_L1_09  0x89
#define LCD_L1_10  0x8a
#define LCD_L1_11  0x8b
#define LCD_L1_12  0x8c
#define LCD_L1_13  0x8d
#define LCD_L1_14  0x8e
#define LCD_L1_15  0x8f
#define LCD_L1_16  0x90
#define LCD_L1_17  0x91
#define LCD_L1_18  0x92
#define LCD_L1_19  0x93

#define LCD_L2_00  0xc0
#define LCD_L2_01  0xc1
#define LCD_L2_02  0xc2
#define LCD_L2_03  0xc3
#define LCD_L2_04  0xc4
#define LCD_L2_05  0xc5
#define LCD_L2_06  0xc6
#define LCD_L2_07  0xc7
#define LCD_L2_08  0xc8
#define LCD_L2_09  0xc9
#define LCD_L2_10  0xca
#define LCD_L2_11  0xcb
#define LCD_L2_12  0xcc
#define LCD_L2_13  0xcd
#define LCD_L2_14  0xce
#define LCD_L2_15  0xcf
#define LCD_L2_16  0xd0
#define LCD_L2_17  0xd1
#define LCD_L2_18  0xd2
#define LCD_L2_19  0xd3

#define LCD_BACKLIGHT_ON  *GpioIntRegs_GPASET_High |= 0x0200
#define LCD_BACKLIGHT_OFF *GpioIntRegs_GPACLEAR_High |= 0x0200
//// khs add
#define LCD_DISPLAY_L_SHIFT  0x10	//전체 화면을 왼쪽으로 쉬프트 시켜준다.
#define LCD_DISPLAY_R_SHIFT  0x14	//전체 화면을 오른쪽으로 쉬프트 시켜준다.
#define LCD_CURSOR_L_SHIFT  0x18	//커서를 왼쪽으로 쉬프트 시켜준다.
#define LCD_CURSOR_R_SHIFT  0x1c	//커서를 오른쪽으로 쉬프트 시켜준다.

/*** For LCD display Char ***/
//2006.4.6
#define	UPARROW			0x01
#define	DOWNARROW		0x02
#define	UPDOWNARROW	0x03
#define	ENTER				0x04
//#define PERCENT			0x05
#define	ANGLE				0x05
#define	THETA				0x06
#define THAU				0x07
#define FULLDOT			0x08

#define LEFTARROW		0x7F
#define RIGHTARROW	0x7E
#define DGREE				0xDF

/*** Menu Index Offset for Setting ***/
#define	SET_MENU_INDEX		30

///////////////////////////////////////////////////////////////////
//
//  외부 peripheral 주소
//
///////////////////////////////////////////////////////////////////
#define KEY_CS						(unsigned int *)0x107002
#define LED_CS						(unsigned int *)0x109002
#define LCD_CS						(unsigned int *)0x10a000

// AD DPRAM 0x4000 ~ 0x47ff
#define AD_buffer_low			(int *)0x4000 
#define AD_buffer_high		(int *)0x4030

// COMM     0x4800 ~ 0x4fff, 8bit
#define COMM_BOOT					(unsigned int *)0x4800
#define COMM_2_DSP				(unsigned int *)0x4801
#define COMM_2_DSP2COMM		(unsigned int *)0x4802

#define COMM_2_ADDRESS		(unsigned int *)0x4806
#define COMM_2_BAUDRATE		(unsigned int *)0x4807
#define COMM_2_MODBUS_RECV_CNT		(unsigned int *)0x4808


#define COMM_2_DATA1			(unsigned int *)0x4ABC

#define COMM2_IA				         (unsigned int *)(0x4800 + 700 + 0 )      //0x4ABC
#define COMM2_IB                 (unsigned int *)(0x4800 + 700 + 4 )      //0x4AC0
#define COMM2_IC                 (unsigned int *)(0x4800 + 700 + 8 )      //0x4AC4
#define COMM2_IN                 (unsigned int *)(0x4800 + 700 + 12)      //0x4AC8
#define COMM2_IAVG               (unsigned int *)(0x4800 + 700 + 16)      //0x4ACC
#define COMM2_IPS                (unsigned int *)(0x4800 + 700 + 20)      //0x4AD0
#define COMM2_INS                (unsigned int *)(0x4800 + 700 + 24)      //0x4AD4
#define COMM2_IA_3RD             (unsigned int *)(0x4800 + 700 + 28)      //0x4AD8
#define COMM2_IA_5TH             (unsigned int *)(0x4800 + 700 + 30)      //0x4ADA
#define COMM2_IA_7TH             (unsigned int *)(0x4800 + 700 + 32)      //0x4ADC
#define COMM2_IA_9TH             (unsigned int *)(0x4800 + 700 + 34)      //0x4ADE
#define COMM2_IA_11TH            (unsigned int *)(0x4800 + 700 + 36)      //0x4AE0
#define COMM2_IA_THDFACTER       (unsigned int *)(0x4800 + 700 + 38)      //0x4AE2
#define COMM2_IA_TDDFACTER       (unsigned int *)(0x4800 + 700 + 40)      //0x4AE4
#define COMM2_IB_3RD             (unsigned int *)(0x4800 + 700 + 42)      //0x4AE6
#define COMM2_IB_5TH             (unsigned int *)(0x4800 + 700 + 44)      //0x4AE8
#define COMM2_IB_7TH             (unsigned int *)(0x4800 + 700 + 46)      //0x4AEA
#define COMM2_IB_9TH             (unsigned int *)(0x4800 + 700 + 48)      //0x4AEC
#define COMM2_IB_11TH            (unsigned int *)(0x4800 + 700 + 50)      //0x4AEE
#define COMM2_IB_THDFACTER       (unsigned int *)(0x4800 + 700 + 52)      //0x4AF0
#define COMM2_IB_TDDFACTER       (unsigned int *)(0x4800 + 700 + 54)      //0x4AF2
#define COMM2_IC_3RD             (unsigned int *)(0x4800 + 700 + 56)      //0x4AF4
#define COMM2_IC_5TH             (unsigned int *)(0x4800 + 700 + 58)      //0x4AF6
#define COMM2_IC_7TH             (unsigned int *)(0x4800 + 700 + 60)      //0x4AF8
#define COMM2_IC_9TH             (unsigned int *)(0x4800 + 700 + 62)      //0x4AFA
#define COMM2_IC_11TH            (unsigned int *)(0x4800 + 700 + 64)      //0x4AFC
#define COMM2_IC_THDFACTER       (unsigned int *)(0x4800 + 700 + 66)      //0x4AFE
#define COMM2_IC_TDDFACTER       (unsigned int *)(0x4800 + 700 + 68)      //0x4B00
#define COMM2_VA                 (unsigned int *)(0x4800 + 700 + 70 )     //0x4B02
#define COMM2_VB                 (unsigned int *)(0x4800 + 700 + 74 )     //0x4B06
#define COMM2_VC                 (unsigned int *)(0x4800 + 700 + 78 )     //0x4B0A
#define COMM2_VO                 (unsigned int *)(0x4800 + 700 + 82 )     //0x4B0E
#define COMM2_VO_MAX             (unsigned int *)(0x4800 + 700 + 86 )     //0x4B12
#define COMM2_VAVG               (unsigned int *)(0x4800 + 700 + 90 )     //0x4B16
#define COMM2_VPS                (unsigned int *)(0x4800 + 700 + 94 )     //0x4B1A
#define COMM2_VNS                (unsigned int *)(0x4800 + 700 + 98 )     //0x4B1E
#define COMM2_ACTIVE_PWR           		(unsigned int *)(0x4800 + 700 + 102)//0x4B22
#define COMM2_REACTIVE_PWR           	(unsigned int *)(0x4800 + 700 + 106)//0x4B26
#define COMM2_POWER_FACTOR       			(unsigned int *)(0x4800 + 700 + 110)//0x4B2A
#define COMM2_FREQUENCY          			(unsigned int *)(0x4800 + 700 + 114)//0x4B2E
#define COMM2_ACTIVE_ENERGY         	(unsigned int *)(0x4800 + 700 + 118)//0x4B32
#define COMM2_REACTIVE_ENERGY         (unsigned int *)(0x4800 + 700 + 122)//0x4B36
#define COMM2_REVERSE_ENGERY       		(unsigned int *)(0x4800 + 700 + 126)//0x4B3A
#define COMM2_REVERSE_REACTIVE_ENGERY (unsigned int *)(0x4800 + 700 + 130)//0x4B3E

#define COMM2_DI_AND_CB_STATUS				(unsigned int *)(0x4800 + 700 + 160)//0x4B5C
#define COMM2_REALY_STATUS						(unsigned int *)(0x4800 + 700 + 162)//0x4B5E
#define COMM2_FAULT_PHASE_INFO				(unsigned int *)(0x4800 + 700 + 164)//0x4B60
#define COMM2_FAULT_RATIO_INFO				(unsigned int *)(0x4800 + 700 + 166)//0x4B62
#define COMM2_RELAY_OPERATION_TIME		(unsigned int *)(0x4800 + 700 + 168)//0x4B64
#define COMM2_RUNNING_HOUR_METER			(unsigned int *)(0x4800 + 700 + 172)//0x4B68


#define COMM_2_Ia         (unsigned int *)0x4ABC
#define COMM_2_Ib         (unsigned int *)0x4Ac0
#define COMM_2_Ic         (unsigned int *)0x4Ac4
#define COMM_2_Io         (unsigned int *)0x4Ac8
#define COMM_2_Ips        (unsigned int *)0x4Ad0
#define COMM_2_Ins        (unsigned int *)0x4Ad4
#define COMM_2_Ia_3rd     (unsigned int *)0x4Ad8
#define COMM_2_Ia_5th     (unsigned int *)0x4Adc
#define COMM_2_Ia_7th     (unsigned int *)0x4Ae0
#define COMM_2_Ia_9th     (unsigned int *)0x4Ae4
#define COMM_2_Ia_thd     (unsigned int *)0x4Ae8
#define COMM_2_Ia_tdd     (unsigned int *)0x4Aec
#define COMM_2_Ib_3rd     (unsigned int *)0x4Af0
#define COMM_2_Ib_5th     (unsigned int *)0x4Af4
#define COMM_2_Ib_7th     (unsigned int *)0x4Af8
#define COMM_2_Ib_9th     (unsigned int *)0x4Afc
#define COMM_2_Ib_thd     (unsigned int *)0x4b00
#define COMM_2_Ib_tdd     (unsigned int *)0x4b04
#define COMM_2_Ic_3rd     (unsigned int *)0x4b08
#define COMM_2_Ic_5th     (unsigned int *)0x4b0c
#define COMM_2_Ic_7th     (unsigned int *)0x4b10
#define COMM_2_Ic_9th     (unsigned int *)0x4b14
#define COMM_2_Ic_thd     (unsigned int *)0x4b18
#define COMM_2_Ic_tdd     (unsigned int *)0x4b1c
#define COMM_2_Vab        (unsigned int *)0x4b20
#define COMM_2_Vbc        (unsigned int *)0x4b24
#define COMM_2_Vca        (unsigned int *)0x4b28
#define COMM_2_Va         (unsigned int *)0x4b2c
#define COMM_2_Vb         (unsigned int *)0x4b30
#define COMM_2_Vc         (unsigned int *)0x4b34
#define COMM_2_Vo         (unsigned int *)0x4b38
#define COMM_2_Vo_max     (unsigned int *)0x4b3c
#define COMM_2_Vps        (unsigned int *)0x4b40
#define COMM_2_Vns        (unsigned int *)0x4b44
#define COMM_2_Vb_degree  (unsigned int *)0x4b48
#define COMM_2_Vc_degree  (unsigned int *)0x4b4c
#define COMM_2_Ia_degree  (unsigned int *)0x4b50
#define COMM_2_Ib_degree  (unsigned int *)0x4b54
#define COMM_2_Ic_degree  (unsigned int *)0x4b58
#define COMM_2_pf         (unsigned int *)0x4b5c
#define COMM_2_frequency  (unsigned int *)0x4b60
#define COMM_2_P          (unsigned int *)0x4b64
#define COMM_2_Q          (unsigned int *)0x4b68
#define COMM_2_PE         (unsigned int *)0x4b6c
#define COMM_2_QE         (unsigned int *)0x4b70
#define COMM_2_PEr        (unsigned int *)0x4b74
#define COMM_2_QEr        (unsigned int *)0x4b78
#define COMM_2_Vd         (unsigned int *)0x4b7c
#define COMM_2_Dd         (unsigned int *)0x4b80
#define COMM_2_Fd         (unsigned int *)0x4b84
#define COMM_2_thermal    (unsigned int *)0x4b88
#define COMM_2_DI         (unsigned int *)0x4b8c
#define COMM_2_DO1        (unsigned int *)0x4b8d
#define COMM_2_DO2        (unsigned int *)0x4b8e
#define COMM_2_CB         (unsigned int *)0x4b8f
#define COMM_2_relay1     (unsigned int *)0x4b90
#define COMM_2_relay2     (unsigned int *)0x4b91
#define COMM_2_fault      (unsigned int *)0x4b92
#define COMM_2_fault_ratio (unsigned int *)0x4b93
#define COMM_2_fault_time (unsigned int *)0x4b97
#define COMM_2_runhour    (unsigned int *)0x4b9b
#define COMM_2_SP         (unsigned int *)0x4b9f

// EVENT 저장 8bit임
#define FRAM_START				(unsigned int *)0x120000

#define WAVE_SAVE_OFFSET	0x18100	//0x1d020
#define EVENT_ROLLOVER		(unsigned int *)(0x140000 + WAVE_SAVE_OFFSET)		//0x21D010 (in FLASH Memory)
#define EVENT_SP					(unsigned int *)(0x140001 + WAVE_SAVE_OFFSET)

#define EVENT_YEAR        (unsigned int *)(0x140002 + WAVE_SAVE_OFFSET)
#define EVENT_MONTH       (unsigned int *)(0x140003 + WAVE_SAVE_OFFSET)
#define EVENT_DAY         (unsigned int *)(0x140004 + WAVE_SAVE_OFFSET)
#define EVENT_HOUR        (unsigned int *)(0x140005 + WAVE_SAVE_OFFSET)
#define EVENT_MINUTE      (unsigned int *)(0x140006 + WAVE_SAVE_OFFSET)
#define EVENT_SECOND      (unsigned int *)(0x140007 + WAVE_SAVE_OFFSET)
#define EVENT_MS1         (unsigned int *)(0x140008 + WAVE_SAVE_OFFSET)
#define EVENT_MS2         (unsigned int *)(0x140009 + WAVE_SAVE_OFFSET)
#define EVENT_INDEX1      (unsigned int *)(0x14000a + WAVE_SAVE_OFFSET)
#define EVENT_INDEX2      (unsigned int *)(0x14000b + WAVE_SAVE_OFFSET)
#define EVENT_CONTENT1    (unsigned int *)(0x14000c + WAVE_SAVE_OFFSET)
#define EVENT_CONTENT2    (unsigned int *)(0x14000d + WAVE_SAVE_OFFSET)
#define EVENT_RATIO1      (unsigned int *)(0x14000e + WAVE_SAVE_OFFSET)
#define EVENT_RATIO2      (unsigned int *)(0x14000f + WAVE_SAVE_OFFSET)
#define EVENT_OPTIME1     (unsigned int *)(0x140010 + WAVE_SAVE_OFFSET)
#define EVENT_OPTIME2     (unsigned int *)(0x140011 + WAVE_SAVE_OFFSET)
#define EVENT_OPTIME3     (unsigned int *)(0x140012 + WAVE_SAVE_OFFSET)
#define EVENT_OPTIME4     (unsigned int *)(0x140013 + WAVE_SAVE_OFFSET)

#define MRAM_START				(unsigned int *)0x140000	//khs, 요것이 MRAM


#define MRAM_RUNNING_HOUR1	(unsigned int *)0x158000	//RUNNING HOUR METER MRAM 주소
#define MRAM_RUNNING_HOUR2	(unsigned int *)0x158001	//RUNNING HOUR METER MRAM 주소
#define MRAM_Vo_MAX1				(unsigned int *)0x158002	//Vo MAX MRAM 주소
#define MRAM_Vo_MAX2				(unsigned int *)0x158003	//Vo MAX MRAM 주소
#define MRAM_CHECK				(unsigned int *)0x17FFF0	//MRAM 상시체크 주소

//누적값
//유효전력
#define EP1             (unsigned int *)0x120f00
#define EP2             (unsigned int *)0x120f01
#define EP3             (unsigned int *)0x120f02
#define EP4             (unsigned int *)0x120f03

//무효전력
#define EQ1             (unsigned int *)0x120f04
#define EQ2             (unsigned int *)0x120f05
#define EQ3             (unsigned int *)0x120f06
#define EQ4             (unsigned int *)0x120f07

//역방향유효전력
#define REP1            (unsigned int *)0x120f08
#define REP2            (unsigned int *)0x120f09
#define REP3            (unsigned int *)0x120f0a
#define REP4            (unsigned int *)0x120f0b

//역방향무효전력
#define REQ1            (unsigned int *)0x120f0c
#define REQ2            (unsigned int *)0x120f0d
#define REQ3            (unsigned int *)0x120f0e
#define REQ4            (unsigned int *)0x120f0f

//최대영상전압
#define VoMAX1          (unsigned int *)0x120f10
#define VoMAX2          (unsigned int *)0x120f11
#define VoMAX3          (unsigned int *)0x120f12
#define VoMAX4          (unsigned int *)0x120f13

//최대영상전류
#define IoMAX1          (unsigned int *)0x120f14
#define IoMAX2          (unsigned int *)0x120f15
#define IoMAX3          (unsigned int *)0x120f16
#define IoMAX4          (unsigned int *)0x120f17

#define CB_CLOSE_TIME1	(unsigned int *)0x120f18
#define CB_CLOSE_TIME2	(unsigned int *)0x120f19

#define REMOTE_LOCAL		(unsigned int *)0x120f80

//-------- FLASH MEMORY ADDRESS(SETTING VALUE)
#define FLASH_5555			(unsigned int *)0x185555
#define FLASH_2aaa			(unsigned int *)0x182aaa

#define OCR50_1_USE     (unsigned int *)0x180000  //50-1
#define OCR50_1_MODE    (unsigned int *)0x180001
#define OCR50_1_I       (unsigned int *)0x180002
#define OCR50_1_DELAY   (unsigned int *)0x180003
#define OCR50_1_DO      (unsigned int *)0x180004
#define OCR50_1_CRC     (unsigned int *)0x180005

#define OCR50_2_USE     (unsigned int *)0x180800  //50-2
#define OCR50_2_MODE    (unsigned int *)0x180801
#define OCR50_2_I       (unsigned int *)0x180802
#define OCR50_2_DELAY   (unsigned int *)0x180803
#define OCR50_2_DO      (unsigned int *)0x180804
#define OCR50_2_CRC     (unsigned int *)0x180805

#define OCR51_1_USE     (unsigned int *)0x181000  //51-1
#define OCR51_1_MODE    (unsigned int *)0x181001
#define OCR51_1_I       (unsigned int *)0x181002
#define OCR51_1_LEVER   (unsigned int *)0x181003
#define OCR51_1_DO      (unsigned int *)0x181004
#define OCR51_1_CRC     (unsigned int *)0x181005

#define OCR51_2_USE     (unsigned int *)0x181800  //51-2
#define OCR51_2_MODE    (unsigned int *)0x181801
#define OCR51_2_I       (unsigned int *)0x181802
#define OCR51_2_LEVER   (unsigned int *)0x181803
#define OCR51_2_DO      (unsigned int *)0x181804
#define OCR51_2_CRC     (unsigned int *)0x181805

#define OCGR50_USE      (unsigned int *)0x182000  //50G
#define OCGR50_MODE     (unsigned int *)0x182001
#define OCGR50_Io       (unsigned int *)0x182002
#define OCGR50_DELAY    (unsigned int *)0x182003
#define OCGR50_DO       (unsigned int *)0x182004
#define OCGR50_CRC      (unsigned int *)0x182005

#define OCGR51_USE      (unsigned int *)0x182800  //51G
#define OCGR51_MODE     (unsigned int *)0x182801
#define OCGR51_Io       (unsigned int *)0x182802
#define OCGR51_LEVER    (unsigned int *)0x182803
#define OCGR51_DO       (unsigned int *)0x182804
#define OCGR51_CRC      (unsigned int *)0x182805

#define UVR_1_USE       (unsigned int *)0x183000  //27R
#define UVR_1_V         (unsigned int *)0x183001
#define UVR_1_DELAY     (unsigned int *)0x183002
#define UVR_1_MODE			(unsigned int *)0x183003
#define UVR_1_DO        (unsigned int *)0x183004
#define UVR_1_CRC       (unsigned int *)0x183004

#define UVR_2_USE       (unsigned int *)0x183800  //27M
#define UVR_2_V         (unsigned int *)0x183801
#define UVR_2_DELAY     (unsigned int *)0x183802
#define UVR_2_MODE			(unsigned int *)0x183803
#define UVR_2_DO        (unsigned int *)0x183804
#define UVR_2_CRC       (unsigned int *)0x183804

#define UVR_3_USE       (unsigned int *)0x184000  //27S
#define UVR_3_V         (unsigned int *)0x184001
#define UVR_3_DELAY     (unsigned int *)0x184002
#define UVR_3_MODE			(unsigned int *)0x184003
#define UVR_3_DO        (unsigned int *)0x184004
#define UVR_3_CRC       (unsigned int *)0x184004

#define P47_USE					(unsigned int *)0x184800  //47P
#define P47_V						(unsigned int *)0x184801
#define P47_DELAY				(unsigned int *)0x184802
#define P47_DO        	(unsigned int *)0x184803
#define P47_CRC       	(unsigned int *)0x184804

#define N47_USE					(unsigned int *)0x185000  //47N
#define N47_V						(unsigned int *)0x185001
#define N47_DELAY				(unsigned int *)0x185002
#define N47_DO        	(unsigned int *)0x185003
#define N47_CRC       	(unsigned int *)0x185004

#define OVR_USE         (unsigned int *)0x185800	//OVR
#define OVR_MODE        (unsigned int *)0x185801
#define OVR_V           (unsigned int *)0x185802
#define OVR_LEVER       (unsigned int *)0x185803
#define OVR_DELAY       (unsigned int *)0x185804
#define OVR_DO          (unsigned int *)0x185805
#define OVR_CRC         (unsigned int *)0x185806

#define OVGR_USE        (unsigned int *)0x186000	//OVGR
#define OVGR_MODE       (unsigned int *)0x186001
#define OVGR_Vo         (unsigned int *)0x186002
#define OVGR_LEVER      (unsigned int *)0x186003
#define OVGR_DO         (unsigned int *)0x186004
#define OVGR_CRC        (unsigned int *)0x186005

#define DGR_USE         (unsigned int *)0x186800  //DGR
#define DGR_Io          (unsigned int *)0x186801
#define DGR_Vo          (unsigned int *)0x186802
#define DGR_ANGLE       (unsigned int *)0x186803
#define DGR_DELAY       (unsigned int *)0x186804
#define DGR_DO          (unsigned int *)0x186805
#define DGR_CRC         (unsigned int *)0x186806

#define SGR_USE         (unsigned int *)0x187000  //SGR
#define SGR_Io          (unsigned int *)0x187001
#define SGR_Vo          (unsigned int *)0x187002
#define SGR_ANGLE       (unsigned int *)0x187003
#define SGR_DELAY       (unsigned int *)0x187004
#define SGR_DO          (unsigned int *)0x187005
#define SGR_CRC         (unsigned int *)0x187006

#define SYNCRO_USE      (unsigned int *)0x187800  //SYNCRO
#define SYNCRO_VOLTAGE  (unsigned int *)0x187801
#define SYNCRO_PHASE    (unsigned int *)0x187802
#define SYNCRO_FREQ     (unsigned int *)0x187803
#define SYNCRO_DELAY    (unsigned int *)0x187804
#define SYNCRO_DO       (unsigned int *)0x187805
#define SYNCRO_CRC      (unsigned int *)0x187806

// SYSTEM SECTOR
#define CT_PRIMARY      (unsigned int *)0x188000
#define NCT_PRIMARY     (unsigned int *)0x188001
#define PT_PRIMARY_HIGH	(unsigned int *)0x188002
#define PT_PRIMARY_LOW	(unsigned int *)0x188003
#define RATED_CURRENT   (unsigned int *)0x188004
#define CPT_CRC         (unsigned int *)0x188005

#define MOD_ADDR				(unsigned int *)0x188800
#define MOD_ADDR_CRC   	(unsigned int *)0x188801

#define AUTO_DISP_MODE	(unsigned int *)0x189000
#define AUTO_DISP_CRC		(unsigned int *)0x189001

#define PASSWORD1       (unsigned int *)0x189800
#define PASSWORD_CRC    (unsigned int *)0x189801

#define PT_SECONDARY    (unsigned int *)0x18A000
#define PT_TERTIARY    	(unsigned int *)0x18A001
#define GPT_CRC			    (unsigned int *)0x18A002

#define SUPERVISION_USE (unsigned int *)0x18A800
#define TC_TIME					(unsigned int *)0x18A801
#define TC_DO						(unsigned int *)0x18A802
#define CC_TIME					(unsigned int *)0x18A803
#define CC_DO						(unsigned int *)0x18A804
#define SUPERVISION_CRC (unsigned int *)0x18A805

#define LOCAL_CTRL_USE	(unsigned int *)0x18B000
#define CLOSE_TIME			(unsigned int *)0x18B001
#define OPEN_TIME				(unsigned int *)0x18B002
#define LOCAL_CTRL_CRC	(unsigned int *)0x18B003

#define DGR_ANGLE_COMP	(unsigned int *)0x18B800
#define SGR_ANGLE_COMP	(unsigned int *)0x18B801
#define DSGR_ANGLE_CRC	(unsigned int *)0x18B802

#define UVR27R_MODE			(unsigned int *)0x18C000
#define UVR27M_MODE			(unsigned int *)0x18C001
#define UVR27S_MODE			(unsigned int *)0x18C002
#define UVR27_MODE_CRC	(unsigned int *)0x18C003

#define DISP_3PHASE_USE	(unsigned int *)0x18C800
#define DISP_3PHASE_CRC	(unsigned int *)0x18C801

#define MODBUS_BAUDRATE	(unsigned int *)0x18D001
#define MODBUS_DELAY		(unsigned int *)0x18D002
#define MODBUS_CRC			(unsigned int *)0x18D003

#define OCR_MODE		    (unsigned int *)0x18D801
#define OCR_DI_NUMBER		(unsigned int *)0x18D802
#define OCR_MODE_CRC    (unsigned int *)0x18D803

#define SYSTEM_ALARM_USE	(unsigned int *)0x18E000
#define SYSTEM_ALARM_DO		(unsigned int *)0x18E001
#define SYSTEM_ALARM_CRC	(unsigned int *)0x18E002

// CORE SECTOR
#define RATED_Hz        (unsigned int *)0x190000
#define RATED_CT        (unsigned int *)0x190001
#define GR_SELECT       (unsigned int *)0x190002
#define CORE_CRC        (unsigned int *)0x190003

//khs, 2015-04-08 오전 11:37:13
#define DI_DEBOUNCE1    (unsigned int *)0x190800
#define DI_DEBOUNCE2    (unsigned int *)0x190801
#define DI_DEBOUNCE3    (unsigned int *)0x190802
#define DI_DEBOUNCE4    (unsigned int *)0x190803
#define DI_DEBOUNCE5    (unsigned int *)0x190804
#define DI_DEBOUNCE6    (unsigned int *)0x190805
#define DI_DEBOUNCE7    (unsigned int *)0x190806
#define DI_DEBOUNCE8    (unsigned int *)0x190807
#define DI_CRC       		(unsigned int *)0x190808

#define DO_PROPERTY     (unsigned int *)0x191000
#define DO_CRC       		(unsigned int *)0x191001

//#define SUPERV_MODE     (unsigned int *)0x189000
//#define TCS_TIME        (unsigned int *)0x189001
//#define TCS_RELAY       (unsigned int *)0x189002
//#define CCS_TIME        (unsigned int *)0x189003
//#define CCS_RELAY       (unsigned int *)0x189004

#define COMM_ADDRESS    (unsigned int *)0x18a000
#define COMM_BAUDRATE   (unsigned int *)0x18a001
//#define COMM_CRC        (unsigned int *)0x18a002 //이거 열면 에러가 엄청나게 발생
//-------- FLASH MEMORY ADDRESS(SETTING VALUE) END

//-------- 이전 FLASH MEMORY ADDRESS(MRAM으로 수정 필요)
// Flash
//#define FLASH_SECTOR000 (unsigned int *)0x180000
////#define FLASH_WAVE_Ia   (unsigned int *)0x140000	
////#define FLASH_WAVE_Ib   (unsigned int *)0x142a30	
////#define FLASH_WAVE_Ic   (unsigned int *)0x145460
////#define FLASH_WAVE_In   (unsigned int *)0x147e90
////#define FLASH_WAVE_Va		(unsigned int *)0x14fd20
////#define FLASH_WAVE_Vb   (unsigned int *)0x152750
////#define FLASH_WAVE_Vc   (unsigned int *)0x155180
////#define FLASH_WAVE_Vn   (unsigned int *)0x157bb0
////#define FLASH_WAVE_RELAY (unsigned int *)0x15a5e0
////#define FLASH_WAVE_DI   (unsigned int *)0x15B3F0
////#define FLASH_WAVE_DO   (unsigned int *)0x15C200

#define FLASH_WAVE_Ia   (unsigned int *)0x140000		//전류는 4종류만 보기 때문에 SRAM 주소와 다르게 설정되므로
#define FLASH_WAVE_Ib   (unsigned int *)0x142a30		//혼돈하지 말 것. 특히 전압Va 시작 주소가 다르다.
#define FLASH_WAVE_Ic   (unsigned int *)0x145460
#define FLASH_WAVE_In   (unsigned int *)0x147e90
#define FLASH_WAVE_Va		(unsigned int *)0x14a8c0
#define FLASH_WAVE_Vb   (unsigned int *)0x14d2f0
#define FLASH_WAVE_Vc   (unsigned int *)0x14fd20
#define FLASH_WAVE_Vn   (unsigned int *)0x152750
#define FLASH_WAVE_RELAY (unsigned int *)0x155180		//Relay, DI, DO 정보는 3600 샘플데이타만 있다.
#define FLASH_WAVE_DI   (unsigned int *)0x155f90
#define FLASH_WAVE_DO   (unsigned int *)0x156da0

#define OFFSET_IA   		(unsigned int *)0x15d010
#define SCALE_FACTOR_IA (unsigned int *)0x15d011
#define OFFSET_IB   		(unsigned int *)0x15d013
#define SCALE_FACTOR_IB (unsigned int *)0x15d014
#define OFFSET_IC   		(unsigned int *)0x15d016
#define SCALE_FACTOR_IC (unsigned int *)0x15d017
#define OFFSET_IN   		(unsigned int *)0x15d019
#define SCALE_FACTOR_IN (unsigned int *)0x15d01A
#define OFFSET_VA   		(unsigned int *)0x15d01C
#define SCALE_FACTOR_VA (unsigned int *)0x15d01D
#define OFFSET_VB   		(unsigned int *)0x15d01F
#define SCALE_FACTOR_VB (unsigned int *)0x15d020
#define OFFSET_VC   		(unsigned int *)0x15d022
#define SCALE_FACTOR_VC (unsigned int *)0x15d023
#define OFFSET_VN   		(unsigned int *)0x15d025
#define SCALE_FACTOR_VN (unsigned int *)0x15d026

#define MODBUS_ADDR_IN_WAVE	(unsigned int *)0x15d028
#define RATED_FREQ_IN_WAVE	(unsigned int *)0x15d029

#define CT_RATIO_IN_WAVE		(unsigned int *)0x15d02A
#define PT_RATIO_IN_WAVE		(unsigned int *)0x15d02F

#define DATE_TIME_IN_WAVE		(unsigned int *)0x15d035
#define MSEC_IN_WAVE				(unsigned int *)0x15d037
#define TYPE_IN_WAVE				(unsigned int *)0x15d038
#define TRIP_TITLE_IN_WAVE	(unsigned int *)0x15d039
#define WAVE_WRITE_CHECK 		(unsigned int *)0x15d03A	//0x19AFFF //0x1d020 //0x15d011


#define FLASH_CHECK_SECTOR (unsigned int *)0x19e000
//-------- 이전 FLASH MEMORY ADDRESS END

//-------- SRAM1 MEMORY ADDRESS
/*
#define Pre_Ia_wave_buffer      (unsigned int *)0x200000 // 150주기 5400word 0x1518
#define Post_Ia_wave_buffer     (unsigned int *)0x201518 // 150주기
#define Pre_Ib_wave_buffer      (unsigned int *)0x202a30 // 150주기 5400word 0x1518
#define Post_Ib_wave_buffer     (unsigned int *)0x203f48 // 150주기
#define Pre_Ic_wave_buffer      (unsigned int *)0x205460 // 150주기
#define Post_Ic_wave_buffer     (unsigned int *)0x206978 // 150주기
#define Pre_In_wave_buffer      (unsigned int *)0x207e90 // 150주기
#define Post_In_wave_buffer     (unsigned int *)0x2093a8 // 150주기

#define Pre_Izct_wave_buffer    (unsigned int *)0x20d2f0 // 150주기
#define Post_Izct_wave_buffer   (unsigned int *)0x20e808 // 150주기


#define Pre_Va_wave_buffer      (unsigned int *)0x20a8c0 // 150주기
#define Post_Va_wave_buffer     (unsigned int *)0x211238 // 150주기
#define Pre_Vb_wave_buffer      (unsigned int *)0x20bdd8 // 150주기
#define Post_Vb_wave_buffer     (unsigned int *)0x20d2f0 // 150주기
#define Pre_Vc_wave_buffer      (unsigned int *)0x20e808 // 150주기
#define Post_Vc_wave_buffer     (unsigned int *)fd20 // 150주기
#define Pre_Vn_wave_buffer      (unsigned int *)0x211238 // 150주기
#define Post_Vn_wave_buffer     (unsigned int *)0x212750 // 150주기
#define Pre_relay_wave_buffer   (unsigned int *)0x213c68 // 150주기 -> 1800개 digital은 12개만
#define Post_relay_wave_buffer  (unsigned int *)0x215180 // 150주기 0x708
#define Pre_DI_wave_buffer      (unsigned int *)0x216698 // 150주기
#define Post_DI_wave_buffer     (unsigned int *)0x217bb0 // 150주기
#define Pre_DO_wave_buffer      (unsigned int *)0x2190c8 // 150주기
#define Post_DO_wave_buffer     (unsigned int *)0x21baf8 // 150주기
#define Wave_Information				(unsigned int *)0x21d010 //
*/
#define Pre_Ia_wave_buffer      (unsigned int *)0x200000L // 150주기 5400word 0x1518
#define Post_Ia_wave_buffer     (unsigned int *)(0x200000L + 0x1518L) 		// 150주기	, 0x201518
#define Pre_Ib_wave_buffer      (unsigned int *)(0x200000L + 0x1518L * 2L) // 150주기 5400word 0x1518, 0x202A30
#define Post_Ib_wave_buffer     (unsigned int *)(0x200000L + 0x1518L * 3L) // 150주기	, 0x203F48
#define Pre_Ic_wave_buffer      (unsigned int *)(0x200000L + 0x1518L * 4L) // 150주기	, 0x205460
#define Post_Ic_wave_buffer     (unsigned int *)(0x200000L + 0x1518L * 5L) // 150주기	, 0x206978
#define Pre_In_wave_buffer      (unsigned int *)(0x200000L + 0x1518L * 6L) // 150주기	, 0x207E90, Izct 도 같이 저장한다.
#define Post_In_wave_buffer     (unsigned int *)(0x200000L + 0x1518L * 7L) // 150주기	, 0x2093A8
#define Pre_Is_wave_buffer      (unsigned int *)(0x200000L + 0x1518L * 8L) // 150주기	, 0x20A8C0
#define Post_Is_wave_buffer     (unsigned int *)(0x200000L + 0x1518L * 9L) // 150주기	, 0x20BDD8
#define Pre_Izct_wave_buffer    (unsigned int *)(0x200000L + 0x1518L * 10L)// 150주기	, 0x20D2F0
#define Post_Izct_wave_buffer   (unsigned int *)(0x200000L + 0x1518L * 11L)// 150주기	, 0x20E808
#define Pre_Va_wave_buffer      (unsigned int *)(0x200000L + 0x1518L * 12L)// 150주기	, 0x20FD20
#define Post_Va_wave_buffer     (unsigned int *)(0x200000L + 0x1518L * 13L)// 150주기	, 0x211238
#define Pre_Vb_wave_buffer      (unsigned int *)(0x200000L + 0x1518L * 14L)// 150주기	, 0x212750
#define Post_Vb_wave_buffer     (unsigned int *)(0x200000L + 0x1518L * 15L)// 150주기	, 0x213C68
#define Pre_Vc_wave_buffer      (unsigned int *)(0x200000L + 0x1518L * 16L)// 150주기	, 0x215180
#define Post_Vc_wave_buffer     (unsigned int *)(0x200000L + 0x1518L * 17L)// 150주기	, 0x216698
#define Pre_Vn_wave_buffer      (unsigned int *)(0x200000L + 0x1518L * 18L)// 150주기	, 0x217BB0
#define Post_Vn_wave_buffer     (unsigned int *)(0x200000L + 0x1518L * 19L)// 150주기	, 0x2190C8

#define Pre_relay_wave_buffer   (unsigned int *)(0x200000L + 0x1518L * 20L)               // 150주기 -> 1800개 digital은 12개만	, 0x21A5E0
#define Post_relay_wave_buffer  (unsigned int *)(0x200000L + 0x1518L * 20L + 0x708L * 1L) // 150주기 0x708	, 0x21ACE8
#define Pre_DI_wave_buffer      (unsigned int *)(0x200000L + 0x1518L * 20L + 0x708L * 2L) // 150주기	, 0x21B3F0
#define Post_DI_wave_buffer     (unsigned int *)(0x200000L + 0x1518L * 20L + 0x708L * 3L) // 150주기	, 0x21BAF8
#define Pre_DO_wave_buffer      (unsigned int *)(0x200000L + 0x1518L * 20L + 0x708L * 4L) // 150주기	, 0x21C200
#define Post_DO_wave_buffer     (unsigned int *)(0x200000L + 0x1518L * 20L + 0x708L * 5L) // 150주기	, 0x21C908
#define Wave_Information				(unsigned int *)0x21d010 //

#define SRAM_HEALTH_CHECK     	(unsigned int *)0x220000
#define Manager_tx_long         (unsigned int *)0x230000 // 0x2200
#define Manager_tx_long_six     (unsigned int *)0x230006 // 0x2200
#define Manager_tx_long_seven   (unsigned int *)0x230007 // 0x2200
#define Manager_tx_long_eleven	(unsigned int *)0x23000b // 0x2200
#define Manager_tx_long_twelve  (unsigned int *)0x23000c // 0x2200
//-------- SRAM1 MEMORY ADDRESS END

#define IO_Control							(unsigned int *)0x117001
#define DI_CS										(unsigned int *)0x113001 

#define TLE6208_INH				*GpioIntRegs_GPBSET_High |= 0x0800
#define TLE6208_LOCK			*GpioIntRegs_GPBCLEAR_High |= 0x0800

#define TLE6208_CS_HIGH		*GpioIntRegs_GPBSET_High   |= 0x0200
#define TLE6208_CS_LOW		*GpioIntRegs_GPBCLEAR_High |= 0x0200

#define TLE6208_CLK_HIGH	*GpioIntRegs_GPBSET_High   |= 0x0100
#define TLE6208_CLK_LOW		*GpioIntRegs_GPBCLEAR_High   |= 0x0100

#define TLE6208_DI_HIGH		*GpioIntRegs_GPBSET_High   |= 0x0040
#define TLE6208_DI_LOW		*GpioIntRegs_GPBCLEAR_High   |= 0x0040

#define EEROM_CS_HIGH			*GpioIntRegs_GPASET_Low   |= 0x0020
#define EEROM_CS_LOW			*GpioIntRegs_GPACLEAR_Low   |= 0x0020

#define EEROM_CLK_HIGH		*GpioIntRegs_GPASET_Low   |= 0x0010
#define EEROM_CLK_LOW			*GpioIntRegs_GPACLEAR_Low   |= 0x0010

#define EEROM_DATA_HIGH		*GpioIntRegs_GPASET_Low   |= 0x0004
#define EEROM_DATA_LOW		*GpioIntRegs_GPACLEAR_Low   |= 0x0004

// 60Hz - 1, 50Hz - 0
#define SET_60Hz	*GpioIntRegs_GPASET_Low |= 0x1000
#define SET_50Hz	*GpioIntRegs_GPACLEAR_Low |= 0x1000

// serial flash handling
#define SFLASH_WD	*GpioIntRegs_GPASET_High |= 0x0800
#define SFLASH_WE	*GpioIntRegs_GPACLEAR_High |= 0x0800

#define SFLASH_CSD	*GpioIntRegs_GPASET_High |= 0x0008
#define SFLASH_CSE	*GpioIntRegs_GPACLEAR_High |= 0x0008

#define SFLASH_CLKH	*GpioIntRegs_GPASET_High |= 0x0004
#define SFLASH_CLKL	*GpioIntRegs_GPACLEAR_High |= 0x0004

#define SFLASH_DATH	*GpioIntRegs_GPASET_High |= 0x0001
#define SFLASH_DATL	*GpioIntRegs_GPACLEAR_High |= 0x0001

#define ebss_start (unsigned int *)0xc000 

///////////////////////////////////////////////////////////////
// DSP 운용에 관한 것
///////////////////////////////////////////////////////////////
// DO NOT MODIFY THIS LINE.
#define CPU_RATE    6.667L   // for a 150MHz CPU clock speed (SYSCLKOUT)
#define DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)

// interrupt pending masking
#define PIEACK_GROUP1   0x0001
#define PIEACK_GROUP2   0x0002
#define PIEACK_GROUP3   0x0004
#define PIEACK_GROUP4   0x0008
#define PIEACK_GROUP5   0x0010
#define PIEACK_GROUP6   0x0020
#define PIEACK_GROUP7   0x0040
#define PIEACK_GROUP8   0x0080
#define PIEACK_GROUP9   0x0100
#define PIEACK_GROUP10  0x0200
#define PIEACK_GROUP11  0x0400
#define PIEACK_GROUP12  0x0800

// DSP register address
#define FlashRegs_FOPT               (unsigned int *)0x0a80
#define FlashRegs_FSTDBYWAIT         (unsigned int *)0x0a84
#define FlashRegs_FACTIVEWAIT        (unsigned int *)0x0a85
#define FlashRegs_FBANKWAIT          (unsigned int *)0x0a86
#define FlashRegs_FOTPWAIT           (unsigned int *)0x0a87

#define XintfRegs_XTIMING0_Low       (unsigned int *)0x0b20
#define XintfRegs_XTIMING0_High      (unsigned int *)0x0b21
#define XintfRegs_XTIMING6_Low       (unsigned int *)0x0b2c
#define XintfRegs_XTIMING6_High      (unsigned int *)0x0b2d
#define XintfRegs_XTIMING7_Low       (unsigned int *)0x0b2e
#define XintfRegs_XTIMING7_High      (unsigned int *)0x0b2f
#define XintfRegs_XINTCNF2_Low       (unsigned int *)0x0b34
#define XintfRegs_XINTCNF2_High      (unsigned int *)0x0b35
#define XintfRegs_XBANK              (unsigned int *)0x0b38

#define CpuTimer0Regs_PRD            (unsigned int *)0x0c02
#define CpuTimer0Regs_PRDH           (unsigned int *)0x0c03
#define CpuTimer0Regs_TCR            (unsigned int *)0x0c04
#define CpuTimer0Regs_TPR            (unsigned int *)0x0c06
#define CpuTimer0Regs_TPRH           (unsigned int *)0x0c07

#define CpuTimer1Regs_PRD            (unsigned int *)0x0c0a
#define CpuTimer1Regs_PRDH           (unsigned int *)0x0c0b
#define CpuTimer1Regs_TCR            (unsigned int *)0x0c0c
#define CpuTimer1Regs_TPR            (unsigned int *)0x0c0e
#define CpuTimer1Regs_TPRH           (unsigned int *)0x0c0f

#define CpuTimer2Regs_PRD            (unsigned int *)0x0c12
#define CpuTimer2Regs_PRDH           (unsigned int *)0x0c13
#define CpuTimer2Regs_TCR            (unsigned int *)0x0c14
#define CpuTimer2Regs_TPR            (unsigned int *)0x0c16
#define CpuTimer2Regs_TPRH           (unsigned int *)0x0c17

#define PieCtrlRegs_PIECTRL          (unsigned int *)0x0ce0
#define PieCtrlRegs_PIEACK           (unsigned int *)0x0ce1
#define PieCtrlRegs_PIEIER1          (unsigned int *)0x0ce2
#define PieCtrlRegs_PIEIFR1          (unsigned int *)0x0ce3
#define PieCtrlRegs_PIEIER2          (unsigned int *)0x0ce4
#define PieCtrlRegs_PIEIFR2          (unsigned int *)0x0ce5
#define PieCtrlRegs_PIEIER3          (unsigned int *)0x0ce6
#define PieCtrlRegs_PIEIFR3          (unsigned int *)0x0ce7
#define PieCtrlRegs_PIEIER4          (unsigned int *)0x0ce8
#define PieCtrlRegs_PIEIFR4          (unsigned int *)0x0ce9
#define PieCtrlRegs_PIEIER5          (unsigned int *)0x0cea
#define PieCtrlRegs_PIEIFR5          (unsigned int *)0x0ceb
#define PieCtrlRegs_PIEIER6          (unsigned int *)0x0cec
#define PieCtrlRegs_PIEIFR6          (unsigned int *)0x0ced
#define PieCtrlRegs_PIEIER7          (unsigned int *)0x0cee
#define PieCtrlRegs_PIEIFR7          (unsigned int *)0x0cef
#define PieCtrlRegs_PIEIER8          (unsigned int *)0x0cf0
#define PieCtrlRegs_PIEIFR8          (unsigned int *)0x0cf1
#define PieCtrlRegs_PIEIER9          (unsigned int *)0x0cf2
#define PieCtrlRegs_PIEIFR9          (unsigned int *)0x0cf3
#define PieCtrlRegs_PIEIER10         (unsigned int *)0x0cf4
#define PieCtrlRegs_PIEIFR10         (unsigned int *)0x0cf5
#define PieCtrlRegs_PIEIER11         (unsigned int *)0x0cf6
#define PieCtrlRegs_PIEIFR11         (unsigned int *)0x0cf7
#define PieCtrlRegs_PIEIER12         (unsigned int *)0x0cf8
#define PieCtrlRegs_PIEIFR12         (unsigned int *)0x0cf9

#define GpioCtrlRegs_GPACTRL_Low     (unsigned int *)0x6f80
#define GpioCtrlRegs_GPACTRL_High    (unsigned int *)0x6f81
#define GpioCtrlRegs_GPAQSEL1_Low    (unsigned int *)0x6f82
#define GpioCtrlRegs_GPAQSEL1_High   (unsigned int *)0x6f83
#define GpioCtrlRegs_GPAQSEL2_Low    (unsigned int *)0x6f84
#define GpioCtrlRegs_GPAQSEL2_High   (unsigned int *)0x6f85
#define GpioCtrlRegs_GPAMUX1_Low     (unsigned int *)0x6f86
#define GpioCtrlRegs_GPAMUX1_High    (unsigned int *)0x6f87
#define GpioCtrlRegs_GPAMUX2_Low     (unsigned int *)0x6f88
#define GpioCtrlRegs_GPAMUX2_High    (unsigned int *)0x6f89
#define GpioCtrlRegs_GPADIR_Low      (unsigned int *)0x6f8a
#define GpioCtrlRegs_GPADIR_High     (unsigned int *)0x6f8b
#define GpioCtrlRegs_GPAPUD_Low      (unsigned int *)0x6f8c
#define GpioCtrlRegs_GPAPUD_High     (unsigned int *)0x6f8d

#define GpioCtrlRegs_GPBCTRL_Low     (unsigned int *)0x6f90
#define GpioCtrlRegs_GPBCTRL_High    (unsigned int *)0x6f91
#define GpioCtrlRegs_GPBQSEL1_Low    (unsigned int *)0x6f92
#define GpioCtrlRegs_GPBQSEL1_High   (unsigned int *)0x6f93
#define GpioCtrlRegs_GPBQSEL2_Low    (unsigned int *)0x6f94
#define GpioCtrlRegs_GPBQSEL2_High   (unsigned int *)0x6f95
#define GpioCtrlRegs_GPBMUX1_Low     (unsigned int *)0x6f96
#define GpioCtrlRegs_GPBMUX1_High    (unsigned int *)0x6f97
#define GpioCtrlRegs_GPBMUX2_Low     (unsigned int *)0x6f98
#define GpioCtrlRegs_GPBMUX2_High    (unsigned int *)0x6f99
#define GpioCtrlRegs_GPBDIR_Low      (unsigned int *)0x6f9a
#define GpioCtrlRegs_GPBDIR_High     (unsigned int *)0x6f9b
#define GpioCtrlRegs_GPBPUD_Low      (unsigned int *)0x6f9c
#define GpioCtrlRegs_GPBPUD_High     (unsigned int *)0x6f9d

#define GpioCtrlRegs_GPCMUX1_Low     (unsigned int *)0x6fa6
#define GpioCtrlRegs_GPCMUX1_High    (unsigned int *)0x6fa7
#define GpioCtrlRegs_GPCMUX2_Low     (unsigned int *)0x6fa8
#define GpioCtrlRegs_GPCMUX2_High    (unsigned int *)0x6fa9
#define GpioCtrlRegs_GPCDIR_Low      (unsigned int *)0x6faa
#define GpioCtrlRegs_GPCDIR_High     (unsigned int *)0x6fab
#define GpioCtrlRegs_GPCPUD_Low      (unsigned int *)0x6fac
#define GpioCtrlRegs_GPCPUD_High     (unsigned int *)0x6fad

#define GpioIntRegs_GPADAT_Low       (unsigned int *)0x6fc0
#define GpioIntRegs_GPADAT_High      (unsigned int *)0x6fc1
#define GpioIntRegs_GPASET_Low       (unsigned int *)0x6fc2
#define GpioIntRegs_GPASET_High      (unsigned int *)0x6fc3
#define GpioIntRegs_GPACLEAR_Low     (unsigned int *)0x6fc4
#define GpioIntRegs_GPACLEAR_High    (unsigned int *)0x6fc5
#define GpioIntRegs_GPATOGGLE_Low    (unsigned int *)0x6fc6
#define GpioIntRegs_GPATOGGLE_High   (unsigned int *)0x6fc7

#define GpioIntRegs_GPBDAT_Low       (unsigned int *)0x6fc8
#define GpioIntRegs_GPBDAT_High      (unsigned int *)0x6fc9
#define GpioIntRegs_GPBSET_Low       (unsigned int *)0x6fca
#define GpioIntRegs_GPBSET_High      (unsigned int *)0x6fcb
#define GpioIntRegs_GPBCLEAR_Low     (unsigned int *)0x6fcc
#define GpioIntRegs_GPBCLEAR_High    (unsigned int *)0x6fcd
#define GpioIntRegs_GPBTOGGLE_Low    (unsigned int *)0x6fce
#define GpioIntRegs_GPBTOGGLE_High   (unsigned int *)0x6fcf

#define GpioIntRegs_GPCDAT_Low       (unsigned int *)0x6fd0
#define GpioIntRegs_GPCDAT_High      (unsigned int *)0x6fd1
#define GpioIntRegs_GPCSET_Low       (unsigned int *)0x6fd2
#define GpioIntRegs_GPCSET_High      (unsigned int *)0x6fd3
#define GpioIntRegs_GPCCLEAR_Low     (unsigned int *)0x6fd4
#define GpioIntRegs_GPCCLEAR_High    (unsigned int *)0x6fd5
#define GpioIntRegs_GPCTOGGLE_Low    (unsigned int *)0x6fd6
#define GpioIntRegs_GPCTOGGLE_High   (unsigned int *)0x6fd7

#define GpioIntRegs_GPIOXINT1SEL     (unsigned int *)0x6fe0
#define GpioIntRegs_GPIOXINT2SEL     (unsigned int *)0x6fe1
#define GpioIntRegs_GPIOXNMISEL      (unsigned int *)0x6fe2
#define GpioIntRegs_GPIOXINT3SEL     (unsigned int *)0x6fe3
#define GpioIntRegs_GPIOXINT4SEL     (unsigned int *)0x6fe4
#define GpioIntRegs_GPIOXINT5SEL     (unsigned int *)0x6fe5
#define GpioIntRegs_GPIOXINT6SEL     (unsigned int *)0x6fe6
#define GpioIntRegs_GPIOXINT7SEL     (unsigned int *)0x6fe7
#define GpioIntRegs_GPIOLPMSEL       (unsigned int *)0x6fe8

#define SysCtrlRegs_PLLSTS           (unsigned int *)0x7011
#define SysCtrlRegs_HISPCP           (unsigned int *)0x701a
#define SysCtrlRegs_LOSPCP           (unsigned int *)0x701b
#define SysCtrlRegs_PCLKCR0          (unsigned int *)0x701c
#define SysCtrlRegs_PCLKCR1          (unsigned int *)0x701d
#define SysCtrlRegs_PCLKCR3          (unsigned int *)0x7020

#define SysCtrlRegs_PLLCR            (unsigned int *)0x7021
#define SysCtrlRegs_WDCR             (unsigned int *)0x7029

#define SpiaRegs_SPICCR              (unsigned int *)0x7040
#define SpiaRegs_SPICTL              (unsigned int *)0x7041
#define SpiaRegs_SPISTS              (unsigned int *)0x7042
#define SpiaRegs_SPIBRR              (unsigned int *)0x7044
#define SpiaRegs_SPIRXBUF            (unsigned int *)0x7047
#define SpiaRegs_SPITXBUF            (unsigned int *)0x7048

#define XIntruptRegs_XINT1CR         (unsigned int *)0x7070
#define XIntruptRegs_XINT2CR         (unsigned int *)0x7071
#define XIntruptRegs_XINT3CR         (unsigned int *)0x7072
#define XIntruptRegs_XINT4CR         (unsigned int *)0x7073
#define XIntruptRegs_XINT5CR         (unsigned int *)0x7074
#define XIntruptRegs_XINT6CR         (unsigned int *)0x7075
#define XIntruptRegs_XINT7CR         (unsigned int *)0x7076
#define XIntruptRegs_XNMICR          (unsigned int *)0x7077

#define ScibRegs_SCICCR              (unsigned int *)0x7750
#define ScibRegs_SCICTL1             (unsigned int *)0x7751
#define ScibRegs_SCIHBAUD            (unsigned int *)0x7752
#define ScibRegs_SCILBAUD            (unsigned int *)0x7753
#define ScibRegs_SCICTL2             (unsigned int *)0x7754
#define ScibRegs_SCIRXST             (unsigned int *)0x7755
#define ScibRegs_SCIRXBUF            (unsigned int *)0x7757
#define ScibRegs_SCITXBUF            (unsigned int *)0x7759
#define ScibRegs_SCIFFTX             (unsigned int *)0x775a
#define ScibRegs_SCIFFRX             (unsigned int *)0x775b
#define ScibRegs_SCIFFCT             (unsigned int *)0x775c

#define ScicRegs_SCICCR              (unsigned int *)0x7770
#define ScicRegs_SCICTL1             (unsigned int *)0x7771
#define ScicRegs_SCIHBAUD            (unsigned int *)0x7772
#define ScicRegs_SCILBAUD            (unsigned int *)0x7773
#define ScicRegs_SCICTL2             (unsigned int *)0x7774
#define ScicRegs_SCIRXST             (unsigned int *)0x7775
#define ScicRegs_SCIRXBUF            (unsigned int *)0x7777
#define ScicRegs_SCITXBUF            (unsigned int *)0x7779
#define ScicRegs_SCIFFTX             (unsigned int *)0x777a
#define ScicRegs_SCIFFRX             (unsigned int *)0x777b
#define ScicRegs_SCIFFCT             (unsigned int *)0x777c

#define I2caRegs_I2COAR              (unsigned int *)0x7900 // Own address register
#define I2caRegs_I2CIER              (unsigned int *)0x7901 // Interrupt enable
#define I2caRegs_I2CSTR              (unsigned int *)0x7902 // Interrupt status
#define I2caRegs_I2CCLKL             (unsigned int *)0x7903 // Clock divider low
#define I2caRegs_I2CCLKH             (unsigned int *)0x7904 // Clock divider high
#define I2caRegs_I2CCNT              (unsigned int *)0x7905 // Data count
#define I2caRegs_I2CDRR              (unsigned int *)0x7906 // Data recieve
#define I2caRegs_I2CSAR              (unsigned int *)0x7907 // Slave address
#define I2caRegs_I2CDXR              (unsigned int *)0x7908 // Data transmit
#define I2caRegs_I2CMDR              (unsigned int *)0x7909 // Mode
#define I2caRegs_I2CISRC             (unsigned int *)0x790a // Interrupt source
#define I2caRegs_I2CEMDR             (unsigned int *)0x790b // extended mode register
#define I2caRegs_I2CPSC              (unsigned int *)0x790c // Pre-scaler
#define I2caRegs_I2CFFTX             (unsigned int *)0x7920 // Transmit FIFO
#define I2caRegs_I2CFFRX             (unsigned int *)0x7921 // Recieve FIFO

#define  DEBUG_HIGH  *GpioIntRegs_GPASET_Low |= 0x0002
#define  DEBUG_LOW   *GpioIntRegs_GPACLEAR_Low |= 0x0002

#define	HIMAP_TYPE		1		//(FI, MS, ML, T = 1, 2, 3, 4)
#define	VERSION				700

#define NIC_RESET_OFF  *GpioIntRegs_GPASET_High |= 0x0400
#define NIC_RESET_ON *GpioIntRegs_GPACLEAR_High |= 0x0400

#define EVENT_TOTAL_COUNT		(200 - 1)//전체 이벤트 개수 총 200 개 일때

//#endif	//_DEFINE_H_

