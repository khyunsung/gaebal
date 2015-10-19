#include "define.h"
#include "variable.h"
#include "prototype.h"

//28335는 32bit machine이지만 16bit(1word) 처리함. 외부 peripheral도 16bit
//char 16bit, int 16bit, long 16bit
//전압 계전은 모두 PT에 걸리는 전압 기준임. 3상4선은 상전압, 3상3선은 선간전압(HIMAP-BCS는 모두 선간전압 기준임)
//Ia, Ib, Ic, Io은 모두 최대 100[A], Va, Vb, Vc, Vo는 모두 최대 380[V]
//계측은 36샘플이며, A/D에서 3샘플을 한꺼번에 12번 적어주는 방식임
void main(void) 
{
	// H/W 초기화 및 관리....
	// 아래 세개 함수는 H/W 변동이 없는 한 수정 없음

	cpu_setup();         // DSP setup에 관한 것은 TI guide에 따름
	Watchdog_Disable_FM31L27x();
	lcd_setup();	     // LCD 컨트롤러에 따름
	interrupt_control(); // 인터럽트 등록

	// 나머지 system setup 및 변수정리
	booting_setting_check();

	// Enable Global interrupt INTM
	EINT;	// 여기서부터 인터럽트 활성 화

	//Watchdog_Enable_FM31L27x();
	// 진짜 main 함수
	real_main();
}

void real_main(void)
{
	unsigned int i;
	unsigned int j;
//	unsigned int temp;

	menu_init();

	for(;;)
	{	
		//relay
		if(SAMPLE.ending)	
		{
			PROTECTIVE_RELAY();
		}

		// key
		if(TIMER.key > 80)	{key_drive();}

		// lcd
		if(TIMER.lcd > 200)	{menu_drive();}//300

		// 기본계측 및 선간,위상
		if(TIMER.measurement > 4)
		{
			if(DISPLAY.switching == 0)	{measure_display();}
			else												{measure2_display();}
		}

		// AD 인터럽트에서 1초간 전력값이 누적되었다고 알려오면 호출
		if(DISPLAY.Power_Up == 0xaaaa)	{power_update();}

		// 20ms 마다 순서대로 계산함
		if(HARMONICS.timer > 20)	{harmonics();}

//-------- manager 통신
		// 뭔진모르지만 데이타가 들어온 상태인데....
		if(MANAGER.rx_status == 1)
		{
			// 케이블 끊어지거나 알수 없는 이유로 2초가 지나도록 프레임 수신 완료(MANAGER.rx_status == 2)가 안되면
			// 시리얼 관련 변수를 초기화
			if(MANAGER.rx_timeout > 2000)
			{
				MANAGER.rx_count = 0;	// 입력 개수 카운터 초기화
				MANAGER.rx_status = 0; // 시리얼 상태 플래그 초기화
			}
		}

		// 일단 예측된 프레임 입력이 완료된 상태
		else if(MANAGER.rx_status == 2)
		{
			// 맨 첫바이트 헤더 검사
			// header 이상하면 pc로 nak 송신
			if(MANAGER.rx_buffer[0] != '#')	{serial_ok_nak_send(0x01);}

			// 두번째 id 검사하는데 0 또는 ADDRESS.address에 지정된 값이 들어오지 않으면 nak
			// id 이상
			else if((MANAGER.rx_buffer[1] != 0) && (MANAGER.rx_buffer[1] != ADDRESS.address))	{serial_ok_nak_send(0x02);}

			// 일단 헤더는 정상 통과
			// 일단 정상
			else
			{
				// 전체 시리얼 데이터의 건전성 판단을 위해 같이 딸려 들어온 crc 값과 비교한다.
				// crc 계산
				j = COMM_CRC(MANAGER.rx_buffer, MANAGER.rx_length - 2);

				// 프레임으로 같이 들어온 crc 상위바이트
				i = MANAGER.rx_buffer[MANAGER.rx_length - 2];
				i <<= 8;
				// 프레임으로 같이 들어온 crc 하위바이트를 or 하면 crc 값 추출
				i |= MANAGER.rx_buffer[MANAGER.rx_length - 1];

				// 입력된 crc 값과 계산한 crc값이 같으면 프레임은 건전하다라고 판단
				if(i == j)	{manager_handling();}
				// 프레임 불량. nak
				else				{serial_ok_nak_send(0x03);}
			}

			MANAGER.rx_count = 0;
			MANAGER.rx_status = 0;
		}
//-------- manager 통신 END

//-------- wave 처리
		if((WAVE.post_count == 5400) && (WAVE.post_start == 0x1234))	{wave_save_process();}
//-------- wave 처리 END

//-------- tcs/ccs
//		if(SUPERVISION.mode == 0xaaaa)
//		{
//			if(SUPERVISION.monitor_update)
//			{
//				SUPERVISION.monitor_update = 0; // 플래그 초기화
//
//				SUPERVISION.monitoring[0] /= 720; // 1초동안 값 평균
//				supervision_relay(0); // tcs 실행
//
//				SUPERVISION.monitoring[1] /= 720; // 1초동안 값 평균
//				supervision_relay(1);	//ccs 실행
//			}
//		}
//-------- tcs/ccs END

//-------- event 저장
		// di/do on/off 이벤트만 여기서 한꺼번에 저장
		// 1ms 인터럽트 안에서 최소한의 동작만 하고 빠지기 위함
		// 오차가 나봐야 최대 1ms 오차 발생하므로 공학적으로 의미없음
		if(EVENT.di_off & 0x0000ffff)	{event_direct_save(&EVENT.di_off);}
		if(EVENT.di_on & 0x0000ffff)	{event_direct_save(&EVENT.di_on);}

//		if(SYSTEM.do_status != SYSTEM.do_status_backup)	// do 상태가 바뀌면
//		{
//			temp = SYSTEM.do_status_backup ^ SYSTEM.do_status;	// 틀어진 놈 필터링
//			for(i = 0; i < 9; i++)
//			{
//				if(temp & DO_ON_BIT[i])	// 틀어졌나?
//				{
//					if(SYSTEM.do_status & DO_ON_BIT[i])	// close? 비트가 1
//					{
//						EVENT.do_on |= ON_BIT[i];
//					}
//					else	// open
//					{
//						EVENT.do_off |= ON_BIT[i];
//					}
//				}
//				//-------- DO display, Wave 저장, COMM용 (DO display 기능은 현재 없음)
//				if(SYSTEM.do_status & DO_ON_BIT[i])
//				{
//					DIGITAL_OUTPUT.do_status |= ON_BIT[i]; // DIGITAL_OUTPUT.do_status - DO display 변수 최종
//				}
//				else
//				{
//					DIGITAL_OUTPUT.do_status &= OFF_BIT[i];
//				}
//				//-------- DO display, Wave 저장, COMM용 END
//			}
//			SYSTEM.do_status_backup = SYSTEM.do_status;
//		}
//		// 위에서 분류해 놓은대로 이벤트 저장
//		if(EVENT.do_off & 0x0000ffff)	{event_direct_save(&EVENT.do_off);}
//		if(EVENT.do_on & 0x0000ffff)	{event_direct_save(&EVENT.do_on);}
//-------- event 저장 END

//-------- LED로 표시할 data 1초마다 체크
		if(TIMER.led > 700)	{
			led_handling();
			modbus_comm_card_check();
			SCI_Port_Err_Check();
			self_diagnostic();
//			fault_wave_send_check();
			flash_crc_check();
			//Watchdog_Kick_FM31L27x();
			//Watchdog_Read_FM31L27x(0x0a);
		}
		*LED_CS = SYSTEM.led_on; // 주기적으로 led값을 써주지 않으면 led가 꺼지는것 처럼 보임 (latch 회로가 없음), 1ms도 허용치 않음
//-------- LED로 표시할 data 1초마다 체크 END

//-------- 내부 rtc 시간 읽어오기
		if(TIME.update != 0xffff)
		{
			if(TIME.milisecond > 0)
			{
				rtc_handling();
				TIME.milisecond = 0;
			}
		}
//-------- 내부 rtc 시간 읽어오기 END

		// cb open 또는 cb close 후 500ms 넘어있으면 동작해제
		// cb trip 들어가 있으면
		if(SYSTEM.do_control & 0x0008)
		{
			if((RELAY_STATUS.pickup == 0) && (RELAY_STATUS.operation_realtime == 0))
			{
				// cb trip 해제
				//cb open이 명백하거나 타이머 넘었을경우
				if((DIGITAL_INPUT.di_status & 0x0002) || (TIMER.cb_open > 500))
				{
					// cb trip 해제
					SYSTEM.do_control &= ~0x0008;
				}
			}
		}
		//제어모드 시 cb close는 latch 검사하지 않음
		//cb close pulse
		if(LOCAL_CONTROL.mode == 0xaaaa)
		{
			//cb close 들어간 경우
			if(SYSTEM.do_control & 0x0020)
			{
				// cb close 해제
				if(TIMER.cb_close > 500)
				{
					// cb close 해제
					SYSTEM.do_control &= ~0x0020;
				}
			}
		}

//-------- RUNNING HOUR METER
		Cal_RHour();
//-------- RUNNING HOUR METER END

//-------- HIMIX RS-485 송신
		if(HIMIX.timer > 4)	{himix_drive();}
//-------- HIMIX RS-485 송신 END

//-------- remote 통신 사용하는 경우에만 호출
		if(COMM.use)	{comm_drive();}
//-------- remote 통신 사용하는 경우에만 호출 END
	}
}


// 3개 프레임 1초에 한번씩 날리기
// 1개 프레임 당 333ms 시간 가지고 있음
// crc 1ms 한번씩 56번
void himix_drive(void)
{
	// 전압 시작
	if(HIMIX.index == 0)
	{
		HIMIX.tx_buffer[0] = 0x7e;
		HIMIX.tx_buffer[1] = 0x30;
		HIMIX.tx_buffer[2] = 0x31;
		
		HIMIX.temp32 = (unsigned long)DISPLAY.True_RMS[Va];
		
		HIMIX.buffer_index = 3;
		++HIMIX.index;
		
		HIMIX.number_start = 0;
		
		HIMIX.crc_count = 0;
		
		HIMIX.tx_buffer[39] = 0;
		HIMIX.tx_buffer[40] = 0;
		HIMIX.tx_buffer[41] = 0;
		HIMIX.tx_buffer[42] = 0;
		HIMIX.tx_buffer[43] = 0;
		HIMIX.tx_buffer[44] = 0;
		HIMIX.tx_buffer[45] = 0;
		HIMIX.tx_buffer[46] = 0;
		HIMIX.tx_buffer[47] = 0;
		HIMIX.tx_buffer[48] = 0;
		HIMIX.tx_buffer[49] = 0;
		HIMIX.tx_buffer[50] = 0;
		HIMIX.tx_buffer[51] = 0;
		HIMIX.tx_buffer[52] = 0;
		HIMIX.tx_buffer[53] = 0;
		HIMIX.tx_buffer[54] = 0;
		HIMIX.tx_buffer[55] = 0;
		HIMIX.tx_buffer[56] = 0;
	}
	
	// 전류 시작
	else if(HIMIX.index == 37)
	{
		HIMIX.tx_buffer[0] = 0x7e;
		HIMIX.tx_buffer[1] = 0x30;
		HIMIX.tx_buffer[2] = 0x32;
		
		HIMIX.temp32 = (unsigned long)DISPLAY.True_RMS[Ia] * 10;
		
		HIMIX.buffer_index = 3;
		++HIMIX.index;
		
		HIMIX.number_start = 0;
		
		HIMIX.crc_count = 0;
	}
	
	// 전압,전류 최상위
	else if((HIMIX.index == 1)  || (HIMIX.index == 10) || (HIMIX.index == 19) || (HIMIX.index == 28) || (HIMIX.index == 38) || (HIMIX.index == 47) || (HIMIX.index == 56) || (HIMIX.index == 65))
	{
		// 최상위
		HIMIX.temp16 = HIMIX.temp32 / 100000000;
		
		if(HIMIX.temp16 == 0)
		HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
		
		else
		{
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
			HIMIX.number_start = 1;
		}
		
		HIMIX.temp32 %= 100000000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전압,전류 두번쩨
	else if((HIMIX.index == 2)  || (HIMIX.index == 11) || (HIMIX.index == 20) || (HIMIX.index == 29) || (HIMIX.index == 39) || (HIMIX.index == 48) || (HIMIX.index == 57) || (HIMIX.index == 66))
	{
		//두번째
		HIMIX.temp16 = HIMIX.temp32 / 10000000;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
			}
		}
		
		HIMIX.temp32 %= 10000000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전압,전류 세번쩨
	else if((HIMIX.index == 3)  || (HIMIX.index == 12) || (HIMIX.index == 21) || (HIMIX.index == 30) || (HIMIX.index == 40) || (HIMIX.index == 49) || (HIMIX.index == 58) || (HIMIX.index == 67))
	{
		//세번째
		HIMIX.temp16 = HIMIX.temp32 / 1000000;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
			}
		}
		
		HIMIX.temp32 %= 1000000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전압,전류 네번쩨
	else if((HIMIX.index == 4)  || (HIMIX.index == 13) || (HIMIX.index == 22) || (HIMIX.index == 31) || (HIMIX.index == 41) || (HIMIX.index == 50) || (HIMIX.index == 59) || (HIMIX.index == 68))
	{
		//네번째
		HIMIX.temp16 = HIMIX.temp32 / 100000;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
			}
		}
		
		HIMIX.temp32 %= 100000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전압,전류 다섯번쩨
	else if((HIMIX.index == 5)  || (HIMIX.index == 14) || (HIMIX.index == 23) || (HIMIX.index == 32) || (HIMIX.index == 42) || (HIMIX.index == 51) || (HIMIX.index == 60) || (HIMIX.index == 69))
	{
		//다섯번째
		HIMIX.temp16 = HIMIX.temp32 / 10000;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
			}
		}
		
		HIMIX.temp32 %= 10000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전압,전류 여섯번쩨
	else if((HIMIX.index == 6)  || (HIMIX.index == 15) || (HIMIX.index == 24) || (HIMIX.index == 33) || (HIMIX.index == 43) || (HIMIX.index == 52) || (HIMIX.index == 61) || (HIMIX.index == 70))
	{
		//여섯번째
		HIMIX.temp16 = HIMIX.temp32 / 1000;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
			}
		}
		
		HIMIX.temp32 %= 1000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전압,전류 일곱번쩨
	else if((HIMIX.index == 7)  || (HIMIX.index == 16) || (HIMIX.index == 25) || (HIMIX.index == 34) || (HIMIX.index == 44) || (HIMIX.index == 53) || (HIMIX.index == 62) || (HIMIX.index == 71))
	{
		//일곱번째
		HIMIX.temp16 = HIMIX.temp32 / 100;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
			}
		}
		
		HIMIX.temp32 %= 100;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전압,전류 여덟번쩨
	else if((HIMIX.index == 8)  || (HIMIX.index == 17) || (HIMIX.index == 26) || (HIMIX.index == 35) || (HIMIX.index == 45) || (HIMIX.index == 54) || (HIMIX.index == 63) || (HIMIX.index == 72))
	{
		//여덟번째
		HIMIX.temp16 = HIMIX.temp32 / 10;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
			}
		}
		
		HIMIX.temp32 %= 10;
		
		if((HIMIX.index == 45) || (HIMIX.index == 54) || (HIMIX.index == 63) || (HIMIX.index == 72))
		{
			HIMIX.tx_buffer[HIMIX.buffer_index] |= 0x80;
		}
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전압,전류 아홉번쩨
	else if((HIMIX.index == 9)  || (HIMIX.index == 18) || (HIMIX.index == 27) || (HIMIX.index == 36) || (HIMIX.index == 46) || (HIMIX.index == 55) || (HIMIX.index == 64) || (HIMIX.index == 73))
	{
		if(HIMIX.index == 9)
		{
			//아홉번째
			// 여기는 무조건 기호
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
			HIMIX.tx_buffer[HIMIX.buffer_index] |= 0x80;
			
			HIMIX.temp32 = (unsigned long)DISPLAY.True_RMS[Vb];
			
			++HIMIX.buffer_index;
			++HIMIX.index;
		}
		
		else if(HIMIX.index == 18)
		{
			// 여기는 무조건 기호
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
			HIMIX.tx_buffer[HIMIX.buffer_index] |= 0x80;
			
			HIMIX.temp32 = (unsigned long)DISPLAY.True_RMS[Vc];
			
			++HIMIX.buffer_index;
			++HIMIX.index;
		}
		
		else if(HIMIX.index == 27)
		{
			// 여기는 무조건 기호
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
			HIMIX.tx_buffer[HIMIX.buffer_index] |= 0x80;
			
			HIMIX.temp32 = (unsigned long)DISPLAY.True_RMS[Vn];
			
			++HIMIX.buffer_index;
			++HIMIX.index;
		}
		
		else if(HIMIX.index == 36)
		{
			if(HIMIX.crc_count == 0)
			{
				// 여기는 무조건 기호
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
				HIMIX.tx_buffer[HIMIX.buffer_index] |= 0x80;
				
				HIMIX.crc[0] = 0;
				HIMIX.crc[1] = 0;
				
				HIMIX.crc_count = 1;
			}
			
			//for(i=1;i<56;i++)
			//himix_crc(localTxBufB[i]);
	
			himix_crc(HIMIX.tx_buffer[HIMIX.crc_count]);
			
			++HIMIX.crc_count;
			
			if(HIMIX.crc_count == 56)
			{
				//발사
				//485 driver 활성
				HIMIX_SEND;
				// tx interrupt 활성
				*ScicRegs_SCICTL2 |= 0x0001;
				
				// tx intrrupt 활성화 후 최초 한번 써야함
				HIMIX.tx_count = 1;
				*ScicRegs_SCITXBUF = HIMIX.tx_buffer[0];
				
				++HIMIX.index;
			}
		}
			
		else if(HIMIX.index == 46)
		{
			// 여기는 무조건 기호
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
			
			HIMIX.temp32 = (unsigned long)DISPLAY.True_RMS[Ib] * 10;
						
			++HIMIX.buffer_index;
			++HIMIX.index;
		}
		
		else if(HIMIX.index == 55)
		{
			// 여기는 무조건 기호
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
			
			HIMIX.temp32 = (unsigned long)DISPLAY.True_RMS[Ic] * 10;
						
			++HIMIX.buffer_index;
			++HIMIX.index;
		}
		
		else if(HIMIX.index == 64)
		{
			// 여기는 무조건 기호
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
			
			HIMIX.temp32 = (unsigned long)DISPLAY.True_RMS[In] * 10;
			
			++HIMIX.buffer_index;
			++HIMIX.index;
		}	
		
		else if(HIMIX.index == 73)
		{
			if(HIMIX.crc_count == 0)
			{
				// 여기는 무조건 기호
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
				HIMIX.tx_buffer[HIMIX.buffer_index] |= 0x80;
				
				HIMIX.crc[0] = 0;
				HIMIX.crc[1] = 0;
				
				HIMIX.crc_count = 1;
			}
			
			//for(i=1;i<56;i++)
			//himix_crc(localTxBufB[i]);
	
			himix_crc(HIMIX.tx_buffer[HIMIX.crc_count]);
			
			++HIMIX.crc_count;
			
			if(HIMIX.crc_count == 56)
			{
				//발사
				++HIMIX.index;
			}
		}	
	}
	
	
	// 전력 시작
	else if(HIMIX.index == 74)
	{
		HIMIX.tx_buffer[0] = 0x7e;
		HIMIX.tx_buffer[1] = 0x30;
		HIMIX.tx_buffer[2] = 0x33;
		
		if(DISPLAY.p3 < 0)
		{
			HIMIX.minus_flag = 1;			
			HIMIX.temp32 = (unsigned long)(DISPLAY.p3 * -10);
		}
		
		else
		{
			HIMIX.minus_flag = 0;
			HIMIX.temp32 = (unsigned long)(DISPLAY.p3 * 10);
		}
		
		HIMIX.buffer_index = 3;		
		++HIMIX.index;
		
		HIMIX.number_start = 0;
		
		HIMIX.crc_count = 0;
	}
	
	// 전력 최상위
	else if((HIMIX.index == 75)  || (HIMIX.index == 84)  || (HIMIX.index == 93) || (HIMIX.index == 102) || (HIMIX.index == 111) || (HIMIX.index == 120))
	{
		// 최상위
		HIMIX.temp16 = HIMIX.temp32 / 100000000;
		
		if(HIMIX.temp16 == 0)
		HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
		
		else
		{
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
			HIMIX.number_start = 1;
		}
		
		HIMIX.temp32 %= 100000000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전력 두번쩨
	else if((HIMIX.index == 76)  || (HIMIX.index == 85)  || (HIMIX.index == 94) || (HIMIX.index == 103) || (HIMIX.index == 112) || (HIMIX.index == 121))
	{
		//두번째
		HIMIX.temp16 = HIMIX.temp32 / 10000000;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
				
				if(HIMIX.minus_flag)
				{
					HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;				
					HIMIX.minus_flag = 0;
				}
			}
		}
		
		HIMIX.temp32 %= 10000000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전력 세번쩨
	else if((HIMIX.index == 77)  || (HIMIX.index == 86)  || (HIMIX.index == 95) || (HIMIX.index == 104) || (HIMIX.index == 113) || (HIMIX.index == 122))
	{
		//세번째
		HIMIX.temp16 = HIMIX.temp32 / 1000000;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
				
				if(HIMIX.minus_flag)
				{
					HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;				
					HIMIX.minus_flag = 0;
				}
			}
		}
		
		HIMIX.temp32 %= 1000000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전력 네번쩨
	else if((HIMIX.index == 78)  || (HIMIX.index == 87)  || (HIMIX.index == 96) || (HIMIX.index == 105) || (HIMIX.index == 114) || (HIMIX.index == 123))
	{
		//네번째
		HIMIX.temp16 = HIMIX.temp32 / 100000;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
				
				if(HIMIX.minus_flag)
				{
					HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;
					HIMIX.minus_flag = 0;
				}
			}
		}
		
		HIMIX.temp32 %= 100000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전력 다섯번쩨
	else if((HIMIX.index == 79)  || (HIMIX.index == 88)  || (HIMIX.index == 97) || (HIMIX.index == 106) || (HIMIX.index == 115) || (HIMIX.index == 124))
	{
		//다섯번째
		HIMIX.temp16 = HIMIX.temp32 / 10000;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
				
				if(HIMIX.minus_flag)
				{
					HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;
					HIMIX.minus_flag = 0;
				}
			}
		}
		
		HIMIX.temp32 %= 10000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전력 여섯번쩨
	else if((HIMIX.index == 80)  || (HIMIX.index == 89)  || (HIMIX.index == 98) || (HIMIX.index == 107) || (HIMIX.index == 116) || (HIMIX.index == 125))
	{
		//여섯번째
		HIMIX.temp16 = HIMIX.temp32 / 1000;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
				
				if(HIMIX.minus_flag)
				{
					HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;
					HIMIX.minus_flag = 0;
				}
			}
		}
		
		HIMIX.temp32 %= 1000;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전력 일곱번쩨
	else if((HIMIX.index == 81)  || (HIMIX.index == 90)  || (HIMIX.index == 99) || (HIMIX.index == 108) || (HIMIX.index == 117) || (HIMIX.index == 126))
	{
		//일곱번째
		HIMIX.temp16 = HIMIX.temp32 / 100;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
				
				if(HIMIX.minus_flag)
				{
					HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;
					HIMIX.minus_flag = 0;
				}
			}
		}
		
		HIMIX.temp32 %= 100;
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전력 여덟번쩨
	else if((HIMIX.index == 82)  || (HIMIX.index == 91)  || (HIMIX.index == 100) || (HIMIX.index == 109) || (HIMIX.index == 118) || (HIMIX.index == 127))
	{
		//여덟번째
		HIMIX.temp16 = HIMIX.temp32 / 10;
		
		if(HIMIX.number_start)
		HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
		
		else
		{
			if(HIMIX.temp16 == 0)
			HIMIX.tx_buffer[HIMIX.buffer_index] = 0;
			
			else
			{
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp16];
				HIMIX.number_start = 1;
				
				if(HIMIX.minus_flag)
				{
					HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;
					HIMIX.minus_flag = 0;
				}
			}
		}
		
		HIMIX.temp32 %= 10;
		
		if((HIMIX.index == 82) || (HIMIX.index == 91) || (HIMIX.index == 118)|| (HIMIX.index == 127))
		{
			HIMIX.tx_buffer[HIMIX.buffer_index] |= 0x80;
		}
		
		++HIMIX.buffer_index;
		++HIMIX.index;
	}
	
	// 전력 아홉번쩨
	else if((HIMIX.index == 83)  || (HIMIX.index == 92)  || (HIMIX.index == 101) || (HIMIX.index == 110) || (HIMIX.index == 119) || (HIMIX.index == 128))
	{		
		if(HIMIX.index == 83)
		{
			//아홉번째
			// 여기는 무조건 기호
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
		
			if(HIMIX.minus_flag)
			{
				HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;
				HIMIX.minus_flag = 0;
			}
		
			if(DISPLAY.q3 < 0)
			{
				HIMIX.minus_flag = 1;			
				HIMIX.temp32 = (unsigned long)(DISPLAY.q3 * -10);
			}
		
			else
			{
				HIMIX.minus_flag = 0;
				HIMIX.temp32 = (unsigned long)(DISPLAY.q3 * 10);
			}
			
			HIMIX.number_start = 0;
			
			++HIMIX.buffer_index;
			++HIMIX.index;
		}
		
		else if(HIMIX.index == 92)
		{
			//아홉번째
			// 여기는 무조건 기호
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
		
			if(HIMIX.minus_flag)
			{
				HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;
				HIMIX.minus_flag = 0;
			}
			
			
			if(ACCUMULATION.energy_p < 0)
			{
				HIMIX.minus_flag = 1;			
				HIMIX.temp32 = (unsigned long)(ACCUMULATION.energy_p * -1);
			}
		
			else
			{
				HIMIX.minus_flag = 0;
				HIMIX.temp32 = (unsigned long)ACCUMULATION.energy_p;
			}
			
			HIMIX.number_start = 0;
			
			++HIMIX.buffer_index;
			++HIMIX.index;
		}
		
		else if(HIMIX.index == 101)
		{
			//아홉번째
			// 여기는 무조건 기호
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
			HIMIX.tx_buffer[HIMIX.buffer_index] |= 0x80;
		
			if(HIMIX.minus_flag)
			{
				HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;
				HIMIX.minus_flag = 0;
			}
			
			
			if(ACCUMULATION.energy_q < 0)
			{
				HIMIX.minus_flag = 1;			
				HIMIX.temp32 = (unsigned long)(ACCUMULATION.energy_q * -1);
			}
		
			else
			{
				HIMIX.minus_flag = 0;
				HIMIX.temp32 = (unsigned long)ACCUMULATION.energy_q;
			}
			
			HIMIX.number_start = 0;
			
			++HIMIX.buffer_index;
			++HIMIX.index;
		}
		
		else if(HIMIX.index == 110)
		{
			//아홉번째
			// 여기는 무조건 기호
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
			HIMIX.tx_buffer[HIMIX.buffer_index] |= 0x80;
		
			if(HIMIX.minus_flag)
			{
				HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;
				HIMIX.minus_flag = 0;
			}
			
			HIMIX.minus_flag = 0;
			HIMIX.temp32 = (unsigned long)(MEASUREMENT.frequency * 10);
			
			HIMIX.number_start = 0;
			
			++HIMIX.buffer_index;
			++HIMIX.index;
		}
		
		else if(HIMIX.index == 119)
		{
			//아홉번째
			// 여기는 무조건 기호
			HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
			
		
			if(HIMIX.minus_flag)
			{
				HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;
				HIMIX.minus_flag = 0;
			}
			
			if(DISPLAY.pf3 < 0)
			{
				HIMIX.minus_flag = 1;			
				HIMIX.temp32 = (unsigned long)(DISPLAY.pf3 * -10);
			}
		
			else
			{
				HIMIX.minus_flag = 0;
				HIMIX.temp32 = (unsigned long)(DISPLAY.pf3 * 10);
			}
			
			HIMIX.number_start = 0;
			
			++HIMIX.buffer_index;
			++HIMIX.index;
		}
		
		else if(HIMIX.index == 128)
		{
			if(HIMIX.crc_count == 0)
			{
				//아홉번째
				// 여기는 무조건 기호
				HIMIX.tx_buffer[HIMIX.buffer_index] = himix_number[HIMIX.temp32];
			
		
				if(HIMIX.minus_flag)
				{
					HIMIX.tx_buffer[HIMIX.buffer_index - 1] = 0x40;
					HIMIX.minus_flag = 0;
				}
			
				HIMIX.crc[0] = 0;
				HIMIX.crc[1] = 0;
				
				HIMIX.crc_count = 1;
			}
			
			//for(i=1;i<56;i++)
			//himix_crc(localTxBufB[i]);
	
			himix_crc(HIMIX.tx_buffer[HIMIX.crc_count]);
			
			++HIMIX.crc_count;
			
			if(HIMIX.crc_count == 56)
			{
				//발사
				
				
				HIMIX.index = 0;
			}
		}
	}
	HIMIX.timer = 0;
}


void himix_crc(unsigned int ar_data)
{
	unsigned int k;
	unsigned char CRC_BL,CRC_BH;

	CRC_BL = 0;

	// xor conversion
	for(k=0;k<8;k++)
	{       
		if(((ar_data >> k) & 0x01) != ((HIMIX.crc[1] >> k) & 0x01))
		{
		  CRC_BL += 0x01 << k;
		}
	}

	CRC_BH = CRC_High[CRC_BL];   

	HIMIX.crc[1] = 0;

	// xor conversion
	for(k = 0; k < 8; k++)
	{       
		if(((CRC_BH >> k) & 0x01) != ((HIMIX.crc[0] >> k) & 0x01))
		{
		  HIMIX.crc[1]+=0x01 << k;
		}
	}

	HIMIX.crc[0] = CRC_Low[CRC_BL];
}

// float값 8비트 fram 또는 시리얼 버퍼 처리할 때 사용 읽고 쓰기
// *ar_value   - 값이 저장된 변수의 주소
// *ar_address - destination 주소 
// ar_write - 읽기/쓰기 여부 
void float_to_8bit_fram(void *ar_value, unsigned int *ar_address, unsigned int ar_write)
{
	// *ar_value 가 가리키는 주소에 저장된 값의 속성이 우리는 float인지 알고 있음
	// 근데 문제는 형변환 해서 바이트단위로 쪼개야 하는데 이말은 cpu가 형변환이 raw data 그대로 쪼개줘야 한다.
	// 이걸 포인터로 처리해야 하는데 주소값이 unsigned long 형으로 구성되어 있음 - 0x200000 ~ 0x21FFFF
	// 형 지정이 되지 않는 void 포인터로 이 함수에 전달을 하고, 이 함수에서 강제로 raw 접근을 할 수 있게 주소 지정을 해준다
	unsigned long *point_long;
	unsigned int temp;
	
	// ar_value의 주소를 point_long 포인터 변수에 전달하여 포인팅 하게 한다.
	point_long = ar_value;
	
	// *ar_address 로 포인팅 된 곳에 write 할 경우
	if(ar_write)
	{
		// 어쨋거나 바이트 단위로 네개 바이트로 쪼개서 *ar_address 로 포인팅 된 곳부터 4개
		*ar_address = (*point_long >> 24) & 0x00ff;        // 최상위 바이트
		*(ar_address + 1) = (*point_long >> 16) & 0x00ff;  // 차상위 바이트
		*(ar_address + 2) = (*point_long >> 8) & 0x00ff;   // 차하위 바이트
		*(ar_address + 3) = *point_long & 0x00ff;          // 최하위 바이트
	}
	
	// *ar_address 로 포인팅 된 곳 부터 네개 바이트 값을 읽어 *ar_value로 포인팅 된 곳에 저장할 경우. 현재 시스템에서는 fram 밖에 쓸일이 없음
	else
	{
		
		*point_long = *ar_address & 0x00ff; // *ar_address 첫번째 읽어옴
		*point_long <<= 24; // 최상위 바이트로 shift
		
		temp = *(ar_address + 1) & 0x00ff;  // *ar_address 두번째 읽어옴
		*point_long |= (unsigned long)(temp) << 16; // 차상위 바이트로 shift
		
		temp = *(ar_address + 2) & 0x00ff; // *ar_address 세번째 읽어옴
		*point_long |= (unsigned long)(temp) << 8; // 차하위 바이트로 shift
		
		temp = *(ar_address + 3) & 0x00ff;  // *ar_address 네번째 읽어옴
		*point_long |= (unsigned long)temp; // 최하위 바이트로 shift
	}
}

// ar_write - 읽기/쓰기 여부 
void float_to_integer(float ar_value, unsigned int *ar_address, float scale)
{
	unsigned long l_temp;
	
	// ar_value의 주소를 point_long 포인터 변수에 전달하여 포인팅 하게 한다.
	l_temp = (unsigned long)(ar_value * scale);
	
	*ar_address =       (l_temp >> 24) & 0x00ff;  // 최상위 바이트
	*(ar_address + 1) = (l_temp >> 16) & 0x00ff;  // 차상위 바이트
	*(ar_address + 2) = (l_temp >> 8) & 0x00ff;   // 차하위 바이트
	*(ar_address + 3) =  l_temp & 0x00ff;         // 최하위 바이트
}

void float_to_integer2(float ar_value, unsigned int *ar_address, float scale)
{
	unsigned long l_temp;
	
	// ar_value의 주소를 point_long 포인터 변수에 전달하여 포인팅 하게 한다.
	l_temp = (unsigned long)(ar_value * scale);
	
	*ar_address = (l_temp >> 8) & 0x00ff;   // 최상위 바이트
	*(ar_address + 1) =  l_temp & 0x00ff;   // 차상위 바이트
}
	
// raw data로 정정하는 방법 생각해볼것
// tcs/ccs 감시
// ar_index - 계산이 tcs, ccs 구분
//            0-tcs, 1-ccs
// do 제어부분 검토 후 confirm
//void supervision_relay(unsigned int ar_index)
//{
//	// 설정된 값보다 작아지면
//	if(SUPERVISION.monitoring[ar_index] < SUPERVISION.pickup[ar_index])
//	{
//		// 일종의 pickup
//		if(SUPERVISION.status[ar_index] == RELAY_NORMAL)
//		SUPERVISION.status[ar_index] = RELAY_DETECT;
//		
//		// pickup
//		else if(SUPERVISION.status[ar_index] == RELAY_DETECT)
//		{
//			//카운터 증가
//			++SUPERVISION.pickup_count[ar_index];
//			
//			// 설정된 동작시간이상되면
//			if(SUPERVISION.pickup_count[ar_index] > SUPERVISION.time[ar_index])
//			{
//				// do 제어
//				//86을 일단 지운다
//				SYSTEM.do_control &= 0xff9f;
//				SYSTEM.do_control |= SUPERVISION.do_output[ar_index]; // 지정된 do 동작 비트 셋
//				
//				SUPERVISION.do_output_off[ar_index] = SUPERVISION.do_output[ar_index];
//							
//				SUPERVISION.status[ar_index] = RELAY_PICKUP;
//				
//				
//				// event				
//			}
//		}
//				
//		else
//		{
//			SUPERVISION.status[ar_index] = RELAY_NORMAL;
//			SUPERVISION.pickup_count[ar_index] = 0;
//					
//			// do 제어
//			do_release(&SUPERVISION.do_output_off[ar_index]);
//			//SYSTEM.do_control &= DIGITAL_OUTPUT.property;
//		}
//	}
//}

// 문] "TDD(Total Demand Distortion)에 대한 식이 있는지 그것을 구하면 어떻게 구하는것인지"
//답]   THD[%] = {고조파전류/기본파전류}*100 = {root(I2^2+I3^2+,,,I50^2)/I1}*100
//
//       TDD[%] = {고조파전류/기본파전류(15~30분간 측정한 값중 최대치)}*100 입니다.
//
//        따라서 "1219511"님의 공식에 의한 계산이 맞습니다.
//
// 
//
//문] "고조파 전류 와 고조파 전압의 차이와 무엇이 전기설비에 악영향을 주는지 궁금합니다"
//
//답] 고조파전압은 부하에서 발생하는 고조파전류에 의하여 발생하며 계통임피던스에 비례합니다.
//
//      따라서 부하에서 발생하는 고조파전류에 의해 전기설비에 미치는 영향이 대부분입니다.
//
//      역상고조파전류에 의한 발전기기동실패, 영상고조파전류에 의한 OCGR 오동작 등이 대표적이지요.^^

// 고조파 계산
void harmonics(void)
{
	// Ia sample backup 
	if(HARMONICS.index == 0)
	{
		// display 최저전류 미만
		if(DISPLAY.rms_value[Ia] < DISPLAY_CURRENT_MIN)
		{
			HARMONICS.ia[0] = 0;
			HARMONICS.ia[1] = 0;
			HARMONICS.ia[2] = 0;
			HARMONICS.ia[3] = 0;
			HARMONICS.ia[4] = 0;
			HARMONICS.ia[5] = 0;
			
			HARMONICS.index = 16;
		}
		else
		harmonic_sample_backup(0); // Ia 36샘플 쟁기기
	}
	
	
	// Ia 기본파 real 성분 계산
	else if(HARMONICS.index == 1)
	{		
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_1st_cos);
	}
	
	// Ia 기본파 imag 성분 계산
	else if(HARMONICS.index == 2)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_1st_sin);
	}
	
	// Ia 기본파 rms
	else if(HARMONICS.index == 3)
	{
		HARMONICS.base = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));
	}
	
	// Ia 3조파 real 성분 계산
	else if(HARMONICS.index == 4)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_3rd_cos);
	}
	
	// Ia 3조파 imag 성분 계산
	else if(HARMONICS.index == 5)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_3rd_sin);
	}
	
	// Ia 3조파 rms
	else if(HARMONICS.index == 6)
	{
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));

	  // 기본파 대비 비율 계산
	  HARMONICS.ia[0] = HARMONICS.float_temp / HARMONICS.base;

		// TDD/THD 미리계산
		HARMONICS.ftemp[0] = HARMONICS.ia[0] * HARMONICS.ia[0];

		// 기본파 대비 %로 환산
		HARMONICS.ia[0] *= 100;
	}
	
	// Ia 5조파 real 성분 계산
	else if(HARMONICS.index == 7)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_5th_cos);
	}
	
	// Ia 5조파 imag 성분 계산
	else if(HARMONICS.index == 8)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_5th_sin);
	}
	
	// Ia 5조파 rms
	else if(HARMONICS.index == 9)
	{
		// 3조파 주석 참조
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));

		HARMONICS.ia[1] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ftemp[1] = HARMONICS.ia[1] * HARMONICS.ia[1];

		HARMONICS.ia[1] *= 100;
	}
	
	// Ia 7조파 real 성분 계산  -> 이하 상위 참조
	else if(HARMONICS.index == 10)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_7th_cos);
	}
	
	// Ia 7조파 imag 성분 계산
	else if(HARMONICS.index == 11)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_7th_sin);
	}
	
	// Ia 7조파 rms
	else if(HARMONICS.index == 12)
	{
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));

		HARMONICS.ia[2] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ftemp[2] = HARMONICS.ia[2] * HARMONICS.ia[2];

		HARMONICS.ia[2] *= 100;
	}
	
	// Ia 9조파 real 성분 계산
	else if(HARMONICS.index == 13)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_9th_cos);
	}
	
	// Ia 9조파 imag 성분 계산
	else if(HARMONICS.index == 14)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_9th_sin);
	}
	
	// Ia 9조 rms
	else if(HARMONICS.index == 15)
	{
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));

		HARMONICS.ia[3] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ftemp[3] = HARMONICS.ia[3] * HARMONICS.ia[3];

		HARMONICS.ia[3] *= 100;
	}
	
	// 향후 11조파 요구 시 요부분에 상기처럼 추가함
	// 인덱스 늘어나지만 어차피 보기만 하는거니 급할 거 없음
	
	// Ia THD/TDD  5us
	else if(HARMONICS.index == 16)
	{
		HARMONICS.float_temp = sqrt(HARMONICS.ftemp[0] + HARMONICS.ftemp[1] + HARMONICS.ftemp[2] + HARMONICS.ftemp[3]);

		HARMONICS.ia[4] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ia[5] = HARMONICS.float_temp / HARMONICS.ia_max;

		HARMONICS.ia[4] *= 100;
		HARMONICS.ia[5] *= 100;

		if(HARMONICS.base > HARMONICS.ia_max_save)
		HARMONICS.ia_max_save = HARMONICS.base;
	}
	
	// Ib sample backup
	else if(HARMONICS.index == 17)
	{
		//2.5us
		// display 최저전류 미만
		if(DISPLAY.rms_value[Ib] < DISPLAY_CURRENT_MIN)
		{
			HARMONICS.ib[0] = 0;
			HARMONICS.ib[1] = 0;
			HARMONICS.ib[2] = 0;
			HARMONICS.ib[3] = 0;
			HARMONICS.ib[4] = 0;
			HARMONICS.ib[5] = 0;
			
			HARMONICS.index = 33;
		}
		else
		harmonic_sample_backup(1);
	}
	
	// Ib 기본파 real 성분 계산
	else if(HARMONICS.index == 18)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_1st_cos);
	}
	
	// Ib 기본파 imag 성분 계산
	else if(HARMONICS.index == 19)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_1st_sin);
	}
	
	// Ib 기본파 rms
	else if(HARMONICS.index == 20)
	{
		HARMONICS.base = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));
	}
	
	// Ib 3조파 real 성분 계산
	else if(HARMONICS.index == 21)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_3rd_cos);
	}
	
	// Ib 3조파 imag 성분 계산
	else if(HARMONICS.index == 22)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_3rd_sin);
	}
	
	// Ib 3조파 rms
	else if(HARMONICS.index == 23)
	{
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));
	    
		HARMONICS.ib[0] = HARMONICS.float_temp / HARMONICS.base;
		
		HARMONICS.ftemp[0] = HARMONICS.ib[0] * HARMONICS.ib[0];
		
		HARMONICS.ib[0] *= 100;
	}
	
	// Ib 5조파 real 성분 계산
	else if(HARMONICS.index == 24)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_5th_cos);
	}
	
	// Ib 5조파 imag 성분 계산
	else if(HARMONICS.index == 25)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_5th_sin);
	}
	
	// Ib 5조파 rms
	else if(HARMONICS.index == 26)
	{
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));

		HARMONICS.ib[1] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ftemp[1] = HARMONICS.ib[1] * HARMONICS.ib[1];

		HARMONICS.ib[1] *= 100;
	}
	
	// Ib 7조파 real 성분 계산
	else if(HARMONICS.index == 27)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_7th_cos);
	}
	
	// Ib 7조파 imag 성분 계산
	else if(HARMONICS.index == 28)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_7th_sin);
	}
	
	// Ib 7조파 rms
	else if(HARMONICS.index == 29)
	{
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));

		HARMONICS.ib[2] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ftemp[2] = HARMONICS.ib[2] * HARMONICS.ib[2];

		HARMONICS.ib[2] *= 100;
	}
	
	// Ib 9조파 real 성분 계산
	else if(HARMONICS.index == 30)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_9th_cos);
	}
	
	// Ib 9조파 imag 성분 계산
	else if(HARMONICS.index == 31)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_9th_sin);
	}
	
	// Ib 9조파 rms
	else if(HARMONICS.index == 32)
	{
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));

		HARMONICS.ib[3] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ftemp[3] = HARMONICS.ib[3] * HARMONICS.ib[3];

		HARMONICS.ib[3] *= 100;
	}

	// 향후 11조파 요구 시 요부분에 상기처럼 추가함
	// 인덱스 늘어나지만 어차피 보기만 하는거니 급할 거 없음

	// Ib THD/TDD
	else if(HARMONICS.index == 33)
	{
		HARMONICS.float_temp = sqrt(HARMONICS.ftemp[0] + HARMONICS.ftemp[1] + HARMONICS.ftemp[2] + HARMONICS.ftemp[3]);
		
		HARMONICS.ib[4] = HARMONICS.float_temp / HARMONICS.base;
		
		HARMONICS.ib[5] = HARMONICS.float_temp / HARMONICS.ib_max;
		
		HARMONICS.ib[4] *= 100;
		HARMONICS.ib[5] *= 100;
		
		if(HARMONICS.base > HARMONICS.ib_max_save)
		HARMONICS.ib_max_save = HARMONICS.base;
	}
	
	// Ic sample backup
	else if(HARMONICS.index == 34)
	{
		//2.5us
		if(DISPLAY.rms_value[Ic] < DISPLAY_CURRENT_MIN)
		{
			HARMONICS.ic[0] = 0;
			HARMONICS.ic[1] = 0;
			HARMONICS.ic[2] = 0;
			HARMONICS.ic[3] = 0;
			HARMONICS.ic[4] = 0;
			HARMONICS.ic[5] = 0;
			
			HARMONICS.index = 51;
		}
		else
		harmonic_sample_backup(2);
	}
	
	// Ic 기본파 real 성분 계산
	else if(HARMONICS.index == 35)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_1st_cos);
	}
	
	// Ic 기본파 imag 성분 계산
	else if(HARMONICS.index == 36)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_1st_sin);
	}
	
	// Ic 기본파 rms
	else if(HARMONICS.index == 37)
	{
		HARMONICS.base = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));
	}
	
	// Ic 3조파 real 성분 계산
	else if(HARMONICS.index == 38)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_3rd_cos);
	}
	
	// Ic 3조파 imag 성분 계산
	else if(HARMONICS.index == 39)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_3rd_sin);
	}
	
	// Ic 3조파 rms
	else if(HARMONICS.index == 40)
	{
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));

		HARMONICS.ic[0] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ftemp[0] = HARMONICS.ic[0] * HARMONICS.ic[0];

		HARMONICS.ic[0] *= 100;
	}
	
	// Ic 5조파 real 성분 계산
	else if(HARMONICS.index == 41)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_5th_cos);
	}
	
	// Ic 5조파 imag 성분 계산
	else if(HARMONICS.index == 42)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_5th_sin);
	}
	
	// Ic 5조파 rms
	else if(HARMONICS.index == 43)
	{
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));

		HARMONICS.ic[1] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ftemp[1] = HARMONICS.ic[1] * HARMONICS.ic[1];

		HARMONICS.ic[1] *= 100;
	}
	
	// Ic 7조파 real 성분 계산
	else if(HARMONICS.index == 44)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_7th_cos);
	}
	
	// Ic 7조파 imag 성분 계산
	else if(HARMONICS.index == 45)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_7th_sin);
	}
	
	// Ic 7조파 rms
	else if(HARMONICS.index == 46)
	{
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));

		HARMONICS.ic[2] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ftemp[2] = HARMONICS.ic[2] * HARMONICS.ic[2];

		HARMONICS.ic[2] *= 100;
	}
	
	// Ic 9조파 real 성분 계산
	else if(HARMONICS.index == 47)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.real, harmonics_9th_cos);
	}
	
	// Ic 9조 imag
	else if(HARMONICS.index == 48)
	{
		//8us
		harmonic_real_imag_calculation(&HARMONICS.imag, harmonics_9th_sin);
	}
	
	// Ic 9조 rms
	else if(HARMONICS.index == 49)
	{
		HARMONICS.float_temp = sqrt((HARMONICS.real * HARMONICS.real) + (HARMONICS.imag * HARMONICS.imag));

		HARMONICS.ic[3] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ftemp[3] = HARMONICS.ic[3] * HARMONICS.ic[3];

		HARMONICS.ic[3] *= 100;
	}
	
	// Ic THD/TDD
	else if(HARMONICS.index == 50)
	{
		HARMONICS.float_temp = sqrt(HARMONICS.ftemp[0] + HARMONICS.ftemp[1] + HARMONICS.ftemp[2] + HARMONICS.ftemp[3]);

		HARMONICS.ic[4] = HARMONICS.float_temp / HARMONICS.base;

		HARMONICS.ic[5] = HARMONICS.float_temp / HARMONICS.ic_max;

		HARMONICS.ic[4] *= 100;
		HARMONICS.ic[5] *= 100;

		if(HARMONICS.base > HARMONICS.ic_max_save)
		HARMONICS.ic_max_save = HARMONICS.base;
	}
	
	//Ia,Ib,Ic max
	else
	{
		++HARMONICS.timer_15min;
		
		//about 15min
		if(HARMONICS.timer_15min == 900)
		{
			//전류 최대 값 교체
			HARMONICS.ia_max = HARMONICS.ia_max_save;
			HARMONICS.ib_max = HARMONICS.ib_max_save;
			HARMONICS.ic_max = HARMONICS.ic_max_save;
			
			HARMONICS.ia_max_save = 0;
			HARMONICS.ib_max_save = 0;
			HARMONICS.ic_max_save = 0;
			
			HARMONICS.timer_15min = 0;
		}
	}
	
	++HARMONICS.index;

	if(HARMONICS.index > 51)
	HARMONICS.index = 0;
	
	HARMONICS.timer = 0;
}


// 고조파 계산을 위해 미리 데이터를 백업함
// ar_channel - 계산할 전류 채널 지정
// 0-Ia, 1-Ib, 2-Ic
void harmonic_sample_backup(unsigned int ar_channel)
{
	HARMONICS.sample[0] = SAMPLE.pre_36_buffer[ar_channel][0];
	HARMONICS.sample[1] = SAMPLE.pre_36_buffer[ar_channel][1];
	HARMONICS.sample[2] = SAMPLE.pre_36_buffer[ar_channel][2];
	HARMONICS.sample[3] = SAMPLE.pre_36_buffer[ar_channel][3];
	HARMONICS.sample[4] = SAMPLE.pre_36_buffer[ar_channel][4];
	HARMONICS.sample[5] = SAMPLE.pre_36_buffer[ar_channel][5];
	HARMONICS.sample[6] = SAMPLE.pre_36_buffer[ar_channel][6];
	HARMONICS.sample[7] = SAMPLE.pre_36_buffer[ar_channel][7];
	HARMONICS.sample[8] = SAMPLE.pre_36_buffer[ar_channel][8];
	HARMONICS.sample[9] = SAMPLE.pre_36_buffer[ar_channel][9];
	HARMONICS.sample[10] = SAMPLE.pre_36_buffer[ar_channel][10];
	HARMONICS.sample[11] = SAMPLE.pre_36_buffer[ar_channel][11];
	HARMONICS.sample[12] = SAMPLE.pre_36_buffer[ar_channel][12];
	HARMONICS.sample[13] = SAMPLE.pre_36_buffer[ar_channel][13];
	HARMONICS.sample[14] = SAMPLE.pre_36_buffer[ar_channel][14];
	HARMONICS.sample[15] = SAMPLE.pre_36_buffer[ar_channel][15];
	HARMONICS.sample[16] = SAMPLE.pre_36_buffer[ar_channel][16];
	HARMONICS.sample[17] = SAMPLE.pre_36_buffer[ar_channel][17];
	HARMONICS.sample[18] = SAMPLE.pre_36_buffer[ar_channel][18];
	HARMONICS.sample[19] = SAMPLE.pre_36_buffer[ar_channel][19];
	HARMONICS.sample[20] = SAMPLE.pre_36_buffer[ar_channel][20];
	HARMONICS.sample[21] = SAMPLE.pre_36_buffer[ar_channel][21];
	HARMONICS.sample[22] = SAMPLE.pre_36_buffer[ar_channel][22];
	HARMONICS.sample[23] = SAMPLE.pre_36_buffer[ar_channel][23];
	HARMONICS.sample[24] = SAMPLE.pre_36_buffer[ar_channel][24];
	HARMONICS.sample[25] = SAMPLE.pre_36_buffer[ar_channel][25];
	HARMONICS.sample[26] = SAMPLE.pre_36_buffer[ar_channel][26];
	HARMONICS.sample[27] = SAMPLE.pre_36_buffer[ar_channel][27];
	HARMONICS.sample[28] = SAMPLE.pre_36_buffer[ar_channel][28];
	HARMONICS.sample[29] = SAMPLE.pre_36_buffer[ar_channel][29];
	HARMONICS.sample[30] = SAMPLE.pre_36_buffer[ar_channel][30];
	HARMONICS.sample[31] = SAMPLE.pre_36_buffer[ar_channel][31];
	HARMONICS.sample[32] = SAMPLE.pre_36_buffer[ar_channel][32];
	HARMONICS.sample[33] = SAMPLE.pre_36_buffer[ar_channel][33];
	HARMONICS.sample[34] = SAMPLE.pre_36_buffer[ar_channel][34];
	HARMONICS.sample[35] = SAMPLE.pre_36_buffer[ar_channel][35];
}

void harmonic_real_imag_calculation(float *ar_temp, float const *ar_table)
{
	*ar_temp = 0;
	
	*ar_temp += (float)(HARMONICS.sample[0] * ar_table[0]);
	*ar_temp += (float)(HARMONICS.sample[1] * ar_table[1]);
	*ar_temp += (float)(HARMONICS.sample[2] * ar_table[2]);
	*ar_temp += (float)(HARMONICS.sample[3] * ar_table[3]);
	*ar_temp += (float)(HARMONICS.sample[4] * ar_table[4]);
	*ar_temp += (float)(HARMONICS.sample[5] * ar_table[5]);
	*ar_temp += (float)(HARMONICS.sample[6] * ar_table[6]);
	*ar_temp += (float)(HARMONICS.sample[7] * ar_table[7]);
	*ar_temp += (float)(HARMONICS.sample[8] * ar_table[8]);
	*ar_temp += (float)(HARMONICS.sample[9] * ar_table[9]);
	*ar_temp += (float)(HARMONICS.sample[10] * ar_table[10]);
	*ar_temp += (float)(HARMONICS.sample[11] * ar_table[11]);
	*ar_temp += (float)(HARMONICS.sample[12] * ar_table[12]);
	*ar_temp += (float)(HARMONICS.sample[13] * ar_table[13]);
	*ar_temp += (float)(HARMONICS.sample[14] * ar_table[14]);
	*ar_temp += (float)(HARMONICS.sample[15] * ar_table[15]);
	*ar_temp += (float)(HARMONICS.sample[16] * ar_table[16]);
	*ar_temp += (float)(HARMONICS.sample[17] * ar_table[17]);
	*ar_temp += (float)(HARMONICS.sample[18] * ar_table[18]);
	*ar_temp += (float)(HARMONICS.sample[19] * ar_table[19]);
	*ar_temp += (float)(HARMONICS.sample[20] * ar_table[20]);
	*ar_temp += (float)(HARMONICS.sample[21] * ar_table[21]);
	*ar_temp += (float)(HARMONICS.sample[22] * ar_table[22]);
	*ar_temp += (float)(HARMONICS.sample[23] * ar_table[23]);
	*ar_temp += (float)(HARMONICS.sample[24] * ar_table[24]);
	*ar_temp += (float)(HARMONICS.sample[25] * ar_table[25]);
	*ar_temp += (float)(HARMONICS.sample[26] * ar_table[26]);
	*ar_temp += (float)(HARMONICS.sample[27] * ar_table[27]);
	*ar_temp += (float)(HARMONICS.sample[28] * ar_table[28]);
	*ar_temp += (float)(HARMONICS.sample[29] * ar_table[29]);
	*ar_temp += (float)(HARMONICS.sample[30] * ar_table[30]);
	*ar_temp += (float)(HARMONICS.sample[31] * ar_table[31]);
	*ar_temp += (float)(HARMONICS.sample[32] * ar_table[32]);
	*ar_temp += (float)(HARMONICS.sample[33] * ar_table[33]);
	*ar_temp += (float)(HARMONICS.sample[34] * ar_table[34]);
	*ar_temp += (float)(HARMONICS.sample[35] * ar_table[35]);
}

//  0,  1,  2,  3,   4,   5,  6,  7,  8,  9
// Ia, Ib, Ic, In, In2, ZCT, Va, Vb, Vc, Vn
void measure_display(void) //전압, 전류 값
{
	DISPLAY.rms_value_sum[DISPLAY.index] += MEASUREMENT.rms_value[DISPLAY.index];
	if(DISPLAY.sum_count == 49)	// 50번 누적되었을 경우
	{
		DISPLAY.rms_value_temp[DISPLAY.index] = DISPLAY.rms_value_sum[DISPLAY.index] / 50.;	// 50번 평균값 계산
		if((DISPLAY.index > 5) && (DISPLAY.index < 10)) //전압 채널(6,7,8,9)
		{
			if(DISPLAY.rms_value_temp[DISPLAY.index] < DISPLAY_VOLTAGE_MIN)	{DISPLAY.rms_value_temp[DISPLAY.index] = 0;} //전압 채널 계측 문턱값 (1.9V)
		}
		else if(DISPLAY.index < 4) //전류 채널(0,1,2,3)
		{
			if(DISPLAY.rms_value_temp[DISPLAY.index] < DISPLAY_CURRENT_MIN)	{DISPLAY.rms_value_temp[DISPLAY.index] = 0;} //전류 채널 계측 문턱값 (0.03A)
		}
		else if(DISPLAY.index == 5)
		{
			if(DISPLAY.rms_value_temp[DISPLAY.index] < DISPLAY_Io_MIN)	{DISPLAY.rms_value_temp[DISPLAY.index] = 0;} //영상 전류 채널 계측 문턱값 (0.2mA)
		}

		DISPLAY.rms_value[DISPLAY.index] = DISPLAY.rms_value_temp[DISPLAY.index] * DISPLAY.multipllier[DISPLAY.index]; // CT(PT) ratio 곱해줌 //최종 전압,전류 display 값
		DISPLAY.rms_Iavg_value = ((DISPLAY.rms_value[0] + DISPLAY.rms_value[1] + DISPLAY.rms_value[2]) / 3.0); //최종 전류 평균 display 값
		DISPLAY.rms_Vavg_value = ((DISPLAY.rms_value[6] + DISPLAY.rms_value[7] + DISPLAY.rms_value[8]) / 3.0); //최종 전압 평균 display 값

//	DISPLAY.V1_value = MEASUREMENT.V1_value * CPT.pt_ratio; // PT ratio 곱해줌
//	DISPLAY.V2_value = MEASUREMENT.V2_value * CPT.pt_ratio; // PT ratio 곱해줌
		if(MEASUREMENT.V1_value < 2) //전압 채널 계측 문턱값 (2V) // 오차가 2V 이내에서 있음... 향후 정밀도 보완 필요
		{
			DISPLAY.V1_value = 0; //최종 정상 전압 display 값
		}
		else 
		{
			DISPLAY.V1_value = MEASUREMENT.V1_value * CPT.pt_ratio; // PT ratio 곱해줌 //최종 정상 전압 display 값
		}
		if(MEASUREMENT.V2_value < 2) //전압 채널 계측 문턱값 (2V)
		{
			DISPLAY.V2_value = 0; //최종 역상 전압 display 값
		}
		else 
		{
			DISPLAY.V2_value = MEASUREMENT.V2_value * CPT.pt_ratio; // PT ratio 곱해줌 //최종 역상 전압 display 값
		}

		if(MEASUREMENT.I1_value < 0.03) //전류 채널 계측 문턱값 (0.03A) // 오차가 이내에서 있음... 향후 정밀도 보완 필요
		{
			DISPLAY.I1_value = 0; //최종 정상 전류 display 값
		}
		else 
		{
			DISPLAY.I1_value = MEASUREMENT.I1_value * CPT.ct_ratio; // CT ratio 곱해줌 //최종 정상 전류 display 값
		}
		if(MEASUREMENT.I2_value < 0.03) //전류 채널 계측 문턱값 (0.03A)
		{
			DISPLAY.I2_value = 0; //최종 역상 전류 display 값
		}
		else 
		{
			DISPLAY.I2_value = MEASUREMENT.I2_value * CPT.ct_ratio; // CT ratio 곱해줌 //최종 역상 전류 display 값
		}
		
		if(DISPLAY.index == 9)	// 저장된 영상전압보다 클때마다 MRAM에 저장(저장 위치 수정 필요)
		{
			if(DISPLAY.rms_value[9] > DISPLAY.vo_max)
			{
				DISPLAY.vo_max = (unsigned long)DISPLAY.rms_value[9];

				//MRAM 저장
				*(MRAM_Vo_MAX1) = (DISPLAY.vo_max >> 16) & 0xffff;
				*(MRAM_Vo_MAX2) = DISPLAY.vo_max & 0xffff;
			}
		}

		DISPLAY.rms_value_sum[DISPLAY.index] = 0;
	}
	++DISPLAY.index;
	if(DISPLAY.index == 10)
	{
		DISPLAY.index = 0;
		++DISPLAY.sum_count;				
		if(DISPLAY.sum_count == 50)	{DISPLAY.sum_count = 0;}
		DISPLAY.switching = 0x0001;	//measure_display(), measure_display2() 번갈아가며 계산하라고 지시하는  flag
	}
}

// 위상
void measure2_display(void)
{
	//1.5us 
	//for(temp = 0; temp < 6; temp++)
	//DISPLAY.angle[temp] = 0.0;
	
	//200ns
	DISPLAY.angle[0] = 0.0;
	DISPLAY.angle[1] = 0.0;
	DISPLAY.angle[2] = 0.0;
	DISPLAY.angle[3] = 0.0;
	DISPLAY.angle[4] = 0.0;
	DISPLAY.angle[5] = 0.0;
	DISPLAY.angle[6] = 0.0;
	DISPLAY.angle[7] = 0.0;
	
	if(DISPLAY.rms_value[Va])	{DISPLAY.angle[0] = MEASUREMENT.angle[Va];}
	if(DISPLAY.rms_value[Vb])	{DISPLAY.angle[1] = MEASUREMENT.angle[Vb];}
	if(DISPLAY.rms_value[Vc])	{DISPLAY.angle[2] = MEASUREMENT.angle[Vc];}
	if(DISPLAY.rms_value[Vc])	{DISPLAY.angle[3] = MEASUREMENT.angle[Vn];}
	if(DISPLAY.rms_value[Ia])	{DISPLAY.angle[4] = MEASUREMENT.angle[Ia];}
	if(DISPLAY.rms_value[Ib])	{DISPLAY.angle[5] = MEASUREMENT.angle[Ib];}
	if(DISPLAY.rms_value[Ic])	{DISPLAY.angle[6] = MEASUREMENT.angle[Ic];}
	if(CORE.gr_select == NCT_SELECT)
	{
		if(DISPLAY.rms_value[In])	{DISPLAY.angle[7] = MEASUREMENT.angle[In];}
	}
	else
	{
		if(DISPLAY.rms_value[Is])	{DISPLAY.angle[7] = MEASUREMENT.angle[Is];}
	}
	
	// 인터럽트에서는 라디안으로 계산하고 끝냈음
	// 눈으로 보이는 값은 degree로 환산시킴
	DISPLAY.angle[0] *= 57.29577951;  // 180/PI = 57.29577951
	DISPLAY.angle[1] *= 57.29577951;
	DISPLAY.angle[2] *= 57.29577951;
	DISPLAY.angle[3] *= 57.29577951;
	DISPLAY.angle[4] *= 57.29577951;
	DISPLAY.angle[5] *= 57.29577951;
	DISPLAY.angle[6] *= 57.29577951;
	DISPLAY.angle[7] *= 57.29577951;
	
	//Vbc
	DISPLAY.angle[1] -= DISPLAY.angle[0];
	if(DISPLAY.angle[1] < 0)	{DISPLAY.angle[1] += 360;}
	//Vca
	DISPLAY.angle[2] -= DISPLAY.angle[0];
	if(DISPLAY.angle[2] < 0)	{DISPLAY.angle[2] += 360;}
	//Vn
	DISPLAY.angle[3] -= DISPLAY.angle[0];
	if(DISPLAY.angle[3] < 0)	{DISPLAY.angle[3] += 360;}
	//Ia
	DISPLAY.angle[4] -= DISPLAY.angle[0];
	if(DISPLAY.angle[4] < 0)	{DISPLAY.angle[4] += 360;}
	//Ib
	DISPLAY.angle[5] -= DISPLAY.angle[0];
	if(DISPLAY.angle[5] < 0)	{DISPLAY.angle[5] += 360;}
	//Ic
	DISPLAY.angle[6] -= DISPLAY.angle[0];
	if(DISPLAY.angle[6] < 0)	{DISPLAY.angle[6] += 360;}
	//In or Is
	DISPLAY.angle[7] -= DISPLAY.angle[0];
	if(DISPLAY.angle[7] < 0)	{DISPLAY.angle[7] += 360;}
	//Vab는 무조건 0도
	DISPLAY.angle[0] = 0;

	DISPLAY.switching = 0x0000;
	TIMER.measurement = 0;
}


// 1초에 한번씩 전력계산
void power_update(void)
{
	float float_temp = 0.0;
	
	// 인터럽트에서 기본적인 계산은 마침
	// 여기에  0.01342814 * PT비 
	//         0.012207403 * CT 비 곱해 줌
	// real 값 도출 후 calibration factor 곱해줌
	// S는 S평면 관계식으로 구함
	
	// raw PQ
	// 1초간 720번 누적되었으므로, 평균을 낸다
	// 0.00138s * 720 = 1s
	MEASUREMENT.Pa_value /= 720;
	MEASUREMENT.Pb_value /= 720;
	MEASUREMENT.Pc_value /= 720;
	
	MEASUREMENT.Qa_value /= 720;
	MEASUREMENT.Qb_value /= 720;
	MEASUREMENT.Qc_value /= 720;
	
	// V 또는 I가 0이면 power, pf 0으로 만드는 조건 추가
	// 기본파 전압, 전류 최소값 미만이면 display를 0으로 하기위해
	if((MEASUREMENT.rms_value[Va] < 0.475) || (MEASUREMENT.rms_value[Ia] < 0.0475))
	{
		MEASUREMENT.Pa_value = 0.0;
		
		MEASUREMENT.Qa_value = 0.0;
	}
	
	if((MEASUREMENT.rms_value[Vb] < 0.475) || (MEASUREMENT.rms_value[Ib] < 0.0475))
	{
		MEASUREMENT.Pb_value = 0.0;
		
		MEASUREMENT.Qb_value = 0.0;
	}
	
	if((MEASUREMENT.rms_value[Vc] < 0.475) || (MEASUREMENT.rms_value[Ic] < 0.0475))
	{
		MEASUREMENT.Pc_value = 0.0;
		
		MEASUREMENT.Qc_value = 0.0;
	}
	
		
	
	// real PQ	
	MEASUREMENT.Pa_value *= DISPLAY.p_multipllier[0];
	MEASUREMENT.Qa_value *= DISPLAY.p_multipllier[0];
		
	MEASUREMENT.Pb_value *= DISPLAY.p_multipllier[1];
	MEASUREMENT.Qb_value *= DISPLAY.p_multipllier[1];
	
	MEASUREMENT.Pc_value *= DISPLAY.p_multipllier[2];
	MEASUREMENT.Qc_value *= DISPLAY.p_multipllier[2];
	
	
	// P/Qa
	DISPLAY.power_p[0] = (MEASUREMENT.Pa_value * CALIBRATION.Power_Cos[0]) - (MEASUREMENT.Qa_value * CALIBRATION.Power_Sin[0]);	
	DISPLAY.power_q[0] = (MEASUREMENT.Pa_value * CALIBRATION.Power_Sin[0]) + (MEASUREMENT.Qa_value * CALIBRATION.Power_Cos[0]);
	DISPLAY.power_s[0] = sqrt((DISPLAY.power_p[0] * DISPLAY.power_p[0]) + (DISPLAY.power_q[0] * DISPLAY.power_q[0]));
	
	DISPLAY.power_p[1] = (MEASUREMENT.Pb_value * CALIBRATION.Power_Cos[1]) - (MEASUREMENT.Qb_value * CALIBRATION.Power_Sin[1]);	
	DISPLAY.power_q[1] = (MEASUREMENT.Pb_value * CALIBRATION.Power_Sin[1]) + (MEASUREMENT.Qb_value * CALIBRATION.Power_Cos[1]);
	DISPLAY.power_s[1] = sqrt((DISPLAY.power_p[1] * DISPLAY.power_p[1]) + (DISPLAY.power_q[1] * DISPLAY.power_q[1]));
	
	DISPLAY.power_p[2] = (MEASUREMENT.Pc_value * CALIBRATION.Power_Cos[2]) - (MEASUREMENT.Qc_value * CALIBRATION.Power_Sin[2]);	
	DISPLAY.power_q[2] = (MEASUREMENT.Pc_value * CALIBRATION.Power_Sin[2]) + (MEASUREMENT.Qc_value * CALIBRATION.Power_Cos[2]);
	DISPLAY.power_s[2] = sqrt((DISPLAY.power_p[2] * DISPLAY.power_p[2]) + (DISPLAY.power_q[2] * DISPLAY.power_q[2]));
	
	
		
	
//	rma_bac[rmas_bac_count] = DISPLAY.power_q[0];
//	
//	++rmas_bac_count;
//	if(rmas_bac_count == 500)
//	rmas_bac_count = 0;
	
	
//	// pf
//	for(i = 0; i < 3; i++)
//	{
//		// 1사분면
//		if((DISPLAY.power_p[i] >= 0) && (DISPLAY.power_q[i] < 0))
//		{
//			j = 0x5555;
//			
//			float_temp = -1.0;
//		}
//		
//		// 2사분면
//		else if((DISPLAY.power_p[i] < 0) && (DISPLAY.power_q[i] < 0))
//		{
//			j = 0xaaaa;
//			
//			float_temp = -1.0;
//		}
//			
//		// 3사분면
//		else if((DISPLAY.power_p[i] < 0) && (DISPLAY.power_q[i] >= 0))
//		{
//			j = 0xaaaa;
//			
//			float_temp = 1.0;
//		}
//			
//		// 4사분면
//		else if((DISPLAY.power_p[i] >= 0) && (DISPLAY.power_q[i] >= 0))
//		{
//			j = 0x5555;
//			
//			float_temp = 1.0;
//		}
//		
//		DISPLAY.pf[i] = DISPLAY.power_p[i] / DISPLAY.power_s[i];
//		
//		DISPLAY.pf[i] *= float_temp;
//		
//		//k = i << 1;		
//	}
	
	
	
	// 3상 P,Q, S
	DISPLAY.p3 = DISPLAY.power_p[0] + DISPLAY.power_p[1] + DISPLAY.power_p[2];
	DISPLAY.q3 = DISPLAY.power_q[0] + DISPLAY.power_q[1] + DISPLAY.power_q[2];
	//float_temp2 = sqrt((float_temp * float_temp) + (float_temp1 * float_temp1));
	
	float_temp = DISPLAY.power_s[0] + DISPLAY.power_s[1] + DISPLAY.power_s[2];
	
//	if(TRANSFORMER.pt_wiring == P3W3)
//	{		
		DISPLAY.p3 /= 1.732050807569;
		DISPLAY.q3 /= 1.732050807569;
		float_temp  /= 1.732050807569;
//	}
	
	
	
	DISPLAY.pf3 = DISPLAY.p3 / float_temp;
	
	
	

////	// 3pf
////	// 1사분면
//////	if((float_temp >= 0) && (float_temp1 < 0))
//////	{
//////		j = 0x5555;
//////			
//////		float_temp2 *= -1.0;
//////	}
//////		
//////	// 2사분면
//////	else if((float_temp < 0) && (float_temp1 < 0))
//////	{
//////		j = 0xaaaa;
//////			
//////		float_temp2 *= -1.0;
//////	}
//////			
//////	// 3사분면
//////	else if((float_temp < 0) && (float_temp1 >= 0))
//////	{
//////		j = 0xaaaa;		
//////	}
//////			
//////	// 4사분면
//////	else if((float_temp2 >= 0) && (float_temp3 >= 0))
//////	{
//////		j = 0x5555;		
//////	}
//////	
//////	FRAM_Access(&float_temp2, Pf, 2);
//////				
//////	FRAM_Access(&j, Pf_Dir , 0);
////	
////
	
	
	//PE
	if(DISPLAY.p3 != 0)
	{
		// 전력 * 시간을 초로나눈 값{1/(60초*60분)}
		float_temp = DISPLAY.p3 * 0.000277777;
		
		if(float_temp > 0)
		{
			ACCUMULATION.energy_p += float_temp;
			
			float_to_8bit_fram(&ACCUMULATION.energy_p, EP1, 1);
		}
	}
	
	//QE
	if(DISPLAY.q3 != 0)
	{
		// 전력 * 시간을 초로나눈 값{1/(60초*60분)}
		float_temp = DISPLAY.q3 * 0.000277777;
		
		if(float_temp > 0)
		{
			ACCUMULATION.energy_q += float_temp;
			
			float_to_8bit_fram(&ACCUMULATION.energy_q, EQ1, 1);
		}
	}


//
//	float_temp = 0;
//	// P
//	for(i = 0; i < 3; i++)
//	{		
//		// PE
//		if(DISPLAY.power_p[i] != 0)
//		{
//			// 전력 * 시간을 초로나눈 값{1/(60초*60분)}
//			PQE_buffer[i] = DISPLAY.power_p[i] * 0.000277777;
//			
//			if(PQE_buffer[i] > 0)
//			{
//				float_temp += PQE_buffer[i];
//				
//				//FRAM_Access((PEa + i), &float_temp1, 2);
//				
//				
//				
//				// float 한계치보다 크면
//				if((float_temp1 - PQE_buffer[i]) > 9999999.0)
//				{
//					POWER.PE_Under[i] += PQE_buffer[i];
//					
//					// 따로누적이 한계치보다 작으면
//					if((float_temp1 - POWER.PE_Under[i]) <= 9999999.0)
//					{
//						float_temp1 += POWER.PE_Under[i];
//						
//						POWER.PE_Under[i] = 0.0;
//						
//						//10G 넘으면 0
//					    if(float_temp1 > 10000000000)
//					    float_temp1 = 0.0;
//					    
//					    FRAM_Access(&float_temp1, (PEa + i), 2);
//					}
//				}
//				
//				// 범위내이면
//				else
//				{
//					PQE_buffer[i] += float_temp1;
//					
//					//10G 넘으면 0
//					if(PQE_buffer[i] > 10000000000)
//					PQE_buffer[i] = 0;
//					
//					FRAM_Access(&PQE_buffer[i], (PEa + i), 2);
//				}				
//			}			
//		}		
//	}
//	
//	// 3PE
//	//float_temp = PQE_buffer[0] + PQE_buffer[1] + PQE_buffer[2];
//	
//	if(float_temp > 0)
//	{
//		FRAM_Access(PE3p, &float_temp1, 2);
//		
//		float_temp2 = float_temp + float_temp1;
//		
//		// 10G 넘으면 0
//		if(float_temp2 > 10000000000)
//		float_temp = 0;
//		
//		FRAM_Access(&float_temp2, PE3p, 2);
//	}
//	
//	
//	float_temp = 0;
//	// Q
//	for(i = 0; i < 3; i++)
//	{		
//		// QE
//		if(Q_buffer[i] != 0)
//		{			
//			PQE_buffer[i] = Q_buffer[i] * 0.000277777;
//			
//			if(PQE_buffer[i] > 0)
//			{
//				float_temp += PQE_buffer[i];
//				
//				FRAM_Access((QEa + i), &float_temp1, 2);
//				
//				
//				
//				// float 한계치보다 크면
//				if((float_temp1 - PQE_buffer[i]) > 9999999.0)
//				{
//					POWER.QE_Under[i] += PQE_buffer[i];
//					
//					// 따로누적이 한계치보다 작으면
//					if((float_temp1 - POWER.QE_Under[i]) <= 9999999.0)
//					{
//						float_temp1 += POWER.QE_Under[i];
//						
//						POWER.QE_Under[i] = 0.0;
//						
//						//10G 넘으면 0
//					    if(float_temp1 > 10000000000)
//					    float_temp1 = 0.0;
//					    
//					    FRAM_Access(&float_temp1, (QEa + i), 2);
//					}
//				}
//				
//				// 범위내이면
//				else
//				{
//					PQE_buffer[i] += float_temp1;
//					
//					//10G 넘으면 0
//					if(PQE_buffer[i] > 10000000000)
//					PQE_buffer[i] = 0;
//					
//					FRAM_Access(&PQE_buffer[i], (QEa + i), 2);
//				}				
//			}			
//		}		
//	}
//	
//	// 3QE
//	//float_temp = PQE_buffer[0] + PQE_buffer[1] + PQE_buffer[2];	
//	
//	if(float_temp > 0)
//	{
//		FRAM_Access(QE3p, &float_temp1, 2);
//		
//		float_temp += float_temp1;
//		
//		// 10G 넘으면 0
//		if(float_temp > 10000000000)
//		float_temp = 0;
//		
//		FRAM_Access(&float_temp, QE3p, 2);
//	}
//	
//	
//	
	
	
	
	
	
	
	DISPLAY.Power_Up = 0;
}

void Cal_RHour(void)
{
	if(RUNNING.op_count < 1000)	return;
	RUNNING.op_count = 0; //1초 마다 리셋
        
	RUNNING.RunningHourCNT++; //1초 카운터 저장

	if(RUNNING.RunningHourCNT >= 0x15752A00)	{RUNNING.RunningHourCNT = 0;}	//100000시간 마다 RESET! (3,600*100,000=360,000,000)

	//MRAM 저장
	*(MRAM_RUNNING_HOUR1) = (RUNNING.RunningHourCNT >> 16) & 0xffff;
	*(MRAM_RUNNING_HOUR2) = RUNNING.RunningHourCNT & 0xffff;
}

// 사용하는 인터럽트 설정
// 이 함수는 H/W 변동이 없는 한 수정 없음
void interrupt_control(void)
{
	EALLOW;
	
	// *SysCtrlRegs_PCLKCR3 = 0x3300;
	
	// interrupt vector regist
	
	// sync가 갈수록 틀어져서 문제 발생
	// 1.389ms timer
	//PieVectTable.TINT0 = &TINT0_ISR;
	
	// AD 외부 인터럽트
	PieVectTable.XINT3 = &XINT3_ISR;
	
	// 1ms timer
	PieVectTable.XINT13 = &TINT1_ISR;
	
	// 20us timer
	//PieVectTable.TINT2 = &TINT2_ISR;
	
	// 전면 serial 수신
	PieVectTable.SCIRXINTB = &SCIRXB_ISR;
	
	// 전면 serial 송신
	PieVectTable.SCITXINTB = &SCITXINTB_ISR;
	
	// HIMIX serial 송신
	PieVectTable.SCITXINTC = &SCITXINTC_ISR;
	
	// interrupt enable bit
	// timer 0
	//IER |= M_INT1;  
	// timer 1
	IER |= M_INT13;  
	
	// timer2
	//IER |= M_INT14;
	
	// Enable CPU INT9 for SCI-B
	IER |= M_INT9;
	
	// Enable CPU INT8 for SCI-C
	IER |= M_INT8;
	
	// ad 외부인터럽트
	IER |= M_INT12;
	
	// timer0
	//*PieCtrlRegs_PIEIER1 |= 0x0040;
	
	// Enable SCI-B RX, TX INT in the PIE: Group 9
	*PieCtrlRegs_PIEIER9 |= 0x0004;
	*PieCtrlRegs_PIEIER9 |= 0x0008;
	// PieCtrlRegs.PIEIER9.bit.INTx3 = 1;

	// Enable SCI-C TX INT in the PIE: Group 8
	*PieCtrlRegs_PIEIER8 |= 0x0020;
	
	// sci-c tx interrupt 활성
	// *ScicRegs_SCICTL2 |= 0x0001;
	
	// ad 외부인터럽트
	*PieCtrlRegs_PIEIER12 |= 0x0001;
	
	EDIS;

	ERTM;
}

void modbus_comm_card_check(void)
{
	static unsigned char a = 0;
	static unsigned char cnt = 0;
	static unsigned int nic_reset_start = 0;
	unsigned char tmp;

	if(nic_reset_start == 1) {
		nic_reset_start = 2;
		NIC_RESET_ON;
	} else if(nic_reset_start >= 2) {
		nic_reset_start = 0;
		NIC_RESET_OFF;
	}

	if(cnt++ > 5) {
		cnt = 0;

		tmp = *COMM_2_MODBUS_RECV_CNT & 0xff;

		if(a == tmp) {
			nic_reset_start = 1;
			NIC_RESET_OFF;//통신카드 리셋 (Active Low 펄스신호 발생)
		}

		a = tmp;
	}
}

void SCI_Port_Err_Check(void)
{
	if((*ScibRegs_SCIRXST & 0x80)||(*ScicRegs_SCIRXST & 0x80)) {
		sci_setup();
	}
}
