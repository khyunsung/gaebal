#include "define.h"
#include "extern_variable.h"
#include "extern_prototype.h"

typedef struct Manager_System	//추가, khs, 2015-04-24
{
	unsigned int set_time1;
	unsigned int set_time2;
	unsigned int modbus_id;
	unsigned int local_control;
	int dgr_angle;
	int sgr_angle;
	unsigned int uvr_27r;
	unsigned int uvr_27m;
	unsigned int uvr_27s;
	unsigned int ocr_mode_set;
	unsigned int ocr_di;
	unsigned int pt_rating_2nd;
	unsigned int pt_rating_3rd;
} MANAGER_SYSTEM_SET;
MANAGER_SYSTEM_SET ManagerSystem;

// 전면, 후면 통신 프레임 crc 계산 함수
// *ar_address - 프레임 데이터가 저장된 메모리의 주소
// ar_length - 프레임 전체 바이트 개수
// crc 16bit 
unsigned int COMM_CRC(unsigned int *ar_address, unsigned int ar_length)
{
	unsigned int temp = 0;
	unsigned int crc_hi_temp = 0x00FF;
	unsigned int crc_lo_temp = 0x00FF;
	unsigned int i;
	
	for(i = 0; i < ar_length; i++)
	{	
		temp = (crc_hi_temp ^ ar_address[i]);
		crc_hi_temp = (crc_lo_temp ^ CRC_High[temp]);
		crc_lo_temp = CRC_Low[temp];		
	}
	crc_hi_temp <<= 8;
	crc_hi_temp |= crc_lo_temp;
	return(crc_hi_temp);
}

// 해당 function code에 대한 작업
void manager_handling(void)
{
	unsigned int i, j;
	float float_temp;
	
	//core 정보
	if(MANAGER.rx_buffer[2] == 0x00)
	{
			MANAGER.tx_buffer[0] = 0x23;
			MANAGER.tx_buffer[1] = ADDRESS.address;
			MANAGER.tx_buffer[2] = 0;		//타입정보 프레임: 0
			MANAGER.tx_buffer[3] = 0;		//타입정보 프레임: 0

			// length
			MANAGER.tx_buffer[4] = 0;
			MANAGER.tx_buffer[5] = 6;
			
			MANAGER.tx_buffer[ 6] = HIMAP_TYPE;	//HiMAP Type (FI, MS, ML, T = 1, 2, 3, 4)
			MANAGER.tx_buffer[ 7] = ((GPT.pt_secondary % 190) == 0)?4:((GPT.pt_secondary % 120) == 0)?3:
									((GPT.pt_secondary % 110) == 0)?2:1;	//PT Type (100, 110, 120, 190 = 1, 2, 3, 4)
			MANAGER.tx_buffer[ 8] = ((GPT.pt_tertiary % 190) == 0)?3:((GPT.pt_tertiary % 120) == 2)?2:1;//GPT Type (110, 120, 190 = 1, 2, 3)
			MANAGER.tx_buffer[ 9] = (CORE.rated_ct == 0x5678)?1:2;	//CT Type (5A, 1A = 1, 2)
			MANAGER.tx_buffer[10] = VERSION >> 8;	//Version 1	정수
			MANAGER.tx_buffer[11] = VERSION;	//Version 2	소수

			
			i = COMM_CRC(MANAGER.tx_buffer, 11);
			
			MANAGER.tx_buffer[12] = i >> 8;
			MANAGER.tx_buffer[13] = i & 0x00ff;
			
			MANAGER.tx_length = 14;
			
			MANAGER.isr_tx = MANAGER.tx_buffer;
			
			// tx interrupt 활성
			*ScibRegs_SCICTL2 |= 0x0001;
			
			// tx intrrupt 활성화 후 최초 한번 써야함
			MANAGER.tx_count = 1;
			*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
	}

	// 계전요소 read
	else if(MANAGER.rx_buffer[2] == 0x10)
	{
		//byte count
/* 00	- 50-1
01	- 50-2
02	- 50G
03	- 51-1
04	- 51-2
05	- 51G
06	- 47P
07	- 47N
08	- 27R
09	- 27M
10	- 27S
11	- 59
12	- 64
13	- 67GD
14	- 67GS		*/
		// ocr50-1
		if(MANAGER.rx_buffer[3] == 0x00)			{make_crc_send(MANAGER.tx_buffer, &OCR50_1.use, 10);}
		// ocr50-2
		else if(MANAGER.rx_buffer[3] == 0x01)	{make_crc_send(MANAGER.tx_buffer, &OCR50_2.use, 10);}
		// ocgr50
		else if(MANAGER.rx_buffer[3] == 0x02)	{make_crc_send(MANAGER.tx_buffer, &OCGR50.use, 10);}
		// ocr51-1
		else if(MANAGER.rx_buffer[3] == 0x03)	{make_crc_send(MANAGER.tx_buffer, &OCR51_1.use, 10);}
		// ocr51-2
		else if(MANAGER.rx_buffer[3] == 0x04)	{make_crc_send(MANAGER.tx_buffer, &OCR51_2.use, 10);}
		// ocgr51
		else if(MANAGER.rx_buffer[3] == 0x05)	{make_crc_send(MANAGER.tx_buffer, &OCGR51.use, 10);}
		// 47P
		else if(MANAGER.rx_buffer[3] == 0x06)	{make_crc_send(MANAGER.tx_buffer, &P47.use, 8);}
		// 47N
		else if(MANAGER.rx_buffer[3] == 0x07)	{make_crc_send(MANAGER.tx_buffer, &N47.use, 8);}
		// 27-1
		else if(MANAGER.rx_buffer[3] == 0x08)	{make_crc_send(MANAGER.tx_buffer, &UVR_1.use, 10);}
		// 27-2
		else if(MANAGER.rx_buffer[3] == 0x09)	{make_crc_send(MANAGER.tx_buffer, &UVR_2.use, 10);}
		// 27-3
		else if(MANAGER.rx_buffer[3] == 0x0a)	{make_crc_send(MANAGER.tx_buffer, &UVR_3.use, 10);}
		//DGR/SGR
//khs, 2015-04-08 오전 11:21:40		else if(MANAGER.rx_buffer[3] == 0x06)	{make_crc_send(MANAGER.tx_buffer, &DSGR.use, 12);}
		// 59
		else if(MANAGER.rx_buffer[3] == 0x0b)	{make_crc_send(MANAGER.tx_buffer, &OVR.use, 10);}
		// 64
		else if(MANAGER.rx_buffer[3] == 0x0c)	{make_crc_send(MANAGER.tx_buffer, &OVGR.use, 10);}
		//25
		////khs, 2015-04-08 오전 11:21:40 else if(MANAGER.rx_buffer[3] == 0x0e)	{make_crc_send(MANAGER.tx_buffer, &SYNCRO.use, 14);}

//2015.02.25
//		// 37
//		else if(MANAGER.rx_buffer[3] == 0x0f)	{make_crc_send(MANAGER.tx_buffer, &UCR.use, 10);}
//		// 46
//		else if(MANAGER.rx_buffer[3] == 0x10)	{make_crc_send(MANAGER.tx_buffer, &NSOCR.use, 8);}
//		// 49
//		else if(MANAGER.rx_buffer[3] == 0x11)	{make_crc_send(MANAGER.tx_buffer, &THR.use, 8);}
//		// 50H
//		else if(MANAGER.rx_buffer[3] == 0x12)	{make_crc_send(MANAGER.tx_buffer, &H50.use, 8);}
//		// 51LR
//		else if(MANAGER.rx_buffer[3] == 0x13)	{make_crc_send(MANAGER.tx_buffer, &SL.use, 12);}
//		// 66
//		else if(MANAGER.rx_buffer[3] == 0x14)	{make_crc_send(MANAGER.tx_buffer, &NCH.use, 12);}
//2015.02.25 END
	}

	// 시스템 read
	else if(MANAGER.rx_buffer[2] == 0x20)
	{
		//byte count
		if(MANAGER.rx_buffer[3] == 0x01) {
		
			ManagerSystem.set_time1 = 1;
			ManagerSystem.set_time2 = 2;
			ManagerSystem.modbus_id = ADDRESS.address;
			ManagerSystem.local_control = (LOCAL_CONTROL.mode == 0xaaaa)?2:1;
			ManagerSystem.dgr_angle = 0;
			ManagerSystem.sgr_angle = 0;
			ManagerSystem.uvr_27r = 7;
			ManagerSystem.uvr_27m = 8;
			ManagerSystem.uvr_27s = 9;
			ManagerSystem.ocr_mode_set = (OCR_MODE_SET.ocr_mode == OCR_NORMAL)?2:1;
			ManagerSystem.ocr_di = OCR_MODE_SET.ocr_di_mask;
			ManagerSystem.pt_rating_2nd = 12;
			ManagerSystem.pt_rating_3rd = 13;

			make_crc_send(MANAGER.tx_buffer, &ManagerSystem.set_time1, 13*2);
		}
		// c/pt
		else if(MANAGER.rx_buffer[3] == 0x02) {
//			CPT.ct_primary;
//			CPT.nct_primary;
//			CPT.pt_primary_high;
//			CPT.pt_primary_low;
//			CPT.rated_current;
			make_crc_send(MANAGER.tx_buffer, &CPT.ct_primary, 10);
		}
		// Supservison & System Alarm
		else if(MANAGER.rx_buffer[3] == 0x03) {
			make_crc_send(MANAGER.tx_buffer, &CPT.ct_primary, 10);
		}

		// di debounce/do property
//		if(MANAGER.rx_buffer[3] == 0x01)	{make_crc_send(MANAGER.tx_buffer, DIDO.debounce, 18);}
//		// supervision
//		else if(MANAGER.rx_buffer[3] == 0x02)	{make_crc_send(MANAGER.tx_buffer, &SUPERVISION.mode, 16);}
//		// time read
//		else if(MANAGER.rx_buffer[3] == 0x03)	{make_crc_send(MANAGER.tx_buffer, &TIME.year, 12);}
//		// comm
//		else if(MANAGER.rx_buffer[3] == 0x04)	{make_crc_send(MANAGER.tx_buffer, &ADDRESS.address, 1);}
//		// local control
//		else if(MANAGER.rx_buffer[3] == 0x05)	{make_crc_send(MANAGER.tx_buffer, &LOCAL_CONTROL.mode, 2);}
//		// ocgr/dgr
//		else if(MANAGER.rx_buffer[3] == 0x06)	{make_crc_send(MANAGER.tx_buffer, &SYSTEM_SET.ocgr_dgr, 4);}
//		// ocr mode
//		else if(MANAGER.rx_buffer[3] == 0x07)	{make_crc_send(MANAGER.tx_buffer, &SYSTEM_SET.ocr_mode, 4);}
//		// motor
////	else if(MANAGER.rx_buffer[3] == 0x08)	{make_crc_send(MANAGER.tx_buffer, &MOTOR.full_load_current, 6);} //2015.02.25
	}
	
	// 계측값 송신
	// 계측값 float를 바이트로 변경하는 것 때문에 make_crc_send() 통합하지 못함
	else if(MANAGER.rx_buffer[2] == 0x40)
	{
		// header
		MANAGER.tx_buffer[0] = '#';
		MANAGER.tx_buffer[1] = ADDRESS.address;
		MANAGER.tx_buffer[2] = MANAGER.rx_buffer[2];
		MANAGER.tx_buffer[3] = MANAGER.rx_buffer[3];
	
		// 전압
		if(MANAGER.rx_buffer[3] == 0x00)
		{
			//전체 바이트 수는 이미 알고 있음
			// length
			MANAGER.tx_buffer[4] = 0;
			MANAGER.tx_buffer[5] = 40;
			                              
//DISPLAY.rms_value[Va] = 100;			                              
//DISPLAY.rms_value[Vb] = 200;
//DISPLAY.rms_value[Vc] = 300;
//DISPLAY.rms_value[Vn] = 400;

			//Va
			//float_to_integer(DISPLAY.rms_value[Va], &MANAGER.tx_buffer[6], 1);
			float_to_integer(DISPLAY.rms_value[Va], &MANAGER.tx_buffer[6], 1.0F);
			//Vb
			float_to_integer(DISPLAY.rms_value[Vb], &MANAGER.tx_buffer[10], 1.0F);
			//Vc
			float_to_integer(DISPLAY.rms_value[Vc], &MANAGER.tx_buffer[14], 1.0F);
			//Vn
			float_to_integer(DISPLAY.rms_value[Vn], &MANAGER.tx_buffer[18], 1.0F);
			//Vp
			float_to_integer(MEASUREMENT.V1_value, &MANAGER.tx_buffer[22], 1.0F);
			//Vn
			float_to_integer(MEASUREMENT.V2_value, &MANAGER.tx_buffer[26], 1.0F);
			//Vb 위상
			float_to_integer(DISPLAY.angle[1], &MANAGER.tx_buffer[30], 1.0F);
			//Vc 위상
			float_to_integer(DISPLAY.angle[2], &MANAGER.tx_buffer[34], 1.0F);
			//Vo max
			float_to_integer(ACCUMULATION.vo_max, &MANAGER.tx_buffer[38], 1.0F);
			//주파수
			float_to_integer(MEASUREMENT.frequency, &MANAGER.tx_buffer[42], 1.0F);
			
			
			i = COMM_CRC(MANAGER.tx_buffer, 46);
			
			MANAGER.tx_buffer[46] = i >> 8;
			MANAGER.tx_buffer[47] = i & 0x00ff;
			
			MANAGER.tx_length = 48;
			
			MANAGER.isr_tx = MANAGER.tx_buffer;
			
			// tx interrupt 활성
			*ScibRegs_SCICTL2 |= 0x0001;
			
			// tx intrrupt 활성화 후 최초 한번 써야함
			MANAGER.tx_count = 1;
			*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
		}
		
		//전류
		else if(MANAGER.rx_buffer[3] == 0x01)
		{
			// length
			MANAGER.tx_buffer[4] = 0;
			MANAGER.tx_buffer[5] = 40;
			
			//Ia
			float_to_integer(DISPLAY.rms_value[Ia], &MANAGER.tx_buffer[6], 10.0F);
			//Ib
			float_to_integer(DISPLAY.rms_value[Ib], &MANAGER.tx_buffer[10], 10.0F);
			//Ic
			float_to_integer(DISPLAY.rms_value[Ic], &MANAGER.tx_buffer[14], 10.0F);
			//Io
			if(CORE.gr_select == ZCT_SELECT)
			float_to_integer(DISPLAY.rms_value[Is], &MANAGER.tx_buffer[18], 10.0F);
			
			else
			float_to_integer(DISPLAY.rms_value[In], &MANAGER.tx_buffer[18], 10.0F);
			//Ip
			float_to_integer(MEASUREMENT.I1_value, &MANAGER.tx_buffer[22], 10.0F);
			//Ins
			float_to_integer(MEASUREMENT.I2_value, &MANAGER.tx_buffer[26], 10.0F);
			//IA 위상
			float_to_integer(DISPLAY.angle[3], &MANAGER.tx_buffer[30], 1.0F);
			//Ib 위상
			float_to_integer(DISPLAY.angle[4], &MANAGER.tx_buffer[34], 1.0F);
			//Ic 위상
			float_to_integer(DISPLAY.angle[5], &MANAGER.tx_buffer[38], 1.0F);
			//Io max
			float_to_integer(ACCUMULATION.io_max, &MANAGER.tx_buffer[42], 1.0F);
			
			
			i = COMM_CRC(MANAGER.tx_buffer, 46);
			
			MANAGER.tx_buffer[46] = i >> 8;
			MANAGER.tx_buffer[47] = i & 0x00ff;
			
			MANAGER.tx_length = 48;
			
			MANAGER.isr_tx = MANAGER.tx_buffer;
			
			// tx interrupt 활성
			*ScibRegs_SCICTL2 |= 0x0001;
			
			// tx intrrupt 활성화 후 최초 한번 써야함
			MANAGER.tx_count = 1;
			*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
		}
		
		//고조파 Ia
		else if(MANAGER.rx_buffer[3] == 0x02)
		{
			// length
			MANAGER.tx_buffer[4] = 0;
			MANAGER.tx_buffer[5] = 24;
			
			//3조파
			float_to_integer(HARMONICS.ia[0], &MANAGER.tx_buffer[6], 10.0F);
			//5조파
			float_to_integer(HARMONICS.ia[1], &MANAGER.tx_buffer[10], 10.0F);
			//7조파
			float_to_integer(HARMONICS.ia[2], &MANAGER.tx_buffer[14], 10.0F);
			//9조파
			float_to_integer(HARMONICS.ia[3], &MANAGER.tx_buffer[18], 10.0F);			
			//thd
			float_to_integer(HARMONICS.ia[4], &MANAGER.tx_buffer[22], 10.0F);
			//tdd
			float_to_integer(HARMONICS.ia[5], &MANAGER.tx_buffer[26], 10.0F);
			
			i = COMM_CRC(MANAGER.tx_buffer, 30);
			
			MANAGER.tx_buffer[30] = i >> 8;
			MANAGER.tx_buffer[31] = i & 0x00ff;
			
			MANAGER.tx_length = 32;
			
			MANAGER.isr_tx = MANAGER.tx_buffer;
			
			// tx interrupt 활성
			*ScibRegs_SCICTL2 |= 0x0001;
			
			// tx intrrupt 활성화 후 최초 한번 써야함
			MANAGER.tx_count = 1;
			*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
		}
		
		//고조파 Ib
		else if(MANAGER.rx_buffer[3] == 0x03)
		{
			// length
			MANAGER.tx_buffer[4] = 0;
			MANAGER.tx_buffer[5] = 24;
			
			//3조파
			float_to_integer(HARMONICS.ib[0], &MANAGER.tx_buffer[6], 10.0F);
			//5조파
			float_to_integer(HARMONICS.ib[1], &MANAGER.tx_buffer[10], 10.0F);
			//7조파
			float_to_integer(HARMONICS.ib[2], &MANAGER.tx_buffer[14], 10.0F);
			//9조파
			float_to_integer(HARMONICS.ib[3], &MANAGER.tx_buffer[18], 10.0F);			
			//thd
			float_to_integer(HARMONICS.ib[4], &MANAGER.tx_buffer[22], 10.0F);
			//tdd
			float_to_integer(HARMONICS.ib[5], &MANAGER.tx_buffer[26], 10.0F);
			
			i = COMM_CRC(MANAGER.tx_buffer, 30);
			
			MANAGER.tx_buffer[30] = i >> 8;
			MANAGER.tx_buffer[31] = i & 0x00ff;
			
			MANAGER.tx_length = 32;
			
			MANAGER.isr_tx = MANAGER.tx_buffer;
			
			// tx interrupt 활성
			*ScibRegs_SCICTL2 |= 0x0001;
			
			// tx intrrupt 활성화 후 최초 한번 써야함
			MANAGER.tx_count = 1;
			*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
		}
		
		//고조파 Ic
		else if(MANAGER.rx_buffer[3] == 0x04)
		{
			// length
			MANAGER.tx_buffer[4] = 0;
			MANAGER.tx_buffer[5] = 24;
			
			//3조
			float_to_integer(HARMONICS.ic[0], &MANAGER.tx_buffer[6], 10.0F);
			//5조
			float_to_integer(HARMONICS.ic[1], &MANAGER.tx_buffer[10], 10.0F);
			//7조
			float_to_integer(HARMONICS.ic[2], &MANAGER.tx_buffer[14], 10.0F);
			//9조
			float_to_integer(HARMONICS.ic[3], &MANAGER.tx_buffer[18], 10.0F);			
			//thd
			float_to_integer(HARMONICS.ic[4], &MANAGER.tx_buffer[22], 10.0F);
			//tdd
			float_to_integer(HARMONICS.ic[5], &MANAGER.tx_buffer[26], 10.0F);
			
			i = COMM_CRC(MANAGER.tx_buffer, 30);
			
			MANAGER.tx_buffer[30] = i >> 8;
			MANAGER.tx_buffer[31] = i & 0x00ff;
			
			MANAGER.tx_length = 32;
			
			MANAGER.isr_tx = MANAGER.tx_buffer;
			
			// tx interrupt 활성
			*ScibRegs_SCICTL2 |= 0x0001;
			
			// tx intrrupt 활성화 후 최초 한번 써야함
			MANAGER.tx_count = 1;
			*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
		}
		
		//전력
		else if(MANAGER.rx_buffer[3] == 0x05)
		{
			// length
			MANAGER.tx_buffer[4] = 0;
			MANAGER.tx_buffer[5] = 28;
			
			//유효
			float_to_integer(DISPLAY.p3, &MANAGER.tx_buffer[6], 10.0F);
			//무효
			float_to_integer(DISPLAY.q3, &MANAGER.tx_buffer[10], 10.0F);
			//유효량
			float_to_integer(ACCUMULATION.energy_p, &MANAGER.tx_buffer[14], 1.0F);
			//무효량
			float_to_integer(ACCUMULATION.energy_q, &MANAGER.tx_buffer[18], 1.0F);			
			//역유효량
			float_to_integer(ACCUMULATION.energy_rp, &MANAGER.tx_buffer[22], 1.0F);
			//역무효량
			float_to_integer(ACCUMULATION.energy_rq, &MANAGER.tx_buffer[26], 1.0F);
			//역률
			float_to_integer(DISPLAY.pf3, &MANAGER.tx_buffer[30], 100.0F);
			
			i = COMM_CRC(MANAGER.tx_buffer, 34);
			
			MANAGER.tx_buffer[34] = i >> 8;
			MANAGER.tx_buffer[35] = i & 0x00ff;
			
			MANAGER.tx_length = 36;
			
			MANAGER.isr_tx = MANAGER.tx_buffer;
			
			// tx interrupt 활성
			*ScibRegs_SCICTL2 |= 0x0001;
			
			// tx intrrupt 활성화 후 최초 한번 써야함
			MANAGER.tx_count = 1;
			*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
		}
		
		//di/do status
		else if(MANAGER.rx_buffer[3] == 0x06)
		make_crc_send(MANAGER.tx_buffer, &DIDO.di_status, 4);
		
		//열량
		else if(MANAGER.rx_buffer[3] == 0x07)
		{
			// length
			MANAGER.tx_buffer[4] = 0;
			MANAGER.tx_buffer[5] = 4;
			
//		float_to_integer(THR.Q, &MANAGER.tx_buffer[6], 1.0F); //2015.02.25
			
			i = COMM_CRC(MANAGER.tx_buffer, 10);
			
			MANAGER.tx_buffer[10] = i >> 8;
			MANAGER.tx_buffer[11] = i & 0x00ff;
			
			MANAGER.tx_length = 12;
			
			MANAGER.isr_tx = MANAGER.tx_buffer;
			
			// tx interrupt 활성
			*ScibRegs_SCICTL2 |= 0x0001;
			
			// tx intrrupt 활성화 후 최초 한번 써야함
			MANAGER.tx_count = 1;
			*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
		}
		
		//25 계측
		else if(MANAGER.rx_buffer[3] == 0x08)
		{
			// length
			MANAGER.tx_buffer[4] = 0;
			MANAGER.tx_buffer[5] = 12;
//khs, 2015-04-08 오전 11:21:40
//			float_to_integer(SYNCRO.difference_voltage, &MANAGER.tx_buffer[6], 1);
//			float_to_integer(SYNCRO.difference_phase, &MANAGER.tx_buffer[10], 1);
//			float_to_integer(SYNCRO.difference_freq, &MANAGER.tx_buffer[14], 1);
			
			i = COMM_CRC(MANAGER.tx_buffer, 18);
			
			MANAGER.tx_buffer[18] = i >> 8;
			MANAGER.tx_buffer[19] = i & 0x00ff;
			
			MANAGER.tx_length = 20;
			
			MANAGER.isr_tx = MANAGER.tx_buffer;
			
			// tx interrupt 활성
			*ScibRegs_SCICTL2 |= 0x0001;
			
			// tx intrrupt 활성화 후 최초 한번 써야함
			MANAGER.tx_count = 1;
			*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
		}
	}
	
	// event read
	else if(MANAGER.rx_buffer[2] == 0x50)
	{
		// 이벤트 개수
		if(MANAGER.rx_buffer[3] == 0x00)
		{
			// 이 요청 받을 때 기준으로 이벤트 정보 백업
			MANAGER.event_sp_backup = EVENT.sp;
			
			MANAGER.event_rollover_backup = EVENT.rollover;
			
			if(MANAGER.event_rollover_backup == 0xaa)
			{	
				// 전체 블럭수 미리 계산
				MANAGER.event_send_block = 20;
							
				i = 200;
				
				make_crc_send(MANAGER.tx_buffer, &i, 2);
			}
			
			else
			{
				// 전체 블럭수 미리 계산				
				// 10개 단위
				i = MANAGER.event_sp_backup / 10;
				
				// 나머지가 있으면 한번더
				if(MANAGER.event_sp_backup % 10)
				++i;
				
				MANAGER.event_send_block = i;
				
				make_crc_send(MANAGER.tx_buffer, &MANAGER.event_sp_backup, 2);
			}
		}
		
		// 200개 기준 1 ~ 20
		// 회당 10개
		// 0부터 10개씩 날린다.
		// 시간순서 배열은 매니저가 한다.
		else
		{
			if(MANAGER.event_send_block > 20)
			; //nak 날림
			
			else if(MANAGER.rx_buffer[3] > MANAGER.event_send_block)
			; //nak 날림
			
			else
			{
				// header
				MANAGER.tx_buffer[0] = '#';
				MANAGER.tx_buffer[1] = ADDRESS.address;
				MANAGER.tx_buffer[2] = MANAGER.rx_buffer[2];
				MANAGER.tx_buffer[3] = MANAGER.rx_buffer[3];
			
			
				// pc가 보낸 블럭 수
				i = MANAGER.rx_buffer[3];
				
				// 포인터 스타트 포인트
				--i;
				
				// 10개 단위, 하나의 이벤트 18바이트로 구성
				i *= 180;
				
				// 첫번째는 반다시 있으니 걍 보냄
				// 1st
				MANAGER.tx_buffer[  6] = *(EVENT_YEAR     + i) & 0x00ff;
				MANAGER.tx_buffer[  7] = *(EVENT_MONTH    + i) & 0x00ff;
				MANAGER.tx_buffer[  8] = *(EVENT_DAY      + i) & 0x00ff;
				MANAGER.tx_buffer[  9] = *(EVENT_HOUR     + i) & 0x00ff;
				MANAGER.tx_buffer[ 10] = *(EVENT_MINUTE   + i) & 0x00ff;
				MANAGER.tx_buffer[ 11] = *(EVENT_SECOND   + i) & 0x00ff;
				MANAGER.tx_buffer[ 12] = *(EVENT_MS1      + i) & 0x00ff;
				MANAGER.tx_buffer[ 13] = *(EVENT_MS2      + i) & 0x00ff;
				MANAGER.tx_buffer[ 14] = *(EVENT_INDEX1   + i) & 0x00ff;
				MANAGER.tx_buffer[ 15] = *(EVENT_INDEX2   + i) & 0x00ff;
				MANAGER.tx_buffer[ 16] = *(EVENT_CONTENT1 + i) & 0x00ff;
				MANAGER.tx_buffer[ 17] = *(EVENT_CONTENT2 + i) & 0x00ff;
				MANAGER.tx_buffer[ 18] = *(EVENT_RATIO1   + i) & 0x00ff;
				MANAGER.tx_buffer[ 19] = *(EVENT_RATIO2   + i) & 0x00ff;
				MANAGER.tx_buffer[ 20] = *(EVENT_OPTIME1  + i) & 0x00ff;
				MANAGER.tx_buffer[ 21] = *(EVENT_OPTIME2  + i) & 0x00ff;
				MANAGER.tx_buffer[ 22] = *(EVENT_OPTIME3  + i) & 0x00ff;
				MANAGER.tx_buffer[ 23] = *(EVENT_OPTIME4  + i) & 0x00ff;
					
				// rollover가 아니면
				if(MANAGER.event_rollover_backup != 0xaa)
				{
					// 요청 블럭에서 1뺌
					j = MANAGER.rx_buffer[3] - 1;
					
					// 곱하기 10
					j *= 10;
					
					// 두번째
					j += 1;
					
					// sp가 같으면 다 보냈다는 의미
					if(j == MANAGER.event_sp_backup)
					{
						j = 18;
						
						goto event_send;
					}
				}
				
				// 2nd
				j = i + 18;
				
				MANAGER.tx_buffer[ 24] = *(EVENT_YEAR     + j) & 0x00ff;
				MANAGER.tx_buffer[ 25] = *(EVENT_MONTH    + j) & 0x00ff;
				MANAGER.tx_buffer[ 26] = *(EVENT_DAY      + j) & 0x00ff;
				MANAGER.tx_buffer[ 27] = *(EVENT_HOUR     + j) & 0x00ff;
				MANAGER.tx_buffer[ 28] = *(EVENT_MINUTE   + j) & 0x00ff;
				MANAGER.tx_buffer[ 29] = *(EVENT_SECOND   + j) & 0x00ff;
				MANAGER.tx_buffer[ 30] = *(EVENT_MS1      + j) & 0x00ff;
				MANAGER.tx_buffer[ 31] = *(EVENT_MS2      + j) & 0x00ff;
				MANAGER.tx_buffer[ 32] = *(EVENT_INDEX1   + j) & 0x00ff;
				MANAGER.tx_buffer[ 33] = *(EVENT_INDEX2   + j) & 0x00ff;
				MANAGER.tx_buffer[ 34] = *(EVENT_CONTENT1 + j) & 0x00ff;
				MANAGER.tx_buffer[ 35] = *(EVENT_CONTENT2 + j) & 0x00ff;
				MANAGER.tx_buffer[ 36] = *(EVENT_RATIO1   + j) & 0x00ff;
				MANAGER.tx_buffer[ 37] = *(EVENT_RATIO2   + j) & 0x00ff;
				MANAGER.tx_buffer[ 38] = *(EVENT_OPTIME1  + j) & 0x00ff;
				MANAGER.tx_buffer[ 39] = *(EVENT_OPTIME2  + j) & 0x00ff;
				MANAGER.tx_buffer[ 40] = *(EVENT_OPTIME3  + j) & 0x00ff;
				MANAGER.tx_buffer[ 41] = *(EVENT_OPTIME4  + j) & 0x00ff;
				
				// rollover가 아니면
				if(MANAGER.event_rollover_backup != 0xaa)
				{
					// 요청 블럭에서 1뺌
					j = MANAGER.rx_buffer[3] - 1;
					
					// 곱하기 10
					j *= 10;
					
					// 세번째
					j += 2;
					
					// sp가 같으면 다 보냈다는 의미
					if(j == MANAGER.event_sp_backup)
					{
						j = 36;
						
						goto event_send;
					}
				}
				
				// 3rd
				j = i + 36;
				
				MANAGER.tx_buffer[ 42] = *(EVENT_YEAR     + j) & 0x00ff;
				MANAGER.tx_buffer[ 43] = *(EVENT_MONTH    + j) & 0x00ff;
				MANAGER.tx_buffer[ 44] = *(EVENT_DAY      + j) & 0x00ff;
				MANAGER.tx_buffer[ 45] = *(EVENT_HOUR     + j) & 0x00ff;
				MANAGER.tx_buffer[ 46] = *(EVENT_MINUTE   + j) & 0x00ff;
				MANAGER.tx_buffer[ 47] = *(EVENT_SECOND   + j) & 0x00ff;
				MANAGER.tx_buffer[ 48] = *(EVENT_MS1      + j) & 0x00ff;
				MANAGER.tx_buffer[ 49] = *(EVENT_MS2      + j) & 0x00ff;
				MANAGER.tx_buffer[ 50] = *(EVENT_INDEX1   + j) & 0x00ff;
				MANAGER.tx_buffer[ 51] = *(EVENT_INDEX2   + j) & 0x00ff;
				MANAGER.tx_buffer[ 52] = *(EVENT_CONTENT1 + j) & 0x00ff;
				MANAGER.tx_buffer[ 53] = *(EVENT_CONTENT2 + j) & 0x00ff;
				MANAGER.tx_buffer[ 54] = *(EVENT_RATIO1   + j) & 0x00ff;
				MANAGER.tx_buffer[ 55] = *(EVENT_RATIO2   + j) & 0x00ff;
				MANAGER.tx_buffer[ 56] = *(EVENT_OPTIME1  + j) & 0x00ff;
				MANAGER.tx_buffer[ 57] = *(EVENT_OPTIME2  + j) & 0x00ff;
				MANAGER.tx_buffer[ 58] = *(EVENT_OPTIME3  + j) & 0x00ff;
				MANAGER.tx_buffer[ 59] = *(EVENT_OPTIME4  + j) & 0x00ff;
				
				// rollover가 아니면
				if(MANAGER.event_rollover_backup != 0xaa)
				{
					// 요청 블럭에서 1뺌
					j = MANAGER.rx_buffer[3] - 1;
					
					// 곱하기 10
					j *= 10;
					
					// 네번째
					j += 3;
					
					// sp가 같으면 다 보냈다는 의미
					if(j == MANAGER.event_sp_backup)
					{
						j = 54;
						
						goto event_send;
					}
				}
				
				// 4th
				j = i + 54;
				
				MANAGER.tx_buffer[ 60] = *(EVENT_YEAR     + j) & 0x00ff;
				MANAGER.tx_buffer[ 61] = *(EVENT_MONTH    + j) & 0x00ff;
				MANAGER.tx_buffer[ 62] = *(EVENT_DAY      + j) & 0x00ff;
				MANAGER.tx_buffer[ 63] = *(EVENT_HOUR     + j) & 0x00ff;
				MANAGER.tx_buffer[ 64] = *(EVENT_MINUTE   + j) & 0x00ff;
				MANAGER.tx_buffer[ 65] = *(EVENT_SECOND   + j) & 0x00ff;
				MANAGER.tx_buffer[ 66] = *(EVENT_MS1      + j) & 0x00ff;
				MANAGER.tx_buffer[ 67] = *(EVENT_MS2      + j) & 0x00ff;
				MANAGER.tx_buffer[ 68] = *(EVENT_INDEX1   + j) & 0x00ff;
				MANAGER.tx_buffer[ 69] = *(EVENT_INDEX2   + j) & 0x00ff;
				MANAGER.tx_buffer[ 70] = *(EVENT_CONTENT1 + j) & 0x00ff;
				MANAGER.tx_buffer[ 71] = *(EVENT_CONTENT2 + j) & 0x00ff;
				MANAGER.tx_buffer[ 72] = *(EVENT_RATIO1   + j) & 0x00ff;
				MANAGER.tx_buffer[ 73] = *(EVENT_RATIO2   + j) & 0x00ff;
				MANAGER.tx_buffer[ 74] = *(EVENT_OPTIME1  + j) & 0x00ff;
				MANAGER.tx_buffer[ 75] = *(EVENT_OPTIME2  + j) & 0x00ff;
				MANAGER.tx_buffer[ 76] = *(EVENT_OPTIME3  + j) & 0x00ff;
				MANAGER.tx_buffer[ 77] = *(EVENT_OPTIME4  + j) & 0x00ff;
				
				// rollover가 아니면
				if(MANAGER.event_rollover_backup != 0xaa)
				{
					// 요청 블럭에서 1뺌
					j = MANAGER.rx_buffer[3] - 1;
					
					// 곱하기 10
					j *= 10;
					
					// 다섯번째
					j += 4;
					
					// sp가 같으면 다 보냈다는 의미
					if(j == MANAGER.event_sp_backup)
					{
						j = 72;
						
						goto event_send;
					}
				}
				
				// 5th
				j = i + 72;
				
				MANAGER.tx_buffer[ 78] = *(EVENT_YEAR     + j) & 0x00ff;
				MANAGER.tx_buffer[ 79] = *(EVENT_MONTH    + j) & 0x00ff;
				MANAGER.tx_buffer[ 80] = *(EVENT_DAY      + j) & 0x00ff;
				MANAGER.tx_buffer[ 81] = *(EVENT_HOUR     + j) & 0x00ff;
				MANAGER.tx_buffer[ 82] = *(EVENT_MINUTE   + j) & 0x00ff;
				MANAGER.tx_buffer[ 83] = *(EVENT_SECOND   + j) & 0x00ff;
				MANAGER.tx_buffer[ 84] = *(EVENT_MS1      + j) & 0x00ff;
				MANAGER.tx_buffer[ 85] = *(EVENT_MS2      + j) & 0x00ff;
				MANAGER.tx_buffer[ 86] = *(EVENT_INDEX1   + j) & 0x00ff;
				MANAGER.tx_buffer[ 87] = *(EVENT_INDEX2   + j) & 0x00ff;
				MANAGER.tx_buffer[ 88] = *(EVENT_CONTENT1 + j) & 0x00ff;
				MANAGER.tx_buffer[ 89] = *(EVENT_CONTENT2 + j) & 0x00ff;
				MANAGER.tx_buffer[ 90] = *(EVENT_RATIO1   + j) & 0x00ff;
				MANAGER.tx_buffer[ 91] = *(EVENT_RATIO2   + j) & 0x00ff;
				MANAGER.tx_buffer[ 92] = *(EVENT_OPTIME1  + j) & 0x00ff;
				MANAGER.tx_buffer[ 93] = *(EVENT_OPTIME2  + j) & 0x00ff;
				MANAGER.tx_buffer[ 94] = *(EVENT_OPTIME3  + j) & 0x00ff;
				MANAGER.tx_buffer[ 95] = *(EVENT_OPTIME4  + j) & 0x00ff;
				
				// rollover가 아니면
				if(MANAGER.event_rollover_backup != 0xaa)
				{
					// 요청 블럭에서 1뺌
					j = MANAGER.rx_buffer[3] - 1;
					
					// 곱하기 10
					j *= 10;
					
					// 여섯번째
					j += 5;
					
					// sp가 같으면 다 보냈다는 의미
					if(j == MANAGER.event_sp_backup)
					{
						j = 90;
						
						goto event_send;
					}
				}
				
				// 6th
				j = i + 90;
				
				MANAGER.tx_buffer[ 96] = *(EVENT_YEAR     + j) & 0x00ff;
				MANAGER.tx_buffer[ 97] = *(EVENT_MONTH    + j) & 0x00ff;
				MANAGER.tx_buffer[ 98] = *(EVENT_DAY      + j) & 0x00ff;
				MANAGER.tx_buffer[ 99] = *(EVENT_HOUR     + j) & 0x00ff;
				MANAGER.tx_buffer[100] = *(EVENT_MINUTE   + j) & 0x00ff;
				MANAGER.tx_buffer[101] = *(EVENT_SECOND   + j) & 0x00ff;
				MANAGER.tx_buffer[102] = *(EVENT_MS1      + j) & 0x00ff;
				MANAGER.tx_buffer[103] = *(EVENT_MS2      + j) & 0x00ff;
				MANAGER.tx_buffer[104] = *(EVENT_INDEX1   + j) & 0x00ff;
				MANAGER.tx_buffer[105] = *(EVENT_INDEX2   + j) & 0x00ff;
				MANAGER.tx_buffer[106] = *(EVENT_CONTENT1 + j) & 0x00ff;
				MANAGER.tx_buffer[107] = *(EVENT_CONTENT2 + j) & 0x00ff;
				MANAGER.tx_buffer[108] = *(EVENT_RATIO1   + j) & 0x00ff;
				MANAGER.tx_buffer[109] = *(EVENT_RATIO2   + j) & 0x00ff;
				MANAGER.tx_buffer[110] = *(EVENT_OPTIME1  + j) & 0x00ff;
				MANAGER.tx_buffer[111] = *(EVENT_OPTIME2  + j) & 0x00ff;
				MANAGER.tx_buffer[112] = *(EVENT_OPTIME3  + j) & 0x00ff;
				MANAGER.tx_buffer[113] = *(EVENT_OPTIME4  + j) & 0x00ff;
				
				// rollover가 아니면
				if(MANAGER.event_rollover_backup != 0xaa)
				{
					// 요청 블럭에서 1뺌
					j = MANAGER.rx_buffer[3] - 1;
					
					// 곱하기 10
					j *= 10;
					
					// 일곱번째
					j += 6;
					
					// sp가 같으면 다 보냈다는 의미
					if(j == MANAGER.event_sp_backup)
					{
						j = 108;
						
						goto event_send;
					}
				}
				
				// 7th
				j = i + 108;
				
				MANAGER.tx_buffer[114] = *(EVENT_YEAR     + j) & 0x00ff;
				MANAGER.tx_buffer[115] = *(EVENT_MONTH    + j) & 0x00ff;
				MANAGER.tx_buffer[116] = *(EVENT_DAY      + j) & 0x00ff;
				MANAGER.tx_buffer[117] = *(EVENT_HOUR     + j) & 0x00ff;
				MANAGER.tx_buffer[118] = *(EVENT_MINUTE   + j) & 0x00ff;
				MANAGER.tx_buffer[119] = *(EVENT_SECOND   + j) & 0x00ff;
				MANAGER.tx_buffer[120] = *(EVENT_MS1      + j) & 0x00ff;
				MANAGER.tx_buffer[121] = *(EVENT_MS2      + j) & 0x00ff;
				MANAGER.tx_buffer[122] = *(EVENT_INDEX1   + j) & 0x00ff;
				MANAGER.tx_buffer[123] = *(EVENT_INDEX2   + j) & 0x00ff;
				MANAGER.tx_buffer[124] = *(EVENT_CONTENT1 + j) & 0x00ff;
				MANAGER.tx_buffer[125] = *(EVENT_CONTENT2 + j) & 0x00ff;
				MANAGER.tx_buffer[126] = *(EVENT_RATIO1   + j) & 0x00ff;
				MANAGER.tx_buffer[127] = *(EVENT_RATIO2   + j) & 0x00ff;
				MANAGER.tx_buffer[128] = *(EVENT_OPTIME1  + j) & 0x00ff;
				MANAGER.tx_buffer[129] = *(EVENT_OPTIME2  + j) & 0x00ff;
				MANAGER.tx_buffer[130] = *(EVENT_OPTIME3  + j) & 0x00ff;
				MANAGER.tx_buffer[131] = *(EVENT_OPTIME4  + j) & 0x00ff;
				
				
				// rollover가 아니면
				if(MANAGER.event_rollover_backup != 0xaa)
				{
					// 요청 블럭에서 1뺌
					j = MANAGER.rx_buffer[3] - 1;
					
					// 곱하기 10
					j *= 10;
					
					// 여덟번째
					j += 7;
					
					// sp가 같으면 다 보냈다는 의미
					if(j == MANAGER.event_sp_backup)
					{
						j = 126;
						
						goto event_send;
					}
				}
				
				// 8th
				j = i + 126;
				
				MANAGER.tx_buffer[132] = *(EVENT_YEAR     + j) & 0x00ff;
				MANAGER.tx_buffer[133] = *(EVENT_MONTH    + j) & 0x00ff;
				MANAGER.tx_buffer[134] = *(EVENT_DAY      + j) & 0x00ff;
				MANAGER.tx_buffer[135] = *(EVENT_HOUR     + j) & 0x00ff;
				MANAGER.tx_buffer[136] = *(EVENT_MINUTE   + j) & 0x00ff;
				MANAGER.tx_buffer[137] = *(EVENT_SECOND   + j) & 0x00ff;
				MANAGER.tx_buffer[138] = *(EVENT_MS1      + j) & 0x00ff;
				MANAGER.tx_buffer[139] = *(EVENT_MS2      + j) & 0x00ff;
				MANAGER.tx_buffer[140] = *(EVENT_INDEX1   + j) & 0x00ff;
				MANAGER.tx_buffer[141] = *(EVENT_INDEX2   + j) & 0x00ff;
				MANAGER.tx_buffer[142] = *(EVENT_CONTENT1 + j) & 0x00ff;
				MANAGER.tx_buffer[143] = *(EVENT_CONTENT2 + j) & 0x00ff;
				MANAGER.tx_buffer[144] = *(EVENT_RATIO1   + j) & 0x00ff;
				MANAGER.tx_buffer[145] = *(EVENT_RATIO2   + j) & 0x00ff;
				MANAGER.tx_buffer[146] = *(EVENT_OPTIME1  + j) & 0x00ff;
				MANAGER.tx_buffer[147] = *(EVENT_OPTIME2  + j) & 0x00ff;
				MANAGER.tx_buffer[148] = *(EVENT_OPTIME3  + j) & 0x00ff;
				MANAGER.tx_buffer[149] = *(EVENT_OPTIME4  + j) & 0x00ff;
				
				// rollover가 아니면
				if(MANAGER.event_rollover_backup != 0xaa)
				{
					// 요청 블럭에서 1뺌
					j = MANAGER.rx_buffer[3] - 1;
					
					// 곱하기 10
					j *= 10;
					
					// 아홉번째
					j += 8;
					
					// sp가 같으면 다 보냈다는 의미
					if(j == MANAGER.event_sp_backup)
					{
						j = 144;
						
						goto event_send;
					}
				}
				
				// 9th
				j = i + 144;
				
				MANAGER.tx_buffer[150] = *(EVENT_YEAR     + j) & 0x00ff;
				MANAGER.tx_buffer[151] = *(EVENT_MONTH    + j) & 0x00ff;
				MANAGER.tx_buffer[152] = *(EVENT_DAY      + j) & 0x00ff;
				MANAGER.tx_buffer[153] = *(EVENT_HOUR     + j) & 0x00ff;
				MANAGER.tx_buffer[154] = *(EVENT_MINUTE   + j) & 0x00ff;
				MANAGER.tx_buffer[155] = *(EVENT_SECOND   + j) & 0x00ff;
				MANAGER.tx_buffer[156] = *(EVENT_MS1      + j) & 0x00ff;
				MANAGER.tx_buffer[157] = *(EVENT_MS2      + j) & 0x00ff;
				MANAGER.tx_buffer[158] = *(EVENT_INDEX1   + j) & 0x00ff;
				MANAGER.tx_buffer[159] = *(EVENT_INDEX2   + j) & 0x00ff;
				MANAGER.tx_buffer[160] = *(EVENT_CONTENT1 + j) & 0x00ff;
				MANAGER.tx_buffer[161] = *(EVENT_CONTENT2 + j) & 0x00ff;
				MANAGER.tx_buffer[162] = *(EVENT_RATIO1   + j) & 0x00ff;
				MANAGER.tx_buffer[163] = *(EVENT_RATIO2   + j) & 0x00ff;
				MANAGER.tx_buffer[164] = *(EVENT_OPTIME1  + j) & 0x00ff;
				MANAGER.tx_buffer[165] = *(EVENT_OPTIME2  + j) & 0x00ff;
				MANAGER.tx_buffer[166] = *(EVENT_OPTIME3  + j) & 0x00ff;
				MANAGER.tx_buffer[167] = *(EVENT_OPTIME4  + j) & 0x00ff;
				
				// rollover가 아니면
				if(MANAGER.event_rollover_backup != 0xaa)
				{
					// 요청 블럭에서 1뺌
					j = MANAGER.rx_buffer[3] - 1;
					
					// 곱하기 10
					j *= 10;
					
					// 열번째
					j += 9;
					
					// sp가 같으면 다 보냈다는 의미
					if(j == MANAGER.event_sp_backup)
					{
						j = 162;
						
						goto event_send;
					}
				}
				
				// 10th
				j = i + 162;
				
				MANAGER.tx_buffer[168] = *(EVENT_YEAR     + j) & 0x00ff;
				MANAGER.tx_buffer[169] = *(EVENT_MONTH    + j) & 0x00ff;
				MANAGER.tx_buffer[170] = *(EVENT_DAY      + j) & 0x00ff;
				MANAGER.tx_buffer[171] = *(EVENT_HOUR     + j) & 0x00ff;
				MANAGER.tx_buffer[172] = *(EVENT_MINUTE   + j) & 0x00ff;
				MANAGER.tx_buffer[173] = *(EVENT_SECOND   + j) & 0x00ff;
				MANAGER.tx_buffer[174] = *(EVENT_MS1      + j) & 0x00ff;
				MANAGER.tx_buffer[175] = *(EVENT_MS2      + j) & 0x00ff;
				MANAGER.tx_buffer[176] = *(EVENT_INDEX1   + j) & 0x00ff;
				MANAGER.tx_buffer[177] = *(EVENT_INDEX2   + j) & 0x00ff;
				MANAGER.tx_buffer[178] = *(EVENT_CONTENT1 + j) & 0x00ff;
				MANAGER.tx_buffer[179] = *(EVENT_CONTENT2 + j) & 0x00ff;
				MANAGER.tx_buffer[180] = *(EVENT_RATIO1   + j) & 0x00ff;
				MANAGER.tx_buffer[181] = *(EVENT_RATIO2   + j) & 0x00ff;
				MANAGER.tx_buffer[182] = *(EVENT_OPTIME1  + j) & 0x00ff;
				MANAGER.tx_buffer[183] = *(EVENT_OPTIME2  + j) & 0x00ff;
				MANAGER.tx_buffer[184] = *(EVENT_OPTIME3  + j) & 0x00ff;
				MANAGER.tx_buffer[185] = *(EVENT_OPTIME4  + j) & 0x00ff;
				
				j = 180;
				
				// length
event_send:		MANAGER.tx_buffer[4] = j >> 8;
				MANAGER.tx_buffer[5] = j & 0x00ff;
	
				i = COMM_CRC(MANAGER.tx_buffer, 6 + j);
	
				MANAGER.tx_buffer[6 + j] = i >> 8;
				MANAGER.tx_buffer[7 + j] = i & 0x00ff;
				
				MANAGER.tx_length = j + 8;
				
				MANAGER.isr_tx = MANAGER.tx_buffer;
				
				// tx interrupt 활성
				*ScibRegs_SCICTL2 |= 0x0001;
				
				// tx intrrupt 활성화 후 최초 한번 써야함
				MANAGER.tx_count = 1;
				*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
			}
		}
	}
	
	// wave 존재/calibration
	else if(MANAGER.rx_buffer[2] == 0x60)
	{
		// wave 존재여부
		if(MANAGER.rx_buffer[3] == 0x00)
		{
			//khs, 2015-04-08 오전 11:21:40
			//make_crc_send(MANAGER.tx_buffer, WAVE_WRITE_CHECK, 2);
		}
		
		// calibration factor
		else if(MANAGER.rx_buffer[3] == 0x01)
		{
			// header
			MANAGER.tx_buffer[0] = '#';
			MANAGER.tx_buffer[1] = ADDRESS.address;
			MANAGER.tx_buffer[2] = MANAGER.rx_buffer[2];
			MANAGER.tx_buffer[3] = MANAGER.rx_buffer[3];
			
			// length
			MANAGER.tx_buffer[4] = 0;
			MANAGER.tx_buffer[5] = 32;
			
			//internal ct ratio
			float_temp = INTERNAL_CT_RATIO;
			float_to_integer(float_temp, &MANAGER.tx_buffer[6], 1.0F);
			
			//internal nct ratio
			float_temp = INTERNAL_NCT_RATIO;
			float_to_integer(float_temp, &MANAGER.tx_buffer[10], 1.0F);
			
			//internal pt ratio
			float_temp = INTERNAL_PT_RATIO;
			float_to_integer(float_temp, &MANAGER.tx_buffer[14], 1.0F);
			
			//internal gpt ratio
			float_temp = INTERNAL_GPT_RATIO;
			float_to_integer(float_temp, &MANAGER.tx_buffer[18], 1.0F);

//khs, 2015-04-08 오전 11:21:40
//			//1차측 ct ratio
//			float_to_integer(TRANSFORMER.ct_ratio, &MANAGER.tx_buffer[22], 1.0F);
//
//			//1차측 nct ratio
//			float_to_integer(TRANSFORMER.nct_ratio, &MANAGER.tx_buffer[26], 1.0F);
//
//			//1차측 pt ratio
//			float_to_integer(TRANSFORMER.pt_ratio, &MANAGER.tx_buffer[30], 1.0F);
//
//			//1차측 gpt ratio
//			float_to_integer(TRANSFORMER.gpt_ratio, &MANAGER.tx_buffer[34], 1.0F);
			
			
			i = COMM_CRC(MANAGER.tx_buffer, 38);
			
			MANAGER.tx_buffer[38] = i >> 8;
			MANAGER.tx_buffer[39] = i & 0x00ff;
			
			MANAGER.tx_length = 40;
			
			MANAGER.isr_tx = MANAGER.tx_buffer;
			
			// tx interrupt 활성
			*ScibRegs_SCICTL2 |= 0x0001;
			
			// tx intrrupt 활성화 후 최초 한번 써야함
			MANAGER.tx_count = 1;
			*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
		}
	}
	
	
	// Ia wave
	else if(MANAGER.rx_buffer[2] == 0x61)
	{
		if(MANAGER.rx_buffer[3] > 0x15)
		{
			//nak
			serial_ok_nak_send(0x05);
			
			return;
		}
		
		// header
		*(Manager_tx_long    ) = '#';
		*(Manager_tx_long + 1) = ADDRESS.address;
		*(Manager_tx_long + 2) = MANAGER.rx_buffer[2];
		*(Manager_tx_long + 3) = MANAGER.rx_buffer[3];
		
		// 마지막
		if(MANAGER.rx_buffer[3] == 0x15)
		{
			*(Manager_tx_long + 4) = 0x00;
			*(Manager_tx_long + 5) = 0x60;
			 
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Ia, MANAGER.rx_buffer[3] * 512, 48);
		}
		
		else
		{
			*(Manager_tx_long + 4) = 0x04;
			*(Manager_tx_long + 5) = 0x00;
			
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Ia, MANAGER.rx_buffer[3] * 512, 512);
		}
	}
	
	// Ib wave
	else if(MANAGER.rx_buffer[2] == 0x62)
	{
		if(MANAGER.rx_buffer[3] > 0x15)
		{
			//nak
			serial_ok_nak_send(0x05);
			
			return;
		}
		
		// header
		*(Manager_tx_long    ) = '#';
		*(Manager_tx_long + 1) = ADDRESS.address;
		*(Manager_tx_long + 2) = MANAGER.rx_buffer[2];
		*(Manager_tx_long + 3) = MANAGER.rx_buffer[3];
		
		// 마지막
		if(MANAGER.rx_buffer[3] == 0x15)
		{
			*(Manager_tx_long + 4) = 0x00;
			*(Manager_tx_long + 5) = 0x60;
			 
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Ib, MANAGER.rx_buffer[3] * 512, 48);
		}
		
		else
		{
			*(Manager_tx_long + 4) = 0x04;
			*(Manager_tx_long + 5) = 0x00;
			
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Ib, MANAGER.rx_buffer[3] * 512, 512);
		}
	}
	
	// Ic wave
	else if(MANAGER.rx_buffer[2] == 0x63)
	{
		if(MANAGER.rx_buffer[3] > 0x15)
		{
			//nak
			serial_ok_nak_send(0x05);
			
			return;
		}
		
		// header
		*(Manager_tx_long    ) = '#';
		*(Manager_tx_long + 1) = ADDRESS.address;
		*(Manager_tx_long + 2) = MANAGER.rx_buffer[2];
		*(Manager_tx_long + 3) = MANAGER.rx_buffer[3];
		
		// 마지막
		if(MANAGER.rx_buffer[3] == 0x15)
		{
			*(Manager_tx_long + 4) = 0x00;
			*(Manager_tx_long + 5) = 0x60;
			 
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Ic, MANAGER.rx_buffer[3] * 512, 48);
		}
		
		else
		{
			*(Manager_tx_long + 4) = 0x04;
			*(Manager_tx_long + 5) = 0x00;
			
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Ic, MANAGER.rx_buffer[3] * 512, 512);
		}
	}
	
	// In wave
	else if(MANAGER.rx_buffer[2] == 0x64)
	{
		if(MANAGER.rx_buffer[3] > 0x15)
		{
			//nak
			serial_ok_nak_send(0x05);
			
			return;
		}
		
		// header
		*(Manager_tx_long    ) = '#';
		*(Manager_tx_long + 1) = ADDRESS.address;
		*(Manager_tx_long + 2) = MANAGER.rx_buffer[2];
		*(Manager_tx_long + 3) = MANAGER.rx_buffer[3];
		
		// 마지막
		if(MANAGER.rx_buffer[3] == 0x15)
		{
			*(Manager_tx_long + 4) = 0x00;
			*(Manager_tx_long + 5) = 0x60;
			 
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_In, MANAGER.rx_buffer[3] * 512, 48);
		}
		
		else
		{
			*(Manager_tx_long + 4) = 0x04;
			*(Manager_tx_long + 5) = 0x00;
			
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_In, MANAGER.rx_buffer[3] * 512, 512);
		}
	}
	
	// Va wave
	else if(MANAGER.rx_buffer[2] == 0x65)
	{
		if(MANAGER.rx_buffer[3] > 0x15)
		{
			//nak
			serial_ok_nak_send(0x05);
			
			return;
		}
		
		// header
		*(Manager_tx_long    ) = '#';
		*(Manager_tx_long + 1) = ADDRESS.address;
		*(Manager_tx_long + 2) = MANAGER.rx_buffer[2];
		*(Manager_tx_long + 3) = MANAGER.rx_buffer[3];
		
		// 마지막
		if(MANAGER.rx_buffer[3] == 0x15)
		{
			*(Manager_tx_long + 4) = 0x00;
			*(Manager_tx_long + 5) = 0x60;
			 
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Va, MANAGER.rx_buffer[3] * 512, 48);
		}
		
		else
		{
			*(Manager_tx_long + 4) = 0x04;
			*(Manager_tx_long + 5) = 0x00;
			
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Va, MANAGER.rx_buffer[3] * 512, 512);
		}
	}
	
	// Vb wave
	else if(MANAGER.rx_buffer[2] == 0x66)
	{
		if(MANAGER.rx_buffer[3] > 0x15)
		{
			//nak
			serial_ok_nak_send(0x05);
			
			return;
		}
		
		// header
		*(Manager_tx_long    ) = '#';
		*(Manager_tx_long + 1) = ADDRESS.address;
		*(Manager_tx_long + 2) = MANAGER.rx_buffer[2];
		*(Manager_tx_long + 3) = MANAGER.rx_buffer[3];
		
		// 마지막
		if(MANAGER.rx_buffer[3] == 0x15)
		{
			*(Manager_tx_long + 4) = 0x00;
			*(Manager_tx_long + 5) = 0x60;
			 
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Vb, MANAGER.rx_buffer[3] * 512, 48);
		}
		
		else
		{
			*(Manager_tx_long + 4) = 0x04;
			*(Manager_tx_long + 5) = 0x00;
			
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Vb, MANAGER.rx_buffer[3] * 512, 512);
		}
	}
	
	// Vc wave
	else if(MANAGER.rx_buffer[2] == 0x67)
	{
		if(MANAGER.rx_buffer[3] > 0x15)
		{
			//nak
			serial_ok_nak_send(0x05);
			
			return;
		}
		
		// header
		*(Manager_tx_long    ) = '#';
		*(Manager_tx_long + 1) = ADDRESS.address;
		*(Manager_tx_long + 2) = MANAGER.rx_buffer[2];
		*(Manager_tx_long + 3) = MANAGER.rx_buffer[3];
		
		// 마지막
		if(MANAGER.rx_buffer[3] == 0x15)
		{
			*(Manager_tx_long + 4) = 0x00;
			*(Manager_tx_long + 5) = 0x60;
			 
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Vc, MANAGER.rx_buffer[3] * 512, 48);
		}
		
		else
		{
			*(Manager_tx_long + 4) = 0x04;
			*(Manager_tx_long + 5) = 0x00;
			
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Vc, MANAGER.rx_buffer[3] * 512, 512);
		}
	}
	
	
	// Vn wave
	else if(MANAGER.rx_buffer[2] == 0x68)
	{
		if(MANAGER.rx_buffer[3] > 0x15)
		{
			//nak
			serial_ok_nak_send(0x05);
			
			return;
		}
		
		// header
		*(Manager_tx_long    ) = '#';
		*(Manager_tx_long + 1) = ADDRESS.address;
		*(Manager_tx_long + 2) = MANAGER.rx_buffer[2];
		*(Manager_tx_long + 3) = MANAGER.rx_buffer[3];
		
		// 마지막
		if(MANAGER.rx_buffer[3] == 0x15)
		{
			*(Manager_tx_long + 4) = 0x00;
			*(Manager_tx_long + 5) = 0x60;
			 
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Vn, MANAGER.rx_buffer[3] * 512, 48);
		}
		
		else
		{
			*(Manager_tx_long + 4) = 0x04;
			*(Manager_tx_long + 5) = 0x00;
			
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_Vn, MANAGER.rx_buffer[3] * 512, 512);
		}
	}
	
	
	// Realy wave
	else if(MANAGER.rx_buffer[2] == 0x69)
	{
		if(MANAGER.rx_buffer[3] > 0x07)
		{
			//nak
			serial_ok_nak_send(0x05);
			
			return;
		}
		
		// header
		*(Manager_tx_long    ) = '#';
		*(Manager_tx_long + 1) = ADDRESS.address;
		*(Manager_tx_long + 2) = MANAGER.rx_buffer[2];
		*(Manager_tx_long + 3) = MANAGER.rx_buffer[3];
		
		// 마지막
		if(MANAGER.rx_buffer[3] == 0x07)
		{
			*(Manager_tx_long + 4) = 0x01;
			*(Manager_tx_long + 5) = 0xb0;
			 
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_RELAY, MANAGER.rx_buffer[3] * 512, 216);
		}
		
		else
		{
			*(Manager_tx_long + 4) = 0x04;
			*(Manager_tx_long + 5) = 0x00;
			
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_RELAY, MANAGER.rx_buffer[3] * 512, 512);
		}
	}
	
	// DI wave
	else if(MANAGER.rx_buffer[2] == 0x6a)
	{
		if(MANAGER.rx_buffer[3] > 0x07)
		{
			//nak
			serial_ok_nak_send(0x05);
			
			return;
		}
		
		// header
		*(Manager_tx_long    ) = '#';
		*(Manager_tx_long + 1) = ADDRESS.address;
		*(Manager_tx_long + 2) = MANAGER.rx_buffer[2];
		*(Manager_tx_long + 3) = MANAGER.rx_buffer[3];
		
		// 마지막
		if(MANAGER.rx_buffer[3] == 0x07)
		{
			*(Manager_tx_long + 4) = 0x01;
			*(Manager_tx_long + 5) = 0xb0;
			 
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_DI, MANAGER.rx_buffer[3] * 512, 216);
		}
		
		else
		{
			*(Manager_tx_long + 4) = 0x04;
			*(Manager_tx_long + 5) = 0x00;
			
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_DI, MANAGER.rx_buffer[3] * 512, 512);
		}
	}
	
	
	// DO wave
	else if(MANAGER.rx_buffer[2] == 0x6b)
	{
		if(MANAGER.rx_buffer[3] > 0x07)
		{
			//nak
			serial_ok_nak_send(0x05);
			
			return;
		}
		
		// header
		*(Manager_tx_long    ) = '#';
		*(Manager_tx_long + 1) = ADDRESS.address;
		*(Manager_tx_long + 2) = MANAGER.rx_buffer[2];
		*(Manager_tx_long + 3) = MANAGER.rx_buffer[3];
		
		// 마지막
		if(MANAGER.rx_buffer[3] == 0x07)
		{
			*(Manager_tx_long + 4) = 0x01;
			*(Manager_tx_long + 5) = 0xb0;
			 
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_DO, MANAGER.rx_buffer[3] * 512, 216);
		}
		
		else
		{
			*(Manager_tx_long + 4) = 0x04;
			*(Manager_tx_long + 5) = 0x00;
			
			//맨마지막 wordcount
			wave_dump_serial_sram(FLASH_WAVE_DO, MANAGER.rx_buffer[3] * 512, 512);
		}
	}
		
	
//	/////////////////////////////////////////////////////////////////////
	//wrtie
	// 계전요소 write
	else if(MANAGER.rx_buffer[2] == 0x80)
	{
		//word count
		// ocr50-1
		if(MANAGER.rx_buffer[3] == 0x00) {
			serial_write(5, &OCR50_1.use, OCR50_1_USE);
			event_direct_save(&EVENT.relay_set);
		// ocr50-2
		} else if(MANAGER.rx_buffer[3] == 0x01)
		serial_write(5, &OCR50_2.use, OCR50_2_USE);
		
		// ocr51-1
		else if(MANAGER.rx_buffer[3] == 0x02)
		serial_write(5, &OCR51_1.use, OCR51_1_USE);
		
		// ocr51-2
		else if(MANAGER.rx_buffer[3] == 0x03)
		serial_write(5, &OCR51_2.use, OCR51_2_USE);
				
		// ocgr50
		else if(MANAGER.rx_buffer[3] == 0x04)
		serial_write(5, &OCGR50.use, OCGR50_USE);
		
		// ocgr51
		else if(MANAGER.rx_buffer[3] == 0x05)
		serial_write(5, &OCGR51.use, OCGR51_USE);

//2015.02.24
// DGR, SGR 나누어서 보내는 것 필요
		// DGR/SGR
//		else if(MANAGER.rx_buffer[3] == 0x06)
//		serial_write(6, &DSGR.use, DSGR_USE);
//2015.02.24 END
		
		// 59
		else if(MANAGER.rx_buffer[3] == 0x07)
		serial_write(5, &OVR.use, OVR_USE);
		
		// 64
		else if(MANAGER.rx_buffer[3] == 0x08)
		serial_write(5, &OVGR.use, OVGR_USE);
		
		// 27-1
		else if(MANAGER.rx_buffer[3] == 0x09)
		serial_write(4, &UVR_1.use, UVR_1_USE);
		
		// 27-2
		else if(MANAGER.rx_buffer[3] == 0x0a)
		serial_write(4, &UVR_2.use, UVR_2_USE);
		
		// 27-3
		else if(MANAGER.rx_buffer[3] == 0x0b)
		serial_write(4, &UVR_3.use, UVR_3_USE);
		
		// 47P
		else if(MANAGER.rx_buffer[3] == 0x0c)
		;//serial_write(4, &P47.use, P47_USE); //2015.02.24
		
		// 47N
		else if(MANAGER.rx_buffer[3] == 0x0d)
		;//serial_write(4, &N47.use, N47_USE); //2015.02.24

//2015.02.24
		// 25
//		else if(MANAGER.rx_buffer[3] == 0x0e)
//		serial_write(6, &SYNCRO.use, SYNCRO_USE);
//		
//		
//		// 37
//		else if(MANAGER.rx_buffer[3] == 0x0f)
//		serial_write(5, &UCR.use, UCR_USE);
//		
//		// 46
//		else if(MANAGER.rx_buffer[3] == 0x10)
//		serial_write(4, &NSOCR.use, NSOCR_USE);
//		
//		// 49
//		else if(MANAGER.rx_buffer[3] == 0x11)
//		serial_write(6, &THR.use, THR_USE);
//		
//		// 50H
//		else if(MANAGER.rx_buffer[3] == 0x12)
//		serial_write(4, &H50.use, H50_USE);
//		
//		// 51LR
//		else if(MANAGER.rx_buffer[3] == 0x13)
//		serial_write(6, &SL.use, S_L_USE);
//		
//		// 66
//		else if(MANAGER.rx_buffer[3] == 0x14)
//		serial_write(6, &NCH.use, NCH_USE);
//2015.02.24 END
	}
	
	// system write
	else if(MANAGER.rx_buffer[2] == 0x90)
	{
		
//MANAGER_SYSTEM_SET
//		if(MANAGER.rx_buffer[3] == 0x01) {
//			
//		}
		
//		// c/pt
//		if(MANAGER.rx_buffer[3] == 0x00)
//		;//khs, 2015-04-08 오전 11:30:47 serial_write(6, &TRANSFORMER.ct_primary, CT_PRIMARY);
//		
//		// di debounce / do property
//		else if(MANAGER.rx_buffer[3] == 0x01)
//		;//khs, 2015-04-08 오전 11:30:47 serial_write(9, DIDO.debounce, DI_DEBOUNCE1);
//				
//		//supervision
//		else if(MANAGER.rx_buffer[3] == 0x02)
//		;
//		
//		// time read
//		else if(MANAGER.rx_buffer[3] == 0x03)
//		{
//			for(i = 0; i < 6; i++)
//			{
//				MANAGER.temp[i] = MANAGER.rx_buffer[6 + (i << 1)];
//				
//				MANAGER.temp[i] <<= 8;
//				
//				MANAGER.temp[i] |= MANAGER.rx_buffer[7 + (i << 1)];
//			}
//			
//			TIME.update = 0;
//			TIME.milisecond = 0;
//			TIME.buffer = MANAGER.temp;
//			
//			
//			EVENT.system_set |= TIME_SET_EVENT;
//			event_direct_save(&EVENT.system_set);
//			
//			serial_ok_nak_send(0x00);
//		}
//		
//		//comm
//		else if(MANAGER.rx_buffer[3] == 0x04)
//		;
//		
//		//local control
//		else if(MANAGER.rx_buffer[3] == 0x05)
//		;//serial_write(1, &LOCAL_CONTROL.mode, LOCAL_CTRL_USE); //2015.02.24
//		
//		//ocgr/dgr/zct angle
//		else if(MANAGER.rx_buffer[3] == 0x06)
//		;//khs, 2015-04-08 오전 11:30:47 serial_write(2, &SYSTEM_SET.ocgr_dgr, OCGR_DGR_SEL);
//		
//		//ocr mode
//		else if(MANAGER.rx_buffer[3] == 0x06)
//		;//khs, 2015-04-08 오전 11:30:47 serial_write(1, &SYSTEM_SET.ocr_mode, OCR_PROPERTY);
	}
	
	// reset command
	else if(MANAGER.rx_buffer[2] == 0xa0)
	{
		//event clear
		if(MANAGER.rx_buffer[3] == 0x00)
		{			
			*EVENT_ROLLOVER = 0;
			*EVENT_SP = 0;
						
			EVENT.sp = 0;
			EVENT.rollover = 0;
						
			event_direct_save(&EVENT.data_reset);
			
			serial_ok_nak_send(0);
		}
		
		//energy  clear
		else if(MANAGER.rx_buffer[3] == 0x01)
		{			
			ACCUMULATION.energy_p = 0;
			ACCUMULATION.energy_q = 0;
			ACCUMULATION.energy_rp = 0;
			ACCUMULATION.energy_rq = 0;
			
			float_to_integer(ACCUMULATION.energy_p, EP1, 1.0F);
			float_to_integer(ACCUMULATION.energy_q, EQ1, 1.0F);
			float_to_integer(ACCUMULATION.energy_rp, REP1, 1.0F);
			float_to_integer(ACCUMULATION.energy_rq, REQ1, 1.0F);
	
			EVENT.data_reset |= ENERGY_RESET_EVENT;
			
			event_direct_save(&EVENT.data_reset);
			
			serial_ok_nak_send(0);
		}
		
		//vo max  clear
		else if(MANAGER.rx_buffer[3] == 0x02)
		{			
			ACCUMULATION.vo_max = 0;
						
			float_to_integer(ACCUMULATION.vo_max, VoMAX1, 1.0F);
			
			EVENT.data_reset |= Vo_RESET_EVENT;
			
			event_direct_save(&EVENT.data_reset);
			
			serial_ok_nak_send(0);
		}
		
		//Io max  clear
		else if(MANAGER.rx_buffer[3] == 0x03)
		{			
			ACCUMULATION.io_max = 0;
						
			float_to_integer(ACCUMULATION.io_max, IoMAX1, 1.0F);
			
			
			EVENT.data_reset |= Io_RESET_EVENT;
			
			event_direct_save(&EVENT.data_reset);
			
			serial_ok_nak_send(0);
		}
		
		//CB Close Time  clear
		else if(MANAGER.rx_buffer[3] == 0x03)
		{
			SUPERVISION.cb_close_time = 0;
						
			*CB_CLOSE_TIME1 = 0;
			*CB_CLOSE_TIME2 = 0;
			
			EVENT.data_reset |= RHOUR_RESET_EVENT;
			
			event_direct_save(&EVENT.data_reset);
			
			serial_ok_nak_send(0);
		}
		
		
		//serial_write(1, &LOCAL_CONTROL.mode, LOCAL_CTRL_USE); //2015.02.24
	}
		
	
	
	// debug
	else if(MANAGER.rx_buffer[2] == 0x18)
	{
		//내부변수는 0xXXXX 주소임
		//모든변수를 unsigned int로 포인팅하여 보내줌
		//MANAGER.rx_buffer[6], MANAGER.rx_buffer[7] - 시작주소
		//MANAGER.rx_buffer[8], word개수
		// header
		MANAGER.tx_buffer[0] = '#';
		MANAGER.tx_buffer[1] = ADDRESS.address;
		MANAGER.tx_buffer[2] = MANAGER.rx_buffer[2];
		MANAGER.tx_buffer[3] = MANAGER.rx_buffer[3];
		
		// length
		MANAGER.tx_buffer[4] = 0;
		MANAGER.tx_buffer[5] = MANAGER.rx_buffer[8] << 1;
	}
	
	//엄한값
	else
	serial_ok_nak_send(0x06);
	
	
		
//		
//	
//	// data
//	// word 바이트로 변경
//	k = ar_bytelength >> 1;
//	
//	for(i = 0; i < k; i++)
//	{
//		//상위바이트
//		*(tx_buffer + 6 + (i * 2)) = (*(ar_variable + i) >> 8);
//		
//		//하위바이트
//		*(tx_buffer + 7 + (i * 2)) = (*(ar_variable + i) & 0x00ff);
//	}
//	
//	i = COMM_CRC(tx_buffer, 6 + ar_bytelength);
//			
//	*(tx_buffer + 6 + ar_bytelength) = i >> 8;
//	*(tx_buffer + 7 + ar_bytelength) = i & 0x00ff;
//	
//	MANAGER.tx_length = ar_bytelength + 8;
//	
//	MANAGER.isr_tx = tx_buffer;
//	
//	// tx interrupt 활성
//	*ScibRegs_SCICTL2 |= 0x0001;
//			
//	// tx intrrupt 활성화 후 최초 한번 써야함
//	MANAGER.tx_count = 1;
//	*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
		
	
	
//	MANAGER.rx_count = 0;
//	MANAGER.rx_status = 0;
}

// 설정 값 읽기 요청에 대한 내용을 바이트 단위로 쪼개어 송신버퍼에 나열하고 프레임  crc를 만든 후  pc로 송신까지 처리 하는 함수
// *tx_buffer  - 송신 데이터를 저장할 배열 주소
// *ar_variable - 보내야할 설정 값이 저장되어 있는 변수들의 최초 주소값
// ar_bytelength - 송신할 데이터의 바이트 개수
void make_crc_send(unsigned int *tx_buffer, unsigned int *ar_variable, unsigned int ar_bytelength)
{
	unsigned int i;
	unsigned int k;
	
	// 송신을 위한 기본작업
	// header 제작
	*(tx_buffer) = '#';
	*(tx_buffer + 1) = ADDRESS.address;
	*(tx_buffer + 2) = MANAGER.rx_buffer[2]; // F/C1 수신된 function code를 그대로 되돌려 줌. 왜. 정상 프레임이니까
	*(tx_buffer + 3) = MANAGER.rx_buffer[3]; // F/C2 수신된 function code를 그대로 되돌려 줌. 왜. 정상 프레임이니까
	
	// 넘겨받은 ar_bytelength 개수 저장 
	*(tx_buffer + 4) = (ar_bytelength >> 8);
	*(tx_buffer + 5) = (ar_bytelength & 0x00ff);
	
	// 실제 data
	// byte 개수 word 개수로 변경
	// 실제 저장된 설정 값은 2바이트 word로 구성되어 있으므로, 넘겨받은 바이트 개수의 절반이 word 개수가 되므로.
	k = ar_bytelength >> 1; // 나누기 2
	
	// 지정된 주소부터(*ar_variable) word 개수만큼 읽어서 송신버퍼에 나열
	for(i = 0; i < k; i++)
	{
		//상위바이트
		*(tx_buffer + 6 + (i * 2)) = (*(ar_variable + i) >> 8);
		
		//하위바이트
		*(tx_buffer + 7 + (i * 2)) = (*(ar_variable + i) & 0x00ff);
	}
	
	// 헤더와 설정값들이 모두 나열된 후 프레임의 crc를 계산함
	i = COMM_CRC(tx_buffer, 6 + ar_bytelength);
	
	// 데이터 개수를 알고 있기 때문에 전체 송신데이터가 몇개인지 알수 있음
	// 마지막 두바이트에 crc값을 저장함
	*(tx_buffer + 6 + ar_bytelength) = i >> 8;     // crc상위바이트
	*(tx_buffer + 7 + ar_bytelength) = i & 0x00ff; // crc하위바이트
	
	// 시리얼 송신 인터럽트에서 보낼 바이트 개수 지정
	MANAGER.tx_length = ar_bytelength + 8; // 데이터 바이트개수 + 헤더(6) + crc(2)
	
	// 시리얼 송신 인터럽트에서 보낼 송신버퍼 주소 지정
	MANAGER.isr_tx = tx_buffer;
	
	// tx interrupt 활성
	*ScibRegs_SCICTL2 |= 0x0001;
			
	// tx intrrupt 활성화 후 최초 한번 써야함
	MANAGER.tx_count = 1;
	*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
}

// 설정값 쓰기 요청에 대한 수신데이터 WORD로 변환, 후처리 함수 호출
// ar_length - 설정 값 word 단위 개수
// *ar_data  - 설정 값 저장할 변수 주소
// *ar_address - 설정 값 저장할 flash rom 주소
void serial_write(unsigned int ar_length, unsigned int *ar_data, unsigned int *ar_address)
{
	unsigned int i;
	
	// 지정된 word 단위 개수만틈 수신버퍼에서 word로 변환하여 임시변수에 저장
	for(i = 0; i < ar_length; i++)
	{
		// 상위 바이트
		MANAGER.temp[i] = MANAGER.rx_buffer[6 + (i << 1)];
		
		// 상위바이트 MSB쪽으로 shift
		MANAGER.temp[i] <<= 8;
		
		// 하위 바이트 or
		MANAGER.temp[i] |= MANAGER.rx_buffer[7 + (i << 1)];
	}
	
	// flash rom에서 같은 섹터에 위치한 설정 값들
	if(ar_address == DI_DEBOUNCE1)
	{
		// do 속성 변경 이벤트와 di debounce 변경 이벤트를 구분하기 위해 직접 값을 비교하도록 함
		// 수신버퍼에 있는 DO 속성 설정 값과 기존에 저장된 값이 틀리면		
		if(MANAGER.temp[8] != DIDO.property)
		EVENT.group_extra = 1; // setting_save()에서 이벤트 구분하기 위해 임시변수에 1 저장
		
		// di debounce 설정이 하나라도 변경되면
		if((MANAGER.temp[0] != DIDO.debounce[0]) ||
		   (MANAGER.temp[1] != DIDO.debounce[1]) ||
		   (MANAGER.temp[2] != DIDO.debounce[2]) ||
		   (MANAGER.temp[3] != DIDO.debounce[3]) ||
		   (MANAGER.temp[4] != DIDO.debounce[4]) ||
		   (MANAGER.temp[5] != DIDO.debounce[5]) ||
		   (MANAGER.temp[6] != DIDO.debounce[6]) ||
		   (MANAGER.temp[7] != DIDO.debounce[7]))
		{
			// do 설정도 바뀌었다면, 최종적으로 setting_save()에서 이벤트 저장하는 문장이 있음
			// 하지만 한번에 두개 이벤트 저장이 불가하므로 여기서 미리 di 이벤트를 찍어준다
			if(EVENT.group_extra)
			{
				// di 이벤트 비트맵 작성 후
				EVENT.system_set |= DI_DEBOUNCE_EVENT;
				// 이벤트 저장
				event_direct_save(&EVENT.system_set);
			}
			
			// di 설정만 바뀌었음
			else
			EVENT.group_extra = 0; // setting_save()에서 이벤트 구분하기 위해 임시변수에 0 저장
		}
	}
	
	else if(ar_address == OCGR_DGR_SEL)
	{
		// sgr 사용 시 setting_save()에서 이벤트 구분하기 위해
		if(CORE.gr_select == ZCT_SELECT)
		EVENT.group_extra = 1;
		
		// ocgr/dgr 사용 시 setting_save()에서 이벤트 구분하기 위해
		else
		EVENT.group_extra = 0;
	}
	
	
	// ack
	// 일단 설정값 저장 함수를 호출하고 결과가 양호하면 후처리 실행
	if(setting_save(MANAGER.temp, ar_address, ar_length))
	{
		// 후처리에서도 양호하면 최종적으로 ack 프레임을 pc에 송신해줌
		if(setting_load(ar_data, ar_length, ar_address))
		serial_ok_nak_send(0x00);
		
		// 후처리에서 불량하면 pc에 nak 프레임을 pc에 송신해줌
		else
		serial_ok_nak_send(0x04);
	}
	
	// flash 저장 불량 시, nak
	else
	serial_ok_nak_send(0x04);
}

// 전면 시리얼 통신 수신된 프레임에 대해 ack, nak를 pc로 송신 위한 함수
// ar_nak_code - ack, nak 코드
// ack는 0, 나머지는 nak
void serial_ok_nak_send(unsigned int ar_nak_code)
{
	unsigned int i;
	
	// 송신프레임 정비
	MANAGER.tx_buffer[0] = '#';           // 시작헤더
	MANAGER.tx_buffer[1] = ADDRESS.address;  // 내 아이디
	
	// 0이 아니면 nak임
	if(ar_nak_code)
	{
		MANAGER.tx_buffer[2] = 0xff;         // 정의된대로 0xff
		MANAGER.tx_buffer[3] = ar_nak_code;  // 전달받은 nak code 
	}
	
	// ack일 경우는 정의된대로 수신받은 function code를 그대로 반사
	else	
	{
		MANAGER.tx_buffer[2] = MANAGER.rx_buffer[2];
		MANAGER.tx_buffer[3] = MANAGER.rx_buffer[3];
	}
	
	// 단순히 ack,nak를 보내는 것이니 따로 데이터는 없음
	// 따라서 data length는 빵
	MANAGER.tx_buffer[4] = 0;
	MANAGER.tx_buffer[5] = 0;
	
	// crc 계산
	i = COMM_CRC(MANAGER.tx_buffer, 6);
	
	// 마지막 두개 바이트에 crc를 저장
	MANAGER.tx_buffer[6] = i >> 8;
	MANAGER.tx_buffer[7] = i & 0x00ff;
	
	// 총 보내야할 데이터 개수 지정
	MANAGER.tx_length = 8;
	
	// 전면 시리얼 송신 배열 주소 전달. 
	// wave, 이벤트 같은 것들은 데이타가 커서 내부메모리에 저장 못함. 얘들은 외부 sram에 저장함
	// 송신 코드는 동일하고 저장 위치만 다르기 때문에 *MANAGER.isr_tx를 만들어 시작 위치만 변경하여 코드를 공유함
	MANAGER.isr_tx = MANAGER.tx_buffer;
	
	//전면 시리얼  tx interrupt 활성
	*ScibRegs_SCICTL2 |= 0x0001;
	
	// tx 인터럽트가 걸릴려면 무조건 한번은 송신버퍼에 값을 써줘야함.
	// tx intrrupt 활성화 후 최초 한번 써야함	
	MANAGER.tx_count = 1; // 송신 데이터 개수 카운터 1증가
	*ScibRegs_SCITXBUF = *MANAGER.isr_tx; // 일단 하나 발사
}
	
//300us 소요
void wave_dump_serial_sram(unsigned int *ar_flash, unsigned int ar_offset, unsigned int ar_wordcount)
{
	unsigned int *flash_point;
	unsigned int i, j;
			
	flash_point = ar_flash + ar_offset;
	
	for(i = 0; i < ar_wordcount; i++)
	{
		j = i << 1;
		
		*(Manager_tx_long_six + j) = *(flash_point + i) >> 8;
		*(Manager_tx_long_seven + j) = *(flash_point + i) & 0x00ff;
	}
	
	i = COMM_CRC(Manager_tx_long, 6 + (ar_wordcount << 1));
	
	*(Manager_tx_long_six + (ar_wordcount << 1)) = i >> 8;
	*(Manager_tx_long_seven + (ar_wordcount << 1)) = i & 0x00ff;
	
	MANAGER.tx_length = 8 + (ar_wordcount << 1);
	
	MANAGER.isr_tx = Manager_tx_long;
	
	// tx interrupt 활성
	*ScibRegs_SCICTL2 |= 0x0001;
			
	// tx intrrupt 활성화 후 최초 한번 써야함
	MANAGER.tx_count = 1;
	*ScibRegs_SCITXBUF = *MANAGER.isr_tx;
}
	
//계측 35us
void comm_drive(void)
{
	unsigned int i;
	float float_temp = 0;
	
	// 통신보드가 뭘 달라고 하는지 여기서 확인함
	i = *COMM_2_DSP & 0x00ff;
	
	if(i)
	{
		//계측
		if(i == 1)
		{
			if(COMM.index == 0)
			{
				//Ia
				float_to_integer(DISPLAY.rms_value[Ia], COMM_2_Ia, 10.0F);
				//Ib
				float_to_integer(DISPLAY.rms_value[Ib], COMM_2_Ib, 10.0F);
				//Ic
				float_to_integer(DISPLAY.rms_value[Ic], COMM_2_Ic, 10.0F);
				//Io
				if(CORE.gr_select == ZCT_SELECT)
				float_to_integer(DISPLAY.rms_value[Is], COMM_2_Io, 10.0F);
				
				else
				float_to_integer(DISPLAY.rms_value[In], COMM_2_Io, 10.0F);
				
				//Io max
				float_to_integer(ACCUMULATION.io_max, COMM_2_Io_max, 10.0F);
			}
			
			else if(COMM.index == 1)
			{
				//Ips
				float_to_integer(MEASUREMENT.I1_value, COMM_2_Ips, 10.0F);
				//Ins
				float_to_integer(MEASUREMENT.I2_value, COMM_2_Ins, 10.0F);
				//Ia 3rd
				float_to_integer(HARMONICS.ia[0], COMM_2_Ia_3rd, 10.0F);
				//Ia 5th
				float_to_integer(HARMONICS.ia[1], COMM_2_Ia_5th, 10.0F);
				//Ia 7th
				float_to_integer(HARMONICS.ia[2], COMM_2_Ia_7th, 10.0F);
				//Ia 9th
				float_to_integer(HARMONICS.ia[3], COMM_2_Ia_9th, 10.0F);
			}
			
			else if(COMM.index == 2)
			{
				//Ia thd
				float_to_integer(HARMONICS.ia[4], COMM_2_Ia_thd, 1);
				//Ia tdd
				float_to_integer(HARMONICS.ia[5], COMM_2_Ia_tdd, 1);
				//Ib 3rd
				float_to_integer(HARMONICS.ib[0], COMM_2_Ib_3rd, 1);
				//Ib 5th
				float_to_integer(HARMONICS.ib[1], COMM_2_Ib_5th, 1);
				//Ib 7th
				float_to_integer(HARMONICS.ib[2], COMM_2_Ib_7th, 1);
				//Ib 9th
				float_to_integer(HARMONICS.ib[3], COMM_2_Ib_9th, 1);
			}
			
			else if(COMM.index == 3)
			{
				//Ib thd
				float_to_integer(HARMONICS.ib[4], COMM_2_Ib_thd, 1);
				//Ib tdd
				float_to_integer(HARMONICS.ib[5], COMM_2_Ib_tdd, 1);
				//Ic 3rd
				float_to_integer(HARMONICS.ic[0], COMM_2_Ic_3rd, 1);
				//Ic 5th
				float_to_integer(HARMONICS.ic[1], COMM_2_Ic_5th, 1);
				//Ic 7th
				float_to_integer(HARMONICS.ic[2], COMM_2_Ic_7th, 1);
				//Ic 9th
				float_to_integer(HARMONICS.ic[3], COMM_2_Ic_9th, 1);
			}
			
			else if(COMM.index == 4)
			{
				//Ic thd
				float_to_integer(HARMONICS.ic[4], COMM_2_Ic_thd, 1);
				//Ic tdd
				float_to_integer(HARMONICS.ic[5], COMM_2_Ic_tdd, 1);
				
//			if(TRANSFORMER.pt_wiring == P3W3)
				{
					//선간
					//Vab
					float_to_integer(DISPLAY.rms_value[Va], COMM_2_Vab, 1);
					//Vbc
					float_to_integer(DISPLAY.rms_value[Vb], COMM_2_Vbc, 1);
					//Vca
					float_to_integer(DISPLAY.rms_value[Vc], COMM_2_Vca, 1);
					
					//Va 허당
					float_to_integer(float_temp, COMM_2_Va, 1);
				}
//				else //3상4선
//				{
//					//선간
//					//Vab
//					float_to_integer(DISPLAY.line2line[0], COMM_2_Vab, 1);
//					//Vbc
//					float_to_integer(DISPLAY.line2line[1], COMM_2_Vbc, 1);
//					//Vca
//					float_to_integer(DISPLAY.line2line[2], COMM_2_Vca, 1);
//					
//					//상
//					//Va
//					float_to_integer(DISPLAY.rms_value[Va], COMM_2_Va, 1);
//				}
			}
			
			else if(COMM.index == 5)
			{
//			if(TRANSFORMER.pt_wiring == P3W3)
				{
					//Vb 허당
					float_to_integer(float_temp, COMM_2_Vb, 1);
					//Vc 허당
					float_to_integer(float_temp, COMM_2_Vc, 1);
				}
//				else //3상4선
//				{
//					//Vb
//					float_to_integer(DISPLAY.rms_value[Vb], COMM_2_Vb, 1);
//					//Vc
//					float_to_integer(DISPLAY.rms_value[Vc], COMM_2_Vc, 1);
//				}
				
				//Vn
				float_to_integer(DISPLAY.rms_value[Vn], COMM_2_Vo, 1);
				//Vn max
				float_to_integer(ACCUMULATION.vo_max, COMM_2_Vo_max, 1);
				//Vp
				float_to_integer(MEASUREMENT.V1_value, COMM_2_Vps, 1);
				//Vn
				float_to_integer(MEASUREMENT.V2_value, COMM_2_Vns, 1);
			}
			
			else if(COMM.index == 6)
			{
				//Vb 위상
				float_to_integer(DISPLAY.angle[1], COMM_2_Vb_degree, 1);
				//Vc 위상
				float_to_integer(DISPLAY.angle[2], COMM_2_Vc_degree, 1);
				//Ia 위상
				float_to_integer(DISPLAY.angle[3], COMM_2_Ia_degree, 1);
				//Ib 위상
				float_to_integer(DISPLAY.angle[4], COMM_2_Ib_degree, 1);
				//Ic 위상
				float_to_integer(DISPLAY.angle[5], COMM_2_Ic_degree, 1);
				//역률
				float_to_integer(DISPLAY.pf3, COMM_2_pf, 1);
			}
			
			else if(COMM.index == 7)
			{
				//주파수
				float_to_integer(MEASUREMENT.frequency, COMM_2_frequency, 1);
				//유효전력
				float_to_integer(DISPLAY.p3, COMM_2_P, 1);
				//무효전력
				float_to_integer(DISPLAY.q3, COMM_2_Q, 1);
				//유효전력량
				float_to_integer(ACCUMULATION.energy_p, COMM_2_PE, 1);
				//무효전력량
				float_to_integer(ACCUMULATION.energy_q, COMM_2_QE, 1);
				//역유효전력량
				float_to_integer(ACCUMULATION.energy_rp, COMM_2_PEr, 1);
			}
			
			else if(COMM.index == 8)
			{
				//역무효전력량
				float_to_integer(ACCUMULATION.energy_rq, COMM_2_QEr, 1);
				
				//syncro 계측
//				if(SYNCRO.use == 0xaaaa)
//				{
//					//전압차
//					float_to_integer(SYNCRO.difference_voltage, COMM_2_Vd, 1);
//					//위상차
//					float_to_integer(SYNCRO.difference_phase, COMM_2_Dd, 1);
//					//주파수차
//					float_to_integer(SYNCRO.difference_freq, COMM_2_Fd, 1);
//				}
				
				//허당
//				else
//				{
					//전압차
					float_to_integer(float_temp, COMM_2_Vd, 1);
					//위상차
					float_to_integer(float_temp, COMM_2_Dd, 1);
					//주파수차
					float_to_integer(float_temp, COMM_2_Fd, 1);
//				}
				
//2015.02.24
//				if(CORE.model == MS)
//				float_to_integer(THR.Q, COMM_2_thermal, 1);
//				else
//2015.02.24 END
				float_to_integer(float_temp, COMM_2_thermal, 1);
				
				//di status
				*COMM_2_DI = DIDO.di_status;
				
				//do status
				*COMM_2_DO1 = DIDO.do_status >> 8;
				*COMM_2_DO2 = DIDO.do_status;
			}
			
			else if(COMM.index == 9)
			{
				//cb status
				*COMM_2_CB = DIDO.do_status;
				
				//계전요소
				*COMM_2_relay1 = RELAY_STATUS.operation_sum_holding >> 8;
				*COMM_2_relay2 = RELAY_STATUS.operation_sum_holding ;
				
				//사고상
				*COMM_2_fault = RELAY_STATUS.popup_property;
				
				//사고ratio
				float_to_integer(RELAY_STATUS.popup_ratio, COMM_2_fault_ratio, 1);
				//동작시간
				float_to_integer(RELAY_STATUS.popup_ratio, COMM_2_fault_time, 1);
				//run hour
				float_to_integer(SUPERVISION.cb_close_time, COMM_2_runhour, 1);
				//이벤트 sp
				*COMM_2_SP = EVENT.sp;
			}
			
			++COMM.index;
			
			if(COMM.index == 10)
			{
				COMM.index = 0;
				
				*COMM_2_DSP = 0;
				
				*COMM_2_DSP2COMM = i;
			}
		}
	}
	
	else
	COMM.index = 0;
}

