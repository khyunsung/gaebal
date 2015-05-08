#include "define.h"
#include "extern_variable.h"
#include "extern_prototype.h"

void booting_setting_check(void)
{
	unsigned int i, j;
	unsigned int temp[100];
	
	void *void_p;
	unsigned int *temp16_p;
	
	//-------- 모든 내부변수 clear
	// 코드변동, 특히 전역변수 변동이 있을 경우 map 파일 .ebss 사이즈 참조해서 확인할 것
	for(i = 0; i < 0x0dd4; i++)
	*(ebss_start + i) = 0;
	//-------- 모든 내부변수 clear 끝
	
	//-------- 초기화면 표시시작
	// lcd 화면 켜기
	LCD_BACKLIGHT_ON;
	// lcd 화면 clear
	lcd_control_write(0x0c);
	delay_us(1000);
	lcd_control_write(LCD_CLEAR);
	delay_us(2000);
	
	LCD.line_buffer1 = "      HYUNDAI";
	LCD.line_buffer2 = "  HEAVY INDUSTRIES";
	
	for(i = 0; i < 20; i++)
	{
		if(LCD.line_buffer1[i] == 0)
		break;
		else
		lcd_character_write(LCD.line_buffer1[i]);
		delay_us(1000);
	}
	lcd_control_write(0xc0);
	delay_us(1000);
	
	for(i = 0; i < 20; i++)
	{
		if(LCD.line_buffer2[i] == 0)
		break;
		else
		lcd_character_write(LCD.line_buffer2[i]);
		delay_us(1000);
	}
	//-------- 초기화면 표시 끝
	
	//-------- LED 초기화 시작
	*LED_CS = FAULT_LED;
	delay_us(50000);
	*LED_CS = FAULT_LED | OP_LED;
	delay_us(50000);
	*LED_CS = FAULT_LED | OP_LED | SYS_FAIL_LED;
	delay_us(50000);
	*LED_CS = FAULT_LED | OP_LED | SYS_FAIL_LED | CB_CLOSE_LED;
	delay_us(50000);
	*LED_CS = FAULT_LED | OP_LED | SYS_FAIL_LED | CB_CLOSE_LED | CB_OPEN_LED;
	delay_us(50000);
	*LED_CS = FAULT_LED | OP_LED | SYS_FAIL_LED | CB_CLOSE_LED | CB_OPEN_LED | REMOTE_LED;
	delay_us(50000);
	*LED_CS = FAULT_LED | OP_LED | SYS_FAIL_LED | CB_CLOSE_LED | CB_OPEN_LED | REMOTE_LED | LOCAL_LED;
	//-------- LED 초기화 끝

	//-------- 시스템 속성을 결정하는 초기 시스템 검사
	// 정격주파수, CT정격, F/M, NCT/ZCT 를 검사
	temp[0] = *RATED_Hz;
	temp[1] = *RATED_CT;
	temp[2] = *GR_SELECT;
	temp[9] = *CORE_CRC;

	CORE.Hz = temp[0];		
	CORE.rated_ct = temp[1];
	CORE.gr_select = temp[2];

	i = Setting_CRC(temp, 3);	// 일단 crc 계산
	if(i != temp[9])	// crc가 틀리면 강제로 rated value set로 진입
	{
		EINT;	// 여기서부터 인터럽트 활성 화
		Screen_Position.y = 151;
		Screen_Position.x = 2;
		Screen_Position.select = 0; //init. screen info.
		KHS_Key_Press = 0xefff;
		menu_drive();
		KHS_Key_Press = 0xffff;
		menu_drive();

		for(;;) {
			if(TIMER.key > 80)	key_drive();
			if(TIMER.lcd > 200)	menu_drive();
		}
	}
//	CORE.Hz = temp[0];		
//	CORE.rated_ct = temp[1];		
//	CORE.gr_select = temp[2];

	//-------- 시스템 속성을 결정하는 초기 시스템 검사 끝	
		
	if(CORE.rated_ct == CT_1A) // ct 정격 1A
	{
		MIN_MAX.current_display = DISPLAY_CUT_I_1A;
	}
	else // ct 정격 5A
	{
		MIN_MAX.current_display = DISPLAY_CUT_I_5A;
	}
	
	if(CORE.Hz == Hz50) // 정격 50Hz
	{
		SET_50Hz; // 외부 cpld 설정
		for(i = 0; i < 12; i++) // cos/sin table 설정
		{
			cos_table[i] = cos_50Hz[i];
			sin_table[i] = sin_50Hz[i];
		}
	}
	else // 정격 60Hz
	{
		SET_60Hz; // 외부 cpld 설정
		for(i = 0; i < 12; i++) // cos/sin table 설정
		{
			cos_table[i] = cos_60Hz[i];
			sin_table[i] = sin_60Hz[i];
		}
	}
	*IO_Control = 0; // cpld start 신호
	
	// do 모듈 ready
	// 시간을 좀 줘야 함
	TLE6208_CS_HIGH;
	TLE6208_CLK_LOW;	
	delay_us(1000);
	
	//C/PT EEROM ready
	EEROM_CS_LOW;
	EEROM_CLK_LOW;
	DELAY_US(1000);
	
	//do 입력억제 해제 후 일정 시간 필요함
	TLE6208_INH;
	
	delay_us(1000000);
	delay_us(1000000);
	
	// rtc 내부 설정레지스터에서 rtc 동작을 on/off 할 수 있음
	// default는 off임 그래서 전원이 인가되면 무조건 rtc on 해줌
	// rtc oscilator on		
	// 아래 순서는 rtc datasheet에 의해 수행함
	// rtc가 깨어나도록 하는 코드는  초기 공장 초기 버전 또는 슈퍼 캐패시터 방전시 유효함
	// 한번 설정 후에는 설정이 계속 유지하므로 반드시 필요하지는 않음
	temp[0] = 0x00;
	i2c_write(0x68, 0x01, 1, temp);
	delay_us(5000);
	i2c_write(0x68, 0x00, 1, temp);
	delay_us(5000); // 여기까지 하면 일단 rtc가 깨어남
	
	// rtc 내부 시간을 read
	// rtc_handling() 운용을 real_main() 상에서 수행
	TIME.update = 2; // rtc 동작을 지정하는 인덱스
	rtc_handling();
	delay_us(1000);
	rtc_handling();//TIME.update == 3
	delay_us(1000);
	rtc_handling();//TIME.update == 4. rtc 시간 읽기 완료
	
	TIME.milisecond = 0;
	delay_us(100000); // rtc 시간을 읽어와서 내부 TIME.xxx 변수에 쟁기기 완료
	
// CALIBRATION FACTOR, SRAM CHECK
	//-------- 시작 화면 표시
//	lcd_control_write(0x0c);
//	delay_us(1000);
//	lcd_control_write(LCD_CLEAR);
//	delay_us(2000);
//	LCD.line_buffer1 = "CAL. FACTOR -";
//	LCD.line_buffer2 = "SRAM CHECK  -";
//	for(i = 0; i < 20; i++)
//	{
//		if(LCD.line_buffer1[i] == 0)	{break;}
//		else	{lcd_character_write(LCD.line_buffer1[i]);}
//		delay_us(1000);
//	}
//	lcd_control_write(0xc0);
//	delay_us(1000);
//	for(i = 0; i < 20; i++)
//	{
//		if(LCD.line_buffer2[i] == 0)	{break;}
//		else	{lcd_character_write(LCD.line_buffer2[i]);}
//		delay_us(1000);
//	}
//	lcd_control_write(LCD_L1_14);
//	delay_us(1000);
	//-------- 시작 화면 표시 END
	
	//-------- calibration factor 읽어서 확인
	for(i = 0; i < 10; i++)	// offset
	{
		eerom_read(i, &CALIBRATION.offset[i]);
		temp[i] = CALIBRATION.offset[i];
	}
	for(i = 0; i < 10; i++)	//slope
	{
		void_p = &CALIBRATION.slope[i];
		temp16_p = (unsigned int*)void_p;
		eerom_read(0x10 + (i << 1), temp16_p);
		eerom_read(0x11 + (i << 1), temp16_p + 1);
		temp[10 + (i << 1)] = *temp16_p;
		temp[11 + (i << 1)] = *(temp16_p + 1);
	}
	for(i = 0; i < 10; i++)	//intercept
	{
		void_p = &CALIBRATION.intercept[i];
		temp16_p = (unsigned int*)void_p;
		eerom_read(0x30 + (i << 1), temp16_p);
		eerom_read(0x31 + (i << 1), temp16_p + 1);
		temp[30 + (i << 1)] = *temp16_p;
		temp[31 + (i << 1)] = *(temp16_p + 1);
	}
	for(i = 0; i < 10; i++)	//angle
	{
		void_p = &CALIBRATION.angle[i];
		temp16_p = (unsigned int*)void_p;
		eerom_read(0x50 + (i << 1), temp16_p);
		eerom_read(0x51 + (i << 1), temp16_p + 1);
		temp[50 + (i << 1)] = *temp16_p;
		temp[51 + (i << 1)] = *(temp16_p + 1);
	}
//	for(i = 0; i < 3; i++)	//power
//	{
//		void_p = &CALIBRATION.slope_power[i];
//		temp16_p = (unsigned int*)void_p;
//		eerom_read(0x70 + (i << 1), temp16_p);
//		eerom_read(0x71 + (i << 1), temp16_p + 1);
//		temp[70 + (i << 1)] = *temp16_p;
//		temp[71 + (i << 1)] = *(temp16_p + 1);
//	}
//	for(i = 0; i < 3; i++)	//power cos
//	{
//		void_p = &CALIBRATION.Power_Cos[i];
//		temp16_p = (unsigned int*)void_p;
//		eerom_read(0x80 + (i << 1), temp16_p);
//		eerom_read(0x81 + (i << 1), temp16_p + 1);
//		temp[76 + (i << 1)] = *temp16_p;
//		temp[77 + (i << 1)] = *(temp16_p + 1);
//	}
//	for(i = 0; i < 3; i++)	//power sin
//	{
//		void_p = &CALIBRATION.Power_Sin[i];
//		temp16_p = (unsigned int*)void_p;
//		eerom_read(0x86 + (i << 1), temp16_p);
//		eerom_read(0x87 + (i << 1), temp16_p + 1);
//		temp[82 + (i << 1)] = *temp16_p;
//		temp[83 + (i << 1)] = *(temp16_p + 1);
//	}
//	void_p = &CALIBRATION.frequency_offset;
//	temp16_p = (unsigned int*)void_p;
//	eerom_read(0x90 + (i << 1), temp16_p);
//	eerom_read(0x91 + (i << 1), temp16_p + 1);
//	temp[88] = *temp16_p;
//	temp[89] = *(temp16_p + 1);
	eerom_read(0xa0, &i);

//j = Setting_CRC(temp, 90);
	j = Setting_CRC(temp, 70);
	if(i != j)	// calibration factor 틀어짐
	{
		SYSTEM.diagnostic |= CALIBRATION_NOT;

		CALIBRATION.offset[0] = 216;
		CALIBRATION.offset[1] = 208;
		CALIBRATION.offset[2] = 205;
		CALIBRATION.offset[3] = 216;
		CALIBRATION.offset[4] = 0;  //계측 안함
		CALIBRATION.offset[5] = 216;
		CALIBRATION.offset[6] = 105;
		CALIBRATION.offset[7] = 199;
		CALIBRATION.offset[8] = 118;
		CALIBRATION.offset[9] = 136;

		CALIBRATION.slope[0] = 0.003988546;
		CALIBRATION.slope[1] = 0.00397542;
		CALIBRATION.slope[2] = 0.00399828;
		CALIBRATION.slope[3] = 0.002995868;
		CALIBRATION.slope[4] = 0;  //계측 안함
		CALIBRATION.slope[5] = 0.0007004878;
		CALIBRATION.slope[6] = 0.01228692;
		CALIBRATION.slope[7] = 0.0122808;
		CALIBRATION.slope[8] = 0.01229699;
		CALIBRATION.slope[9] = 0.006915077;

		CALIBRATION.intercept[0] = -0.00792597;
		CALIBRATION.intercept[1] = -0.0007012307;
		CALIBRATION.intercept[2] = -0.00192056;
		CALIBRATION.intercept[3] = 0.8607486;
		CALIBRATION.intercept[4] = 0;  //계측 안함
		CALIBRATION.intercept[5] = -0.8350161;
		CALIBRATION.intercept[6] = 0.05907083;
		CALIBRATION.intercept[7] = 0.1002612;
		CALIBRATION.intercept[8] = 0.06911052;
		CALIBRATION.intercept[9] = -0.04100425;

		CALIBRATION.angle[0] = 0.07877987;
		CALIBRATION.angle[1] = 0.0681999;
		CALIBRATION.angle[2] = 0.05715979;
		CALIBRATION.angle[3] = 0.04684306;
		CALIBRATION.angle[4] = 0;  //계측 안함
		CALIBRATION.angle[5] = 0;
		CALIBRATION.angle[6] = 0;
		CALIBRATION.angle[7] = -0.01102073;
		CALIBRATION.angle[8] = -0.02152767;
		CALIBRATION.angle[9] = -0.0323419;

//		CALIBRATION.slope_power[0] = 0.1110592;
//		CALIBRATION.slope_power[1] = 0.1115916;
//		CALIBRATION.slope_power[2] = 0.1115241;
//
//		CALIBRATION.Power_Cos[0] = 0.9980001;
//		CALIBRATION.Power_Cos[1] = 1.003941;
//		CALIBRATION.Power_Cos[2] = 1.002217;
//
//		CALIBRATION.Power_Sin[0] = -0.08913197;
//		CALIBRATION.Power_Sin[1] = -0.08636435;
//		CALIBRATION.Power_Sin[2] = -0.08633978;

		CALIBRATION.frequency_offset = 0.001021814;

//	LCD.line_buffer1 = "FAIL";
	}
//else	{LCD.line_buffer1 = "GOOD";}
	CALIBRATION.frequency_offset = 60 - CALIBRATION.frequency_offset;
	//-------- calibration factor 읽어서 확인 END

	//-------- 결과 화면 표시
//	for(i = 0; i < 20; i++)
//	{
//		if(LCD.line_buffer1[i] == 0)	{break;}
//		else	{lcd_character_write(LCD.line_buffer1[i]);}
//		delay_us(1000);
//	}
//	lcd_control_write(LCD_L2_14);
//	delay_us(1000);
	//-------- 결과 화면 표시 END

	//-------- SRAM에 지우고 쓰고 다시 읽어서 확인
//	for(temp32 = 0; temp32 < 0x40000; temp32++)
//	{
//		*(Pre_Ia_wave_buffer + temp32) = 0x1234;
//		i = *(Pre_Ia_wave_buffer + temp32);
//		*(Pre_Ia_wave_buffer + temp32) = 0;
//		if(i != 0x1234)
//		{
//			LCD.line_buffer1 = "FAIL";
//			SYSTEM.diagnostic |= SRAM_FAIL;
//			break;
//		}
//	}
//	if(temp32 == 0x40000)	{LCD.line_buffer1 = "GOOD";}
	//-------- SRAM에 지우고 쓰고 다시 읽어서 확인 END

	//-------- 결과 화면 표시
//	for(i = 0; i < 20; i++)
//	{
//		if(LCD.line_buffer1[i] == 0)	{break;}
//		else	{lcd_character_write(LCD.line_buffer1[i]);}
//		delay_us(1000);
//	}
//	delay_us(500000);
//	lcd_control_write(0x0c);
//	delay_us(1000);
//	lcd_control_write(LCD_CLEAR);
//	delay_us(2000);
	//-------- 결과 화면 표시 END

// FLASH, FRAM CHECK
	//-------- 시작 화면 표시
//	LCD.line_buffer1 = "FLASH CHECK -";
//	LCD.line_buffer2 = "FRAM  CHECK -";
//	for(i = 0; i < 20; i++)
//	{
//		if(LCD.line_buffer1[i] == 0) {break;}
//		else	{lcd_character_write(LCD.line_buffer1[i]);}
//		delay_us(1000);
//	}
//	lcd_control_write(0xc0);
//	delay_us(1000);
//	for(i = 0; i < 20; i++)
//	{
//		if(LCD.line_buffer2[i] == 0)	{break;}
//		else	{lcd_character_write(LCD.line_buffer2[i]);}
//		delay_us(1000);
//	}
//	lcd_control_write(LCD_L1_14);
//	delay_us(1000);
	//-------- 시작 화면 표시 END

	//-------- FLASH에 지우고 쓰고 다시 읽어서 확인
//	flash_sector_erase(FLASH_CHECK_SECTOR);
//	for(i = 0; i < 0x800; i++)	{flash_word_write((FLASH_CHECK_SECTOR + i), 0x1234);}
//	for(i = 0; i < 0x800; i++)
//	{
//		j = *(FLASH_CHECK_SECTOR + i);
//		if(j != 0x1234)
//		{
//			LCD.line_buffer1 = "FAIL";
//			SYSTEM.diagnostic |= FLASH_FAIL;
//			break;
//		}
//	}
//	if(i == 0x800)	{LCD.line_buffer1 = "GOOD";}
	//-------- FLASH에 지우고 쓰고 다시 읽어서 확인 END

	//-------- 결과 화면 표시
//	for(i = 0; i < 20; i++)
//	{
//		if(LCD.line_buffer1[i] == 0)	{break;}
//		else	{lcd_character_write(LCD.line_buffer1[i]);}
//		delay_us(1000);
//	}
//	flash_sector_erase(FLASH_CHECK_SECTOR);
//	lcd_control_write(LCD_L2_14);
//	delay_us(1000);
	//-------- 결과 화면 표시 END

	//-------- FRAM에 지우고 쓰고 다시 읽어서 확인
//	for(temp32 = 0; temp32 < 0x20000; temp32++)
//	{
//		j = *(FRAM_START + temp32);	//save
//		*(FRAM_START + temp32) = 0x12;	//write
//		i = *(FRAM_START + temp32) & 0x00ff;	// read
//		*(FRAM_START + temp32) = j;	//rewrite
//		if(i != 0x12)
//		{
//			LCD.line_buffer1 = "FAIL";
//			SYSTEM.diagnostic |= FRAM_FAIL;
//			break;
//		}
//	}
//	if(temp32 == 0x20000)	{LCD.line_buffer1 = "GOOD";}
	//-------- FRAM에 지우고 쓰고 다시 읽어서 확인

	//-------- 결과 화면 표시
//	for(i = 0; i < 20; i++)
//	{
//		if(LCD.line_buffer1[i] == 0)	{break;}
//		else	{lcd_character_write(LCD.line_buffer1[i]);}
//		delay_us(1000);
//	}
//	delay_us(500000);
//	lcd_control_write(0x0c);
//	delay_us(1000);
//	lcd_control_write(LCD_CLEAR);
//	delay_us(2000);
	//-------- 결과 화면 표시 END

	//통신 인터페이스 있으면 실시

	// event 관련 변수
	EVENT.pickup      = 0x00000000;
	EVENT.operation   = 0x01000000;
	EVENT.relay_set   = 0x02000000;
	EVENT.system_set  = 0x03000000;
	EVENT.data_reset  = 0x04000000;
	EVENT.di_off      = 0x05000000;
	EVENT.di_on       = 0x06000000;
	EVENT.do_off      = 0x07000000;
	EVENT.do_on       = 0x08000000;
	EVENT.mode_change = 0x09000000;
	EVENT.control     = 0x0a000000;

	EVENT.sp = *EVENT_SP & 0x00ff;	// event check
	if(EVENT.sp > 199)	// 200개 이상이면 불량
	{
		// 관련 변수 초기화
		EVENT.sp = 0;
		EVENT.rollover = 0;
		*EVENT_ROLLOVER = 0;
		*EVENT_SP = 0;
	}
	else	{EVENT.rollover = *EVENT_ROLLOVER & 0x00ff;}

	// system boot event 저장
	EVENT.system_set |= 0x00800000;
	event_direct_save(&EVENT.system_set);
	
	
// flash에 저장된 주요 설정값 check
// 항상 setting_save() 함수 호출 시에는 setting_load()를 실시
// setting_save() 함수는 1.CORE 설정시, 2.FLASH RESET 시, 3. 부팅 시 4. 메뉴 설정 시, 5. 통신 설정 시 호출 됨
// setting_save()에서 저장 및 확인 후 정상 일 때, 다시 setting_load() 및 사후 처리 실시
// setting_save()에서 저장 및 확인 후 불량 일 때, pop 화면에 FLASH WRITE ERROR 표시

//-------- SYSTEM 설정 값 확인
	//GPT RATING
	if(setting_load(&GPT.pt_secondary, 2, PT_SECONDARY))
	{
		i = 0;
		if((GPT.pt_secondary != 100) && (GPT.pt_secondary != 110) && (GPT.pt_secondary != 120) && (GPT.pt_secondary != 190))
		{
			GPT.pt_secondary = 110;
			i = 1;
		}
		if((GPT.pt_tertiary != 110) && (GPT.pt_tertiary != 120) && (GPT.pt_tertiary != 190))
		{
			GPT.pt_tertiary = 190;
			i = 1;
		}
	}
	else
	{
		GPT.pt_secondary = 110;
		GPT.pt_tertiary = 190;
		i = 1;
	}
	if(i)
	{
		if(setting_save(&GPT.pt_secondary, PT_SECONDARY, 2))
		{
			setting_load(&GPT.pt_secondary, 2, PT_SECONDARY);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// CPT RATING
	if(setting_load(&CPT.ct_primary, 5, CT_PRIMARY))
	{
		i = 0;
		i |= setting_min_max_check(&CPT.ct_primary, CT_MIN, CT_MAX);
		i |= setting_min_max_check(&CPT.nct_primary, CT_MIN, CT_MAX);
		i |= pt_setting_min_max_check(&CPT.pt_primary, PT_MIN, PT_MAX);
		i |= setting_min_max_check(&CPT.rated_current, 0, 50000);
	}
	else
	{
		// 전체 초기화
		if(CORE.rated_ct == CT_5A)	{CPT.ct_primary = CT_5A_RATED; CPT.nct_primary = CT_5A_RATED;}
		else												{CPT.ct_primary = CT_1A_RATED; CPT.nct_primary = CT_1A_RATED;}

		if(GPT.pt_secondary == 100)			 {CPT.pt_primary = 100000; CPT.pt_primary_low = 0x86a0; CPT.pt_primary_high = 0x0001;} // 0x186a0 ->100000
		else if(GPT.pt_secondary == 110) {CPT.pt_primary = 110000; CPT.pt_primary_low = 0xadb0; CPT.pt_primary_high = 0x0001;} // 0x1adb0 ->110000
		else if(GPT.pt_secondary == 120) {CPT.pt_primary = 120000; CPT.pt_primary_low = 0xd4c0; CPT.pt_primary_high = 0x0001;} // 0x1d4c0 ->120000
		else if(GPT.pt_secondary == 190) {CPT.pt_primary = 190000; CPT.pt_primary_low = 0xe630; CPT.pt_primary_high = 0x0002;} // 0x2E630 ->190000
			
		if(CORE.rated_ct == CT_5A)	{CPT.rated_current = 50000;}
		else												{CPT.rated_current = 10000;}
		i = 1;
	}
	if(i)
	{
		if(setting_save(&CPT.ct_primary, CT_PRIMARY, 5))
		{
			setting_load(&CPT.ct_primary, 5, CT_PRIMARY);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	//ADDRESS
	if(setting_load(&ADDRESS.address, 1, MOD_ADDR))
	{
		i = 0;
		i = setting_min_max_check(&ADDRESS.address, 1, 254);
	}
	else
	{
		ADDRESS.address = 254;
		i = 1;
	}
	if(i)
	{
		if(setting_save(&ADDRESS.address, MOD_ADDR, 1))
		{
			setting_load(&ADDRESS.address, 1, MOD_ADDR);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	//AUTO DISPLAY
	if(setting_load(&AUTO_DISPLAY.mode, 1, AUTO_DISP_MODE))
	{
		i = 0;
		i = setting_min_max_check(&AUTO_DISPLAY.mode, 0, 5);
	}
	else
	{
		AUTO_DISPLAY.mode = 0;
		i = 1;
	}
	if(i)
	{
		if(setting_save(&AUTO_DISPLAY.mode, AUTO_DISP_MODE, 1))
		{
			setting_load(&AUTO_DISPLAY.mode, 1, AUTO_DISP_MODE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	//PASSWORD
	if(setting_load(&PASSWORD.real, 1, PASSWORD1))
	{
		i = 0;
		i = setting_min_max_check(&PASSWORD.real, 1111, 9999);
	}
	else
	{
		PASSWORD.real = 1111;
		i = 1;
	}
	if(i)
	{
		if(setting_save(&PASSWORD.real, PASSWORD1, 1))
		{
			setting_load(&PASSWORD.real, 1, PASSWORD1);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	//LOCAL CONTROL
	if(setting_load(&LOCAL_CTRL.use, 3, LOCAL_CTRL_USE))
	{
		i = 0;

		if((LOCAL_CTRL.use != ENABLE) && (LOCAL_CTRL.use != DISABLE))
		{
			LOCAL_CTRL.use = DISABLE;
			i = 1;
		}
		i |= setting_min_max_check(&LOCAL_CTRL.close_time, 0, 5);
		i |= setting_min_max_check(&LOCAL_CTRL.open_time, 0, 5);
	}
	else
	{
		LOCAL_CTRL.use = DISABLE;
		LOCAL_CTRL.close_time = 2;
		LOCAL_CTRL.open_time = 0;
		i = 1;
	}
	if(i)
	{
		if(setting_save(&LOCAL_CTRL.use, LOCAL_CTRL_USE, 3))
		{
			setting_load(&LOCAL_CTRL.use, 3, LOCAL_CTRL_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	//UVR MODE
	if(setting_load(&UVR_MODE.setting1, 3, UVR27R_MODE))
	{
		i = 0;
		if((UVR_MODE.setting1 != AUTO) && (UVR_MODE.setting1 != MANUAL))
		{
			UVR_MODE.setting1 = MANUAL;
			i = 1;
		}
		if((UVR_MODE.setting2 != AUTO) && (UVR_MODE.setting2 != MANUAL))
		{
			UVR_MODE.setting2 = MANUAL;
			i |= 1;
		}
		if((UVR_MODE.setting3 != AUTO) && (UVR_MODE.setting3 != MANUAL))
		{
			UVR_MODE.setting3 = MANUAL;
			i |= 1;
		}
	}
	else
	{
		UVR_MODE.setting1 = MANUAL;
		UVR_MODE.setting2 = MANUAL;
		UVR_MODE.setting3 = MANUAL;
		
		i = 1;
	}
	if(i)
	{
		if(setting_save(&UVR_MODE.setting1, UVR27R_MODE, 3))
		{
			setting_load(&UVR_MODE.setting1, 3, UVR27R_MODE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	//3PHASE DISPLAY
	if(setting_load(&DISPLAY_3PHASE.use, 1, DISP_3PHASE_USE))
	{
		i = 0;
		if((DISPLAY_3PHASE.use != ENABLE) && (DISPLAY_3PHASE.use != DISABLE))
		{
			DISPLAY_3PHASE.use = DISABLE;
			i = 1;
		}
	}
	else
	{
		DISPLAY_3PHASE.use = DISABLE;
		i = 1;
	}
	if(i)
	{
		if(setting_save(&DISPLAY_3PHASE.use, DISP_3PHASE_USE, 1))
		{
			setting_load(&DISPLAY_3PHASE.use, 1, DISP_3PHASE_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	//MODBUS COMM.
	if(setting_load(&MODBUS.baudrate, 2, MODBUS_BAUDRATE))
	{
		i = 0;
		i |= setting_min_max_check(&MODBUS.baudrate, 0, 5);
		i |= setting_min_max_check(&MODBUS.delay, 0, 50);
	}
	else
	{
		MODBUS.baudrate = 2;  //38400
		MODBUS.delay = 0;  //0ms
		i = 1;
	}
	if(i)
	{
		if(setting_save(&MODBUS.baudrate, MODBUS_BAUDRATE, 2))
		{
			setting_load(&MODBUS.baudrate, 2, MODBUS_BAUDRATE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	//OCR MODE 
	if(setting_load(&OCR_MODE_SET.ocr_mode, 2, OCR_MODE))
	{
		i = 0;
		if((OCR_MODE_SET.ocr_mode != NORMAL) && (OCR_MODE_SET.ocr_mode != SELECT))
		{
			OCR_MODE_SET.ocr_mode = SELECT;
			i = 1;
		}
		if(OCR_MODE_SET.ocr_mode == SELECT)
		{
			i |= setting_min_max_check(&OCR_MODE_SET.di_number, 0, 8);
		}
	}
	else
	{
		OCR_MODE_SET.ocr_mode = SELECT;
		OCR_MODE_SET.di_number = 3;

		i = 1;
	}
	if(i)
	{
		if(setting_save(&OCR_MODE_SET.ocr_mode, OCR_MODE, 2))
		{
			setting_load(&OCR_MODE_SET.ocr_mode, 2, OCR_MODE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	//SYSTEM ALARM 
	if(setting_load(&SYSTEM_ALARM.use, 2, SYSTEM_ALARM_USE))
	{
		i = 0;
		if((SYSTEM_ALARM.use != ENABLE) && (SYSTEM_ALARM.use != DISABLE))
		{
			SYSTEM_ALARM.use = DISABLE;
			i = 1;
		}
		i |= aux_do_check(&SYSTEM_ALARM.do_relay);
	}
	else
	{
		SYSTEM_ALARM.use = DISABLE;
		SYSTEM_ALARM.do_relay = 0;

		i = 1;
	}
	if(i)
	{
		if(setting_save(&SYSTEM_ALARM.use, SYSTEM_ALARM_USE, 2))
		{
			setting_load(&SYSTEM_ALARM.use, 2, SYSTEM_ALARM_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	//DI DEBOUNCE
	if(setting_load(DIGITAL_INPUT.debounce, 8, DI_DEBOUNCE1))
	{
		i = 0;

		i |= setting_min_max_check(&DIGITAL_INPUT.debounce[0], 0, 1000);
		i |= setting_min_max_check(&DIGITAL_INPUT.debounce[1], 0, 1000);
		i |= setting_min_max_check(&DIGITAL_INPUT.debounce[2], 0, 1000);
		i |= setting_min_max_check(&DIGITAL_INPUT.debounce[3], 0, 1000);
		i |= setting_min_max_check(&DIGITAL_INPUT.debounce[4], 0, 1000);
		i |= setting_min_max_check(&DIGITAL_INPUT.debounce[5], 0, 1000);
		i |= setting_min_max_check(&DIGITAL_INPUT.debounce[6], 0, 1000);
		i |= setting_min_max_check(&DIGITAL_INPUT.debounce[7], 0, 1000);
	}
	else
	{
		DIGITAL_INPUT.debounce[0] = 0;
		DIGITAL_INPUT.debounce[1] = 0;
		DIGITAL_INPUT.debounce[2] = 0;
		DIGITAL_INPUT.debounce[3] = 0;
		DIGITAL_INPUT.debounce[4] = 0;
		DIGITAL_INPUT.debounce[5] = 0;
		DIGITAL_INPUT.debounce[6] = 0;
		DIGITAL_INPUT.debounce[7] = 0;

		i = 1;
	}
	if(i)
	{
		if(setting_save(DIGITAL_INPUT.debounce, DI_DEBOUNCE1, 8))
		{
			setting_load(DIGITAL_INPUT.debounce, 8, DI_DEBOUNCE1);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	//DO PROPERTY
	if(setting_load(&DIGITAL_OUTPUT.property, 1, DO_PROPERTY))
	{
		i = 0;
		i = setting_min_max_check(&DIGITAL_OUTPUT.property, 0x0, 0xFF);
	}
	else
	{
		DIGITAL_OUTPUT.property = 0x0;

		i = 1;
	}
	if(i)
	{
		if(setting_save(&DIGITAL_OUTPUT.property, DO_PROPERTY, 1))
		{
			setting_load(&DIGITAL_OUTPUT.property, 1, DO_PROPERTY);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}



	//supervison
//	if(setting_load(&SUPERVISION.mode, 8, SUPERV_MODE))
//	{
//		i = 0;
//
//		i |= setting_min_max_check(&SUPERVISION.source_level, 80, 400);
//		i |= setting_min_max_check(&SUPERVISION.tcs_level, 10, 90);
//		i |= setting_min_max_check(&SUPERVISION.tcs_time, 1, 10);
//		i |= aux_do_check(&SUPERVISION.tcs_do_relay);
//		i |= setting_min_max_check(&SUPERVISION.ccs_level, 10, 90);
//		i |= setting_min_max_check(&SUPERVISION.ccs_time, 1, 10);
//		i |= aux_do_check(&SUPERVISION.ccs_do_relay);
//	}
//	else
//	{
//		// 전체 초기화
//		SUPERVISION.mode = 0;
//		SUPERVISION.source_level = 400;
//		SUPERVISION.tcs_level = 90;
//		SUPERVISION.tcs_time = 10;
//		SUPERVISION.tcs_do_relay = 0;
//		SUPERVISION.ccs_level = 90;
//		SUPERVISION.ccs_time = 10;
//		SUPERVISION.ccs_do_relay = 0;
//
//		i = 1;
//	}
//	if(i)
//	setting_save(&SUPERVISION.mode, SUPERV_MODE, 8);
	
	//ocgr/dgr/zct
//	if(setting_load(&SYSTEM_SET.ocgr_dgr, 2, OCGR_DGR_SEL))
//	{
//		i = 0;
//		i = setting_min_max_check(&SYSTEM_SET.zct_angle, 0, 359);
//	}
//	else
//	{
//		SYSTEM_SET.ocgr_dgr = 0;
//		SYSTEM_SET.zct_angle = 359;
//
//		i = 1;
//	}
//	if(i)
//	setting_save(&SYSTEM_SET.ocgr_dgr, OCGR_DGR_SEL, 2);
//-------- SYSTEM 설정 값 확인 END


//-------- 계전요소 설정 값 확인
	// ocr50_1
	if(setting_load(&OCR50_1.use, 5, OCR50_1_USE))
	{
		i = 0;
		//범위check
		if((OCR50_1.mode != INSTANT) && (OCR50_1.mode != DEFINITE))
		{
			OCR50_1.mode = DEFINITE;
			i = 1;
		}
		if(CORE.rated_ct == CT_5A)	{i |= setting_min_max_check(&OCR50_1.current_set, OCR50_I_MIN[0], OCR50_I_MAX[0]);}
		else												{i |= setting_min_max_check(&OCR50_1.current_set, OCR50_I_MIN[1], OCR50_I_MAX[1]);}
		i |= setting_min_max_check(&OCR50_1.delay_time, OCR50_T_MIN, OCR50_T_MAX);
		i |= aux_do_check(&OCR50_1.do_relay);
	}
	else
	{
		//default
		OCR50_1.use = 0;
		OCR50_1.mode = DEFINITE;
		if(CORE.rated_ct == CT_5A)	{OCR50_1.current_set = OCR50_I_MAX[0];}
		else												{OCR50_1.current_set = OCR50_I_MAX[1];}
		OCR50_1.delay_time = OCR50_T_MAX;
		OCR50_1.do_relay = 0;
			
		i = 1;
	}
	if(i)
	{
		if(setting_save(&OCR50_1.use, OCR50_1_USE, 5))
		{
			setting_load(&OCR50_1.use, 5, OCR50_1_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// ocr50_2
	if(setting_load(&OCR50_2.use, 5, OCR50_2_USE))
	{
		i = 0;
		//범위check
		if((OCR50_2.mode != INSTANT) && (OCR50_2.mode != DEFINITE))
		{
			OCR50_2.mode = DEFINITE;
			i = 1;
		}
		if(CORE.rated_ct == CT_5A)	{i |= setting_min_max_check(&OCR50_2.current_set, OCR50_I_MIN[0], OCR50_I_MAX[0]);}
		else												{i |= setting_min_max_check(&OCR50_2.current_set, OCR50_I_MIN[1], OCR50_I_MAX[1]);}
		i |= setting_min_max_check(&OCR50_2.delay_time, OCR50_T_MIN, OCR50_T_MAX);
		i |= aux_do_check(&OCR50_2.do_relay);
	}
	else
	{
		//default
		OCR50_2.use = 0;
		OCR50_2.mode = DEFINITE;
		if(CORE.rated_ct == CT_5A)	{OCR50_2.current_set = OCR50_I_MAX[0];}
		else												{OCR50_2.current_set = OCR50_I_MAX[1];}
		OCR50_2.delay_time = OCR50_T_MAX;
		OCR50_2.do_relay = 0;
			
		i = 1;
	}
	if(i)
	{
		if(setting_save(&OCR50_2.use, OCR50_2_USE, 5))
		{
			setting_load(&OCR50_2.use, 5, OCR50_2_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}
		
	// ocr51_1
	if(setting_load(&OCR51_1.use, 5, OCR51_1_USE))
	{
		i = 0;
		//범위check
		if((OCR51_1.mode != INVERSE) && (OCR51_1.mode != V_INVERSE) && (OCR51_1.mode != E_INVERSE))
		{
			OCR51_1.mode = INVERSE;
			i = 1;
		}
		if(CORE.rated_ct == CT_5A)	{i |= setting_min_max_check(&OCR51_1.current_set, OCR51_I_MIN[0], OCR51_I_MAX[0]);}
		else												{i |= setting_min_max_check(&OCR51_1.current_set, OCR51_I_MIN[1], OCR51_I_MAX[1]);}
		i |= setting_min_max_check(&OCR51_1.time_lever, OCR51_TL_MIN, OCR51_TL_MAX);
		i |= aux_do_check(&OCR51_1.do_relay);
	}
	else
	{
		//default
		OCR51_1.use = 0;
		OCR51_1.mode = INVERSE;
		if(CORE.rated_ct == CT_5A)	{OCR51_1.current_set = OCR51_I_MAX[0];}
		else												{OCR51_1.current_set = OCR51_I_MAX[1];}
		OCR51_1.time_lever = OCR51_TL_MAX;
		OCR51_1.do_relay = 0;
		
		i = 1;
	}
	if(i)
	{
		if(setting_save(&OCR51_1.use, OCR51_1_USE, 5))
		{
			setting_load(&OCR51_1.use, 5, OCR51_1_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// ocr51_2
	if(setting_load(&OCR51_2.use, 5, OCR51_2_USE))
	{
		i = 0;
		//범위check
		if((OCR51_2.mode != INVERSE) && (OCR51_2.mode != V_INVERSE) && (OCR51_2.mode != E_INVERSE))
		{
			OCR51_2.mode = INVERSE;
			i = 1;
		}
		if(CORE.rated_ct == CT_5A)	{i |= setting_min_max_check(&OCR51_2.current_set, OCR51_I_MIN[0], OCR51_I_MAX[0]);}
		else												{i |= setting_min_max_check(&OCR51_2.current_set, OCR51_I_MIN[1], OCR51_I_MAX[1]);}
		i |= setting_min_max_check(&OCR51_2.time_lever, OCR51_TL_MIN, OCR51_TL_MAX);
		i |= aux_do_check(&OCR51_2.do_relay);
	}
	else
	{
		//default
		OCR51_2.use = 0;
		OCR51_2.mode = INVERSE;
		if(CORE.rated_ct == CT_5A)	{OCR51_2.current_set = OCR51_I_MAX[0];}
		else												{OCR51_2.current_set = OCR51_I_MAX[1];}
		OCR51_2.time_lever = OCR51_TL_MAX;
		OCR51_2.do_relay = 0;
		
		i = 1;
	}
	if(i)
	{
		if(setting_save(&OCR51_2.use, OCR51_2_USE, 5))
		{
			setting_load(&OCR51_2.use, 5, OCR51_2_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// ocgr50
	if(setting_load(&OCGR50.use, 5, OCGR50_USE))
	{
		i = 0;
		//범위check
		if((OCGR50.mode != INSTANT) && (OCGR50.mode != DEFINITE))
		{
			OCGR50.mode = DEFINITE;
			i = 1;
		}
		if(CORE.rated_ct == CT_5A)	{i |= setting_min_max_check(&OCGR50.current_set, OCGR50_I_MIN[0], OCGR50_I_MAX[0]);}
		else												{i |= setting_min_max_check(&OCGR50.current_set, OCGR50_I_MIN[1], OCGR50_I_MAX[1]);}
		i |= setting_min_max_check(&OCGR50.delay_time, OCGR50_T_MIN, OCGR50_T_MAX);		
		i |= aux_do_check(&OCGR50.do_relay);
	}
	else
	{
		//default
		OCGR50.use = 0;
		OCGR50.mode = DEFINITE;
		if(CORE.rated_ct == CT_5A)	{OCGR50.current_set = OCGR50_I_MAX[0];}
		else												{OCGR50.current_set = OCGR50_I_MAX[1];}
		OCGR50.delay_time = OCGR50_T_MAX;
		OCGR50.do_relay = 0;
					
		i = 1;
	}
	if(i)
	{
		if(setting_save(&OCGR50.use, OCGR50_USE, 5))
		{
			setting_load(&OCGR50.use, 5, OCGR50_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// ocgr51
	if(setting_load(&OCGR51.use, 5, OCGR51_USE))
	{
		i = 0;
		//범위check
		if((OCGR51.mode != INVERSE) && (OCGR51.mode != V_INVERSE) && (OCGR51.mode != E_INVERSE))
		{
			OCGR51.mode = INVERSE;
			i = 1;
		}
		if(CORE.rated_ct == CT_5A)	{i |= setting_min_max_check(&OCGR51.current_set, OCGR51_I_MIN[0], OCGR51_I_MAX[0]);}
		else												{i |= setting_min_max_check(&OCGR51.current_set, OCGR51_I_MIN[1], OCGR51_I_MAX[1]);}
		i |= setting_min_max_check(&OCGR51.time_lever, OCGR51_TL_MIN, OCGR51_TL_MAX);
		i |= aux_do_check(&OCGR51.do_relay);
	}
	else
	{
		//default
		OCGR51.use = 0;
		OCGR51.mode = INVERSE;
		if(CORE.rated_ct == CT_5A)	{OCGR51.current_set = OCGR51_I_MAX[0];}
		else												{OCGR51.current_set = OCGR51_I_MAX[1];}
		OCGR51.time_lever = OCGR51_TL_MAX;
		OCGR51.do_relay = 0;
	   		
		i = 1;
	}
	if(i)
	{
		if(setting_save(&OCGR51.use, OCGR51_USE, 5))
		{
			setting_load(&OCGR51.use, 5, OCGR51_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// uvr_1
	if(setting_load(&UVR_1.use, 4, UVR_1_USE))
	{
		i = 0;

		if(GPT.pt_secondary == 100)			 {i |= setting_min_max_check(&UVR_1.voltage_set, UVR_1_V_MIN[0], UVR_1_V_MAX[0]);}
		else if(GPT.pt_secondary == 110) {i |= setting_min_max_check(&UVR_1.voltage_set, UVR_1_V_MIN[1], UVR_1_V_MAX[1]);}
		else if(GPT.pt_secondary == 120) {i |= setting_min_max_check(&UVR_1.voltage_set, UVR_1_V_MIN[2], UVR_1_V_MAX[2]);}
		else if(GPT.pt_secondary == 190) {i |= setting_min_max_check(&UVR_1.voltage_set, UVR_1_V_MIN[3], UVR_1_V_MAX[3]);}
		i |= setting_min_max_check(&UVR_1.delay_time, UVR_T_MIN, UVR_T_MAX);		
		i |= aux_do_check(&UVR_1.do_relay);
	}
	else
	{
		//default
		UVR_1.use = 0;
		if(GPT.pt_secondary == 100)			 {UVR_1.voltage_set = UVR_1_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {UVR_1.voltage_set = UVR_1_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {UVR_1.voltage_set = UVR_1_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {UVR_1.voltage_set = UVR_1_V_MAX[3];}
		UVR_1.delay_time = UVR_T_MAX;
		UVR_1.do_relay = 0;
			 
		i = 1;
	}
	if(i)
	{
		if(setting_save(&UVR_1.use, UVR_1_USE, 4))
		{
			setting_load(&UVR_1.use, 4, UVR_1_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// uvr_2
	if(setting_load(&UVR_2.use, 4, UVR_2_USE))
	{
		i = 0;

		if(GPT.pt_secondary == 100)			 {i |= setting_min_max_check(&UVR_2.voltage_set, UVR_2_V_MIN[0], UVR_2_V_MAX[0]);}
		else if(GPT.pt_secondary == 110) {i |= setting_min_max_check(&UVR_2.voltage_set, UVR_2_V_MIN[1], UVR_2_V_MAX[1]);}
		else if(GPT.pt_secondary == 120) {i |= setting_min_max_check(&UVR_2.voltage_set, UVR_2_V_MIN[2], UVR_2_V_MAX[2]);}
		else if(GPT.pt_secondary == 190) {i |= setting_min_max_check(&UVR_2.voltage_set, UVR_2_V_MIN[3], UVR_2_V_MAX[3]);}
		i |= setting_min_max_check(&UVR_2.delay_time, UVR_T_MIN, UVR_T_MAX);		
		i |= aux_do_check(&UVR_2.do_relay);
	}
	else
	{
		//default
		UVR_2.use = 0;
		if(GPT.pt_secondary == 100)			 {UVR_2.voltage_set = UVR_2_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {UVR_2.voltage_set = UVR_2_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {UVR_2.voltage_set = UVR_2_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {UVR_2.voltage_set = UVR_2_V_MAX[3];}
		UVR_2.delay_time = UVR_T_MAX;
		UVR_2.do_relay = 0;
			 
		i = 1;
	}
	if(i)
	{
		if(setting_save(&UVR_2.use, UVR_2_USE, 4))
		{
			setting_load(&UVR_2.use, 4, UVR_2_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// uvr_3
	if(setting_load(&UVR_3.use, 4, UVR_3_USE))
	{
		i = 0;

		if(GPT.pt_secondary == 100)			 {i |= setting_min_max_check(&UVR_3.voltage_set, UVR_3_V_MIN[0], UVR_3_V_MAX[0]);}
		else if(GPT.pt_secondary == 110) {i |= setting_min_max_check(&UVR_3.voltage_set, UVR_3_V_MIN[1], UVR_3_V_MAX[1]);}
		else if(GPT.pt_secondary == 120) {i |= setting_min_max_check(&UVR_3.voltage_set, UVR_3_V_MIN[2], UVR_3_V_MAX[2]);}
		else if(GPT.pt_secondary == 190) {i |= setting_min_max_check(&UVR_3.voltage_set, UVR_3_V_MIN[3], UVR_3_V_MAX[3]);}
		i |= setting_min_max_check(&UVR_3.delay_time, UVR_T_MIN, UVR_T_MAX);		
		i |= aux_do_check(&UVR_3.do_relay);
	}
	else
	{
		//default
		UVR_3.use = 0;
		if(GPT.pt_secondary == 100)			 {UVR_3.voltage_set = UVR_3_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {UVR_3.voltage_set = UVR_3_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {UVR_3.voltage_set = UVR_3_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {UVR_3.voltage_set = UVR_3_V_MAX[3];}
		UVR_3.delay_time = UVR_T_MAX;
		UVR_3.do_relay = 0;
			 
		i = 1;
	}
	if(i)
	{
		if(setting_save(&UVR_3.use, UVR_3_USE, 4))
		{
			setting_load(&UVR_3.use, 4, UVR_3_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// p47
	if(setting_load(&P47.use, 4, P47_USE))
	{
		i = 0;
		
		if(GPT.pt_secondary == 100)			 {i |= setting_min_max_check(&P47.voltage_set, P47_V_MIN[0], P47_V_MAX[0]);}
		else if(GPT.pt_secondary == 110) {i |= setting_min_max_check(&P47.voltage_set, P47_V_MIN[1], P47_V_MAX[1]);}
		else if(GPT.pt_secondary == 120) {i |= setting_min_max_check(&P47.voltage_set, P47_V_MIN[2], P47_V_MAX[2]);}
		else if(GPT.pt_secondary == 190) {i |= setting_min_max_check(&P47.voltage_set, P47_V_MIN[3], P47_V_MAX[3]);}
		i |= setting_min_max_check(&P47.delay_time, P47_T_MIN, P47_T_MAX);
		i |= aux_do_check(&P47.do_relay);
	}
	else
	{
		//default
		P47.use = 0;
		if(GPT.pt_secondary == 100)			 {P47.voltage_set = P47_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {P47.voltage_set = P47_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {P47.voltage_set = P47_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {P47.voltage_set = P47_V_MAX[3];}
		P47.delay_time = P47_T_MAX;
		P47.do_relay = 0;
		
		i = 1;
	}
	if(i)
	{
		if(setting_save(&P47.use, P47_USE, 4))
		{
			setting_load(&P47.use, 4, P47_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// n47
	if(setting_load(&N47.use, 4, N47_USE))
	{
		i = 0;
		
		if(GPT.pt_secondary == 100)			 {i |= setting_min_max_check(&N47.voltage_set, N47_V_MIN[0], N47_V_MAX[0]);}
		else if(GPT.pt_secondary == 110) {i |= setting_min_max_check(&N47.voltage_set, N47_V_MIN[1], N47_V_MAX[1]);}
		else if(GPT.pt_secondary == 120) {i |= setting_min_max_check(&N47.voltage_set, N47_V_MIN[2], N47_V_MAX[2]);}
		else if(GPT.pt_secondary == 190) {i |= setting_min_max_check(&N47.voltage_set, N47_V_MIN[3], N47_V_MAX[3]);}
		i |= setting_min_max_check(&N47.delay_time, N47_T_MIN, N47_T_MAX);
		i |= aux_do_check(&N47.do_relay);
	}
	else
	{
		//default
		N47.use = 0;
		if(GPT.pt_secondary == 100)			 {N47.voltage_set = N47_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {N47.voltage_set = N47_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {N47.voltage_set = N47_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {N47.voltage_set = N47_V_MAX[3];}
		N47.delay_time = N47_T_MAX;
		N47.do_relay = 0;
		
		i = 1;
	}
	if(i)
	{
		if(setting_save(&N47.use, N47_USE, 4))
		{
			setting_load(&N47.use, 4, N47_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// ovr
	if(setting_load(&OVR.use, 6, OVR_USE))
	{
		i = 0;
		//범위check
		if((OVR.mode != DEFINITE) && (OVR.mode != INVERSE))
		{
			OVR.mode = DEFINITE;
			i = 1;
		}
		if(GPT.pt_secondary == 100)			 {i |= setting_min_max_check(&OVR.voltage_set, OVR_V_MIN[0], OVR_V_MAX[0]);}
		else if(GPT.pt_secondary == 110) {i |= setting_min_max_check(&OVR.voltage_set, OVR_V_MIN[1], OVR_V_MAX[1]);}
		else if(GPT.pt_secondary == 120) {i |= setting_min_max_check(&OVR.voltage_set, OVR_V_MIN[2], OVR_V_MAX[2]);}
		else if(GPT.pt_secondary == 190) {i |= setting_min_max_check(&OVR.voltage_set, OVR_V_MIN[3], OVR_V_MAX[3]);}
		
		i |= setting_min_max_check(&OVR.delay_time, OVR_T_MIN, OVR_T_MAX);
		i |= setting_min_max_check(&OVR.time_lever, OVR_TL_MIN, OVR_TL_MAX);
		i |= aux_do_check(&OVR.do_relay);
	}
	else
	{
		//default
		OVR.use = 0;
		OVR.mode = DEFINITE;
		if(GPT.pt_secondary == 100)			 {OVR.voltage_set = OVR_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {OVR.voltage_set = OVR_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {OVR.voltage_set = OVR_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {OVR.voltage_set = OVR_V_MAX[3];}
		OVR.delay_time = OVR_T_MAX;
		OVR.time_lever = OVR_TL_MAX;
		OVR.do_relay = 0;
		
		i = 1;
	}
	if(i)
	{
		if(setting_save(&OVR.use, OVR_USE, 6))
		{
			setting_load(&OVR.use, 6, OVR_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}		

	// ovgr
	if(setting_load(&OVGR.use, 5, OVGR_USE))
	{
		i = 0;
		//범위check
		if((OVGR.mode != INSTANT) && (OVGR.mode != INVERSE))
		{
			OVGR.mode = INVERSE;
			i = 1;
		}
		if(GPT.pt_tertiary == 110)			{i |= setting_min_max_check(&OVGR.voltage_set, OVGR_V_MIN[0], OVGR_V_MAX[0]);}
		else if(GPT.pt_tertiary == 120) {i |= setting_min_max_check(&OVGR.voltage_set, OVGR_V_MIN[1], OVGR_V_MAX[1]);}
		else if(GPT.pt_tertiary == 190) {i |= setting_min_max_check(&OVGR.voltage_set, OVGR_V_MIN[2], OVGR_V_MAX[2]);}
		i |= setting_min_max_check(&OVGR.time_lever, OVGR_TL_MIN, OVGR_TL_MAX);
		i |= aux_do_check(&OVGR.do_relay);
	}
	else
	{
		//default
		OVGR.use = 0;
		OVGR.mode = INVERSE;
		if(GPT.pt_tertiary == 110)			{OVGR.voltage_set = OVGR_V_MAX[0];}
		else if(GPT.pt_tertiary == 120) {OVGR.voltage_set = OVGR_V_MAX[1];}
		else if(GPT.pt_tertiary == 190) {OVGR.voltage_set = OVGR_V_MAX[2];}
		OVGR.time_lever = OVGR_TL_MAX;
		OVGR.do_relay = 0;
		
		i = 1;
	}
	if(i)
	{
		if(setting_save(&OVGR.use, OVGR_USE, 5))
		{
			setting_load(&OVGR.use, 5, OVGR_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// DGR
	if(setting_load(&DGR.use, 6, DGR_USE))
	{
		i = 0;
		//범위check
		if(CORE.rated_ct == CT_5A)			{i |= setting_min_max_check(&DGR.current_set, DGR_I_MIN[0], DGR_I_MAX[0]);}
		else														{i |= setting_min_max_check(&DGR.current_set, DGR_I_MIN[1], DGR_I_MAX[1]);}
		if(GPT.pt_tertiary == 110)			{i |= setting_min_max_check(&DGR.voltage_set, DGR_V_MIN[0], DGR_V_MAX[0]);}
		else if(GPT.pt_tertiary == 120) {i |= setting_min_max_check(&DGR.voltage_set, DGR_V_MIN[1], DGR_V_MAX[1]);}
		else if(GPT.pt_tertiary == 190) {i |= setting_min_max_check(&DGR.voltage_set, DGR_V_MIN[2], DGR_V_MAX[2]);}
		i |= setting_min_max_check(&DGR.angle_set, DGR_A_MIN, DGR_A_MAX);		
		i |= setting_min_max_check(&DGR.delay_time, DGR_T_MIN, DGR_T_MAX);
		i |= aux_do_check(&DGR.do_relay);
	}
	else
	{
		//default
		DGR.use = 0;
		if(CORE.rated_ct == CT_5A)			{DGR.current_set = DGR_I_MAX[0];}
		else														{DGR.current_set = DGR_I_MAX[1];}
		if(GPT.pt_tertiary == 110)			{DGR.voltage_set = DGR_V_MAX[0];}
		else if(GPT.pt_tertiary == 120) {DGR.voltage_set = DGR_V_MAX[1];}
		else if(GPT.pt_tertiary == 190) {DGR.voltage_set = DGR_V_MAX[2];}
		DGR.angle_set = DGR_A_MAX;
		DGR.delay_time = DGR_T_MAX;
		DGR.do_relay = 0;
				
		i = 1;
	}
	if(i)
	{
		if(setting_save(&DGR.use, DGR_USE, 6))
		{
			setting_load(&DGR.use, 6, DGR_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

	// SGR
	if(setting_load(&SGR.use, 6, SGR_USE))
	{
		i = 0;
		//범위check
		i |= setting_min_max_check(&SGR.current_set, SGR_I_MIN, SGR_I_MAX);
		if(GPT.pt_tertiary == 110)			{i |= setting_min_max_check(&SGR.voltage_set, SGR_V_MIN[0], SGR_V_MAX[0]);}
		else if(GPT.pt_tertiary == 120) {i |= setting_min_max_check(&SGR.voltage_set, SGR_V_MIN[1], SGR_V_MAX[1]);}
		else if(GPT.pt_tertiary == 190) {i |= setting_min_max_check(&SGR.voltage_set, SGR_V_MIN[2], SGR_V_MAX[2]);}
		i |= setting_min_max_check(&SGR.angle_set, SGR_A_MIN, SGR_A_MAX);		
		i |= setting_min_max_check(&SGR.delay_time, SGR_T_MIN, SGR_T_MAX);
		i |= aux_do_check(&SGR.do_relay);
	}
	else
	{
		//default
		SGR.use = 0;
		SGR.current_set = SGR_I_MAX;
		if(GPT.pt_tertiary == 110)			{SGR.voltage_set = SGR_V_MAX[0];}
		else if(GPT.pt_tertiary == 120) {SGR.voltage_set = SGR_V_MAX[1];}
		else if(GPT.pt_tertiary == 190) {SGR.voltage_set = SGR_V_MAX[2];}
		SGR.angle_set = SGR_A_MAX;
		SGR.delay_time = SGR_T_MAX;
		SGR.do_relay = 0;
				
		i = 1;
	}
	if(i)
	{
		if(setting_save(&SGR.use, SGR_USE, 6))
		{
			setting_load(&SGR.use, 6, SGR_USE);
		}
		else
		{
			//FLASH WRITE ERROR pop up 화면
		}
	}

//-------- 계전요소 설정 값 확인 END

// comm
	// 통신보드가 삽입되어 있나 확인	
	*COMM_2_DATA1 = 0x12;
	*(COMM_2_DATA1 + 1) = 0x34;
	
	i = *COMM_2_DATA1;
	j = *(COMM_2_DATA1 + 1);
	
	// 통신보드가 삽입되어 있으면 메모리에 값이 제대로 읽기/쓰기 가 될 것임	
	if((i == 0x12) && (j == 0x34))
	COMM.use = 0xaaaa;
	else
	COMM.use = 0;

	if(setting_load(&ADDRESS.address, 1, MOD_ADDR))
	{
		i = 0;
		i |= setting_min_max_check(&ADDRESS.address, COMM_ADDR_MIN, COMM_ADDR_MAX);
		i |= setting_min_max_check(&COMM.baudrate, COMM_BAUD_MIN, COMM_BAUD_MAX);
	}
	else
	{
		//default
		ADDRESS.address = COMM_ADDR_MAX;
		COMM.baudrate = COMM_BAUD_MAX;
		i = 1;
	}
	if(i)
		setting_save(&ADDRESS.address, MOD_ADDR, 1);
	
	*COMM_2_ADDRESS = ADDRESS.address;
	*COMM_2_BAUDRATE = COMM.baudrate;
	*COMM_BOOT = 0x55;
	
	
//-------- display용 factor
	DISPLAY.multipllier[0] = CPT.ct_ratio;
	DISPLAY.multipllier[1] = CPT.ct_ratio;
	DISPLAY.multipllier[2] = CPT.ct_ratio;
	DISPLAY.multipllier[3] = CPT.nct_ratio;
	DISPLAY.multipllier[4] = CPT.nct_ratio;
//DISPLAY.multipllier[5] = 133.33333333333333333333333333333; //외부 ZCT 비 = 200:1.5=133.33[mA]=0.13333[A]
	DISPLAY.multipllier[5] = 1000;
	DISPLAY.multipllier[6] = CPT.pt_ratio;
	DISPLAY.multipllier[7] = CPT.pt_ratio;
	DISPLAY.multipllier[8] = CPT.pt_ratio;
	DISPLAY.multipllier[9] = CPT.gpt_ratio;

	// true rms calibration 값과 ct/pt ratio를 곱함
//	for(i = 0; i < 10; i++)
//	{
//		DISPLAY.multipllier[i] = CALIBRATION.slope_trms[i];
//	}
//
//	DISPLAY.multipllier[0] *= INTERNAL_CT_RATIO;
//	DISPLAY.multipllier[1] *= INTERNAL_CT_RATIO;
//	DISPLAY.multipllier[2] *= INTERNAL_CT_RATIO;
//	DISPLAY.multipllier[3] *= INTERNAL_CT_RATIO;
//	DISPLAY.multipllier[4] *= INTERNAL_NCT_RATIO;
//	DISPLAY.multipllier[5] *= INTERNAL_ZCT_RATIO;
//	DISPLAY.multipllier[6] *= INTERNAL_PT_RATIO;
//	DISPLAY.multipllier[7] *= INTERNAL_PT_RATIO;
//	DISPLAY.multipllier[8] *= INTERNAL_PT_RATIO;
//	DISPLAY.multipllier[9] *= INTERNAL_GPT_RATIO;
//	
//	DISPLAY.multipllier[0] *= CPT.ct_ratio;
//	DISPLAY.multipllier[1] *= CPT.ct_ratio;
//	DISPLAY.multipllier[2] *= CPT.ct_ratio;
//	DISPLAY.multipllier[3] *= CPT.nct_ratio;
//	DISPLAY.multipllier[4] *= CPT.nct_ratio;
//	//ZCT
//	//DISPLAY.multipllier[5] *= 133.33333333333333333333333333333;
//	DISPLAY.multipllier[6] *= CPT.pt_ratio;
//	DISPLAY.multipllier[7] *= CPT.pt_ratio;
//	DISPLAY.multipllier[8] *= CPT.pt_ratio;
//	DISPLAY.multipllier[9] *= CPT.gpt_ratio;
//	
//	for(i = 0; i < 3; i++)
//	{
//		DISPLAY.p_multipllier[i] = CALIBRATION.slope_power[i];
//		DISPLAY.p_multipllier[i] *= INTERNAL_CPT_RATIO;
//		DISPLAY.p_multipllier[i] *= CPT.ct_ratio;
//		DISPLAY.p_multipllier[i] *= CPT.pt_ratio;
//	}
//-------- display용 factor END

//-------- 누적값 읽어들이기
//	float_to_8bit_fram(&ACCUMULATION.energy_p, EP1, 0);
//	float_to_8bit_fram(&ACCUMULATION.energy_q, EQ1, 0);
//	float_to_8bit_fram(&ACCUMULATION.energy_rp, REP1, 0);
//	float_to_8bit_fram(&ACCUMULATION.energy_rq, REQ1, 0);
//	float_to_8bit_fram(&ACCUMULATION.vo_max, VoMAX1, 0);
//	float_to_8bit_fram(&ACCUMULATION.io_max, IoMAX1, 0);
//-------- 누적값 읽어들이기 END

//	//cb 투입 누적시간
//	SUPERVISION.cb_close_time = *CB_CLOSE_TIME1;
//	SUPERVISION.cb_close_time <<= 8;
//	SUPERVISION.cb_close_time |= *CB_CLOSE_TIME2 & 0x00ff;
//	
//	// remote, local 상태 읽어들이기	
//	SYSTEM.remote1_local0 = *REMOTE_LOCAL;
//	SYSTEM.remote1_local0 &= 0x000f;
//	if(SYSTEM.remote1_local0 > 1)
//	{
//		SYSTEM.remote1_local0 = 0;
//		*REMOTE_LOCAL = 0;
//	}	

//-------- LED, DO 초기화
	SYSTEM.led_on = OP_LED;	// op led on

//-------- LED, DO 초기화 END

	// 샘플 필터링 활성 화
	SAMPLE.normal = 0x1234;
}


// do 설정값 체크
// *ar_aux - do 설정값 저장된 전역변수
// return - 정상 유무
unsigned int aux_do_check(unsigned int *ar_aux)
{
	if(*ar_aux > AUX_RELAY_MAX)
	{
		*ar_aux = 0;
		return(1);
	}
	return(0);
}

// 설정값 체크
// *ar_value - 비교할 값
// ar_min, ar_max - 비교할 최소 최대 값
// return - 크기 이상여부
unsigned int setting_min_max_check(unsigned int *ar_value, unsigned int ar_min, unsigned int ar_max)
{
	// 최소보다 작거나 최대보다 크면 최대값으로 초기화
	if((*ar_value < ar_min) || (*ar_value > ar_max))
	{
		*ar_value = ar_max;
		return(1);
	}
	return(0);
}

unsigned int pt_setting_min_max_check(unsigned long *ar_value, unsigned int ar_min, unsigned long ar_max)
{
	// 최소보다 작거나 최대보다 크면 최대값으로 초기화
	if((*ar_value < ar_min) || (*ar_value > ar_max))
	{
		*ar_value = ar_max;
		return(1);
	}
	return(0);
}
