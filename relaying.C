#include "define.h"
#include "extern_variable.h"
#include "extern_prototype.h"

void relay_normal_to_detect(unsigned long *ar_op_count, unsigned int *ar_op_status)
{
//	*ar_op_count = 0;
//	*ar_op_status = RELAY_DETECT;
}

void relay_detect_to_pickup(unsigned long *ar_op_count, unsigned int *ar_op_status, unsigned long ar_event_mask, unsigned int ar_relay_status)
{
//	*ar_op_count = 0;
//	EVENT.pickup |= ar_event_mask;
//	event_direct_save(&EVENT.pickup);
//	// op 대기
//	*ar_op_status = RELAY_PICKUP;
//	*(ar_op_status + 1) = DROPOUT_NORMAL;
//	RELAY_STATUS.pickup |= ar_relay_status;
}

void relay_pickup_to_operation(unsigned int *ar_do_out, unsigned int ar_relay_bit, float ar_ratio, unsigned long ar_event_mask, unsigned int *ar_op_status)
{
	// do 제어
	//86을 일단 지운다
//	SYSTEM.do_control &= 0xff9f;
//	SYSTEM.do_control |= *ar_do_out;
	
	//backup
	*(ar_do_out + 1) = *ar_do_out;
					
	WAVE.relay |= ar_relay_bit;
	// wave capture 시작 추가
	if((WAVE.post_start != 0x1234) && (WAVE.hold == 0))	WAVE.post_start = 0x1234;
						
//ar_ratio *= 100;
//EVENT.ratio = (unsigned int)ar_ratio;
//EVENT.operation |= ar_event_mask;
//event_direct_save(&EVENT.operation);
//*ar_op_status = RELAY_TRIP;
//EVENT.optime += DO_CONTACT_TIME;
	TIMER.cb_open = 0;
//RELAY_STATUS.pickup &= ~ar_relay_bit;
//RELAY_STATUS.operation_sum_holding |= ar_relay_bit;
	
	// under 요소들 최초 동작 후 ack 누른후 popup이 뜨지 않아야 disable 가능하기 때문에
	// popup을 띄우지 않는다
//	if((RELAY_STATUS.popup_mask == 0) && (RELAY_STATUS.operation_realtime == 0))
//	{		
//		RELAY_STATUS.popup_property = ar_event_mask;
//		RELAY_STATUS.popup_ratio = ar_ratio;
//		RELAY_STATUS.popup_optime = EVENT.optime;
//		
//		
//		RELAY_STATUS.popup_mask = 0x1234;
//		
//		//SYSTEM.return_position = SYSTEM.position;
//		
//		SYSTEM.position = 0x00000040;
//		
//		LCD.refresh_status = 0;
//	}
//	RELAY_STATUS.operation_realtime |= ar_relay_bit;
}

void relay_opeartion_to_dropout(unsigned int *ar_dropout_count, unsigned int *ar_drop_status)
{
	*ar_dropout_count = 0;
	*ar_drop_status = DROPOUT_DETECT;
}

void do_release(unsigned int *ar_do_out_off)
{
	unsigned int temp16;
	unsigned int temp162 = 0;
	unsigned int i;
	
	*ar_do_out_off = 0;
	
//2015.02.24
//	if(CORE.model == MS)
//	{
//		temp162 = UCR.do_output_off | NSOCR.do_output_off | THR.do_output_off;
//		temp162 |= H50.do_output_off | SL.do_output_off | NCH.do_output_off;
//	}
//	else
//	{
//2015.02.24 END
		temp162 = OCR50_2.do_output_off | OCR51_1.do_output_off | OCR51_2.do_output_off | OVR.do_output_off | OVGR.do_output_off;
		temp162 |= UVR_2.do_output_off | UVR_3.do_output_off | P47.do_output_off | N47.do_output_off;
//	} //2015.02.24
	
//temp162 |= OCR50_1.do_output_off | OCGR50.do_output_off | OCGR51.do_output_off | DGR.do_output_off | SGR.do_output_off | UVR_1.do_output_off;
	temp162 |= OCR50_1.do_output_off | OCGR50.do_output_off | OCGR51.do_output_off | UVR_1.do_output_off;
	
	if(SUPERVISION.mode == 0xaaaa)
	temp162 |= SUPERVISION.do_output_off[0] | SUPERVISION.do_output_off[1];
	
	// 현재 값
	temp16 = SYSTEM.do_control;
	
	for(i = 0; i < 7; i++)
	{
		// 해당 do set이면
		if(temp16 & DO_ON_BIT[i])
		{
			// 접점해제인데
			if((temp162 & DO_ON_BIT[i]) == 0)
			{
				// pulse면
				if((DIGITAL_OUTPUT.property & DO_ON_BIT[i]) == 0)
				temp16 &= DO_OFF_BIT[i];
			}
		}
	}
	
	// do 제어
	SYSTEM.do_control = temp16;
}

void relay_dropout_to_normal(unsigned long *ar_event_ready, unsigned int *ar_op_status, unsigned int ar_relay_bit, unsigned int *ar_do_out_off)
{
//	do_release(ar_do_out_off);
//					
//	*ar_op_status = RELAY_NORMAL;
//	*(ar_op_status + 1) = DROPOUT_NORMAL;
					
	WAVE.relay &= ~ar_relay_bit;
	
	// pickup 후 바로 빠졌을 때를 위해
//	RELAY_STATUS.pickup &= ~ar_relay_bit;
//	RELAY_STATUS.operation_realtime &= ~ar_relay_bit;
}

float over_phase_value_check(unsigned int ar_phase, float ar_compare, unsigned long *ar_event)
{
	unsigned int phase_temp = 0;
	float compare_value = 0;
	
	if(MEASUREMENT.rms_value[ar_phase    ] >= ar_compare)
	{
		phase_temp = 0x0001;
		
		compare_value = MEASUREMENT.rms_value[ar_phase];
	}
					
	if(MEASUREMENT.rms_value[ar_phase + 1] >= ar_compare)
	{
		phase_temp |= 0x0002;
		
		if(MEASUREMENT.rms_value[ar_phase + 1] > compare_value)
		compare_value = MEASUREMENT.rms_value[ar_phase + 1];
	}
					
	if(MEASUREMENT.rms_value[ar_phase + 2] >= ar_compare)
	{
		phase_temp |= 0x0004;
		
		if(MEASUREMENT.rms_value[ar_phase + 2] > compare_value)
		compare_value = MEASUREMENT.rms_value[ar_phase + 2];
	}
					
	*ar_event &= 0xffffff00;    
	*ar_event |= (unsigned long)phase_temp;
	                            
	compare_value /= ar_compare;
	
	return(compare_value);
}

float under_phase_value_check(unsigned int ar_phase, float ar_compare, unsigned long *ar_event)
{
	unsigned int phase_temp;
	float compare_value;
	
	if(MEASUREMENT.rms_value[ar_phase    ] <= ar_compare)
	{
		phase_temp = 0x0001;
		
		compare_value = MEASUREMENT.rms_value[ar_phase];
	}
					
	if(MEASUREMENT.rms_value[ar_phase + 1] <= ar_compare)
	{
		phase_temp |= 0x0002;
		
		if(MEASUREMENT.rms_value[ar_phase + 1] < compare_value)
		compare_value = MEASUREMENT.rms_value[ar_phase + 1];
	}
					
	if(MEASUREMENT.rms_value[ar_phase + 2] <= ar_compare)
	{
		phase_temp |= 0x0004;
		
		if(MEASUREMENT.rms_value[ar_phase + 2] < compare_value)
		compare_value = MEASUREMENT.rms_value[ar_phase + 2];
	}
					
	*ar_event &= 0xffffff00;
	*ar_event |= (unsigned long)phase_temp;
	
	compare_value /= ar_compare;
	
	return(compare_value);
}

void OCR50_1_RELAY(void)
{
	if(OCR50_1.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > OCR50_1.Pickup_Threshold)
		{
			if(OCR50_1.op_status == RELAY_NORMAL)
			{
				OCR50_1.op_status = RELAY_DETECT;
				OCR50_1.op_count = 0;
			}
			else if(OCR50_1.op_status == RELAY_DETECT)
			{
				if(OCR50_1.op_count > OCR50_1.pickup_limit)
				{	
					OCR50_1.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_OCR50_1;  //alarm ON
					OCR50_1.Pickup_Time = OCR50_1.op_count;
					OCR50_1.op_count = 0;
				}
			}
			else if(OCR50_1.op_status == RELAY_PICKUP)
			{
				if(OCR50_1.op_count > OCR50_1.delay_ms)
				{
					Relay_On(OCR50_1.do_output);

					OCR50_1.op_status	= RELAY_TRIP;
					OCR50_1.Op_Ratio	= PROTECT.Max_RMS / OCR50_1.Pickup_Threshold; //배수
					OCR50_1.Op_Phase	= PROTECT.Op_Phase; //상
					OCR50_1.Delay_Time = OCR50_1.op_count;
					OCR50_1.Op_Time		= OCR50_1.Delay_Time + OCR50_1.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= OCR50_1.do_output; //DO 출력
//				OCR50_1.do_output_off = OCR50_1.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_OCR50_1; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_OCR50_1;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_OCR50_1;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < OCR50_1.Dropout_Threshold)  //under 99%
			{
				if((OCR50_1.op_status == RELAY_DETECT) || (OCR50_1.op_status == RELAY_PICKUP))
				{
					OCR50_1.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_OCR50_1; //계전요소 alarm OFF
				}
				else if(OCR50_1.op_status == RELAY_TRIP)
				{
					Relay_Off(OCR50_1.do_output); //DO open
					OCR50_1.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_OCR50_1; //동작 상태 변수 해제
				}
			}
		}
	}
}

void OCR50_2_RELAY(void)
{
	if(OCR50_2.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > OCR50_2.Pickup_Threshold)
		{
			if(OCR50_2.op_status == RELAY_NORMAL)
			{
				OCR50_2.op_status = RELAY_DETECT;
				OCR50_2.op_count = 0;
			}
			else if(OCR50_2.op_status == RELAY_DETECT)
			{
				if(OCR50_2.op_count > OCR50_2.pickup_limit)
				{	
					OCR50_2.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_OCR50_2;  //alarm ON
					OCR50_2.Pickup_Time = OCR50_2.op_count;
					OCR50_2.op_count = 0;
				}
			}
			else if(OCR50_2.op_status == RELAY_PICKUP)
			{
				if(OCR50_2.op_count > OCR50_2.delay_ms)
				{
					Relay_On(OCR50_2.do_output);

					OCR50_2.op_status	= RELAY_TRIP;
					OCR50_2.Op_Ratio	= PROTECT.Max_RMS / OCR50_2.Pickup_Threshold; //배수
					OCR50_2.Op_Phase	= PROTECT.Op_Phase; //상
					OCR50_2.Delay_Time = OCR50_2.op_count;
					OCR50_2.Op_Time		= OCR50_2.Delay_Time + OCR50_2.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= OCR50_2.do_output;
//				OCR50_2.do_output_off = OCR50_2.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_OCR50_2; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_OCR50_2;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_OCR50_2;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < OCR50_2.Dropout_Threshold)  //under 99%
			{
				if((OCR50_2.op_status == RELAY_DETECT) || (OCR50_2.op_status == RELAY_PICKUP))
				{
					OCR50_2.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_OCR50_2; //계전요소 alarm OFF
				}
				else if(OCR50_2.op_status == RELAY_TRIP)
				{
					Relay_Off(OCR50_2.do_output); //DO open, test 용
					OCR50_2.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_OCR50_2; //동작 상태 변수 해제
				}
			}
		}
	}
}

void OCR51_1_RELAY(void)
{
	if(OCR51_1.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > OCR51_1.Pickup_Threshold)
		{
			if(OCR51_1.op_status == RELAY_NORMAL)
			{
				OCR51_1.op_status = RELAY_DETECT;
				OCR51_1.op_count = 0;
			}
			else if(OCR51_1.op_status == RELAY_DETECT)
			{
				if(OCR51_1.op_count > OCR51_1.pickup_limit)
				{	
					OCR51_1.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_OCR51_1;  //alarm ON
					OCR51_1.Pickup_Time = OCR51_1.op_count;
					OCR51_1.op_count = 0;
				}
			}
			else if(OCR51_1.op_status == RELAY_PICKUP)
			{
				OCR51_1.Op_Ratio	= PROTECT.Max_RMS / OCR51_1.Pickup_Threshold;

				OCR51_1.Op_Time_set = Inverse_GetDelayTime(OCR51_1.mode, OCR51_1.time_lever, OCR51_1.Op_Ratio);
				OCR51_1.Op_Time_set -= (INVERSE_PICKUP_LIMIT+TOTAL_DELAY_51);
				if(OCR51_1.op_count > OCR51_1.Op_Time_set)
				{
					Relay_On(OCR51_1.do_output);

					OCR51_1.op_status	= RELAY_TRIP;
					OCR51_1.Op_Ratio	= PROTECT.Max_RMS / OCR51_1.Pickup_Threshold; //배수
					OCR51_1.Op_Phase	= PROTECT.Op_Phase; //상
					OCR51_1.Delay_Time = OCR51_1.op_count;
					OCR51_1.Op_Time		= OCR51_1.Delay_Time + OCR51_1.Pickup_Time + TOTAL_DELAY_51; //동작 시간

//				SYSTEM.do_control |= OCR51_1.do_output;
//				OCR51_1.do_output_off = OCR51_1.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_OCR51_1; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_OCR51_1;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_OCR51_1;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < OCR51_1.Dropout_Threshold)  //under 99%
			{
				if((OCR51_1.op_status == RELAY_DETECT) || (OCR51_1.op_status == RELAY_PICKUP))
				{
					OCR51_1.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_OCR51_1; //계전요소 alarm OFF
				}
				else if(OCR51_1.op_status == RELAY_TRIP)
				{
					Relay_Off(OCR51_1.do_output); //DO open
					OCR51_1.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_OCR51_1; //동작 상태 변수 해제
				}
			}
		}
	}
}

void OCR51_2_RELAY(void)
{
	if(OCR51_2.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > OCR51_2.Pickup_Threshold)
		{
			if(OCR51_2.op_status == RELAY_NORMAL)
			{
				OCR51_2.op_status = RELAY_DETECT;
				OCR51_2.op_count = 0;
			}
			else if(OCR51_2.op_status == RELAY_DETECT)
			{
				if(OCR51_2.op_count > OCR51_2.pickup_limit)
				{	
					OCR51_2.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_OCR51_2;  //alarm ON
					OCR51_2.Pickup_Time = OCR51_2.op_count;
					OCR51_2.op_count = 0;
				}
			}
			else if(OCR51_2.op_status == RELAY_PICKUP)
			{
				OCR51_2.Op_Ratio	= PROTECT.Max_RMS / OCR51_2.Pickup_Threshold;

				OCR51_2.Op_Time_set = Inverse_GetDelayTime(OCR51_2.mode, OCR51_2.time_lever, OCR51_2.Op_Ratio);
				OCR51_2.Op_Time_set -= (INVERSE_PICKUP_LIMIT+TOTAL_DELAY_51);
				if(OCR51_2.op_count > OCR51_2.Op_Time_set)
				{
					Relay_On(OCR51_2.do_output);

					OCR51_2.op_status	= RELAY_TRIP;
					OCR51_2.Op_Ratio	= PROTECT.Max_RMS / OCR51_2.Pickup_Threshold; //배수
					OCR51_2.Op_Phase	= PROTECT.Op_Phase; //상
					OCR51_2.Delay_Time = OCR51_2.op_count;
					OCR51_2.Op_Time		= OCR51_2.Delay_Time + OCR51_2.Pickup_Time + TOTAL_DELAY_51; //동작 시간

//				SYSTEM.do_control |= OCR51_2.do_output;
//				OCR51_2.do_output_off = OCR51_2.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_OCR51_2; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_OCR51_2;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_OCR51_2;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < OCR51_2.Dropout_Threshold)  //under 99%
			{
				if((OCR51_2.op_status == RELAY_DETECT) || (OCR51_2.op_status == RELAY_PICKUP))
				{
					OCR51_2.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_OCR51_2; //계전요소 alarm OFF
				}
				else if(OCR51_2.op_status == RELAY_TRIP)
				{
					Relay_Off(OCR51_2.do_output); //DO open
					OCR51_2.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_OCR51_2; //동작 상태 변수 해제
				}
			}
		}
	}
}

void OCGR50_RELAY(void)
{
	if(OCGR50.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > OCGR50.Pickup_Threshold)
		{
			if(OCGR50.op_status == RELAY_NORMAL)
			{
				OCGR50.op_status = RELAY_DETECT;
				OCGR50.op_count = 0;
			}
			else if(OCGR50.op_status == RELAY_DETECT)
			{
				if(OCGR50.op_count > OCGR50.pickup_limit)
				{	
					OCGR50.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_OCGR50;  //alarm ON
					OCGR50.Pickup_Time = OCGR50.op_count;
					OCGR50.op_count = 0;
				}
			}
			else if(OCGR50.op_status == RELAY_PICKUP)
			{
				if(OCGR50.op_count > OCGR50.delay_ms)
				{
					Relay_On(OCGR50.do_output);

					OCGR50.op_status	= RELAY_TRIP;
					OCGR50.Op_Ratio	= PROTECT.Max_RMS / OCGR50.Pickup_Threshold; //배수
					OCGR50.Op_Phase	= PROTECT.Op_Phase; //상
					OCGR50.Delay_Time = OCGR50.op_count;
					OCGR50.Op_Time		= OCGR50.Delay_Time + OCGR50.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= OCGR50.do_output; //DO 출력
//				OCGR50.do_output_off = OCGR50.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_OCGR50; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_OCGR50;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_OCGR50;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < OCGR50.Dropout_Threshold)  //under 99%
			{
				if((OCGR50.op_status == RELAY_DETECT) || (OCGR50.op_status == RELAY_PICKUP))
				{
					OCGR50.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_OCGR50; //계전요소 alarm OFF
				}
				else if(OCGR50.op_status == RELAY_TRIP)
				{
					Relay_Off(OCGR50.do_output); //DO open
					OCGR50.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_OCGR50; //동작 상태 변수 해제
				}
			}
		}
	}
}

void OCGR51_RELAY(void)
{
	if(OCGR51.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > OCGR51.Pickup_Threshold)
		{
			if(OCGR51.op_status == RELAY_NORMAL)
			{
				OCGR51.op_status = RELAY_DETECT;
				OCGR51.op_count = 0;
			}
			else if(OCGR51.op_status == RELAY_DETECT)
			{
				if(OCGR51.op_count > OCGR51.pickup_limit)
				{	
					OCGR51.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_OCGR51;  //alarm ON
					OCGR51.Pickup_Time = OCGR51.op_count;
					OCGR51.op_count = 0;
				}
			}
			else if(OCGR51.op_status == RELAY_PICKUP)
			{
				OCGR51.Op_Ratio	= PROTECT.Max_RMS / OCGR51.Pickup_Threshold;

				OCGR51.Op_Time_set = Inverse_GetDelayTime(OCGR51.mode, OCGR51.time_lever, OCGR51.Op_Ratio);
				OCGR51.Op_Time_set -= (INVERSE_PICKUP_LIMIT+TOTAL_DELAY_51);
				if(OCGR51.op_count > OCGR51.Op_Time_set)
				{
					Relay_On(OCGR51.do_output);

					OCGR51.op_status	= RELAY_TRIP;
					OCGR51.Op_Ratio	= PROTECT.Max_RMS / OCGR51.Pickup_Threshold; //배수
					OCGR51.Op_Phase	= PROTECT.Op_Phase; //상
					OCGR51.Delay_Time = OCGR51.op_count;
					OCGR51.Op_Time		= OCGR51.Delay_Time + OCGR51.Pickup_Time + TOTAL_DELAY_51; //동작 시간

//				SYSTEM.do_control |= OCGR51.do_output;
//				OCGR51.do_output_off = OCGR51.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_OCGR51; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_OCGR51;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_OCGR51;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < OCGR51.Dropout_Threshold)  //under 99%
			{
				if((OCGR51.op_status == RELAY_DETECT) || (OCGR51.op_status == RELAY_PICKUP))
				{
					OCGR51.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_OCGR51; //계전요소 alarm OFF
				}
				else if(OCGR51.op_status == RELAY_TRIP)
				{
					Relay_Off(OCGR51.do_output); //DO open
					OCGR51.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_OCGR51; //동작 상태 변수 해제
				}
			}
		}
	}
}

void UVR_1_RELAY(void)
{
	if(UVR_1.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > UVR_1.Pickup_Threshold)
		{
			if(UVR_1.op_status == RELAY_NORMAL)
			{
				UVR_1.op_status = RELAY_DETECT;
				UVR_1.op_count = 0;
			}
			else if(UVR_1.op_status == RELAY_DETECT)
			{
				if(UVR_1.op_count > UVR_1.pickup_limit)
				{	
					UVR_1.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_UVR_1;  //alarm ON
					UVR_1.Pickup_Time = UVR_1.op_count;
					UVR_1.op_count = 0;
				}
			}
			else if(UVR_1.op_status == RELAY_PICKUP)
			{
				if(UVR_1.op_count > UVR_1.delay_ms)
				{
					Relay_On(UVR_1.do_output);

					UVR_1.op_status	= RELAY_TRIP;
					UVR_1.Op_Ratio	= PROTECT.Max_RMS / UVR_1.Pickup_Threshold; //배수
					UVR_1.Op_Phase	= PROTECT.Op_Phase; //상
					UVR_1.Delay_Time = UVR_1.op_count;
					UVR_1.Op_Time		= UVR_1.Delay_Time + UVR_1.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= UVR_1.do_output; //DO 출력
//				UVR_1.do_output_off = UVR_1.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_UVR_1; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_UVR_1;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_UVR_1;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < UVR_1.Dropout_Threshold)  //under 99%
			{
				if((UVR_1.op_status == RELAY_DETECT) || (UVR_1.op_status == RELAY_PICKUP))
				{
					UVR_1.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_UVR_1; //계전요소 alarm OFF
				}
				else if(UVR_1.op_status == RELAY_TRIP)
				{
					Relay_Off(UVR_1.do_output); //DO open
					UVR_1.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_UVR_1; //동작 상태 변수 해제
				}
			}
		}
	}
}

void UVR_2_RELAY(void)
{
	if(UVR_2.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > UVR_2.Pickup_Threshold)
		{
			if(UVR_2.op_status == RELAY_NORMAL)
			{
				UVR_2.op_status = RELAY_DETECT;
				UVR_2.op_count = 0;
			}
			else if(UVR_2.op_status == RELAY_DETECT)
			{
				if(UVR_2.op_count > UVR_2.pickup_limit)
				{	
					UVR_2.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_UVR_2;  //alarm ON
					UVR_2.Pickup_Time = UVR_2.op_count;
					UVR_2.op_count = 0;
				}
			}
			else if(UVR_2.op_status == RELAY_PICKUP)
			{
				if(UVR_2.op_count > UVR_2.delay_ms)
				{
					Relay_On(UVR_2.do_output);

					UVR_2.op_status	= RELAY_TRIP;
					UVR_2.Op_Ratio	= PROTECT.Max_RMS / UVR_2.Pickup_Threshold; //배수
					UVR_2.Op_Phase	= PROTECT.Op_Phase; //상
					UVR_2.Delay_Time = UVR_2.op_count;
					UVR_2.Op_Time		= UVR_2.Delay_Time + UVR_2.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= UVR_2.do_output; //DO 출력
//				UVR_2.do_output_off = UVR_2.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_UVR_2; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_UVR_2;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_UVR_2;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < UVR_2.Dropout_Threshold)  //under 99%
			{
				if((UVR_2.op_status == RELAY_DETECT) || (UVR_2.op_status == RELAY_PICKUP))
				{
					UVR_2.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_UVR_2; //계전요소 alarm OFF
				}
				else if(UVR_2.op_status == RELAY_TRIP)
				{
					Relay_Off(UVR_2.do_output); //DO open
					UVR_2.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_UVR_2; //동작 상태 변수 해제
				}
			}
		}
	}
}

void UVR_3_RELAY(void)
{
	if(UVR_3.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > UVR_3.Pickup_Threshold)
		{
			if(UVR_3.op_status == RELAY_NORMAL)
			{
				UVR_3.op_status = RELAY_DETECT;
				UVR_3.op_count = 0;
			}
			else if(UVR_3.op_status == RELAY_DETECT)
			{
				if(UVR_3.op_count > UVR_3.pickup_limit)
				{	
					UVR_3.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_UVR_3;  //alarm ON
					UVR_3.Pickup_Time = UVR_3.op_count;
					UVR_3.op_count = 0;
				}
			}
			else if(UVR_3.op_status == RELAY_PICKUP)
			{
				if(UVR_3.op_count > UVR_3.delay_ms)
				{
					Relay_On(UVR_3.do_output);

					UVR_3.op_status	= RELAY_TRIP;
					UVR_3.Op_Ratio	= PROTECT.Max_RMS / UVR_3.Pickup_Threshold; //배수
					UVR_3.Op_Phase	= PROTECT.Op_Phase; //상
					UVR_3.Delay_Time = UVR_3.op_count;
					UVR_3.Op_Time		= UVR_3.Delay_Time + UVR_3.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= UVR_3.do_output; //DO 출력
//				UVR_3.do_output_off = UVR_3.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_UVR_3; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_UVR_3;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_UVR_3;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < UVR_3.Dropout_Threshold)  //under 99%
			{
				if((UVR_3.op_status == RELAY_DETECT) || (UVR_3.op_status == RELAY_PICKUP))
				{
					UVR_3.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_UVR_3; //계전요소 alarm OFF
				}
				else if(UVR_3.op_status == RELAY_TRIP)
				{
					Relay_Off(UVR_3.do_output); //DO open
					UVR_3.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_UVR_3; //동작 상태 변수 해제
				}
			}
		}
	}
}

void P47_RELAY(void)
{
	if(P47.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > P47.Pickup_Threshold)
		{
			if(P47.op_status == RELAY_NORMAL)
			{
				P47.op_status = RELAY_DETECT;
				P47.op_count = 0;
			}
			else if(P47.op_status == RELAY_DETECT)
			{
				if(P47.op_count > P47.pickup_limit)
				{	
					P47.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_P47;  //alarm ON
					P47.Pickup_Time = P47.op_count;
					P47.op_count = 0;
				}
			}
			else if(P47.op_status == RELAY_PICKUP)
			{
				if(P47.op_count > P47.delay_ms)
				{
					Relay_On(P47.do_output);

					P47.op_status	= RELAY_TRIP;
					P47.Op_Ratio	= PROTECT.Max_RMS / P47.Pickup_Threshold; //배수
					P47.Op_Phase	= PROTECT.Op_Phase; //상
					P47.Delay_Time = P47.op_count;
					P47.Op_Time		= P47.Delay_Time + P47.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= P47.do_output; //DO 출력
//				P47.do_output_off = P47.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_P47; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_P47;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_P47;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < P47.Dropout_Threshold)  //under 99%
			{
				if((P47.op_status == RELAY_DETECT) || (P47.op_status == RELAY_PICKUP))
				{
					P47.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_P47; //계전요소 alarm OFF
				}
				else if(P47.op_status == RELAY_TRIP)
				{
					Relay_Off(P47.do_output); //DO open
					P47.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_P47; //동작 상태 변수 해제
				}
			}
		}
	}
}

void N47_RELAY(void)
{
	if(N47.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > N47.Pickup_Threshold)
		{
			if(N47.op_status == RELAY_NORMAL)
			{
				N47.op_status = RELAY_DETECT;
				N47.op_count = 0;
			}
			else if(N47.op_status == RELAY_DETECT)
			{
				if(N47.op_count > N47.pickup_limit)
				{	
					N47.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_N47;  //alarm ON
					N47.Pickup_Time = N47.op_count;
					N47.op_count = 0;
				}
			}
			else if(N47.op_status == RELAY_PICKUP)
			{
				if(N47.op_count > N47.delay_ms)
				{
					Relay_On(N47.do_output);

					N47.op_status	= RELAY_TRIP;
					N47.Op_Ratio	= PROTECT.Max_RMS / N47.Pickup_Threshold; //배수
					N47.Op_Phase	= PROTECT.Op_Phase; //상
					N47.Delay_Time = N47.op_count;
					N47.Op_Time		= N47.Delay_Time + N47.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= N47.do_output; //DO 출력
//				N47.do_output_off = N47.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_N47; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_N47;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_N47;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < N47.Dropout_Threshold)  //under 99%
			{
				if((N47.op_status == RELAY_DETECT) || (N47.op_status == RELAY_PICKUP))
				{
					N47.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_N47; //계전요소 alarm OFF
				}
				else if(N47.op_status == RELAY_TRIP)
				{
					Relay_Off(N47.do_output); //DO open
					N47.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_N47; //동작 상태 변수 해제
				}
			}
		}
	}
}

void OVR_RELAY(void)
{
	if(OVR.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > OVR.Pickup_Threshold)
		{
			if(OVR.op_status == RELAY_NORMAL)
			{
				OVR.op_status = RELAY_DETECT;
				OVR.op_count = 0;
			}
			else if(OVR.op_status == RELAY_DETECT)
			{
				if(OVR.op_count > OVR.pickup_limit)
				{	
					OVR.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_OVR;  //alarm ON
					OVR.Pickup_Time = OVR.op_count;
					OVR.op_count = 0;
				}
			}
			else if(OVR.op_status == RELAY_PICKUP)
			{
				if(OVR.op_count > OVR.delay_ms)
				{
					Relay_On(OVR.do_output);

					OVR.op_status	= RELAY_TRIP;
					OVR.Op_Ratio	= PROTECT.Max_RMS / OVR.Pickup_Threshold; //배수
					OVR.Op_Phase	= PROTECT.Op_Phase; //상
					OVR.Delay_Time = OVR.op_count;
					OVR.Op_Time		= OVR.Delay_Time + OVR.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= OVR.do_output; //DO 출력
//				OVR.do_output_off = OVR.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_OVR; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_OVR;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_OVR;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < OVR.Dropout_Threshold)  //under 99%
			{
				if((OVR.op_status == RELAY_DETECT) || (OVR.op_status == RELAY_PICKUP))
				{
					OVR.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_OVR; //계전요소 alarm OFF
				}
				else if(OVR.op_status == RELAY_TRIP)
				{
					Relay_Off(OVR.do_output); //DO open
					OVR.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_OVR; //동작 상태 변수 해제
				}
			}
		}
	}
}

void OVGR_RELAY(void)
{
	if(OVGR.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > OVGR.Pickup_Threshold)
		{
			if(OVGR.op_status == RELAY_NORMAL)
			{
				OVGR.op_status = RELAY_DETECT;
				OVGR.op_count = 0;
			}
			else if(OVGR.op_status == RELAY_DETECT)
			{
				if(OVGR.op_count > OVGR.pickup_limit)
				{	
					OVGR.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_OVGR;  //alarm ON
					OVGR.Pickup_Time = OVGR.op_count;
					OVGR.op_count = 0;
				}
			}
			else if(OVGR.op_status == RELAY_PICKUP)
			{
				if(OVGR.op_count > OVGR.delay_ms)
				{
					Relay_On(OVGR.do_output);

					OVGR.op_status	= RELAY_TRIP;
					OVGR.Op_Ratio	= PROTECT.Max_RMS / OVGR.Pickup_Threshold; //배수
					OVGR.Op_Phase	= PROTECT.Op_Phase; //상
					OVGR.Delay_Time = OVGR.op_count;
					OVGR.Op_Time		= OVGR.Delay_Time + OVGR.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= OVGR.do_output; //DO 출력
//				OVGR.do_output_off = OVGR.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_OVGR; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_OVGR;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_OVGR;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < OVGR.Dropout_Threshold)  //under 99%
			{
				if((OVGR.op_status == RELAY_DETECT) || (OVGR.op_status == RELAY_PICKUP))
				{
					OVGR.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_OVGR; //계전요소 alarm OFF
				}
				else if(OVGR.op_status == RELAY_TRIP)
				{
					Relay_Off(OVGR.do_output); //DO open
					OVGR.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_OVGR; //동작 상태 변수 해제
				}
			}
		}
	}
}

void DGR_RELAY(void)
{
	if(DGR.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > DGR.Pickup_Threshold_Vo)
		{
			if(DGR.op_status == RELAY_NORMAL)
			{
				DGR.op_status = RELAY_DETECT;
				DGR.op_count = 0;
			}
			else if(DGR.op_status == RELAY_DETECT)
			{
				if(DGR.op_count > DGR.pickup_limit)
				{	
					DGR.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_DGR;  //alarm ON
					DGR.Pickup_Time = DGR.op_count;
					DGR.op_count = 0;
				}
			}
			else if(DGR.op_status == RELAY_PICKUP)
			{
				if(DGR.op_count > DGR.delay_ms)
				{
					Relay_On(DGR.do_output);

					DGR.op_status	= RELAY_TRIP;
					DGR.Op_Ratio	= PROTECT.Max_RMS / DGR.Pickup_Threshold_Vo; //배수
					DGR.Op_Phase	= PROTECT.Op_Phase; //상
					DGR.Delay_Time = DGR.op_count;
					DGR.Op_Time		= DGR.Delay_Time + DGR.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= DGR.do_output; //DO 출력
//				DGR.do_output_off = DGR.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_DGR; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_DGR;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_DGR;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < DGR.Dropout_Threshold_Vo)  //under 99%
			{
				if((DGR.op_status == RELAY_DETECT) || (DGR.op_status == RELAY_PICKUP))
				{
					DGR.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_DGR; //계전요소 alarm OFF
				}
				else if(DGR.op_status == RELAY_TRIP)
				{
					Relay_Off(DGR.do_output); //DO open
					DGR.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_DGR; //동작 상태 변수 해제
				}
			}
		}
	}
}

void SGR_RELAY(void)
{
	if(SGR.use == 0xaaaa)
	{
		// 선택 di on일 때 유효
//	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
		if(PROTECT.Max_RMS > SGR.Pickup_Threshold_Vo)
		{
			if(SGR.op_status == RELAY_NORMAL)
			{
				SGR.op_status = RELAY_DETECT;
				SGR.op_count = 0;
			}
			else if(SGR.op_status == RELAY_DETECT)
			{
				if(SGR.op_count > SGR.pickup_limit)
				{	
					SGR.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_SGR;  //alarm ON
					SGR.Pickup_Time = SGR.op_count;
					SGR.op_count = 0;
				}
			}
			else if(SGR.op_status == RELAY_PICKUP)
			{
				if(SGR.op_count > SGR.delay_ms)
				{
					Relay_On(SGR.do_output);

					SGR.op_status	= RELAY_TRIP;
					SGR.Op_Ratio	= PROTECT.Max_RMS / SGR.Pickup_Threshold_Vo; //배수
					SGR.Op_Phase	= PROTECT.Op_Phase; //상
					SGR.Delay_Time = SGR.op_count;
					SGR.Op_Time		= SGR.Delay_Time + SGR.Pickup_Time + TOTAL_DELAY; //동작 시간

//				SYSTEM.do_control |= SGR.do_output; //DO 출력
//				SGR.do_output_off = SGR.do_output; //DO backup
//				DO_Output(0x0008); //test 용
//				TIMER.cb_open = 0;

					RELAY_STATUS.pickup									&= ~F_SGR; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_SGR;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_SGR;  //누적 동작 상태 변수 설정
					
//				Save_Screen_Info(); //POP UP 해제가 안되서 일단 막음
				}
			}
		}
		else
		{
			if(PROTECT.Max_RMS < SGR.Dropout_Threshold_Vo)  //under 99%
			{
				if((SGR.op_status == RELAY_DETECT) || (SGR.op_status == RELAY_PICKUP))
				{
					SGR.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_SGR; //계전요소 alarm OFF
				}
				else if(SGR.op_status == RELAY_TRIP)
				{
					Relay_Off(SGR.do_output); //DO open
					SGR.op_status = RELAY_NORMAL; //50_1상태 NORMAL
					RELAY_STATUS.operation_realtime &= ~F_SGR; //동작 상태 변수 해제
				}
			}
		}
	}
}

void protective_relay(void)
{
	PROTECT.Op_Phase = Ia;
	PROTECT.Max_RMS = MEASUREMENT.rms_value[Ia];
	if(PROTECT.Max_RMS  < MEASUREMENT.rms_value[Ib])
	{
		PROTECT.Op_Phase = Ib;
		PROTECT.Max_RMS = MEASUREMENT.rms_value[Ib];
	}
	if(PROTECT.Max_RMS < MEASUREMENT.rms_value[Ic])
	{
		PROTECT.Op_Phase = Ic;
		PROTECT.Max_RMS = MEASUREMENT.rms_value[Ic];
	}

	// OCR50-1
	OCR50_1_RELAY();
//OCR50_2_RELAY();
	OCR51_1_RELAY();
	OCR51_2_RELAY();
	OCGR50_RELAY();
	OCGR51_RELAY();
	UVR_1_RELAY();
	UVR_2_RELAY();
	UVR_3_RELAY();
	P47_RELAY();
	N47_RELAY();
	OVR_RELAY();
	OVGR_RELAY();
	DGR_RELAY();
	SGR_RELAY();
//	if(OCR50_1.use == 0xaaaa)
//	{
//		// 선택 di on일 때 유효
////	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == 0x0000))
//	
//		// 삼상 중 하나라도 크면
//		if((MEASUREMENT.rms_value[Ia] >= OCR50_1.Pickup_Threshold) || (MEASUREMENT.rms_value[Ib] >= OCR50_1.Pickup_Threshold) || (MEASUREMENT.rms_value[Ic] >= OCR50_1.Pickup_Threshold))
//		{
//			// 최초
//			if(OCR50_1.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&OCR50_1.op_count, &OCR50_1.op_status);
//			
//			// 대기
//			else if(OCR50_1.op_status == RELAY_DETECT)
//			{
//				if(OCR50_1.op_count > OCR50_1.pickup_limit)
//				{	
//					compare_value = over_phase_value_check(Ia, OCR50_1.Pickup_Threshold, &OCR50_1.event_ready);
//			
//					relay_detect_to_pickup(&OCR50_1.op_count, &OCR50_1.op_status, OCR50_1.event_ready, WAVE_OCR50_1_BIT);
//				}
//			}
//			
//			else if(OCR50_1.op_status == RELAY_PICKUP)
//			{
//				// 동작
//				if(OCR50_1.op_count > OCR50_1.delay_ms)
//				{
//					compare_value = over_phase_value_check(Ia, OCR50_1.Pickup_Threshold, &OCR50_1.event_ready);
//					
//					EVENT.optime = OCR50_1.op_count + OCR50_1.pickup_limit;
//					relay_pickup_to_operation(&OCR50_1.do_output, WAVE_OCR50_1_BIT, compare_value, OCR50_1.event_ready, &OCR50_1.op_status);
//				}
//			}
//		}
//		
//		// dropout
//		if((MEASUREMENT.rms_value[Ia] < OCR50_1.Dropout_Threshold) &&
//		   (MEASUREMENT.rms_value[Ib] < OCR50_1.Dropout_Threshold) &&
//		   (MEASUREMENT.rms_value[Ic] < OCR50_1.Dropout_Threshold))
//		{
//			// 사고난 후에 작동하라는 의미
//			if((OCR50_1.drop_status == DROPOUT_NORMAL) && (OCR50_1.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&OCR50_1.Dropout_Count, &OCR50_1.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(OCR50_1.drop_status == DROPOUT_DETECT)
//			{
//				if(OCR50_1.Dropout_Count > DROPOUT_LIMIT)
//		                        		relay_dropout_to_normal(&OCR50_1.event_ready, &OCR50_1.op_status, WAVE_OCR50_1_BIT, &OCR50_1.do_output_off);
//			}
//		}
//	}

	// OCR50-2
//	if(OCR50_2.use == 0xaaaa)
//	{
//		//선택 di off일 때 유효
////	if((SYSTEM_SET.ocr_mode == OCR_TURN) && ((DIGITAL_INPUT.di_status & SYSTEM_SET.ocr_di_mask) == SYSTEM_SET.ocr_di_mask))
////		goto R2;
//	
//		// 삼상 중 하나라도 크면
//		if((MEASUREMENT.rms_value[Ia] >= OCR50_2.Pickup_Threshold) || (MEASUREMENT.rms_value[Ib] >= OCR50_2.Pickup_Threshold) || (MEASUREMENT.rms_value[Ic] >= OCR50_2.Pickup_Threshold))
//		{
//			// 최초
//			if(OCR50_2.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&OCR50_2.op_count, &OCR50_2.op_status);
//			
//			// 대기
//			else if(OCR50_2.op_status == RELAY_DETECT)
//			{
//				if(OCR50_2.op_count > OCR50_2.pickup_limit)
//				{
//					compare_value = over_phase_value_check(Ia, OCR50_2.Pickup_Threshold, &OCR50_2.event_ready);
//						
//					relay_detect_to_pickup(&OCR50_2.op_count, &OCR50_2.op_status, OCR50_2.event_ready, WAVE_OCR50_2_BIT);
//				}
//			}
//		
//			else if(OCR50_2.op_status == RELAY_PICKUP)
//			{
//				// 동작
//				if(OCR50_2.op_count > OCR50_2.delay_ms)
//				{
//					compare_value = over_phase_value_check(Ia, OCR50_2.Pickup_Threshold, &OCR50_2.event_ready);
//				
//					EVENT.optime = OCR50_2.op_count + OCR50_2.pickup_limit;
//					relay_pickup_to_operation(&OCR50_2.do_output, WAVE_OCR50_2_BIT, compare_value, OCR50_2.event_ready, &OCR50_2.op_status);
//				}
//			}
//		}
//	
//		// dropout
//		if((MEASUREMENT.rms_value[Ia] < OCR50_2.Dropout_Threshold) &&
//	   	   (MEASUREMENT.rms_value[Ib] < OCR50_2.Dropout_Threshold) &&
//	   	   (MEASUREMENT.rms_value[Ic] < OCR50_2.Dropout_Threshold))
//		{
//			// 사고난 후에 작동하라는 의미
//			if((OCR50_2.drop_status == DROPOUT_NORMAL) && (OCR50_2.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&OCR50_2.Dropout_Count, &OCR50_2.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(OCR50_2.drop_status == DROPOUT_DETECT)
//			{
//				if(OCR50_2.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&OCR50_2.event_ready, &OCR50_2.op_status, WAVE_OCR50_2_BIT, &OCR50_2.do_output_off);
//			}
//		}
//	}

	// OCR51-1
//	if(OCR51_1.use == 0xaaaa)
//	{
//		if((SYSTEM_SET.ocr_mode == OCR_TURN) && (DIGITAL_INPUT.di_status == 0))
////		goto R3;
//		
//		// 삼상 중 하나라도 크면
//		if((MEASUREMENT.rms_value[Ia] >= OCR51_1.Pickup_Threshold) || (MEASUREMENT.rms_value[Ib] >= OCR51_1.Pickup_Threshold) || (MEASUREMENT.rms_value[Ic] >= OCR51_1.Pickup_Threshold))
//		{
//			// 최초
//			if(OCR51_1.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&OCR51_1.op_count, &OCR51_1.op_status);
//			
//			// 대기
//			else if(OCR51_1.op_status == RELAY_DETECT)
//			{
//				if(OCR51_1.op_count > INVERSE_PICKUP_LIMIT)
//				{	
//					compare_value = over_phase_value_check(Ia, OCR51_1.Pickup_Threshold, &OCR51_1.event_ready);
//			
//					relay_detect_to_pickup(&OCR51_1.op_count, &OCR51_1.op_status, OCR51_1.event_ready, WAVE_OCR51_1_BIT);
//				}
//			}
//		
//			else if(OCR51_1.op_status == RELAY_PICKUP)
//			{
//				OCR51_1.Op_Time_set = Inverse_Op_Timeset(Ia, OCR51_1.setting, OCR51_1.Mbyk, OCR51_1.a);
//				
//				
//				// 동작
//				if(OCR51_1.op_count > OCR51_1.Op_Time_set)
//				{
//					compare_value = over_phase_value_check(Ia, OCR51_1.Pickup_Threshold, &OCR51_1.event_ready);
//				
//					EVENT.optime = OCR51_1.op_count + INVERSE_PICKUP_LIMIT;
//					relay_pickup_to_operation(&OCR51_1.do_output, WAVE_OCR51_1_BIT, compare_value, OCR51_1.event_ready, &OCR51_1.op_status);
//				}
//			}
//		}				
//	
//		// dropout
//		if((MEASUREMENT.rms_value[Ia] < OCR51_1.Dropout_Threshold) &&
//	   	   (MEASUREMENT.rms_value[Ib] < OCR51_1.Dropout_Threshold) &&
//	   	   (MEASUREMENT.rms_value[Ic] < OCR51_1.Dropout_Threshold))
//		{
//			// 사고난 후에 작동하라는 의미
//			if((OCR51_1.drop_status == DROPOUT_NORMAL) && (OCR51_1.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&OCR51_1.Dropout_Count, &OCR51_1.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(OCR51_1.drop_status == DROPOUT_DETECT)
//			{
//				if(OCR51_1.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&OCR51_1.event_ready, &OCR51_1.op_status, WAVE_OCR51_1_BIT, &OCR51_1.do_output_off);
//			}
//		}
//	}

	// OCR51-2
//	if(OCR51_2.use == 0xaaaa)
//	{
//		if((SYSTEM_SET.ocr_mode == OCR_TURN) && (DIGITAL_INPUT.di_status == 0))
////		goto R4;
//		
//		// 삼상 중 하나라도 크면
//		if((MEASUREMENT.rms_value[Ia] >= OCR51_2.Pickup_Threshold) || (MEASUREMENT.rms_value[Ib] >= OCR51_2.Pickup_Threshold) || (MEASUREMENT.rms_value[Ic] >= OCR51_2.Pickup_Threshold))
//		{
//			// 최초
//			if(OCR51_2.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&OCR51_2.op_count, &OCR51_2.op_status);
//			
//			// 대기
//			else if(OCR51_2.op_status == RELAY_DETECT)
//			{
//				if(OCR51_2.op_count > INVERSE_PICKUP_LIMIT)
//				{	
//					compare_value = over_phase_value_check(Ia, OCR51_2.Pickup_Threshold, &OCR51_2.event_ready);
//			
//					relay_detect_to_pickup(&OCR51_2.op_count, &OCR51_2.op_status, OCR51_2.event_ready, WAVE_OCR51_2_BIT);
//				}
//			}
//		
//			else if(OCR51_2.op_status == RELAY_PICKUP)
//			{
//				OCR51_2.Op_Time_set = Inverse_Op_Timeset(Ia, OCR51_2.setting, OCR51_2.Mbyk, OCR51_2.a);
//				
//				
//				// 동작
//				if(OCR51_2.op_count > OCR51_2.Op_Time_set)
//				{
//					compare_value = over_phase_value_check(Ia, OCR51_2.Pickup_Threshold, &OCR51_2.event_ready);
//					
//					EVENT.optime = OCR51_2.op_count + INVERSE_PICKUP_LIMIT;
//					relay_pickup_to_operation(&OCR51_2.do_output, WAVE_OCR51_2_BIT, compare_value, OCR51_2.event_ready, &OCR51_2.op_status);
//				}
//			}
//		}				
//	
//		// dropout
//		if((MEASUREMENT.rms_value[Ia] < OCR51_2.Dropout_Threshold) &&
//	   	   (MEASUREMENT.rms_value[Ib] < OCR51_2.Dropout_Threshold) &&
//	   	   (MEASUREMENT.rms_value[Ic] < OCR51_2.Dropout_Threshold))
//		{
//			// 사고난 후에 작동하라는 의미
//			if((OCR51_2.drop_status == DROPOUT_NORMAL) && (OCR51_2.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&OCR51_2.Dropout_Count, &OCR51_2.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(OCR51_2.drop_status == DROPOUT_DETECT)
//			{
//				if(OCR51_2.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&OCR51_2.event_ready, &OCR51_2.op_status, WAVE_OCR51_2_BIT, &OCR51_2.do_output_off);
//			}
//		}
//	}

	// OCGR50
//	if(OCGR50.use == 0xaaaa)
//	{		
//		if(MEASUREMENT.rms_value[In] >= OCGR50.Pickup_Threshold)
//		{
//			// 최초
//			if(OCGR50.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&OCGR50.op_count, &OCGR50.op_status);
//			
//			// 대기
//			else if(OCGR50.op_status == RELAY_DETECT)
//			{
//				if(OCGR50.op_count > OCGR50.pickup_limit)
//				relay_detect_to_pickup(&OCGR50.op_count, &OCGR50.op_status, OCGR50.event_ready, WAVE_OCGR50_BIT);
//				
//			}
//			
//			else if(OCGR50.op_status == RELAY_PICKUP)
//			{
//				// 동작
//				if(OCGR50.op_count > OCGR50.delay_ms)
//				{					
//					compare_value = MEASUREMENT.rms_value[In] / OCGR50.Pickup_Threshold;
//					
//					EVENT.optime = OCGR50.op_count + OCGR50.pickup_limit;
//					relay_pickup_to_operation(&OCGR50.do_output, WAVE_OCGR50_BIT, compare_value, OCGR50.event_ready, &OCGR50.op_status);
//				}
//			}
//		}
//		
//		// dropout
//		if(MEASUREMENT.rms_value[In] < OCGR50.Dropout_Threshold)
//		{
//			// 사고난 후에 작동하라는 의미
//			if((OCGR50.drop_status == DROPOUT_NORMAL) && (OCGR50.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&OCGR50.Dropout_Count, &OCGR50.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(OCGR50.drop_status == DROPOUT_DETECT)
//			{
//				if(OCGR50.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&OCGR50.event_ready, &OCGR50.op_status, WAVE_OCGR50_BIT, &OCGR50.do_output_off);
//			}
//		}
//	}

	// OCGR51
//	if(OCGR51.use == 0xaaaa)
//	{
//		// 삼상 중 하나라도 크면
//		if(MEASUREMENT.rms_value[In] >= OCGR51.Pickup_Threshold)
//		{
//			// 최초
//			if(OCGR51.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&OCGR51.op_count, &OCGR51.op_status);
//			
//			// 대기
//			else if(OCGR51.op_status == RELAY_DETECT)
//			{
//				if(OCGR51.op_count > INVERSE_PICKUP_LIMIT)
//				relay_detect_to_pickup(&OCGR51.op_count, &OCGR51.op_status, OCGR51.event_ready, WAVE_OCGR51_BIT);
//			}
//			
//			else if(OCGR51.op_status == RELAY_PICKUP)
//			{
//				OCGR51.Op_Time_set = Inverse_Op_Timeset(In, OCGR51.setting, OCGR51.Mbyk, OCGR51.a);					
//					
//				// 동작
//				if(OCGR51.op_count > OCGR51.Op_Time_set)
//				{
//					compare_value = MEASUREMENT.rms_value[In] / OCGR51.Pickup_Threshold;
//					
//					EVENT.optime = OCGR51.op_count + INVERSE_PICKUP_LIMIT;
//					relay_pickup_to_operation(&OCGR51.do_output, WAVE_OCGR51_BIT, compare_value, OCGR51.event_ready, &OCGR51.op_status);
//				}
//			}
//		}
//				
//		
//		// dropout
//		if(MEASUREMENT.rms_value[In] < OCGR51.Dropout_Threshold)
//		{
//			// 사고난 후에 작동하라는 의미
//			if((OCGR51.drop_status == DROPOUT_NORMAL) && (OCGR51.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&OCGR51.Dropout_Count, &OCGR51.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(OCGR51.drop_status == DROPOUT_DETECT)
//			{
//				if(OCGR51.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&OCGR51.event_ready, &OCGR51.op_status, WAVE_OCGR51_BIT, &OCGR51.do_output_off);
//			}
//		}
//	}

	// UVR_1
//	if(UVR_1.use == 0xaaaa)
//	{
//		// 삼상 중 하나라도 작으면
//		if((MEASUREMENT.rms_value[Va] <= UVR_1.Pickup_Threshold) || (MEASUREMENT.rms_value[Vb] <= UVR_1.Pickup_Threshold) || (MEASUREMENT.rms_value[Vc] <= UVR_1.Pickup_Threshold))
//		{
//			// manual이고 전압이 한번도 안넘었으면
////			if((UVR_1.mode != 0xaaaa) && (UVR_1.over_volt != 0xaaaa))
//			if((UVR_1.over_volt != 0xaaaa))
//			goto U1;
//			
//			
//			// 최초
//			if(UVR_1.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&UVR_1.op_count, &UVR_1.op_status);
//			
//			// 대기
//			else if(UVR_1.op_status == RELAY_DETECT)
//			{
//				if(UVR_1.op_count > DEFINITE_PICKUP_LIMIT)
//				{
//					compare_value = under_phase_value_check(Va, UVR_1.Pickup_Threshold, &UVR_1.event_ready);
//					
//					relay_detect_to_pickup(&UVR_1.op_count, &UVR_1.op_status, UVR_1.event_ready, WAVE_UVR_1_BIT);
//				}
//			}
//			
//			else if(UVR_1.op_status == RELAY_PICKUP)
//			{
//				// 동작
//				if(UVR_1.op_count > UVR_1.delay_ms)
//				{
//					compare_value = under_phase_value_check(Va, UVR_1.Pickup_Threshold, &UVR_1.event_ready);
//					
//					EVENT.optime = UVR_1.op_count + DEFINITE_PICKUP_LIMIT;
//					relay_pickup_to_operation(&UVR_1.do_output, WAVE_UVR_1_BIT, compare_value, UVR_1.event_ready, &UVR_1.op_status);
//					
//					UVR_1.over_volt = 0;
//				}
//			}
//		}
//		
//		// dropout
//U1:		if((MEASUREMENT.rms_value[Va] > UVR_1.Dropout_Threshold) &&
//		   (MEASUREMENT.rms_value[Vb] > UVR_1.Dropout_Threshold) &&
//		   (MEASUREMENT.rms_value[Vc] > UVR_1.Dropout_Threshold))
//		{
//			// 사고난 후에 작동하라는 의미
//			if((UVR_1.drop_status == DROPOUT_NORMAL) && (UVR_1.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&UVR_1.Dropout_Count, &UVR_1.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(UVR_1.drop_status == DROPOUT_DETECT)
//			{
//				if(UVR_1.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&UVR_1.event_ready, &UVR_1.op_status, WAVE_UVR_1_BIT, &UVR_1.do_output_off);
//			}
//						
//			UVR_1.over_volt = 0xaaaa;
//		}
//	}

	// UVR_2
//	if(UVR_2.use == 0xaaaa)
//	{
//		// 삼상 중 하나라도 작으면
//		if((MEASUREMENT.rms_value[Va] <= UVR_2.Pickup_Threshold) || (MEASUREMENT.rms_value[Vb] <= UVR_2.Pickup_Threshold) || (MEASUREMENT.rms_value[Vc] <= UVR_2.Pickup_Threshold))
//		{
//			// manual이고 전압이 한번도 안넘었으면
////				if((UVR_2.mode != 0xaaaa) && (UVR_2.over_volt != 0xaaaa))
//			if((UVR_2.over_volt != 0xaaaa))
////			goto U2;
//		
//			// 최초
//			if(UVR_2.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&UVR_2.op_count, &UVR_2.op_status);
//			
//			// 대기
//			else if(UVR_2.op_status == RELAY_DETECT)
//			{
//				if(UVR_2.op_count > DEFINITE_PICKUP_LIMIT)
//				{
//					compare_value = under_phase_value_check(Va, UVR_2.Pickup_Threshold, &UVR_2.event_ready);
//					
//		    		relay_detect_to_pickup(&UVR_2.op_count, &UVR_2.op_status, UVR_2.event_ready, WAVE_UVR_2_BIT);
//				}
//			}
//		    
//			else if(UVR_2.op_status == RELAY_PICKUP)
//			{
//				// 동작
//				if(UVR_2.op_count > UVR_2.delay_ms)
//				{
//		    		compare_value = under_phase_value_check(Va, UVR_2.Pickup_Threshold, &UVR_2.event_ready);
//					
//					EVENT.optime = UVR_2.op_count + DEFINITE_PICKUP_LIMIT;
//					relay_pickup_to_operation(&UVR_2.do_output, WAVE_UVR_2_BIT, compare_value, UVR_2.event_ready, &UVR_2.op_status);
//					
//					UVR_2.over_volt = 0;
//				}
//			}
//		}       
//		
//		// dropout
//			if((MEASUREMENT.rms_value[Va] > UVR_2.Dropout_Threshold) &&
//		   (MEASUREMENT.rms_value[Vb] > UVR_2.Dropout_Threshold) &&
//	       (MEASUREMENT.rms_value[Vc] > UVR_2.Dropout_Threshold))
//		{
//			// 사고난 후에 작동하라는 의미
//			if((UVR_2.drop_status == DROPOUT_NORMAL) && (UVR_2.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&UVR_2.Dropout_Count, &UVR_2.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(UVR_2.drop_status == DROPOUT_DETECT)
//			{
//	    		if(UVR_2.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&UVR_2.event_ready, &UVR_2.op_status, WAVE_UVR_2_BIT, &UVR_2.do_output_off);
//			}
//			
//			UVR_2.over_volt = 0xaaaa;
//		}
//	}

	// UVR_3
//	if(UVR_3.use == 0xaaaa)
//	{
//		// 삼상 중 하나라도 작으면
//		if((MEASUREMENT.rms_value[Va] <= UVR_3.Pickup_Threshold) || (MEASUREMENT.rms_value[Vb] <= UVR_3.Pickup_Threshold) || (MEASUREMENT.rms_value[Vc] <= UVR_3.Pickup_Threshold))
//		{
//			// manual이고 전압이 한번도 안넘었으면
////				if((UVR_3.mode != 0xaaaa) && (UVR_3.over_volt != 0xaaaa))
//			if((UVR_3.over_volt != 0xaaaa))
////			goto U3;
//			
//			// 최초
//			if(UVR_3.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&UVR_3.op_count, &UVR_3.op_status);
//			
//			// 대기
//			else if(UVR_3.op_status == RELAY_DETECT)
//			{
//				if(UVR_3.op_count > DEFINITE_PICKUP_LIMIT)
//				{
//		    		compare_value = under_phase_value_check(Va, UVR_3.Pickup_Threshold, &UVR_3.event_ready);
//					
//					relay_detect_to_pickup(&UVR_3.op_count, &UVR_3.op_status, UVR_3.event_ready, WAVE_UVR_3_BIT);
//				}
//			}
//		    
//			else if(UVR_3.op_status == RELAY_PICKUP)
//			{
//				// 동작
//				if(UVR_3.op_count > UVR_3.delay_ms)
//				{
//		    		compare_value = under_phase_value_check(Va, UVR_3.Pickup_Threshold, &UVR_3.event_ready);
//					
//					EVENT.optime = UVR_3.op_count + DEFINITE_PICKUP_LIMIT;
//					relay_pickup_to_operation(&UVR_3.do_output, WAVE_UVR_3_BIT, compare_value, UVR_3.event_ready, &UVR_3.op_status);
//					
//					UVR_3.over_volt = 0;
//				}
//			}
//		}       
//		
//		// dropout
//			if((MEASUREMENT.rms_value[Va] > UVR_3.Dropout_Threshold) &&
//		   (MEASUREMENT.rms_value[Vb] > UVR_3.Dropout_Threshold) &&
//	       (MEASUREMENT.rms_value[Vc] > UVR_3.Dropout_Threshold))
//		{
//			// 사고난 후에 작동하라는 의미
//			if((UVR_3.drop_status == DROPOUT_NORMAL) && (UVR_3.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&UVR_3.Dropout_Count, &UVR_3.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(UVR_3.drop_status == DROPOUT_DETECT)
//			{
//	    		if(UVR_3.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&UVR_3.event_ready, &UVR_3.op_status, WAVE_UVR_3_BIT, &UVR_3.do_output_off);
//			}
//			
//			UVR_3.over_volt = 0xaaaa;
//		}
//	}

	// P47
//	if(P47.use == 0xaaaa)
//	{
//		if(MEASUREMENT.V1_value >= P47.Pickup_Threshold)
//		{
//			// 최초
//			if(P47.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&P47.op_count, &P47.op_status);
//			
//			// 대기
//			else if(P47.op_status == RELAY_DETECT)
//			{
//				if(P47.op_count > DEFINITE_PICKUP_LIMIT)
//				relay_detect_to_pickup(&P47.op_count, &P47.op_status, P47.event_ready, WAVE_47P_BIT);
//			}
//		
//			else if(P47.op_status == RELAY_PICKUP)
//			{
//				// 동작
//				if(P47.op_count > P47.delay_ms)
//				{
//					compare_value = MEASUREMENT.V1_value / P47.Pickup_Threshold;
//					
//					EVENT.optime = P47.op_count + DEFINITE_PICKUP_LIMIT;
//					relay_pickup_to_operation(&P47.do_output, WAVE_47P_BIT, compare_value, P47.event_ready, &P47.op_status);
//				}
//			}
//		}
//	
//		// dropout
//		if(MEASUREMENT.V1_value < P47.Dropout_Threshold)
//		{
//			// 사고난 후에 작동하라는 의미
//			if((P47.drop_status == DROPOUT_NORMAL) && (P47.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&P47.Dropout_Count, &P47.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(P47.drop_status == DROPOUT_DETECT)
//			{
//	    		if(P47.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&P47.event_ready, &P47.op_status, WAVE_47P_BIT, &P47.do_output_off);
//			}
//		}
//	}

	// N47
//	if(N47.use == 0xaaaa)
//	{
//		if(MEASUREMENT.V2_value >= N47.Pickup_Threshold)
//		{
//			// 최초
//			if(N47.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&N47.op_count, &N47.op_status);
//			
//			// 대기
//			else if(N47.op_status == RELAY_DETECT)
//			{
//				if(N47.op_count > DEFINITE_PICKUP_LIMIT)
//				relay_detect_to_pickup(&N47.op_count, &N47.op_status, N47.event_ready, WAVE_47N_BIT);
//			}
//		
//			else if(N47.op_status == RELAY_PICKUP)
//			{
//				// 동작
//				if(N47.op_count > N47.delay_ms)
//				{
//					compare_value = MEASUREMENT.V2_value / N47.Pickup_Threshold;
//					
//					EVENT.optime = N47.op_count + DEFINITE_PICKUP_LIMIT;
//					relay_pickup_to_operation(&N47.do_output, WAVE_47N_BIT, compare_value, N47.event_ready, &N47.op_status);
//				}
//			}
//		}
//	
//		// dropout
//		if(MEASUREMENT.V2_value < N47.Dropout_Threshold)
//		{
//			// 사고난 후에 작동하라는 의미
//			if((N47.drop_status == DROPOUT_NORMAL) && (N47.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&N47.Dropout_Count, &N47.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(N47.drop_status == DROPOUT_DETECT)
//			{
//    		if(N47.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&N47.event_ready, &N47.op_status, WAVE_47N_BIT, &N47.do_output_off);
//			}
//		}
//	}

	// OVR
//	if(OVR.use == 0xaaaa)
//	{
//		// 삼상 중 하나라도 크면
//		if((MEASUREMENT.rms_value[Va] >= OVR.Pickup_Threshold) || (MEASUREMENT.rms_value[Vb] >= OVR.Pickup_Threshold) || (MEASUREMENT.rms_value[Vc] >= OVR.Pickup_Threshold))
//		{
//			// 최초
//			if(OVR.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&OVR.op_count, &OVR.op_status);
//			
//			// 대기
//			else if(OVR.op_status == RELAY_DETECT)
//			{
//				if(OVR.op_count > OVR.pickup_limit)
//				{	
//					compare_value = over_phase_value_check(Va, OVR.Pickup_Threshold, &OVR.event_ready);
//					
//					relay_detect_to_pickup(&OVR.op_count, &OVR.op_status, OVR.event_ready, WAVE_OVR_BIT);
//				}
//			}
//		
//			else if(OVR.op_status == RELAY_PICKUP)
//			{
//				if(OVR.mode == INVERSE)
//				OVR.Op_Time_set = Inverse_Op_Timeset(Va, OVR.setting, OVR.Mbyk, IEC_INV_a);
//				
//				// 동작
//				if(OVR.op_count > OVR.Op_Time_set)
//				{
//					compare_value = over_phase_value_check(Va, OVR.Pickup_Threshold, &OVR.event_ready);
//				
//					EVENT.optime = OVR.op_count + OVR.pickup_limit;
//					relay_pickup_to_operation(&OVR.do_output, WAVE_OVR_BIT, compare_value, OVR.event_ready, &OVR.op_status);
//				}
//			}
//		}
//			
//		// dropout
//		if((MEASUREMENT.rms_value[Va] < OVR.Dropout_Threshold) &&
//	   	   (MEASUREMENT.rms_value[Vb] < OVR.Dropout_Threshold) &&
//	   	   (MEASUREMENT.rms_value[Vc] < OVR.Dropout_Threshold))
//		{
//			// 사고난 후에 작동하라는 의미
//			if((OVR.drop_status == DROPOUT_NORMAL) && (OVR.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&OVR.Dropout_Count, &OVR.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(OVR.drop_status == DROPOUT_DETECT)
//			{
//				if(OVR.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&OVR.event_ready, &OVR.op_status, WAVE_OVR_BIT, &OVR.do_output_off);
//			}
//		}
//	}

	// OVGR
//	if(OVGR.use == 0xaaaa)
//	{
//		if(MEASUREMENT.rms_value[Vn] >= OVGR.Pickup_Threshold)
//		{
//			// 최초
//			if(OVGR.op_status == RELAY_NORMAL)
//			relay_normal_to_detect(&OVGR.op_count, &OVGR.op_status);
//			
//			// 대기
//			else if(OVGR.op_status == RELAY_DETECT)
//			{
//				if(OVGR.op_count > OVGR.pickup_limit)
//				relay_detect_to_pickup(&OVGR.op_count, &OVGR.op_status, OVGR.event_ready, WAVE_OVGR_BIT);
//			}
//		
//			else if(OVGR.op_status == RELAY_PICKUP)
//			{
//				if(OVGR.mode == INVERSE)
//				OVGR.Op_Time_set = Inverse_Op_Timeset(Vn, OVGR.setting, OVGR.Mbyk, IEC_INV_a);
//				
//				// 동작
//				if(OVGR.op_count > OVGR.Op_Time_set)
//				{
//					compare_value = MEASUREMENT.rms_value[Vn] / OVGR.Pickup_Threshold;
//					
//					EVENT.optime = OVGR.op_count + OVGR.pickup_limit;
//					relay_pickup_to_operation(&OVGR.do_output, WAVE_OVGR_BIT, compare_value, OVGR.event_ready, &OVGR.op_status);
//				}
//			}
//		}
//	
//		// dropout
//		if(MEASUREMENT.rms_value[Vn] < OVGR.Dropout_Threshold)
//		{
//			// 사고난 후에 작동하라는 의미
//			if((OVGR.drop_status == DROPOUT_NORMAL) && (OVGR.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&OVGR.Dropout_Count, &OVGR.drop_status);
//			
//			// 8ms 쳐다 봄
//			else if(OVGR.drop_status == DROPOUT_DETECT)
//			{
//				if(OVGR.Dropout_Count > DROPOUT_LIMIT)
//				relay_dropout_to_normal(&OVGR.event_ready, &OVGR.op_status, WAVE_OVGR_BIT, &OVGR.do_output_off);
//			}
//		}
//	}

	// DGR
//	if(DGR.use == 0xaaaa)
//	{
//		// 일단 영상전압/전류 이상
//		if((MEASUREMENT.rms_value[Vn] >= DGR.Pickup_Threshold_Vo) && (MEASUREMENT.dgr_rms_Io   >= DGR.Pickup_Threshold_Io))
//		{
//			// 위상차
//			DGR.angle_sub = MEASUREMENT.dgr_angle_Io - MEASUREMENT.angle[Vn];
//			
//			DGR.angle_sub *= 57.29577951;
//			
//			if(DGR.angle_sub < 0)
//			DGR.angle_sub += 360;
//			
//			// 위상차 범위이내
//			if((DGR.angle_sub > DGR.angle_low) || (DGR.angle_sub < DGR.angle_high))
//			{
//				// 최초
//				if(DGR.op_status == RELAY_NORMAL)
//				relay_normal_to_detect(&DGR.op_count, &DGR.op_status);
//				
//				// 대기
//				else if(DGR.op_status == RELAY_DETECT)
//				{
//					if(DGR.op_count > DEFINITE_PICKUP_LIMIT)
//					{
//						relay_detect_to_pickup(&DGR.op_count, &DGR.op_status, DGR.event_ready, WAVE_DGR_BIT);
//					}
//				}
//				else if(DGR.op_status == RELAY_PICKUP)
//				{
//					// 동작
//					if(DGR.op_count > DGR.delay_ms)
//					{
//						compare_value = DGR.angle_sub;
//						compare_value *= 100;
//						
//						EVENT.optime = DGR.op_count + DEFINITE_PICKUP_LIMIT;
//						
//						relay_pickup_to_operation(&DGR.do_output, WAVE_DGR_BIT, compare_value, DGR.event_ready, &DGR.op_status);
//					}
//				}
//			}
//		}
//		
//		// dropout
//		if((MEASUREMENT.rms_value[Vn] < DGR.Dropout_Threshold_Vo) ||
//		   (MEASUREMENT.dgr_rms_Io   < DGR.Dropout_Threshold_Io) ||
//		   ((DGR.angle_sub < DGR.angle_low) && (DGR.angle_sub > DGR.angle_high)))
//		{
//			// 사고난 후에 작동하라는 의미
//			if((DGR.drop_status == DROPOUT_NORMAL) && (DGR.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&DGR.Dropout_Count, &DGR.drop_status);
//				
//			// 8ms 쳐다 봄
//			else if(DGR.drop_status == DROPOUT_DETECT)
//			{
//	   		if(DGR.Dropout_Count > DROPOUT_LIMIT)
//				{
//					relay_dropout_to_normal(&DGR.event_ready, &DGR.op_status, WAVE_DGR_BIT, &DGR.do_output_off);
//				}
//			}
//		}
//	}

	// SGR
//	if(SGR.use == 0xaaaa)
//	{
//		// 일단 영상전압/전류 이상
//		if((MEASUREMENT.rms_value[Vn] >= SGR.Pickup_Threshold_Vo) && (MEASUREMENT.sgr_rms_Io   >= SGR.Pickup_Threshold_Io))
//		{
//			// 위상차
//			SGR.angle_sub = MEASUREMENT.sgr_angle_Io - MEASUREMENT.angle[Vn];
//			
//			SGR.angle_sub *= 57.29577951;
//			
//			if(SGR.angle_sub < 0)
//			SGR.angle_sub += 360;
//			
//		
//			// 위상차 범위이내
//			if((SGR.angle_sub > SGR.angle_low) || (SGR.angle_sub < SGR.angle_high))
//			{
//				// 최초
//				if(SGR.op_status == RELAY_NORMAL)
//				relay_normal_to_detect(&SGR.op_count, &SGR.op_status);
//				
//				// 대기
//				else if(SGR.op_status == RELAY_DETECT)
//				{
//					if(SGR.op_count > DEFINITE_PICKUP_LIMIT)
//					{
//						relay_detect_to_pickup(&SGR.op_count, &SGR.op_status, SGR.event_ready, WAVE_SGR_BIT);
//					}
//				}
//				else if(SGR.op_status == RELAY_PICKUP)
//				{
//					// 동작
//					if(SGR.op_count > SGR.delay_ms)
//					{
//						compare_value = SGR.angle_sub;
//						compare_value *= 100;
//						
//						EVENT.optime = SGR.op_count + DEFINITE_PICKUP_LIMIT;
//						
//						relay_pickup_to_operation(&SGR.do_output, WAVE_SGR_BIT, compare_value, SGR.event_ready, &SGR.op_status);
//					}
//				}
//			}
//		}
//		
//		// dropout
//		if((MEASUREMENT.rms_value[Vn] < SGR.Dropout_Threshold_Vo) ||
//		   (MEASUREMENT.sgr_rms_Io   < SGR.Dropout_Threshold_Io) ||
//		   ((SGR.angle_sub < SGR.angle_low) && (SGR.angle_sub > SGR.angle_high)))
//		{
//			// 사고난 후에 작동하라는 의미
//			if((SGR.drop_status == DROPOUT_NORMAL) && (SGR.op_status != RELAY_NORMAL))
//			relay_opeartion_to_dropout(&SGR.Dropout_Count, &SGR.drop_status);
//				
//			// 8ms 쳐다 봄
//			else if(SGR.drop_status == DROPOUT_DETECT)
//			{
//	   		if(SGR.Dropout_Count > DROPOUT_LIMIT)
//				{
//					relay_dropout_to_normal(&SGR.event_ready, &SGR.op_status, WAVE_SGR_BIT, &SGR.do_output_off);
//				}
//			}
//		}
//	}

	SAMPLE.ending = 0;
}

unsigned long Inverse_Op_Timeset(unsigned int ar_channel, float ar_setting, float ar_mbyk, float ar_a)
{
	float compare_value;
	float float_temp;
	float float_temp2;
	
	// Ia, Va
	if((ar_channel == 0) || (ar_channel == 6))
	{
		// a상 일단 저장
		compare_value = MEASUREMENT.rms_value[ar_channel];
		
		// b상이 더크면 저장
		if(MEASUREMENT.rms_value[ar_channel + 1] > compare_value)
		compare_value = MEASUREMENT.rms_value[ar_channel + 1];
					
		// c상이 더크면 저장	
		if(MEASUREMENT.rms_value[ar_channel + 2] > compare_value)
		compare_value = MEASUREMENT.rms_value[ar_channel + 2];
	}
	else	{compare_value = MEASUREMENT.rms_value[ar_channel];} 	//Io, Vo
	
	float_temp = pow((compare_value / ar_setting), ar_a); // 동작 시간 계산
	float_temp -= 1;
	float_temp2 = ar_mbyk / float_temp;
	float_temp2 *= 1000; // 총 동작 시간에서 17ms 까먹은 거 감안
	float_temp2 -= 30;
	return((unsigned long)float_temp2);
}

unsigned long Inverse_GetDelayTime(int mode, float OP_level, float Ratio)
{
	float DelayTime;

	OP_level *= 0.01; 
	switch(mode)
	{
		case  INVERSE:								//Normal inverse type
				DelayTime=(0.14/(pow(Ratio,0.02)-1))*OP_level;
				break;
		case  V_INVERSE:								//Very inverse type
				DelayTime=(13.5/(pow(Ratio,1.0)-1))*OP_level;
				break;
		case  E_INVERSE:								//Extremely inverse type
				DelayTime=(80.0/(pow(Ratio,2.0)-1))*OP_level;
				break;
		default:
				break;
	}
	return((unsigned long)(DelayTime * 1000.));
}


