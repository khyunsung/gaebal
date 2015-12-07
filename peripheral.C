#include "define.h"
#include "extern_variable.h"
#include "extern_prototype.h"

#define	CON			0
#define	DAT			1

/* ------------ LCD COMMAND --------------- */
#define		INIT_LCD	0x38
#define		CLEAR_LCD	0x01
#define		HOME_CUR	0x02
#define		CUR_LEFT	0x10
#define		CUR_RIGHT	0x14
#define		DISPLAY_LEFT	0x18
#define		DISPLAY_RIGHT	0x1c
/*------------- Cursor and Blink ---------- */
#define		C_OFFnNO_Blink 	0x0c
#define		C_ONnNO_Blink 	0x0e
#define		C_OFFnBlink 	0x0d
#define		C_ONnBlink 	0x0f

/*------------- DMA --------------*/
#pragma DATA_SECTION(DmaRegs,"DmaRegsFile");
volatile struct DMA_REGS DmaRegs;
#pragma DATA_SECTION(PieCtrlRegs,"PieCtrlRegsFile");
volatile struct PIE_CTRL_REGS PieCtrlRegs;


// LCD version
//int *LCD_CS 	= (int *)0x400020;
//int *LCD_CS	 	= (int *)0x400020;
int *LCD_RS	 	= (int *)0x400021;
int *LCD_LIGHT		= (int *)0x400022;
unsigned int Lcd_Bl_Count = 2;
//2006.4.6 END

/*  Macro Function Prototype  */
typedef unsigned char UBYTE;
typedef unsigned long ULONG;
ULONG VFD_cnt;

typedef struct{
	char up[30];
	char lo[30];
}SCRB;

typedef struct {
	SCRB *Cur[2];
}BufSel;

BufSel NorScr,FauScr,AckScr;
BufSel scrP,ReserveP;
SCRB *dumpP;
ULONG VFD_cnt;

SCRB VFDNBuffer={"                     "
		,"                     "};
SCRB VFDFBuffer={"                     "
		,"                     "};
SCRB VFDABuffer={"  [[[ Warning ! ]]]  \0"
		," Fault is not Reset. \0"};

const SCRB KHS_LCD_Buffer={"  [[[ Warning ! ]]]  \0"
		," Fault is not Reset. \0"};

void delay(unsigned long ar_delay)
{
	DELAY_US(ar_delay);
}

void delay_us(unsigned long ar_delay)
{
	DELAY_US(ar_delay);
}

void loop_delay(unsigned long ar_delay)
{
	while(ar_delay--);
}
//2006.4.6
/******************** LCD ***************************/
//2006.4.6
void LCD_DATA(unsigned char LCD_CH)
{
	*LCD_RS = 0xff;
	*LCD_RS = 0xff;
//	*LCD_RW = 0x00;
//delay(1000);	//2007.6.20
	loop_delay(500);		//2007.6.20
	*LCD_CS = LCD_CH;
}

//void LCD_COMMAND(unsigned char LCD_COM)
//{
//	*LCD_RS = 0x00;
//	*LCD_RS = 0x00;
////	*LCD_RW = 0x00;
////delay(1000);	//2007.6.20
//	delay(100);		//2007.6.20
//	*LCD_CS = LCD_COM;
//}
//
//void LCD_INIT(void)
//{
//	*LCD_LIGHT = 0xff;
//	delay(10000);
//	LCD_COMMAND(INIT_LCD);
//	delay(10000);
//	LCD_COMMAND(CLEAR_LCD);
//	delay(10000);
//	LCD_COMMAND(0x0f);
//	delay(10000);
//	LCD_COMMAND(HOME_CUR);
//	delay(10000);
//}
//
//void LCD_light_ON(void)
//{
//	*LCD_LIGHT = 0xff;
//}
//
//void LCD_light_OFF(void)
//{
//	*LCD_LIGHT = 0x00;
//}

/*******************************************/
//void VFD_gotopo(int posi)
//{
//	unsigned char posic;
//	posic = (posi/20)*0x40 +(posi%20);
//	posic |= 0x80;
//	LCD_COMMAND(posic);
//}
//
//void VFD_Init(void)
//{
//	NorScr.Cur[0]=&VFDNBuffer;
//	NorScr.Cur[1]=&VFDNBuffer;
//	FauScr.Cur[0]=&VFDFBuffer;
//	FauScr.Cur[1]=&VFDFBuffer;
//	AckScr.Cur[0]=&VFDABuffer;
//	AckScr.Cur[1]=&VFDABuffer;
//	scrP=AckScr;
////	scrP=NorScr;
//	dumpP=scrP.Cur[0];
//
//	*LCD_LIGHT = 0xff;
//	delay(10000);
//	LCD_COMMAND(INIT_LCD);
//	delay(10000);
//	LCD_COMMAND(CLEAR_LCD);
//	delay(10000);
//	LCD_COMMAND(0x0f);
//	delay(10000);
//	LCD_COMMAND(HOME_CUR);
//	delay(10000);
//
////	VFD_cnt=OSTimeGet();
//}
//
//void VFD_gotoxy(int low,int col)
//{
//	int position;
//	if((low<2)&&(col<20))
//	{
//		position = (low*0x40 +col)|0x80;
//		LCD_COMMAND(position);
//	}
//}
//
//void VFD_mem_cpy_test(void)
//{
//	memcpy(&dumpP, &VFDABuffer, sizeof(VFDABuffer));
//}
//
//void VFD_dump()
//{
//	int i;
//
//	for(i=0;i<20;i++)
//	{
//		if(dumpP->up[i] == 0)
//		break;
//		
//		else
//		lcd_character_write(KHS_LCD_Buffer.up[i]);
//		
//		delay_us(1000);
//	}
//	
//	lcd_control_write(0xc0);
//	delay_us(1000);
//	
//	for(i=0;i<20;i++)
//	{
//		if(dumpP->lo[i] == 0)
//		break;
//		
//		else
//		lcd_character_write(KHS_LCD_Buffer.lo[i]);
//		
//		delay_us(1000);
//	}
//
////  Old Himap Ver.
////	VFD_gotoxy(0,0);
////	for(i=0;i<20;i++)
////	{
////		VFD_Putch(dumpP->up[i]);
////		delay(1000);
////	}
////	VFD_gotoxy(1,0);
////	for(i=0;i<20;i++)
////	{
////		VFD_Putch(dumpP->lo[i]);
////		delay(1000);
////	}
//}
//
//void VFD_dump2(const char *string_high, const char *string_low)
//{
//	int i;
//
//	for(i = 0; i < 20; i++)
//	{
//		if(string_high[i] == 0)
//		break;
//		
//		else
//		lcd_character_write(string_high[i]);
//		
//		delay_us(1000);
//	}
//	
//	lcd_control_write(0xc0);
//	delay_us(1000);
//	
//	for(i = 0; i < 20; i++)
//	{
//		if(string_low[i] == 0)
//		break;
//		
//		else
//		lcd_character_write(string_low[i]);
//		
//		delay_us(1000);
//	}
//	}

void VFD_Single_Line_dump(char position, const char *string_high)
{
	int i;

	lcd_control_write(position);
	loop_delay(500);//delay_us(1);

	for(i = 0; i < 20; i++)
	{
		if(string_high[i] == 0)
		break;

		else
		lcd_character_write(string_high[i]);

		loop_delay(500);//delay_us(1);
	}

	loop_delay(500);//delay_us(1);
}

//void VFD_Word_dump(char position, char length, const char *string_high)
//{
//	int i;
//
//	lcd_control_write(position);
//	delay_us(1000);
//
//	for(i = 0; i < length; i++)
//	{
//		if(string_high[i] == 0)
//		break;
//
//		else
//		lcd_character_write(string_high[i]);
//
//		delay_us(1000);
//	}
//
//	delay_us(1000);
//}

const char LCD_Position_Value[2][20] = {
		{0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93},
		{0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3}
};

void VFD_cursor(char y, char x)
{
	if(y == 0 && x == 0) {
		lcd_control_write(0x0c);	//cursor off
	} else {
		lcd_control_write(0x0f);	//cursor on
	}
	loop_delay(500);//delay_us(1000);
	lcd_control_write(LCD_Position_Value[y%2][x%20]);
	loop_delay(500);//delay_us(1000);
}

//void VFD_printf(char *string)
//{
//	while(*string != NULL)
//	{
//		LCD_DATA(*string);
//		string++;
//	}
//}

void VFD_FONT(void)
{
//	//MAKE Enter Font - address(0x1b)
//	VFD_Putch(0X03);
//	VFD_Putch(0X1B);
//	VFD_Putch(0X08);
//	VFD_Putch(0X4A);
//	VFD_Putch(0X9F);
//	VFD_Putch(0XA0);
//	VFD_Putch(0X80);
//	delay500u();
}
//2006.4.6 END


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------


// i2c 장치 읽을 때 사용하는 함수
// ar_slave - 해당 장치의 고유 id
// ar_address - 장치 내부 레지스터 주소
// ar_bcount - 장치에서 read할 데이터 바이트개수
void i2c_read(unsigned int ar_slave, unsigned int ar_address, unsigned int ar_bcount)
{
	// Wait until the STP bit is cleared from any previous master communication.
	for(;;)
	{
		if((*I2caRegs_I2CMDR & 0x0800) == 0x0000)
		break;
	}
	
	// Wait until bus-free status
	for(;;)
	{
		if((*I2caRegs_I2CSTR & 0x1000) == 0x0000)
		break;
	}
	
	// write
	// 읽어올 장치 ID 지정
	*I2caRegs_I2CSAR = ar_slave;
	
	// memory access 시 address high + address low
	// rtc access 시 address		
	if(ar_slave == 0x50)	// 현재 사용 안하는 부분
	{
		*I2caRegs_I2CCNT = 2;
		
		// 주소상위
		*I2caRegs_I2CDXR = (ar_address >> 8);
		// 주소하위
		*I2caRegs_I2CDXR = ar_address;
	}
	
	// RTC 읽어올 때
	else
	{
		// 시작주소 한바이트만 일단 날리기 때문에 		
		*I2caRegs_I2CCNT = 1;
				
		// 읽어올 RTC 내부 주소를 보냄
		*I2caRegs_I2CDXR = ar_address;
	}
	
	// Send start as master transmitter with STT(=1), STP(=0), XA(=0), RM(=0) 
	*I2caRegs_I2CMDR = 0x2620; // 위에 ID와 slave를 보내라는 명령
	
	// Wait until ARDY status bit is set
	for(;;)
	{
		if((*I2caRegs_I2CSTR & 0x0004) == 0x0004)
		break;
	}
		
	// Wait until the STP bit is cleared
	for(;;)
	{
		if((*I2caRegs_I2CMDR & 0x0800) == 0x0000)
		break;
	}
	
	// 지정된 바이트 개수만큼 읽어들이라고 DSP에 명령
	*I2caRegs_I2CCNT = ar_bcount;
	
	// 읽기 개시
	// Send start as master receiver with STT(=1), STP(=1), XA(=0), RM(=0) 
	*I2caRegs_I2CMDR = 0x2c20;
	
	// Wait until STOP condition is detected and clear STOP condition bit
	for(;;)
	{
		// 지정된 개수만큼 읽기가 완료됨
		if((*I2caRegs_I2CSTR & 0x0020) == 0x0020)
		{
			// 레지스터 초기화
			*I2caRegs_I2CSTR |= 0x0020;
			break;
		}
	}
}

// i2c 통신방식 외부장치 드라이버
// ar_slave - 해당 장치의 고유 id
// ar_address - 장치 내부 레지스터 주소
// ar_bcount - 장치에 write할 데이터 바이트개수
// *ar_point - 장치에 write 할 데이터가 저장되어 있는 변수주소
void i2c_write(unsigned int ar_slave, unsigned int ar_address, unsigned int ar_bcount, unsigned int *ar_point)
{
	unsigned int i;

	// Wait until the STP bit is cleared from any previous master communication.
	for(;;)
	{
		if((*I2caRegs_I2CMDR & 0x0800) == 0x0000)
		break;
	}
	
	// Wait until bus-free status
	for(;;)
	{
		if((*I2caRegs_I2CSTR & 0x1000) == 0x0000)
		break;
	}
	
	// write
	// 사용할 장치 ID 지정(slave adress 지정)
	*I2caRegs_I2CSAR = ar_slave;
	
	// 혼자 사용하니깐 busy check 필요없음 -> 예전에는 두개 장치로 설계했는데 최종적으로 RTC 하나만 사용하기로 함
	// 보낼 데이터 바이트개수 지정
	// 메모리 access 시 address high + address low + data 개수
	// rtc access 시 address + data 개수
	// 주소 개수는 넘어오지 않음	
	if(ar_slave == 0x50) // 현재 요부분은 안씀
	{
		*I2caRegs_I2CCNT = ar_bcount + 2;
		
		// slave 주소는 자동으로 보내주므로 address 부터 데이타만 직접 써줌
		// fifo 활성화되어 있어서 fifo로 들어갈 것임
		// 주소상위
		*I2caRegs_I2CDXR = (ar_address >> 8);
		// 주소하위
		*I2caRegs_I2CDXR = ar_address;
	}		
	
	// RTC용
	else
	{	
		// slave ID는 i2c 개시 명령을 내리면 DSP가 자동으로 보내주므로 address 부터 데이타만 직접 써줌	
		// 실제 데이터 + 장치 주소 = dsp가 rtc로 보낼데이터 바이트개수
		*I2caRegs_I2CCNT = ar_bcount + 1; // 
		
		// fifo 활성화되어 있어서 fifo로 들어갈 것임
		// i2c 보내는 데이터 순서는 -> ID + 주소 + 실데이터 순
		// 일단 RTC 주소를 DSP FIFO에 저장
		*I2caRegs_I2CDXR = ar_address;
	}
		
	// 실제 data 지정된 개수만큼 DSP FIFO load
	for(i = 0; i < ar_bcount; i++)
	{
		*I2caRegs_I2CDXR = *(ar_point + i);
	}
	
	// Send start as master transmitter with STT(=1), STP(=1), XA(=0), RM(=0) 
	// i2c 통신 개시 명령
	*I2caRegs_I2CMDR = 0x2E20;
	
	// Wait until STOP condition is detected and clear STOP condition bit
	for(;;)
	{
		// 다 끝나면
		if((*I2caRegs_I2CSTR & 0x0020) == 0x0020)
		{
			// 레지스터 초기화
			*I2caRegs_I2CSTR |= 0x0020;
			
			//끝
			break;
		}
	}
}

void key_drive(void)
{
	// 키값 읽어오기	
	SYSTEM.pushed_key = *KEY_CS & 0x0fff;
	KHS_Key_Press = SYSTEM.pushed_key;
	
	if(SYSTEM.pushed_key != 0)	// 키가 하나도 안누리면 0임
	{
//		if(SYSTEM.pushed_key_count < 2)	// 키 디바운스용
//		{			
//			++SYSTEM.pushed_key_count;
//			SYSTEM.pushed_key = 0;	// 2번 이하로 눌리면 무시함
//		}
//	} 
//	else
//	{
			LCD_BACKLIGHT_ON;	// lcd 백라이트 온
			// 키가 안눌리고 10분이 지나면 백라이트 꺼지게 할 용도의 타이머
			// 키가 계속 눌리면 타이머는 계속 초기화
			TIMER.backlight = 0;
			
			if(SYSTEM.pushed_key == ACK_KEY) //Ack 키
			{
				// 계전요소 사고 시에만 유효하게.
				// 계전요소 누적 상태 변수(RELAY_STATUS.operation_sum_holding)는 0이 아니고, 계전요소 현재 상태 변수(RELAY_STATUS.operation_realtime)는 0 이어야만 유효
				if((RELAY_STATUS.operation_sum_holding) && (RELAY_STATUS.operation_realtime == 0))
				{
					event_direct_save(&EVENT.mode_change);	// ack 키 누름 이벤트 저장
					
					//WAVE.hold = 0; //유보
					
					RELAY_STATUS.operation_sum_holding = 0; // 계전요소 누적 상태 변수 클리어
					SYSTEM.led_on &= ~FAULT_LED; // fault led off
				}
				
				Check_47P_10V();
			}

 //2015.02.25
 
			//if((Temp_key & 0xf00)&&(!PASSpacket.Pass.local_allow)&&(!DoneAck)&&(!InFaultScn))	LocalHandleContrl(Temp_key);
			if((KHS_Key_Press & 0xf00)&&(LOCAL_CTRL.use==ENABLE))	{LocalHandleContrl(KHS_Key_Press);}
 
//		else if((SYSTEM.pushed_key == CB_ON_KEY) || (SYSTEM.pushed_key == CB_OFF_KEY))	// cb on/off
//		{
//				// local control enable 시에만 유효
//				if(LOCAL_CONTROL.mode == 0xaaaa)
//				{
//					// 현재 계전기 상태가 local 일경우에만
//					if(SYSTEM.remote1_local0 == 0)
//					{
//						//막아야할 창이 있음 : 각종 특수화면, popup, confirm 화면 등..
//						//결국 루트화면이거나, 특수화면이 아닌경우에만 유효
//						if((SYSTEM.position == 0x00000000) || (SYSTEM.position > 0x0fffffff))
//						{
//							// lcd_drive() 바로 실행위해
//							TIMER.lcd = 301;
//							
//							// 비밀번호 확인 된 경우
//							if(PASSWORD.check)
//							{
//								// 차단기 on 화면 id
//								if(SYSTEM.pushed_key == CB_ON_KEY)	{SYSTEM.position = 0x00000050;}
//								// 차단기 off 화면 id
//								else																{SYSTEM.position = 0x00000051;}
//							}
//							
//							// 비밀번호 확인 안 된경우
//							else
//							{
//								// 비밀번화 확인 화면 id
//								SYSTEM.position = 0x00000010;
//								// 비밀번호 확인 완료 후 되돌아올 화면 id 저장
//								if(SYSTEM.pushed_key == CB_ON_KEY)	{SYSTEM.return_position = 0x00000050;}
//								else																{SYSTEM.return_position = 0x00000051;}
//							}
//							//lcd 화면 갱신 플래그
//							LCD.refresh_status = 0;
//						}
//					}
//				}
//			}
//	
//			// local/remote
//			else if((SYSTEM.pushed_key == REMOTE_KEY) || (SYSTEM.pushed_key == LOCAL_KEY))
//			{
//				// local control 모드일 경우에만
//				if(LOCAL_CONTROL.mode == 0xaaaa)
//				{
//					// local 상태이면
//					if(SYSTEM.remote1_local0 == 0)
//					{
//						// 리모트 키가 눌렸을 때만 유효
//						if(SYSTEM.pushed_key == REMOTE_KEY)
//						{
//							// 리모트_로칼 상태 변수 리모트로 변경
//							SYSTEM.remote1_local0 = 1;
//							
//							// FRAM에 현재 리모트 상태임을 기록
//							*REMOTE_LOCAL = SYSTEM.remote1_local0;
//							
//							// 로칼에서 리모트로 변경됬음을 이벤트로 기록
//							EVENT.mode_change |= REMOTE_EVENT;
//							event_direct_save(&EVENT.mode_change);
//						}
//					}
//					
//					// remote 이면
//					else
//					{
//						// 로칼 키가 눌렸을 때만 유효
//						if(SYSTEM.pushed_key == LOCAL_KEY)
//						{
//							// 리모트_로칼 상태 변수 로칼로 변경
//							SYSTEM.remote1_local0 = 0;
//							
//							// FRAM에 현재 로칼 상태임을 기록
//							*REMOTE_LOCAL = SYSTEM.remote1_local0;
//							
//							// 리모트에서 로칼로 변경됬음을 이벤트로 기록
//							EVENT.mode_change |= LOCAL_EVENT;
//							event_direct_save(&EVENT.mode_change);
//						}
//					}
//				}
//			}
//2015.02.25 END
			// 눌린 키에대한 동작이 완료되었으므로 카운터 리셋
			SYSTEM.pushed_key_count = 0;
//		}
	}
	else // 키가 눌리지 않았을 경우
	{
		if(TIMER.backlight > 180000) //키가 안눌리고 3분 후 백라이트 소등
		{
			//popup 창은 사용자가 확인 시까지 유지하여야 하므로
			//popup이 아닌경우에만 화면 이동이 가능하도록 함
//		if(popup이 아니라면 추가)
			{
				LCD_BACKLIGHT_OFF;		// lcd 백라이트 off
				TIMER.backlight = 0;	// 백라이트 카운터 리셋
			}
		}
	}

	TIMER.key = 0;	// key_drive() 용 카운터 리셋
}

// lcd controller에 실제 명령을 주는 함수
// ar_char - lcd controller 전달 명령
// 밑에 F/W 구조는 H/W 설계에 의해 4비트씩 쓰게 되어 있음
// 실제 data 쓰기전에 cpld 쪽 이유때문에 하위바이트에 0x04를 써야함
void lcd_control_write(char ar_char)
{
	char temp_high;
	char temp_low;
	
	temp_high = ar_char & 0xf0;	// ar_char를 반으로 가름
	temp_low  = (ar_char << 4);	// H/W으로 MSB 4비트로 인터페이스하게 되어 있으므로 하위 nibble을 상위 nibble로 이동
	
	// 상위 nibble
	*LCD_CS = temp_high | 0x04; // 일단 cpld에 lcd controller에 명령을 준다고 선언
	*LCD_CS = temp_high;	// 상위 nibble write
	
	loop_delay(500);//DELAY_US(1);	// lcd controller가 이것보다 빠르게 값이 들어오면 인식하지 못하므로 delay 삽입
	
	// 하위 nibble
	*LCD_CS = temp_low | 0x04; // 일단 cpld에 lcd controller에 명령을 준다고 선언
	*LCD_CS = temp_low;	// 하위위 nibble write
	
	loop_delay(500);//DELAY_US(1);	// lcd controller가 이것보다 빠르게 값이 들어오면 인식하지 못하므로 delay 삽입
}

// lcd controller에 실제 데이타를 주는 함수
// ar_char - lcd controller 전달 데이타
// 밑에 F/W 구조는 H/W 설계에 의해 4비트씩 쓰게 되어 있음
// 실제 data 쓰기전에 cpld 쪽 이유때문에 하위바이트에 0x05를 써야함
// 상세설명은 lcd_control_write() 참조, cpld에 쓰는 값만 다름 
void lcd_character_write(char ar_char)
{	
	char temp_high;
	char temp_low;
	
	temp_high = ar_char & 0xf0;
	temp_low  = (ar_char << 4);
		
	// 상위 nibble
	*LCD_CS = temp_high | 0x05;	
	*LCD_CS = temp_high;
	
	loop_delay(500);//DELAY_US(1);
	
	// 하위 nibble
	*LCD_CS = temp_low | 0x05;
	*LCD_CS = temp_low;	
	loop_delay(500);//DELAY_US(1);
}

// op : 4, address : 0xc0 - write enable
// op : 4, address : 0x80 - erase all
// op : 4, address : 0x00 - write disable
void eerom_control(unsigned int ar_opcode, unsigned int ar_address)
{
	unsigned int i;
	
	EEROM_CS_HIGH;
	
	//op
	for(i = 0; i < 3; i++)
	{
		EEROM_CLK_LOW;
		
		delay_us(1);
		
		if(ar_opcode & (0x0004 >> i))
		EEROM_DATA_HIGH;
		
		else
		EEROM_DATA_LOW;
		
		EEROM_CLK_HIGH;
		
		delay_us(1);
	}
	
	// adress
	for(i = 0; i < 8; i++)
	{
		EEROM_CLK_LOW;
		
		delay_us(1);
		
		if(ar_address & (0x0080 >> i))
		EEROM_DATA_HIGH;
		
		else
		EEROM_DATA_LOW;
		
		EEROM_CLK_HIGH;
		
		delay_us(1);
	}	
	
	EEROM_CLK_LOW;
	EEROM_CS_LOW;
	
	delay_us(10000);
}

void eerom_write(unsigned int ar_address, void *ar_data)
{
	unsigned int i;
	unsigned int op = 5;
	unsigned int *data_p;
	
	data_p = ar_data;
	
	EEROM_CS_HIGH;
	
	//op
	for(i = 0; i < 3; i++)
	{
		EEROM_CLK_LOW;
		
		delay_us(1);
		
		if(op & (0x0004 >> i))
		EEROM_DATA_HIGH;
		
		else
		EEROM_DATA_LOW;
		
		EEROM_CLK_HIGH;
		
		delay_us(1);
	}
	
	// adress
	for(i = 0; i < 8; i++)
	{
		EEROM_CLK_LOW;
		
		delay_us(1);
		
		if(ar_address & (0x0080 >> i))
		EEROM_DATA_HIGH;
		
		else
		EEROM_DATA_LOW;
		
		EEROM_CLK_HIGH;
		
		delay_us(1);
	}
	
	// data
	for(i = 0; i < 16; i++)
	{
		EEROM_CLK_LOW;
		
		if(*data_p & (0x8000 >> i))
		EEROM_DATA_HIGH;
		
		else
		EEROM_DATA_LOW;
		
		EEROM_CLK_HIGH;
		
		delay_us(1);
	}
	
	EEROM_CLK_LOW;
	EEROM_CS_LOW;
	
	delay_us(2000);
}

void eerom_read(unsigned int ar_address, void *ar_data)
{
	unsigned int i;
	unsigned int op = 6;
	
	unsigned int temp;
	unsigned int temp2 = 0;
	
	unsigned int *data_p;
	
	data_p = ar_data;
	
	EEROM_CS_HIGH;
	
	
	//op
	for(i = 0; i < 3; i++)
	{
		EEROM_CLK_LOW;
		
		delay_us(1);
		
		if(op & (0x0004 >> i))
		EEROM_DATA_HIGH;
		
		else
		EEROM_DATA_LOW;
		
		EEROM_CLK_HIGH;
		
		delay_us(1);
	}
	
	// adress
	for(i = 0; i < 8; i++)
	{
		EEROM_CLK_LOW;
		
		delay_us(1);
		
		if(ar_address & (0x0080 >> i))
		EEROM_DATA_HIGH;
		
		else
		EEROM_DATA_LOW;
		
		EEROM_CLK_HIGH;
		
		delay_us(1);
	}
	
	// data
	for(i = 0; i < 17; i++)
	{
		EEROM_CLK_LOW;
		
		delay_us(1);
		
		temp = (*GpioIntRegs_GPADAT_Low & 0x0008) >> 3;
		
		temp2 |= temp << (16 - i);
		
		EEROM_CLK_HIGH;		
		
		delay_us(1);
	}
	
	EEROM_CLK_LOW;
	EEROM_CS_LOW;
	
	*data_p = temp2;
}

void flash_sector_erase(unsigned int *ar_sector)
{	
	*FLASH_5555 = 0xaa;	
	*FLASH_2aaa = 0x55;	
	*FLASH_5555 = 0x80;	
	*FLASH_5555 = 0xaa;	
	*FLASH_2aaa = 0x55;	
	*ar_sector = 0x30;
	
	delay_us(20000);
}

void flash_word_write(unsigned int *ar_address, unsigned int ar_data)
{
	*FLASH_5555 = 0xaa;	
	*FLASH_2aaa = 0x55;	
	*FLASH_5555 = 0xa0;	
	
	*ar_address = ar_data;
		
	delay_us(100);//20
}

//khs, 2015-04-08 오전 11:42:47
void wave_flash_word_write(unsigned int *ar_address, unsigned int ar_data)
{
//	*FLASH_5555 = 0xaa;	
//	*FLASH_2aaa = 0x55;	
//	*FLASH_5555 = 0xa0;	
	
	*ar_address = ar_data;
		
//	delay_us(20);
	
	++FLASH.destination_count;
	++FLASH.source_count;
		
	if(FLASH.source_count == FLASH.source_count_end)
	{
		++WAVE.save_index;
		FLASH.end_flag = 0;
	}
}

void wave_vi_initial_pre1(void)
{
	FLASH.destination_count = 0;
			
	FLASH.source_count = WAVE.pre_count;
	FLASH.source_count_end = 0x1518;
						
	FLASH.end_flag = 1;
}

void wave_vi_initial_pre2(void)
{
	FLASH.source_count = 0;
	FLASH.source_count_end = WAVE.pre_count;// -1 을 빼야한다. 있으면 5400 샘플 데이타가 저장이 안된다.
			
	FLASH.end_flag = 1;
}

void wave_vi_initial_post(void)
{
	FLASH.destination_count = 0x1518;
			
	FLASH.source_count = 0;
	FLASH.source_count_end = 0x1518;
			
	FLASH.end_flag = 1;	
}

void wave_di_initial_pre1(void)
{
	FLASH.destination_count = 0;
			
	FLASH.source_count = WAVE.pre_count_di;
	FLASH.source_count_end = 0x708;		//0x1518의 1/3배 샘플링한다.
			
	FLASH.end_flag = 1;
}

void wave_di_initial_pre2(void)
{
	FLASH.source_count = 0;
	FLASH.source_count_end = WAVE.pre_count_di;// 확인해 봐야 한다. -1이 필요한지??
			
	FLASH.end_flag = 1;
	
}

void wave_di_initial_post(void)
{
	FLASH.destination_count = 0x708;
			
	FLASH.source_count = 0;
	FLASH.source_count_end = 0x708;
			
	FLASH.end_flag = 1;
}

/*
wave 저장방식
평상시 저장하는 부분 / analog : 5400word/0x1518, digital : 1800word/0x708
//Ia - 0x200000 ~ 0x201517
//Ib - 0x202a30 ~ 0x203F47
//Ic - 0x205460 ~ 0x206977
//In - 0x207e90 ~ 0x2093A7
//Va - 0x20fd20 ~ 0x211237
//Vb - 0x212750 ~ 0x213C67
//Vc - 0x215180 ~ 0x216697
//Vn - 0x217bb0 ~ 0x2190C7
//Ry - 0x21a5e0 ~ 0x21ACE7
//DI - 0x21B3F0 ~ 0x21C907
//DO - 0x21C200 ~ 0x2190C7

Ia - 0x200000 ~ 0x201517
Ib - 0x202a30 ~ 0x203F47
Ic - 0x205460 ~ 0x206977
In - 0x207e90 ~ 0x2093A7
Va - 0x20A8C0 ~ 0x20BDD7
Vb - 0x20D2F0 ~ 0x20E807
Vc - 0x20FD20 ~ 0x211237
Vn - 0x212750 ~ 0x213C67
Ry - 0x215180 ~ 0x216697
DI - 0x217BB0 ~ 0x2190C7
DO - 0x21A5E0 ~ 0x21BAF7

사고 후  저장하는 부분 / analog : 5400word/0x1518, digital : 1800word/0x708
//Ia - 0x201518 ~ 0x202A2F
//Ib - 0x203F48 ~ 0x206977
//Ic - 0x206978 ~ 0x2093A7
//In - 0x2093A8 ~ 0x20A8BF
//Va - 0x211238 ~ 0x213C67
//Vb - 0x213C68 ~ 0x216697
//Vc - 0x216698 ~ 0x2190C7
//Vn - 0x2190C8 ~ 0x21A5DF
//Ry - 0x21ACE8 ~ 0x21B3ef
//DI - 0x21BAF8 ~ 0x21C1ff
//DO - 0x21C908 ~ 0x21D00F

Ia - 0x201518 ~ 0x202A2F
Ib - 0x203F48 ~ 0x20545F
Ic - 0x206978 ~ 0x207E8F
In - 0x2093A8 ~ 0x20A8BF
Va - 0x20BDD8 ~ 0x20D2EF
Vb - 0x20E808 ~ 0x20FD1F
Vc - 0x211238 ~ 0x21274F
Vn - 0x213C68 ~ 0x21517F
Ry - 0x216698 ~ 0x217BAF
DI - 0x2190C8 ~ 0x21A5DF
DO - 0x21BAF8 ~ 0x21D00F

1주기에 36샘플   1초에 60샘플
36*60=2160[샘플]
1초에 2160[샘플]
0.5초에 1080[샘플]
2.5초에 5400[샘플]


사고 후 모든저장이 완료되면
if((WAVE.post_count == 5400) && (WAVE.post_start == 0x1234))

저장 루틴을 탐.
wave_save_process()

wave용 54개 sector를 지움

다 지운 후 사고전 시작 포인트를 알기 위해
FLASH.source_count = WAVE.pre_count;

WAVE.pre_count 부터 0x1518까지 저장

다음으로 
WAVE.pre_count = 0 부터 WAVE.pre_count - 1까지 저장

여기서 WAVE.pre_count = 0이라면 처음에 모두 저장 후 다음 턴은 통과
WAVE.pre_count = 0x1517이라면 처음 통과 다음 턴에 모두 저장


상기 순서로 각채널 저장 후 완료


아날로그값 : 총 10800word
디지털 값 : 총 3600word
*/

//khs, 2015-04-08 오전 11:43:00
void wave_save_process(void)
{
	unsigned long temp32;
	
	if(WAVE.save_index < 54)
	{
		//setcor 지우기 : 전체 54개
		if(WAVE.hold == 0)
		{
			//temp32 = (unsigned long)WAVE.save_index;
			//temp32 *= 0x800;
			//wave_flash_sector_erase(FLASH_SECTOR000 + temp32);
			
			WAVE.timer = 0;
			WAVE.hold = 1;
		}
		
		else
		{
			if(WAVE.timer > 20)
			{
				WAVE.hold = 0;
				
				++WAVE.save_index;
				
				//FLASH.end_flag = 0;
			}
		}
	}
			
	// Ia pre1
	else if(WAVE.save_index == 54)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre1();
		
		else
		wave_flash_word_write(FLASH_WAVE_Ia + FLASH.destination_count, *(Pre_Ia_wave_buffer + FLASH.source_count));		
	}
	
	// Ia pre2
	else if(WAVE.save_index == 55)
	{	
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre2();
		
		else
		wave_flash_word_write(FLASH_WAVE_Ia + FLASH.destination_count, *(Pre_Ia_wave_buffer + FLASH.source_count));
	}
	
	// Ia post
	else if(WAVE.save_index == 56)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_post();
		
		else
		wave_flash_word_write(FLASH_WAVE_Ia + FLASH.destination_count, *(Post_Ia_wave_buffer + FLASH.source_count));
		
	}
	
	// Ib pre1
	else if(WAVE.save_index == 57)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre1();
		
		else
		wave_flash_word_write(FLASH_WAVE_Ib + FLASH.destination_count, *(Pre_Ib_wave_buffer + FLASH.source_count));	
	}
	
	// Ib pre2
	else if(WAVE.save_index == 58)
	{		
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre2();
		
		else
		wave_flash_word_write(FLASH_WAVE_Ib + FLASH.destination_count, *(Pre_Ib_wave_buffer + FLASH.source_count));
	}	
	
	// Ib post
	else if(WAVE.save_index == 59)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_post();
		
		else
		wave_flash_word_write(FLASH_WAVE_Ib + FLASH.destination_count, *(Post_Ib_wave_buffer + FLASH.source_count));
	}
	
	// Ic pre1
	else if(WAVE.save_index == 60)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre1();
		
		else
		wave_flash_word_write(FLASH_WAVE_Ic + FLASH.destination_count, *(Pre_Ic_wave_buffer + FLASH.source_count));	
	}
	
	// Ic pre2
	else if(WAVE.save_index == 61)
	{		
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre2();
		
		else
		wave_flash_word_write(FLASH_WAVE_Ic + FLASH.destination_count, *(Pre_Ic_wave_buffer + FLASH.source_count));
	}	
	
	// Ic post
	else if(WAVE.save_index == 62)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_post();
		
		else
		wave_flash_word_write(FLASH_WAVE_Ic + FLASH.destination_count, *(Post_Ic_wave_buffer + FLASH.source_count));
	}
	
	
	// In pre1
	else if(WAVE.save_index == 63)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre1();
		
		else
		{
			if(CORE.gr_select == ZCT_SELECT)
				wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Pre_In_wave_buffer + FLASH.source_count));
				//wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Pre_Is_wave_buffer + FLASH.source_count));
			
			else
			{
//			if(SYSTEM_SET.ocgr_dgr == DGR_SELECT)
//			wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Pre_In2_wave_buffer + FLASH.source_count));	
//			else
				wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Pre_In_wave_buffer + FLASH.source_count));	
			}
		}
		
	}
	
	// In pre2
	else if(WAVE.save_index == 64)
	{	
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre2();
		
		else
		{
			if(CORE.gr_select == ZCT_SELECT)
				wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Pre_In_wave_buffer + FLASH.source_count));
				//wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Pre_Is_wave_buffer + FLASH.source_count));
			
			else
			{
//			if(SYSTEM_SET.ocgr_dgr == DGR_SELECT)
//			wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Pre_In2_wave_buffer + FLASH.source_count));	
//			else
				wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Pre_In_wave_buffer + FLASH.source_count));	
			}
		}
		
	}	
	
	// In post
	else if(WAVE.save_index == 65)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_post();
		
		else
		{
			if(CORE.gr_select == ZCT_SELECT)
				wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Pre_In_wave_buffer + FLASH.source_count));
				//wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Post_Is_wave_buffer + FLASH.source_count));
			
			else
			{
//			if(SYSTEM_SET.ocgr_dgr == DGR_SELECT)
//			wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Post_In2_wave_buffer + FLASH.source_count));
//			else
				wave_flash_word_write(FLASH_WAVE_In + FLASH.destination_count, *(Post_In_wave_buffer + FLASH.source_count));
			}
		}
	}
	
	
	// Va pre1
	else if(WAVE.save_index == 66)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre1();
		
		else
		wave_flash_word_write(FLASH_WAVE_Va + FLASH.destination_count, *(Pre_Va_wave_buffer + FLASH.source_count));
	}
	
	// Va pre2
	else if(WAVE.save_index == 67)
	{	
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre2();
		
		else
		wave_flash_word_write(FLASH_WAVE_Va + FLASH.destination_count, *(Pre_Va_wave_buffer + FLASH.source_count));
	}
	
	
	// Va post
	else if(WAVE.save_index == 68)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_post();
		
		else
		wave_flash_word_write(FLASH_WAVE_Va + FLASH.destination_count, *(Post_Va_wave_buffer + FLASH.source_count));
		
	}
	
	// Vb pre1
	else if(WAVE.save_index == 69)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre1();
		
		else
		wave_flash_word_write(FLASH_WAVE_Vb + FLASH.destination_count, *(Pre_Vb_wave_buffer + FLASH.source_count));
	}
	
	// Vb pre2
	else if(WAVE.save_index == 70)
	{		
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre2();
		
		else
		wave_flash_word_write(FLASH_WAVE_Vb + FLASH.destination_count, *(Pre_Vb_wave_buffer + FLASH.source_count));
	}	
	
	// Vb post
	else if(WAVE.save_index == 71)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_post();
		
		else
		wave_flash_word_write(FLASH_WAVE_Vb + FLASH.destination_count, *(Post_Vb_wave_buffer + FLASH.source_count));
	}
	
	// Vc pre1
	else if(WAVE.save_index == 72)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre1();
		
		else
		wave_flash_word_write(FLASH_WAVE_Vc + FLASH.destination_count, *(Pre_Vc_wave_buffer + FLASH.source_count));
	}
	
	// Vc pre2
	else if(WAVE.save_index == 73)
	{		
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre2();
		
		else
		wave_flash_word_write(FLASH_WAVE_Vc + FLASH.destination_count, *(Pre_Vc_wave_buffer + FLASH.source_count));
	}	
	
	// Vc post
	else if(WAVE.save_index == 74)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_post();
		
		else
		wave_flash_word_write(FLASH_WAVE_Vc + FLASH.destination_count, *(Post_Vc_wave_buffer + FLASH.source_count));
	}
	
	// Vn pre1
	else if(WAVE.save_index == 75)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre1();
		
		else
		wave_flash_word_write(FLASH_WAVE_Vn + FLASH.destination_count, *(Pre_Vn_wave_buffer + FLASH.source_count));
	}
	
	// Vn pre2
	else if(WAVE.save_index == 76)
	{		
		if(FLASH.end_flag == 0)
		wave_vi_initial_pre2();
		
		else
		wave_flash_word_write(FLASH_WAVE_Vn + FLASH.destination_count, *(Pre_Vn_wave_buffer + FLASH.source_count));
	}	
	
	// Vn post
	else if(WAVE.save_index == 77)
	{
		if(FLASH.end_flag == 0)
		wave_vi_initial_post();
		
		else
		wave_flash_word_write(FLASH_WAVE_Vn + FLASH.destination_count, *(Post_Vn_wave_buffer + FLASH.source_count));
	}
	
	
	
	// relay pre1
	else if(WAVE.save_index == 78)
	{
		if(FLASH.end_flag == 0)
		wave_di_initial_pre1();
		
		else
		wave_flash_word_write(FLASH_WAVE_RELAY + FLASH.destination_count, *(Pre_relay_wave_buffer + FLASH.source_count));
		
	}
	
	// relay pre2
	else if(WAVE.save_index == 79)
	{		
		if(FLASH.end_flag == 0)
		wave_di_initial_pre2();
		
		else
		wave_flash_word_write(FLASH_WAVE_RELAY + FLASH.destination_count, *(Pre_relay_wave_buffer + FLASH.source_count));
	}	
	
	// relay post
	else if(WAVE.save_index == 80)
	{
		if(FLASH.end_flag == 0)
		wave_di_initial_post();
		
		else
		wave_flash_word_write(FLASH_WAVE_RELAY + FLASH.destination_count, *(Post_relay_wave_buffer + FLASH.source_count));
		
	}
	
	// di pre1
	else if(WAVE.save_index == 81)
	{
		if(FLASH.end_flag == 0)
		wave_di_initial_pre1();
		
		else
		wave_flash_word_write(FLASH_WAVE_DI + FLASH.destination_count, *(Pre_DI_wave_buffer + FLASH.source_count));
		
	}
	
	// di pre2
	else if(WAVE.save_index == 82)
	{		
		if(FLASH.end_flag == 0)
		wave_di_initial_pre2();
		
		else
		wave_flash_word_write(FLASH_WAVE_DI + FLASH.destination_count, *(Pre_DI_wave_buffer + FLASH.source_count));
	}	
	
	// di post
	else if(WAVE.save_index == 83)
	{
		if(FLASH.end_flag == 0)
		wave_di_initial_post();
		
		else
		wave_flash_word_write(FLASH_WAVE_DI + FLASH.destination_count, *(Post_DI_wave_buffer + FLASH.source_count));
		
	}
	
	
	// do pre1
	else if(WAVE.save_index == 84)
	{
		if(FLASH.end_flag == 0)
		wave_di_initial_pre1();
		
		else
		wave_flash_word_write(FLASH_WAVE_DO + FLASH.destination_count, *(Pre_DO_wave_buffer + FLASH.source_count));
		
	}
	
	// do pre2
	else if(WAVE.save_index == 85)
	{		
		if(FLASH.end_flag == 0)
		wave_di_initial_pre2();
		
		else
		wave_flash_word_write(FLASH_WAVE_DO + FLASH.destination_count, *(Pre_DO_wave_buffer + FLASH.source_count));
	}	
	
	// do post
	else if(WAVE.save_index == 86)
	{
		if(FLASH.end_flag == 0)
		wave_di_initial_post();
		
		else
		wave_flash_word_write(FLASH_WAVE_DO + FLASH.destination_count, *(Post_DO_wave_buffer + FLASH.source_count));
	}
	//end
	else if(WAVE.save_index == 87)
	{
		//wave 썼음
		*WAVE_WRITE_CHECK = 0x1234;
		
		WAVE.save_index = 0;
		WAVE.post_count = 0;
		WAVE.pre_count = 0;
		WAVE.post_count_di = 0;
		WAVE.pre_count_di = 0;
		WAVE.post_start = 0;
		FLASH.end_flag = 0;
		WAVE.hold = 0xaaaa;

		Save_Fault_Wave_Info(1);	// Save Wave Info to MRAM
	}
}

// led 표시 가공 작업
void led_handling(void)
{
	SYSTEM.led_mode ^= 0x0001;	// fault led 점멸 작업용
	if(SYSTEM.led_mode)
	{
		// 계전요소가 pickup 중이거나 또는 계전요소 동작이 발생했는데 ack가 안눌린경우
		if((RELAY_STATUS.pickup) || (RELAY_STATUS.operation_sum_holding))	{SYSTEM.led_on |= FAULT_LED;} // 일단 led 켠다
	}
	else
	{
		// 계전요소 동작이 없었을 경우 또는 ack를 누른 후
		if(RELAY_STATUS.operation_sum_holding == 0)												{SYSTEM.led_on &= ~FAULT_LED;} // led 끈다
	}

//LOCAL CONTROL 관련 LED 부분
//	if(LOCAL_CONTROL.mode == 0xaaaa) // local 제어가 가능할 경우에만 해당 led 사용함
//	{
//		if((DIDO.di_status & 0x0003) == 0x0001) // close di 1번이 52a
//		{
//			SYSTEM.led_on |= CB_CLOSE_LED;
//			SYSTEM.led_on &= ~CB_OPEN_LED;
//		}
//		else if((DIDO.di_status & 0x0003) == 0x0002)	// open  di 2번이 52b
//		{
//			SYSTEM.led_on &= ~CB_CLOSE_LED;
//			SYSTEM.led_on |= CB_OPEN_LED;
//		}
//		else // 그외에는 차단기 접점고장 또는 차단기 고장으로 간주하고 led를 다꺼버림
//		{
//			SYSTEM.led_on &= ~CB_CLOSE_LED;
//			SYSTEM.led_on &= ~CB_OPEN_LED;
//		}
//
//
//		if(SYSTEM.remote1_local0 == 0) // local이면
//		{
//			SYSTEM.led_on |= LOCAL_LED;
//			SYSTEM.led_on &= ~REMOTE_LED;
//		}
//		else //remote
//		{
//			SYSTEM.led_on &= ~LOCAL_LED;
//			SYSTEM.led_on |= REMOTE_LED;
//		}
//	}
//	else // local 제어 disable 시 아래 led 다 안씀
//	{
//		SYSTEM.led_on &= ~CB_CLOSE_LED;
//		SYSTEM.led_on &= ~CB_OPEN_LED;
//
//		SYSTEM.led_on &= ~LOCAL_LED;
//		SYSTEM.led_on &= ~REMOTE_LED;
//	}
//LOCAL CONTROL 관련 LED 부분 END
	
	// 내부 자가진단 상황이 발생된 경우
	if(SYSTEM.diagnostic)	{SYSTEM.led_on |= SYS_FAIL_LED;}
	else									{SYSTEM.led_on &= ~SYS_FAIL_LED;}
	
	// led 작업용 카운터 초기화
	TIMER.led = 0;
	
	//adc monitor
	// 13235 위아래 50%
	if((SYSTEM.adc_ref_monitor > 19853) ||  (SYSTEM.adc_ref_monitor < 6617))
	{
		// 10초이상 허용범위가 넘어간 경우
		if(SYSTEM.adc_ref_monitor_count > 10)
		{
			SYSTEM.diagnostic |= ADC_FAIL;
			return;
		}
		++SYSTEM.adc_ref_monitor_count;
	}
	else
	{
		SYSTEM.adc_ref_monitor_count = 0;
	}
}	
		
void DO_Output(unsigned int ar_value)
{
	unsigned int i;
	
	TLE6208_CS_LOW;	//chip select
	for(i = 0; i < 16; i++)
	{
		TLE6208_CLK_HIGH;
		if(ar_value & (0x8000 >> i))	{TLE6208_DI_HIGH;}
		else													{TLE6208_DI_LOW;}
		TLE6208_CLK_LOW;			
	}
	TLE6208_CS_HIGH;	
}

void Relay_On(unsigned int ar_value)
{
	unsigned int i;

	SYSTEM.do_out |= ar_value;

	TLE6208_CS_LOW;	//chip select
	for(i = 0; i < 16; i++)
	{
		TLE6208_CLK_HIGH;
		if(SYSTEM.do_out & (0x8000 >> i))	{TLE6208_DI_HIGH;}
		else															{TLE6208_DI_LOW;}
		TLE6208_CLK_LOW;			
	}
	TLE6208_CS_HIGH;	
	
	if(ar_value & 0x2000)			DIGITAL_OUTPUT.do_status |= 0x01;
	if(ar_value & 0x4000)			DIGITAL_OUTPUT.do_status |= 0x02;
	if(ar_value & 0x0800)			DIGITAL_OUTPUT.do_status |= 0x04;
	if(ar_value & 0x1000)			DIGITAL_OUTPUT.do_status |= 0x08;
	if(ar_value & 0x0200)			DIGITAL_OUTPUT.do_status |= 0x10;
	if(ar_value & 0x0080)			DIGITAL_OUTPUT.do_status |= 0x20;
	if(ar_value & 0x0020)			DIGITAL_OUTPUT.do_status |= 0x40;
	if(ar_value & 0x0008)			DIGITAL_OUTPUT.do_status |= 0x80;
}

void Relay_Off(unsigned int ar_value)
{
	unsigned int i;

	SYSTEM.do_out &= ~ar_value;

	TLE6208_CS_LOW;	//chip select
	for(i = 0; i < 16; i++)
	{
		TLE6208_CLK_HIGH;
		if(SYSTEM.do_out & (0x8000 >> i))	{TLE6208_DI_HIGH;}
		else															{TLE6208_DI_LOW;}
		TLE6208_CLK_LOW;			
	}
	TLE6208_CS_HIGH;	
	
	if(ar_value & 0x2000)			DIGITAL_OUTPUT.do_status &= ~0x01;
	if(ar_value & 0x4000)			DIGITAL_OUTPUT.do_status &= ~0x02;
	if(ar_value & 0x0800)			DIGITAL_OUTPUT.do_status &= ~0x04;
	if(ar_value & 0x1000)			DIGITAL_OUTPUT.do_status &= ~0x08;
	if(ar_value & 0x0200)			DIGITAL_OUTPUT.do_status &= ~0x10;
	if(ar_value & 0x0080)			DIGITAL_OUTPUT.do_status &= ~0x20;
	if(ar_value & 0x0020)			DIGITAL_OUTPUT.do_status &= ~0x40;
	if(ar_value & 0x0008)			DIGITAL_OUTPUT.do_status &= ~0x80;
}


// C:\tidcs\c28\DSP2833x\v131\DSP2833x_headers\source\DSP2833x_GlobalVariableDefs.c
// C:\tidcs\c28\DSP2833x\v131\DSP2833x_common\source\DSP2833x_DMA.c
// This function initializes the DMA to a known state.
void DMAInitialize(void)
{
	EALLOW;

	// Perform a hard reset on DMA
	DmaRegs.DMACTRL.bit.HARDRESET = 1;
    asm (" nop"); // one NOP required after HARDRESET

	// Allow DMA to run free on emulation suspend
	DmaRegs.DEBUGCTRL.bit.FREE = 1;

	EDIS;
}

void DMACH1AddrConfig(volatile Uint16 *DMA_Dest,volatile Uint16 *DMA_Source)
{
	EALLOW;
	// Set up SOURCE address:
	DmaRegs.CH1.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source;	// Point to beginning of source buffer
	DmaRegs.CH1.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

	// Set up DESTINATION address:
	DmaRegs.CH1.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;	    // Point to beginning of destination buffer
	DmaRegs.CH1.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;


	EDIS;
}

void DMACH1BurstConfig(Uint16 bsize, int16 srcbstep, int16 desbstep)
{
	EALLOW;

	// Set up BURST registers:
	DmaRegs.CH1.BURST_SIZE.all = bsize;	                // Number of words(X-1) x-ferred in a burst
	DmaRegs.CH1.SRC_BURST_STEP = srcbstep;			    // Increment source addr between each word x-ferred
	DmaRegs.CH1.DST_BURST_STEP = desbstep;              // Increment dest addr between each word x-ferred


	EDIS;
}

void DMACH1TransferConfig(Uint16 tsize, int16 srctstep, int16 deststep)
{
	EALLOW;

	// Set up TRANSFER registers:
	DmaRegs.CH1.TRANSFER_SIZE = tsize;                  // Number of bursts per transfer, DMA interrupt will occur after completed transfer
	DmaRegs.CH1.SRC_TRANSFER_STEP = srctstep;			// TRANSFER_STEP is ignored when WRAP occurs
	DmaRegs.CH1.DST_TRANSFER_STEP = deststep;			// TRANSFER_STEP is ignored when WRAP occurs

	EDIS;
}

void DMACH1WrapConfig(Uint16 srcwsize, int16 srcwstep, Uint16 deswsize, int16 deswstep)
{
	EALLOW;

		// Set up WRAP registers:
	DmaRegs.CH1.SRC_WRAP_SIZE = srcwsize;				// Wrap source address after N bursts
    DmaRegs.CH1.SRC_WRAP_STEP = srcwstep;			    // Step for source wrap

	DmaRegs.CH1.DST_WRAP_SIZE = deswsize;				// Wrap destination address after N bursts
	DmaRegs.CH1.DST_WRAP_STEP = deswstep;				// Step for destination wrap

	EDIS;
}


void DMACH1ModeConfig(Uint16 persel, Uint16 perinte, Uint16 oneshot, Uint16 cont, Uint16 synce, Uint16 syncsel, Uint16 ovrinte, Uint16 datasize, Uint16 chintmode, Uint16 chinte)
{
	EALLOW;

	// Set up MODE Register:
	DmaRegs.CH1.MODE.bit.PERINTSEL = persel;	    // Passed DMA channel as peripheral interrupt source
	DmaRegs.CH1.MODE.bit.PERINTE = perinte;       	// Peripheral interrupt enable
	DmaRegs.CH1.MODE.bit.ONESHOT = oneshot;       	// Oneshot enable
	DmaRegs.CH1.MODE.bit.CONTINUOUS = cont;    		// Continous enable
	DmaRegs.CH1.MODE.bit.SYNCE = synce;         	// Peripheral sync enable/disable
	DmaRegs.CH1.MODE.bit.SYNCSEL = syncsel;       	// Sync effects source or destination
	DmaRegs.CH1.MODE.bit.OVRINTE = ovrinte;         // Enable/disable the overflow interrupt
	DmaRegs.CH1.MODE.bit.DATASIZE = datasize;      	// 16-bit/32-bit data size transfers
	DmaRegs.CH1.MODE.bit.CHINTMODE = chintmode;		// Generate interrupt to CPU at beginning/end of transfer
	DmaRegs.CH1.MODE.bit.CHINTE = chinte;        	// Channel Interrupt to CPU enable

	// Clear any spurious flags:
	DmaRegs.CH1.CONTROL.bit.PERINTCLR = 1;  		// Clear any spurious interrupt flags
	DmaRegs.CH1.CONTROL.bit.SYNCCLR = 1;    		// Clear any spurious sync flags
	DmaRegs.CH1.CONTROL.bit.ERRCLR = 1; 	     	// Clear any spurious sync error flags

	// Initialize PIE vector for CPU interrupt:
	PieCtrlRegs.PIEIER7.bit.INTx1 = 1;              // Enable DMA CH1 interrupt in PIE

	EDIS;
}

// This function starts DMA Channel 1.
void StartDMACH1(void)
{
	EALLOW;
	DmaRegs.CH1.CONTROL.bit.RUN = 1;
	EDIS;
}

void self_diagnostic(void)
{
		static int cnt = 0;
		static int sram_pos = 1;
		static int sram_err = 0;
		static int mram_chk = 1;
		unsigned int i;
		unsigned int j;
		
		i = 0;
		
		if(cnt == 1) {	//SRAM을 256바이트 씩 256번 0x10000(65536)길이를 검사. 값은 0x1111 X (1~256)으로 지정

			for(i = (256*sram_pos); i--;)
				*(SRAM_HEALTH_CHECK + i) = (0x1111*sram_pos) & 0xffff;

		} else if(cnt == 2) {

			for(i = (256*sram_pos), j = 0; i--;)
				if((0x1111*sram_pos) != (*(SRAM_HEALTH_CHECK + i)&0xffff))
					j++;
				
				if(j == 0) 		sram_err = 0;
				else 			sram_err++;
					
				if(sram_err > 5) SYSTEM.diagnostic |= SRAM_FAIL;	//SRAM 에러가 5회 연속 발생하면 시스템 정지 실행
			
			if(sram_pos++ > 256) sram_pos = 1;

		} else if(cnt == 3) {	//MRAM 체크
				*MRAM_CHECK = mram_chk? 0x1234: 0xabcd;
				if(*MRAM_CHECK == 0x1234 && mram_chk == 1) {
					//do nthg.
				} else if(*MRAM_CHECK == 0xabcd && mram_chk == 0) {
					//do nthg.
				} else {
					SYSTEM.diagnostic |= MRAM_FAIL;		//저장된 값이 다르면 MRAM FAIL 발생
				}
				mram_chk ^= 1;
		} else if(cnt == 4) {
		
			;
		} else if(cnt == 5) {
			;
		} else if(cnt == 6) {
			;
		} else {
			cnt = 0;
		}

		cnt++;
		
//	SYSTEM.diagnostic |= CALIBRATION_NOT;
//	SYSTEM.diagnostic |= SRAM_FAIL;
//	SYSTEM.diagnostic |= MRAM_FAIL;
//	SYSTEM.diagnostic |= ADC_FAIL;
//	SYSTEM.diagnostic |= COMM_IF_FAIL;
//	SYSTEM.diagnostic |= FLASH_FAIL;
/*
		" PROBLEM - SRAM     ",
		" PROBLEM - MRAM     ",
		" PROBLEM - ADC      ",
		" PROBLEM - FLASH    ",
		
		SYSTEM.diagnostic = 1;
		SYSTEM.diagnostic = 2;
		SYSTEM.diagnostic = 3;
		SYSTEM.diagnostic = 4;
*/		
}

/*
	The FM31L27x is a family of integrated devices that includes the most commonly needed functions for
	processor-based systems. Major features include nonvolatile memory available in various sizes, realtime
	clock, low-VDD reset, watchdog timer, nonvolatile event counter, lockable 64-bit serial
	number area, and general purpose comparator that can be used for an early power-fail (NMI) interrupt or
	other purpose. The family operates from 2.7 to 3.6V.
	
	Address 		D7	 	D6	 	D5	 	D4	 	D3 		D2 		D1 		D0 		Function 
	10h 				Counter 2 MSB Event Counter 2 MSB FFh
	0Fh 				Counter 2 LSB Event Counter 2 LSB FFh
	0Eh 				Counter 1 MSB Event Counter 1 MSB FFh
	0Dh 				Counter 1 LSB Event Counter 1 LSB FFh
	0Ch                                 RC 		CC 		C2P 	C1P 	Event Count Control
	0Bh 				SNL 	- 		FC 		WP1 	WP0 	VBC 	- 		VTP 	Companion Control
	0Ah 				WDE 	- 		- 		WDT4 	WDT3 	WDT2 	WDT1 	WDT0 	Watchdog Control
	09h 				WTR 	POR 	LB 		- 		WR3 	WR2 	WR1 	WR0 	Watchdog Restart/Flags
	08h 				--------10 years----		---------years------- 	Years 00-99
	07h 				0 		0 		0 		10 mo ---------months------		Month 1-12
	06h 				0 		0 	  -10 date- 	--------date---------		Date 1-31
	05h 				0 		0 		0 		0 		0 		-----day------- 	Day 1-7
	04h 				0 		0 		10 hours hours Hours 0-23
	03h 				0 		----10 minutes-		----minutes--------- 		Minutes 0-59
	02h 				0 		----10 seconds- 	----seconds--------- 		Seconds 0-59
	01h 				/OSCEN resv. CALS CAL4 	CAL3 	CAL2 	CAL1 	CAL0 	CAL/Control
	00h 				resv. CF 		resv. resv. resv. CAL 	W 		R 		RTC Control
*/
void Watchdog_Enable_FM31L27x(void)
{
	unsigned int temp;
	
	temp = 0x9e;	//Enable Watchdog
	i2c_write(0x68, 0x0a, 1, &temp);//0Ah Watchdog Control Register Address
}

void Watchdog_Disable_FM31L27x(void)
{
	unsigned int temp;
	
	temp = 0x1f;	//Disable Watchdog
	i2c_write(0x68, 0x0a, 1, &temp);//0Ah Watchdog Control Register Address
}

void Watchdog_Kick_FM31L27x(void)
{
	unsigned int temp;
	
	temp = 0x0a;
	i2c_write(0x68, 0x09, 1, &temp);//0Ah Watchdog Control Register Address
}

unsigned int Watchdog_Read_FM31L27x(unsigned int addr)
{
	unsigned int temp;
	
	i2c_read(0x68, addr, 1);	//레지스터 addr 주소에서 1개 주소 값을 읽어온다.
	temp = *I2caRegs_I2CDRR;	// i2c_read()에서 읽음완료 후 DSP 수신 FIFO(*I2caRegs_I2CDRR)에서 불러오면 읽기가 최종적으로 완료
	
	return temp;
}
