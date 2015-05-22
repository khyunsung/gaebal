#include "define.h"
#include "extern_variable.h"
#include "extern_prototype.h"

void Save_Relay_Event(float ar_ratio)
{
//  일단 나중에 구현하자 2015-05-11 오후 2:19:04					
//	WAVE.relay |= ar_relay_bit;
//	// wave capture 시작 추가
//	if((WAVE.post_start != 0x1234) && (WAVE.hold == 0))	WAVE.post_start = 0x1234;
						
	EVENT.ratio = (unsigned int)ar_ratio;
	event_direct_save(&EVENT.operation);
}

void relay_dropout_to_normal(unsigned int ar_relay_bit)
{
	WAVE.relay &= ~ar_relay_bit;
}

void Check_47P_10V(void)
{
	if((PROTECT.V1_RMS < 10.0) && (RELAY_STATUS.operation_sum_holding & F_P47))
	{
		Relay_Off(P47.do_output); //DO open
		P47.op_status = RELAY_NORMAL; //P47상태 NORMAL
		RELAY_STATUS.operation_realtime &= ~F_P47; //동작 상태 변수 해제
		RELAY_STATUS.operation_sum_holding &= ~F_P47;
		P47.over_volt_flag = OFF;
	}
}

void RELAY_OCR50_1(void)
{
	if(OCR50_1.use == 0xaaaa)
	{
		if((OCR_MODE_SET.ocr_mode == OCR_NORMAL) || ((OCR_MODE_SET.ocr_mode == OCR_SELECT) && (OCR_MODE_SET.ocr_di_mask == 0x0008))) //DI접점이 4인 경우
		{
			if(PROTECT.Max_I_RMS >= OCR50_1.Pickup_Threshold)
			{
				if(OCR50_1.op_status == RELAY_NORMAL)
				{
					OCR50_1.op_status = RELAY_DETECT;
					OCR50_1.op_count = 0;
				}
				else if(OCR50_1.op_status == RELAY_DETECT)
				{
					if(OCR50_1.op_count >= OCR50_1.pickup_limit)
					{	
						OCR50_1.op_status = RELAY_PICKUP;
						RELAY_STATUS.pickup |= F_OCR50_1;  //alarm ON
						OCR50_1.Pickup_Time = OCR50_1.op_count;
						OCR50_1.op_count = 0;
					}
				}
				else if(OCR50_1.op_status == RELAY_PICKUP)
				{
					if(OCR_MODE_SET.ocr_mode == OCR_SELECT)
					{
						OCR50_1.di_on_flag = 1;
						if(OCR50_1.di_err_flag==1)
						{
							OCR50_1.op_status = RELAY_NORMAL;
							OCR50_1.di_err_flag = 0;
							return;
						}
					}

					if(OCR50_1.op_count >= OCR50_1.delay_ms)
					{
						Relay_On(OCR50_1.do_output);

						OCR50_1.op_status	= RELAY_TRIP;
						OCR50_1.Op_Ratio	= PROTECT.Max_I_RMS / OCR50_1.Pickup_Threshold; //배수
						OCR50_1.Op_Phase	= PROTECT.I_Op_Phase; //상
						OCR50_1.Delay_Time = OCR50_1.op_count;
						OCR50_1.Op_Time		= OCR50_1.Delay_Time + OCR50_1.Pickup_Time + TOTAL_DELAY_50; //동작 시간

						RELAY_STATUS.pickup									&= ~F_OCR50_1; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_OCR50_1;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_OCR50_1;  //누적 동작 상태 변수 설정
						
						EVENT.optime = (unsigned long)OCR50_1.Op_Time;
						EVENT.operation |= (F_OCR50_1 << 16) + OCR50_1.Op_Phase;
						EVENT.fault_type = F_OCR50_1;
						Phase_Info = (Phase_Info == 0)? EVENT.operation: OCR50_1.Op_Phase;
						Save_Relay_Event(OCR50_1.Op_Ratio * 100.0F);
						Save_Screen_Info(OCR50_1.Op_Phase);
					}
				}
			}
			else
			{
				if(PROTECT.Max_I_RMS <= OCR50_1.Dropout_Threshold)  //under 99%
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
		else //OCR_MODE_SET.ocr_mode == OCR_SELECT 이고 DI 정보가 안들어 올 때
		{
			OCR50_1.di_err_flag = 1;
			if(OCR50_1.di_on_flag == 1)	{RELAY_STATUS.pickup &= ~F_OCR50_1;}  //계전요소 alarm OFF
			OCR50_1.di_on_flag = 0;
		}
	}
}

void RELAY_OCR50_2(void)
{
	if(OCR50_2.use == 0xaaaa)
	{
		if((OCR_MODE_SET.ocr_mode == OCR_NORMAL) || ((OCR_MODE_SET.ocr_mode == OCR_SELECT) && (OCR_MODE_SET.ocr_di_mask == 0x0010))) //DI접점이 5인 경우
		{
			if(PROTECT.Max_I_RMS >= OCR50_2.Pickup_Threshold)
			{
				if(OCR50_2.op_status == RELAY_NORMAL)
				{
					OCR50_2.op_status = RELAY_DETECT;
					OCR50_2.op_count = 0;
				}
				else if(OCR50_2.op_status == RELAY_DETECT)
				{
					if(OCR50_2.op_count >= OCR50_2.pickup_limit)
					{
						OCR50_2.op_status = RELAY_PICKUP;
						RELAY_STATUS.pickup |= F_OCR50_2;  //alarm ON
						OCR50_2.Pickup_Time = OCR50_2.op_count;
						OCR50_2.op_count = 0;
					}
				}
				else if(OCR50_2.op_status == RELAY_PICKUP)
				{
					if(OCR_MODE_SET.ocr_mode == OCR_SELECT)
					{
						OCR50_2.di_on_flag = 1;
						if(OCR50_2.di_err_flag==1)
						{
							OCR50_2.op_status = RELAY_NORMAL;
							OCR50_2.di_err_flag = 0;
							return;
						}
					}

					if(OCR50_2.op_count >= OCR50_2.delay_ms)
					{
						Relay_On(OCR50_2.do_output);

						OCR50_2.op_status	= RELAY_TRIP;
						OCR50_2.Op_Ratio	= PROTECT.Max_I_RMS / OCR50_2.Pickup_Threshold; //배수
						OCR50_2.Op_Phase	= PROTECT.I_Op_Phase; //상
						OCR50_2.Delay_Time = OCR50_2.op_count;
						OCR50_2.Op_Time		= OCR50_2.Delay_Time + OCR50_2.Pickup_Time + TOTAL_DELAY_50; //동작 시간

						RELAY_STATUS.pickup									&= ~F_OCR50_2; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_OCR50_2;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_OCR50_2;  //누적 동작 상태 변수 설정
						
						EVENT.optime = (unsigned long)OCR50_2.Op_Time;
						EVENT.operation |= (F_OCR50_2 << 16) + OCR50_2.Op_Phase;
						Phase_Info = (Phase_Info == 0)? EVENT.operation: OCR50_2.Op_Phase;
						EVENT.fault_type = F_OCR50_2;
						Save_Relay_Event(OCR50_2.Op_Ratio * 100.0F);
						Save_Screen_Info(OCR50_2.Op_Phase);
					}
				}
			}
			else
			{
				if(PROTECT.Max_I_RMS <= OCR50_2.Dropout_Threshold)  //under 99%
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
		else //OCR_MODE_SET.ocr_mode == OCR_SELECT 이고 DI 정보가 안들어 올 때
		{
			OCR50_2.di_err_flag = 1;
			if(OCR50_2.di_on_flag == 1)	{RELAY_STATUS.pickup &= ~F_OCR50_2;}  //계전요소 alarm OFF
			OCR50_2.di_on_flag = 0;
		}
	}
}

void RELAY_OCR51_1(void)
{
	if(OCR51_1.use == 0xaaaa)
	{
		if((OCR_MODE_SET.ocr_mode == OCR_NORMAL) || ((OCR_MODE_SET.ocr_mode == OCR_SELECT) && (OCR_MODE_SET.ocr_di_mask == 0x0008))) //DI접점이 4인 경우
		{
			if(PROTECT.Max_I_RMS >= OCR51_1.Pickup_Threshold)
			{
				if(OCR51_1.op_status == RELAY_NORMAL)
				{
					OCR51_1.op_status = RELAY_DETECT;
					OCR51_1.op_count = 0;
				}
				else if(OCR51_1.op_status == RELAY_DETECT)
				{
					if(OCR51_1.op_count >= OCR51_1.pickup_limit)
					{	
						OCR51_1.op_status = RELAY_PICKUP;
						RELAY_STATUS.pickup |= F_OCR51_1;  //alarm ON
						OCR51_1.Pickup_Time = OCR51_1.op_count;
						OCR51_1.op_count = 0;
					}
				}
				else if(OCR51_1.op_status == RELAY_PICKUP)
				{
					if(OCR_MODE_SET.ocr_mode == OCR_SELECT)
					{
						OCR51_1.di_on_flag = 1;
						if(OCR51_1.di_err_flag==1)
						{
							OCR51_1.op_status = RELAY_NORMAL;
							OCR51_1.di_err_flag = 0;
							return;
						}
					}
					OCR51_1.Op_Ratio	= PROTECT.Max_I_RMS / OCR51_1.Pickup_Threshold;
					OCR51_1.Op_Time_set = Inverse_GetDelayTime(OCR51_1.mode, OCR51_1.time_lever, OCR51_1.Op_Ratio);
					OCR51_1.Op_Time_set -= (INVERSE_PICKUP_LIMIT+TOTAL_DELAY_51);
					if(OCR51_1.op_count >= OCR51_1.Op_Time_set)
					{
						Relay_On(OCR51_1.do_output);

						OCR51_1.op_status	= RELAY_TRIP;
						OCR51_1.Op_Phase	= PROTECT.I_Op_Phase; //상
						OCR51_1.Delay_Time = OCR51_1.op_count;
						OCR51_1.Op_Time		= OCR51_1.Delay_Time + OCR51_1.Pickup_Time + TOTAL_DELAY_51; //동작 시간

						RELAY_STATUS.pickup									&= ~F_OCR51_1; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_OCR51_1;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_OCR51_1;  //누적 동작 상태 변수 설정
						
						EVENT.optime = (unsigned long)OCR51_1.Op_Time;
						EVENT.operation |= (F_OCR51_1 << 16) + OCR51_1.Op_Phase;
						Phase_Info = (Phase_Info == 0)? EVENT.operation: OCR51_1.Op_Phase;
						EVENT.fault_type = F_OCR51_1;
						Save_Relay_Event(OCR51_1.Op_Ratio * 100.0F);
						Save_Screen_Info(OCR51_1.Op_Phase);
					}
				}
			}
			else
			{
				if(PROTECT.Max_I_RMS <= OCR51_1.Dropout_Threshold)  //under 99%
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
		else //OCR_MODE_SET.ocr_mode == OCR_SELECT 이고 DI 정보가 안들어 올 때
		{
			OCR51_1.di_err_flag = 1;
			if(OCR51_1.di_on_flag == 1)	{RELAY_STATUS.pickup &= ~F_OCR51_1;}  //계전요소 alarm OFF
			OCR51_1.di_on_flag = 0;
		}
	}
}

void RELAY_OCR51_2(void)
{
	if(OCR51_2.use == 0xaaaa)
	{
		if((OCR_MODE_SET.ocr_mode == OCR_NORMAL) || ((OCR_MODE_SET.ocr_mode == OCR_SELECT) && (OCR_MODE_SET.ocr_di_mask == 0x0010))) //DI접점이 5인 경우
		{
			if(PROTECT.Max_I_RMS >= OCR51_2.Pickup_Threshold)
			{
				if(OCR51_2.op_status == RELAY_NORMAL)
				{
					OCR51_2.op_status = RELAY_DETECT;
					OCR51_2.op_count = 0;
				}
				else if(OCR51_2.op_status == RELAY_DETECT)
				{
					if(OCR51_2.op_count >= OCR51_2.pickup_limit)
					{	
						OCR51_2.op_status = RELAY_PICKUP;
						RELAY_STATUS.pickup |= F_OCR51_2;  //alarm ON
						OCR51_2.Pickup_Time = OCR51_2.op_count;
						OCR51_2.op_count = 0;
					}
				}
				else if(OCR51_2.op_status == RELAY_PICKUP)
				{
					if(OCR_MODE_SET.ocr_mode == OCR_SELECT)
					{
						OCR51_2.di_on_flag = 1;
						if(OCR51_2.di_err_flag==1)
						{
							OCR51_2.op_status = RELAY_NORMAL;
							OCR51_2.di_err_flag = 0;
							return;
						}
					}
					OCR51_2.Op_Ratio	= PROTECT.Max_I_RMS / OCR51_2.Pickup_Threshold;
					OCR51_2.Op_Time_set = Inverse_GetDelayTime(OCR51_2.mode, OCR51_2.time_lever, OCR51_2.Op_Ratio);
					OCR51_2.Op_Time_set -= (INVERSE_PICKUP_LIMIT+TOTAL_DELAY_51);
					if(OCR51_2.op_count >= OCR51_2.Op_Time_set)
					{
						Relay_On(OCR51_2.do_output);

						OCR51_2.op_status	= RELAY_TRIP;
						OCR51_2.Op_Phase	= PROTECT.I_Op_Phase; //상
						OCR51_2.Delay_Time = OCR51_2.op_count;
						OCR51_2.Op_Time		= OCR51_2.Delay_Time + OCR51_2.Pickup_Time + TOTAL_DELAY_51; //동작 시간

						RELAY_STATUS.pickup									&= ~F_OCR51_2; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_OCR51_2;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_OCR51_2;  //누적 동작 상태 변수 설정
						
						EVENT.optime = (unsigned long)OCR51_2.Op_Time;
						EVENT.operation |= (F_OCR51_2 << 16) + OCR51_2.Op_Phase;
						Phase_Info = (Phase_Info == 0)? EVENT.operation: OCR51_2.Op_Phase;
						EVENT.fault_type = F_OCR51_2;
						Save_Relay_Event(OCR51_2.Op_Ratio * 100.0F);
						Save_Screen_Info(OCR51_2.Op_Phase);
					}
				}
			}
			else
			{
				if(PROTECT.Max_I_RMS <= OCR51_2.Dropout_Threshold)  //under 99%
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
		else //OCR_MODE_SET.ocr_mode == OCR_SELECT 이고 DI 정보가 안들어 올 때
		{
			OCR51_2.di_err_flag = 1;
			if(OCR51_2.di_on_flag == 1)	{RELAY_STATUS.pickup &= ~F_OCR51_2;}  //계전요소 alarm OFF
			OCR51_2.di_on_flag = 0;
		}
	}
}

void RELAY_OCGR50(void)
{
	if(OCGR50.use == 0xaaaa)
	{
		if(PROTECT.Max_In_RMS >= OCGR50.Pickup_Threshold)
		{
			if(OCGR50.op_status == RELAY_NORMAL)
			{
				OCGR50.op_status = RELAY_DETECT;
				OCGR50.op_count = 0;
			}
			else if(OCGR50.op_status == RELAY_DETECT)
			{
				if(OCGR50.op_count >= OCGR50.pickup_limit)
				{	
					OCGR50.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_OCGR50;  //alarm ON
					OCGR50.Pickup_Time = OCGR50.op_count;
					OCGR50.op_count = 0;
				}
			}
			else if(OCGR50.op_status == RELAY_PICKUP)
			{
				if(OCGR50.op_count >= OCGR50.delay_ms)
				{
					Relay_On(OCGR50.do_output);

					OCGR50.op_status	= RELAY_TRIP;
					OCGR50.Op_Ratio	= PROTECT.Max_In_RMS / OCGR50.Pickup_Threshold; //배수
					OCGR50.Op_Phase	= PROTECT.In_Op_Phase; //상
					OCGR50.Delay_Time = OCGR50.op_count;
					OCGR50.Op_Time		= OCGR50.Delay_Time + OCGR50.Pickup_Time + TOTAL_DELAY_50; //동작 시간

					RELAY_STATUS.pickup									&= ~F_OCGR50; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_OCGR50;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_OCGR50;  //누적 동작 상태 변수 설정

					EVENT.optime = (unsigned long)OCGR50.Op_Time;
					EVENT.operation |= (F_OCGR50 << 16) + OCGR50.Op_Phase;
					Phase_Info = (Phase_Info == 0)? EVENT.operation: OCGR50.Op_Phase;
					EVENT.fault_type = F_OCGR50;
					Save_Relay_Event(OCGR50.Op_Ratio * 100.0F);
					Save_Screen_Info(OCGR50.Op_Phase);
				}
			}
		}
		else
		{
			if(PROTECT.Max_In_RMS <= OCGR50.Dropout_Threshold)  //under 99%
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

void RELAY_OCGR51(void)
{
	if(OCGR51.use == 0xaaaa)
	{
		if(PROTECT.Max_In_RMS >= OCGR51.Pickup_Threshold)
		{
			if(OCGR51.op_status == RELAY_NORMAL)
			{
				OCGR51.op_status = RELAY_DETECT;
				OCGR51.op_count = 0;
			}
			else if(OCGR51.op_status == RELAY_DETECT)
			{
				if(OCGR51.op_count >= OCGR51.pickup_limit)
				{	
					OCGR51.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_OCGR51;  //alarm ON
					OCGR51.Pickup_Time = OCGR51.op_count;
					OCGR51.op_count = 0;
				}
			}
			else if(OCGR51.op_status == RELAY_PICKUP)
			{
				OCGR51.Op_Ratio	= PROTECT.Max_In_RMS / OCGR51.Pickup_Threshold;

				OCGR51.Op_Time_set_temp = Inverse_GetDelayTime(OCGR51.mode, OCGR51.time_lever, OCGR51.Op_Ratio);
				if(OCGR51.Op_Time_set_temp > (INVERSE_PICKUP_LIMIT+TOTAL_DELAY_51G)) OCGR51.Op_Time_set = OCGR51.Op_Time_set_temp - (INVERSE_PICKUP_LIMIT+TOTAL_DELAY_51G);
				else OCGR51.Op_Time_set = OCGR51.Op_Time_set_temp;

				if(OCGR51.op_count >= OCGR51.Op_Time_set)
				{
					Relay_On(OCGR51.do_output);

					OCGR51.op_status	= RELAY_TRIP;
//				OCGR51.Op_Ratio	= PROTECT.Max_In_RMS / OCGR51.Pickup_Threshold; //배수
					OCGR51.Op_Phase	= PROTECT.In_Op_Phase; //상
					OCGR51.Delay_Time = OCGR51.op_count;
					OCGR51.Op_Time		= OCGR51.Delay_Time + OCGR51.Pickup_Time + TOTAL_DELAY_51G; //동작 시간

					RELAY_STATUS.pickup									&= ~F_OCGR51; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_OCGR51;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_OCGR51;  //누적 동작 상태 변수 설정

					EVENT.optime = (unsigned long)OCGR51.Op_Time;
					EVENT.operation |= (F_OCGR51 << 16) + OCGR51.Op_Phase;
					Phase_Info = (Phase_Info == 0)? EVENT.operation: OCGR51.Op_Phase;
					EVENT.fault_type = F_OCGR51;
					Save_Relay_Event(OCGR51.Op_Ratio * 100.0F);
					Save_Screen_Info(OCGR51.Op_Phase);				}
			}
		}
		else
		{
			if(PROTECT.Max_In_RMS <= OCGR51.Dropout_Threshold)  //under 99%
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

void RELAY_UVR_1(void)
{
	if(UVR_1.use == 0xaaaa)
	{
		if(UVR_MODE.setting1 == MANUAL)
		{
			if((MEASUREMENT.rms_value[Va] > UVR_1.Pickup_Threshold)||(MEASUREMENT.rms_value[Vb] > UVR_1.Pickup_Threshold)||(MEASUREMENT.rms_value[Vc] > UVR_1.Pickup_Threshold))	{UVR_1.over_volt_flag = ON;}

			if(PROTECT.Min_V_RMS <= UVR_1.Pickup_Threshold)
			{
				if((UVR_1.op_status == RELAY_NORMAL) && (UVR_1.over_volt_flag==ON))
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
						UVR_1.Op_Ratio	= PROTECT.Min_V_RMS / UVR_1.Pickup_Threshold; //배수
						UVR_1.Op_Phase	= PROTECT.Min_V_Op_Phase; //상
						UVR_1.Delay_Time = UVR_1.op_count;
						UVR_1.Op_Time		= UVR_1.Delay_Time + UVR_1.Pickup_Time + TOTAL_DELAY_27; //동작 시간
	
						RELAY_STATUS.pickup									&= ~F_UVR_1; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_UVR_1;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_UVR_1;  //누적 동작 상태 변수 설정
						
						Save_Screen_Info(UVR_1.Op_Phase);
						
						EVENT.optime = (unsigned long)UVR_1.Op_Time;
						EVENT.operation |= (F_UVR_1 << 16) + UVR_1.Op_Phase;
						EVENT.fault_type = F_UVR_1;
						Phase_Info = (Phase_Info == 0)? EVENT.operation: UVR_1.Op_Phase;
						Save_Relay_Event(UVR_1.Op_Ratio * 100.0F);
						Save_Screen_Info(UVR_1.Op_Phase);						
					}
				}
			}
			else
			{
				if(PROTECT.Min_V_RMS > UVR_1.Dropout_Threshold)  //under 99%
				{
					if((UVR_1.op_status == RELAY_DETECT) || (UVR_1.op_status == RELAY_PICKUP))
					{
						UVR_1.op_status = RELAY_NORMAL;
						RELAY_STATUS.pickup &= ~F_UVR_1; //계전요소 alarm OFF
						UVR_1.over_volt_flag = OFF;
					}
					else if(UVR_1.op_status == RELAY_TRIP)
					{
						Relay_Off(UVR_1.do_output); //DO open
						UVR_1.op_status = RELAY_NORMAL; //UVR_1상태 NORMAL
						RELAY_STATUS.operation_realtime &= ~F_UVR_1; //동작 상태 변수 해제
						UVR_1.over_volt_flag = OFF;
					}
				}
			}
		}
		else //if(UVR_MODE.setting1 == AUTO)
		{
			if(PROTECT.Min_V_RMS <= UVR_1.Pickup_Threshold)
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
						UVR_1.Op_Ratio	= PROTECT.Min_V_RMS / UVR_1.Pickup_Threshold; //배수
						UVR_1.Op_Phase	= PROTECT.Min_V_Op_Phase; //상
						UVR_1.Delay_Time = UVR_1.op_count;
						UVR_1.Op_Time		= UVR_1.Delay_Time + UVR_1.Pickup_Time + TOTAL_DELAY_27; //동작 시간
	
						RELAY_STATUS.pickup									&= ~F_UVR_1; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_UVR_1;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_UVR_1;  //누적 동작 상태 변수 설정
						
						Save_Screen_Info(UVR_1.Op_Phase);
					}
				}
			}
			else
			{
				if(PROTECT.Min_V_RMS > UVR_1.Dropout_Threshold)  //under 99%
				{
					if((UVR_1.op_status == RELAY_DETECT) || (UVR_1.op_status == RELAY_PICKUP))
					{
						UVR_1.op_status = RELAY_NORMAL;
						RELAY_STATUS.pickup &= ~F_UVR_1; //계전요소 alarm OFF
						UVR_1.over_volt_flag = OFF;
					}
					else if(UVR_1.op_status == RELAY_TRIP)
					{
						Relay_Off(UVR_1.do_output); //DO open
						UVR_1.op_status = RELAY_NORMAL; //UVR_1상태 NORMAL
						RELAY_STATUS.operation_realtime &= ~F_UVR_1; //동작 상태 변수 해제
						UVR_1.over_volt_flag = OFF;
					}
				}
			}
		}
	}
}

void RELAY_UVR_2(void)
{
	if(UVR_2.use == 0xaaaa)
	{
		if(UVR_MODE.setting2 == MANUAL)
		{
			if((MEASUREMENT.rms_value[Va] > UVR_2.Pickup_Threshold)||(MEASUREMENT.rms_value[Vb] > UVR_2.Pickup_Threshold)||(MEASUREMENT.rms_value[Vc] > UVR_2.Pickup_Threshold))	{UVR_2.over_volt_flag = ON;}

			if(PROTECT.Min_V_RMS <= UVR_2.Pickup_Threshold)
			{
				if((UVR_2.op_status == RELAY_NORMAL) && (UVR_2.over_volt_flag==ON))
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
						UVR_2.Op_Ratio	= PROTECT.Min_V_RMS / UVR_2.Pickup_Threshold; //배수
						UVR_2.Op_Phase	= PROTECT.Min_V_Op_Phase; //상
						UVR_2.Delay_Time = UVR_2.op_count;
						UVR_2.Op_Time		= UVR_2.Delay_Time + UVR_2.Pickup_Time + TOTAL_DELAY_27; //동작 시간
	
						RELAY_STATUS.pickup									&= ~F_UVR_2; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_UVR_2;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_UVR_2;  //누적 동작 상태 변수 설정
						
						EVENT.optime = (unsigned long)UVR_2.Op_Time;
						EVENT.operation |= (F_UVR_2 << 16) + UVR_2.Op_Phase;
						EVENT.fault_type = F_UVR_2;
						Phase_Info = (Phase_Info == 0)? EVENT.operation: UVR_2.Op_Phase;
						Save_Relay_Event(UVR_2.Op_Ratio * 100.0F);
						Save_Screen_Info(UVR_2.Op_Phase);		
					}
				}
			}
			else
			{
				if(PROTECT.Min_V_RMS > UVR_2.Dropout_Threshold)  //under 99%
				{
					if((UVR_2.op_status == RELAY_DETECT) || (UVR_2.op_status == RELAY_PICKUP))
					{
						UVR_2.op_status = RELAY_NORMAL;
						RELAY_STATUS.pickup &= ~F_UVR_2; //계전요소 alarm OFF
						UVR_2.over_volt_flag = OFF;
					}
					else if(UVR_2.op_status == RELAY_TRIP)
					{
						Relay_Off(UVR_2.do_output); //DO open
						UVR_2.op_status = RELAY_NORMAL; //UVR_2상태 NORMAL
						RELAY_STATUS.operation_realtime &= ~F_UVR_2; //동작 상태 변수 해제
						UVR_2.over_volt_flag = OFF;
					}
				}
			}
		}
		else //if(UVR_MODE.setting1 == AUTO)
		{
			if(PROTECT.Min_V_RMS <= UVR_2.Pickup_Threshold)
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
						UVR_2.Op_Ratio	= PROTECT.Min_V_RMS / UVR_2.Pickup_Threshold; //배수
						UVR_2.Op_Phase	= PROTECT.Min_V_Op_Phase; //상
						UVR_2.Delay_Time = UVR_2.op_count;
						UVR_2.Op_Time		= UVR_2.Delay_Time + UVR_2.Pickup_Time + TOTAL_DELAY_27; //동작 시간
	
						RELAY_STATUS.pickup									&= ~F_UVR_2; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_UVR_2;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_UVR_2;  //누적 동작 상태 변수 설정
						
						Save_Screen_Info(UVR_2.Op_Phase);
					}
				}
			}
			else
			{
				if(PROTECT.Min_V_RMS > UVR_2.Dropout_Threshold)  //under 99%
				{
					if((UVR_2.op_status == RELAY_DETECT) || (UVR_2.op_status == RELAY_PICKUP))
					{
						UVR_2.op_status = RELAY_NORMAL;
						RELAY_STATUS.pickup &= ~F_UVR_2; //계전요소 alarm OFF
						UVR_2.over_volt_flag = OFF;
					}
					else if(UVR_2.op_status == RELAY_TRIP)
					{
						Relay_Off(UVR_2.do_output); //DO open
						UVR_2.op_status = RELAY_NORMAL; //UVR_2상태 NORMAL
						RELAY_STATUS.operation_realtime &= ~F_UVR_2; //동작 상태 변수 해제
						UVR_2.over_volt_flag = OFF;
					}
				}
			}
		}
	}
}

void RELAY_UVR_3(void)
{
	if(UVR_3.use == 0xaaaa)
	{
		if(UVR_MODE.setting3 == MANUAL)
		{
			if((MEASUREMENT.rms_value[Va] > UVR_3.Pickup_Threshold)||(MEASUREMENT.rms_value[Vb] > UVR_3.Pickup_Threshold)||(MEASUREMENT.rms_value[Vc] > UVR_3.Pickup_Threshold))	{UVR_3.over_volt_flag = ON;}

			if(PROTECT.Min_V_RMS <= UVR_3.Pickup_Threshold)
			{
				if((UVR_3.op_status == RELAY_NORMAL) && (UVR_3.over_volt_flag==ON))
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
						UVR_3.Op_Ratio	= PROTECT.Min_V_RMS / UVR_3.Pickup_Threshold; //배수
						UVR_3.Op_Phase	= PROTECT.Min_V_Op_Phase; //상
						UVR_3.Delay_Time = UVR_3.op_count;
						UVR_3.Op_Time		= UVR_3.Delay_Time + UVR_3.Pickup_Time + TOTAL_DELAY_27; //동작 시간
	
						RELAY_STATUS.pickup									&= ~F_UVR_3; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_UVR_3;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_UVR_3;  //누적 동작 상태 변수 설정
						
						EVENT.optime = (unsigned long)UVR_3.Op_Time;
						EVENT.operation |= (F_UVR_3 << 16) + UVR_3.Op_Phase;
						EVENT.fault_type = F_UVR_3;
						Phase_Info = (Phase_Info == 0)? EVENT.operation: UVR_3.Op_Phase;
						Save_Relay_Event(UVR_3.Op_Ratio * 100.0F);
						Save_Screen_Info(UVR_3.Op_Phase);							}
				}
			}
			else
			{
				if(PROTECT.Min_V_RMS > UVR_3.Dropout_Threshold)  //under 99%
				{
					if((UVR_3.op_status == RELAY_DETECT) || (UVR_3.op_status == RELAY_PICKUP))
					{
						UVR_3.op_status = RELAY_NORMAL;
						RELAY_STATUS.pickup &= ~F_UVR_3; //계전요소 alarm OFF
						UVR_3.over_volt_flag = OFF;
					}
					else if(UVR_3.op_status == RELAY_TRIP)
					{
						Relay_Off(UVR_3.do_output); //DO open
						UVR_3.op_status = RELAY_NORMAL; //UVR_3상태 NORMAL
						RELAY_STATUS.operation_realtime &= ~F_UVR_3; //동작 상태 변수 해제
						UVR_3.over_volt_flag = OFF;
					}
				}
			}
		}
		else //if(UVR_MODE.setting1 == AUTO)
		{
			if(PROTECT.Min_V_RMS <= UVR_3.Pickup_Threshold)
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
						UVR_3.Op_Ratio	= PROTECT.Min_V_RMS / UVR_3.Pickup_Threshold; //배수
						UVR_3.Op_Phase	= PROTECT.Min_V_Op_Phase; //상
						UVR_3.Delay_Time = UVR_3.op_count;
						UVR_3.Op_Time		= UVR_3.Delay_Time + UVR_3.Pickup_Time + TOTAL_DELAY_27; //동작 시간
	
						RELAY_STATUS.pickup									&= ~F_UVR_3; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_UVR_3;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_UVR_3;  //누적 동작 상태 변수 설정
						
						Save_Screen_Info(UVR_3.Op_Phase);
					}
				}
			}
			else
			{
				if(PROTECT.Min_V_RMS > UVR_3.Dropout_Threshold)  //under 99%
				{
					if((UVR_3.op_status == RELAY_DETECT) || (UVR_3.op_status == RELAY_PICKUP))
					{
						UVR_3.op_status = RELAY_NORMAL;
						RELAY_STATUS.pickup &= ~F_UVR_3; //계전요소 alarm OFF
						UVR_3.over_volt_flag = OFF;
					}
					else if(UVR_3.op_status == RELAY_TRIP)
					{
						Relay_Off(UVR_3.do_output); //DO open
						UVR_3.op_status = RELAY_NORMAL; //UVR_3상태 NORMAL
						RELAY_STATUS.operation_realtime &= ~F_UVR_3; //동작 상태 변수 해제
						UVR_3.over_volt_flag = OFF;
					}
				}
			}
		}
	}
}

void RELAY_P47(void)
{
	if(P47.use == 0xaaaa)
	{
		if(PROTECT.V1_RMS > P47.Pickup_Threshold)	{P47.over_volt_flag = ON;}

		if(P47.op_status == RELAY_NORMAL)
		{
			if((PROTECT.V1_RMS <= P47.Pickup_Threshold) && (P47.over_volt_flag==ON))
			{
				P47.op_status = RELAY_DETECT;
				P47.op_count = 0;
			}
		}

		if(P47.op_status == RELAY_DETECT)
		{
			if((PROTECT.V1_RMS > P47.Dropout_Threshold)||(PROTECT.V1_RMS < 5.0))
			{
					P47.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_P47; //계전요소 alarm OFF
					P47.over_volt_flag = OFF;
					return;
			}
			
			if(P47.op_count > P47.pickup_limit)
			{	
				P47.op_status = RELAY_PICKUP;
				RELAY_STATUS.pickup |= F_P47;  //alarm ON
				P47.Pickup_Time = P47.op_count;
				P47.op_count = 0;
			}
		}

		if(P47.op_status == RELAY_PICKUP)
		{
			if((PROTECT.V1_RMS > P47.Dropout_Threshold)||(PROTECT.V1_RMS < 5.0))
			{
				P47.op_status = RELAY_NORMAL;
				RELAY_STATUS.pickup &= ~F_P47; //계전요소 alarm OFF
				P47.over_volt_flag = OFF;
				return;
			}

			if(P47.op_count > P47.delay_ms)
			{
				Relay_On(P47.do_output);

				P47.op_status	= RELAY_TRIP;
				P47.Op_Ratio	= PROTECT.V1_RMS / P47.Pickup_Threshold; //배수
				P47.Op_Phase	= PROTECT.Min_V_Op_Phase; //상
				P47.Delay_Time = P47.op_count;
				P47.Op_Time		= P47.Delay_Time + P47.Pickup_Time + TOTAL_DELAY_47P; //동작 시간

				RELAY_STATUS.pickup									&= ~F_P47; //계전요소 alarm OFF
				RELAY_STATUS.operation_realtime			|= F_P47;  //현재 동작 상태 변수 설정
				RELAY_STATUS.operation_sum_holding	|= F_P47;  //누적 동작 상태 변수 설정
				
				EVENT.optime = (unsigned long)P47.Op_Time;
				EVENT.operation |= (F_P47 << 16) + P47.Op_Phase;
				EVENT.fault_type = F_P47;
				Phase_Info = (Phase_Info == 0)? EVENT.operation: P47.Op_Phase;
				Save_Relay_Event(P47.Op_Ratio * 100.0F);
				Save_Screen_Info(P47.Op_Phase);		
			}
		}

		if(P47.op_status == RELAY_TRIP)
		{
			if((PROTECT.V1_RMS > P47.Dropout_Threshold)||(PROTECT.V1_RMS < 5.0))  //under 99% or 5V 보다 작으면
			{			
				Relay_Off(P47.do_output); //DO open
				P47.op_status = RELAY_NORMAL; //P47상태 NORMAL
				RELAY_STATUS.operation_realtime &= ~F_P47; //동작 상태 변수 해제
				P47.over_volt_flag = OFF;
			}
		}
	}
}

void RELAY_N47(void)
{
	if(N47.use == 0xaaaa)
	{
		if(PROTECT.V2_RMS >= N47.Pickup_Threshold)
		{
			if(N47.op_status == RELAY_NORMAL)
			{
				N47.op_status = RELAY_DETECT;
				N47.op_count = 0;
			}
			else if(N47.op_status == RELAY_DETECT)
			{
				if(N47.op_count >= N47.pickup_limit)
				{	
					N47.op_status = RELAY_PICKUP;
					RELAY_STATUS.pickup |= F_N47;  //alarm ON
					N47.Pickup_Time = N47.op_count;
					N47.op_count = 0;
				}
			}
			else if(N47.op_status == RELAY_PICKUP)
			{
				if(N47.op_count >= N47.delay_ms)
				{
					Relay_On(N47.do_output);

					N47.op_status	= RELAY_TRIP;
					N47.Op_Ratio	= PROTECT.V2_RMS / N47.Pickup_Threshold; //배수 V_Op_Phase
					N47.Op_Phase	= PROTECT.V_Op_Phase; //상
					N47.Delay_Time = N47.op_count;
					N47.Op_Time		= N47.Delay_Time + N47.Pickup_Time + TOTAL_DELAY_47N; //동작 시간

					RELAY_STATUS.pickup									&= ~F_N47; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_N47;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_N47;  //누적 동작 상태 변수 설정

					EVENT.optime = (unsigned long)N47.Op_Time;
					EVENT.operation |= (F_N47 << 16) + N47.Op_Phase;
					EVENT.fault_type = F_N47;
					Phase_Info = (Phase_Info == 0)? EVENT.operation: N47.Op_Phase;
					Save_Relay_Event(N47.Op_Ratio * 100.0F);
					Save_Screen_Info(N47.Op_Phase);		
				}
			}
		}
		else
		{
			if(PROTECT.V2_RMS <= N47.Dropout_Threshold)  //under 99%
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

void RELAY_OVR(void)
{
	if(OVR.use == 0xaaaa)
	{
		if(OVR.mode==DEFINITE)
		{
			if(PROTECT.Max_V_RMS >= OVR.Pickup_Threshold)
			{
				if(OVR.op_status == RELAY_NORMAL)
				{
					OVR.op_status = RELAY_DETECT;
					OVR.op_count = 0;
				}
				else if(OVR.op_status == RELAY_DETECT)
				{
					if(OVR.op_count >= OVR.pickup_limit)
					{	
						OVR.op_status = RELAY_PICKUP;
						RELAY_STATUS.pickup |= F_OVR;  //alarm ON
						OVR.Pickup_Time = OVR.op_count;
						OVR.op_count = 0;
					}
				}
				else if(OVR.op_status == RELAY_PICKUP)
				{
					if(OVR.op_count >= OVR.delay_ms)
					{
						Relay_On(OVR.do_output);
	
						OVR.op_status	= RELAY_TRIP;
						OVR.Op_Ratio	= PROTECT.Max_V_RMS / OVR.Pickup_Threshold; //배수 V_Op_Phase
						OVR.Op_Phase	= PROTECT.V_Op_Phase; //상
						OVR.Delay_Time = OVR.op_count;
						OVR.Op_Time		= OVR.Delay_Time + OVR.Pickup_Time + TOTAL_DELAY_59; //동작 시간
	
						RELAY_STATUS.pickup									&= ~F_OVR; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_OVR;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_OVR;  //누적 동작 상태 변수 설정
	
						EVENT.optime = (unsigned long)OVR.Op_Time;
						EVENT.operation |= (F_OVR << 16) + OVR.Op_Phase;
						EVENT.fault_type = F_OVR;
						Phase_Info = (Phase_Info == 0)? EVENT.operation: OVR.Op_Phase;
						Save_Relay_Event(OVR.Op_Ratio * 100.0F);
						Save_Screen_Info(OVR.Op_Phase);		
					}
				}
			}
			else
			{
				if(PROTECT.Max_V_RMS <= OVR.Dropout_Threshold)  //under 99%
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
		else if(OVR.mode==INVERSE)
		{
			if(PROTECT.Max_V_RMS >= OVR.Pickup_Threshold)
			{
				if(OVR.op_status == RELAY_NORMAL)
				{
					OVR.op_status = RELAY_DETECT;
					OVR.op_count = 0;
				}
				else if(OVR.op_status == RELAY_DETECT)
				{
					if(OVR.op_count >= OVR.pickup_limit)
					{	
						OVR.op_status = RELAY_PICKUP;
						RELAY_STATUS.pickup |= F_OVR;  //alarm ON
						OVR.Pickup_Time = OVR.op_count;
						OVR.op_count = 0;
					}
				}
				else if(OVR.op_status == RELAY_PICKUP)
				{
					OVR.Op_Ratio	= PROTECT.Max_V_RMS / OVR.Pickup_Threshold;
	
					OVR.Op_Time_set_temp = Inverse_GetDelayTime(OVR.mode, OVR.time_lever, OVR.Op_Ratio);
					if(OVR.Op_Time_set_temp > (INVERSE_PICKUP_LIMIT+TOTAL_DELAY_59)) OVR.Op_Time_set = OVR.Op_Time_set_temp - (INVERSE_PICKUP_LIMIT+TOTAL_DELAY_59);
					else OVR.Op_Time_set = OVR.Op_Time_set_temp;
	
					if(OVR.op_count >= OVR.Op_Time_set)
					{
						Relay_On(OVR.do_output);
	
						OVR.op_status	= RELAY_TRIP;
						OVR.Op_Phase	= PROTECT.V_Op_Phase; //상
						OVR.Delay_Time = OVR.op_count;
						OVR.Op_Time		= OVR.Delay_Time + OVR.Pickup_Time + TOTAL_DELAY_59; //동작 시간
	
						RELAY_STATUS.pickup									&= ~F_OVR; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_OVR;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_OVR;  //누적 동작 상태 변수 설정
	
						Save_Screen_Info(OVR.Op_Phase);
					}
				}
			}
			else
			{
				if(PROTECT.Max_V_RMS <= OVR.Dropout_Threshold)  //under 99%
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
}

void RELAY_OVGR(void)
{
	if(OVGR.use == 0xaaaa)
	{
		if(OVGR.mode==INSTANT)
		{
			if(PROTECT.Max_Vn_RMS >= OVGR.Pickup_Threshold)
			{
				if(OVGR.op_status == RELAY_NORMAL)
				{
					OVGR.op_status = RELAY_DETECT;
					OVGR.op_count = 0;
				}
				else if(OVGR.op_status == RELAY_DETECT)
				{
					if(OVGR.op_count >= OVGR.pickup_limit)
					{	
						OVGR.op_status = RELAY_PICKUP;
						RELAY_STATUS.pickup |= F_OVGR;  //alarm ON
						OVGR.Pickup_Time = OVGR.op_count;
						OVGR.op_count = 0;
					}
				}
				else if(OVGR.op_status == RELAY_PICKUP)
				{
					if(OVGR.op_count >= OVGR.delay_ms)
					{
						Relay_On(OVGR.do_output);
	
						OVGR.op_status	= RELAY_TRIP;
						OVGR.Op_Ratio	= PROTECT.Max_Vn_RMS / OVGR.Pickup_Threshold; //배수 V_Op_Phase
						OVGR.Op_Phase	= PROTECT.Vn_Op_Phase; //상
						OVGR.Delay_Time = OVGR.op_count;
						OVGR.Op_Time		= OVGR.Delay_Time + OVGR.Pickup_Time + TOTAL_DELAY_64; //동작 시간
	
						RELAY_STATUS.pickup									&= ~F_OVGR; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_OVGR;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_OVGR;  //누적 동작 상태 변수 설정
	
						EVENT.optime = (unsigned long)OVGR.Op_Time;
						EVENT.operation |= (F_OVGR << 16) + OVGR.Op_Phase;
						EVENT.fault_type = F_OVGR;
						Phase_Info = (Phase_Info == 0)? EVENT.operation: OVGR.Op_Phase;
						Save_Relay_Event(OVGR.Op_Ratio * 100.0F);
						Save_Screen_Info(OVGR.Op_Phase);		
					}
				}
			}
			else
			{
				if(PROTECT.Max_Vn_RMS <= OVGR.Dropout_Threshold)  //under 99%
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
		else if(OVGR.mode==INVERSE)
		{
			if(PROTECT.Max_Vn_RMS >= OVGR.Pickup_Threshold)
			{
				if(OVGR.op_status == RELAY_NORMAL)
				{
					OVGR.op_status = RELAY_DETECT;
					OVGR.op_count = 0;
				}
				else if(OVGR.op_status == RELAY_DETECT)
				{
					if(OVGR.op_count >= OVGR.pickup_limit)
					{	
						OVGR.op_status = RELAY_PICKUP;
						RELAY_STATUS.pickup |= F_OVGR;  //alarm ON
						OVGR.Pickup_Time = OVGR.op_count;
						OVGR.op_count = 0;
					}
				}
				else if(OVGR.op_status == RELAY_PICKUP)
				{
					OVGR.Op_Ratio	= PROTECT.Max_Vn_RMS / OVGR.Pickup_Threshold;
	
					OVGR.Op_Time_set_temp = Inverse_GetDelayTime(OVGR.mode, OVGR.time_lever, OVGR.Op_Ratio);
					if(OVGR.Op_Time_set_temp > (INVERSE_PICKUP_LIMIT+TOTAL_DELAY_64)) OVGR.Op_Time_set = OVGR.Op_Time_set_temp - (INVERSE_PICKUP_LIMIT+TOTAL_DELAY_64);
					else OVGR.Op_Time_set = OVGR.Op_Time_set_temp;
	
					if(OVGR.op_count >= OVGR.Op_Time_set)
					{
						Relay_On(OVGR.do_output);
	
						OVGR.op_status	= RELAY_TRIP;
						OVGR.Op_Phase	= PROTECT.Vn_Op_Phase; //상
						OVGR.Delay_Time = OVGR.op_count;
						OVGR.Op_Time		= OVGR.Delay_Time + OVGR.Pickup_Time + TOTAL_DELAY_64; //동작 시간
	
						RELAY_STATUS.pickup									&= ~F_OVGR; //계전요소 alarm OFF
						RELAY_STATUS.operation_realtime			|= F_OVGR;  //현재 동작 상태 변수 설정
						RELAY_STATUS.operation_sum_holding	|= F_OVGR;  //누적 동작 상태 변수 설정
	
						Save_Screen_Info(OVGR.Op_Phase);
					}
				}
			}
			else
			{
				if(PROTECT.Max_Vn_RMS <= OVGR.Dropout_Threshold)  //under 99%
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
}

void RELAY_DGR(void)
{
	if(DGR.use == 0xaaaa)
	{
		// 위상차
		DGR.diff_angle_rad = MEASUREMENT.angle[In] - MEASUREMENT.angle[Vn];
		DGR.diff_angle_deg = DGR.diff_angle_rad * 57.29577951; // degree로 환산 180/PI = 57.29577951
		if(DGR.diff_angle_deg < 0.)	{DGR.diff_angle_deg += 360;}
		
		if((PROTECT.Max_In_RMS >= DGR.Pickup_Threshold_Io) && (PROTECT.Max_Vn_RMS >= DGR.Pickup_Threshold_Vo) && ((DGR.diff_angle_deg >= DGR.angle_low) || (DGR.diff_angle_deg <= DGR.angle_high)))
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
					DGR.Op_Ratio	= PROTECT.Max_I_RMS / DGR.Pickup_Threshold_Vo; //배수
					DGR.Op_Phase	= PROTECT.V_Op_Phase; //상
					DGR.Delay_Time = DGR.op_count;
					DGR.Op_Time		= DGR.Delay_Time + DGR.Pickup_Time + TOTAL_DELAY_67GD; //동작 시간

					RELAY_STATUS.pickup									&= ~F_DGR; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_DGR;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_DGR;  //누적 동작 상태 변수 설정
					
					EVENT.optime = (unsigned long)DGR.Op_Time;
					EVENT.operation |= (F_DGR << 16) + DGR.Op_Phase;
					EVENT.fault_type = F_DGR;
					Phase_Info = (Phase_Info == 0)? EVENT.operation: DGR.Op_Phase;
					Save_Relay_Event(DGR.Op_Ratio * 100.0F);
					Save_Screen_Info(DGR.Op_Phase);		
				}
			}
		}
		else //		if((PROTECT.Max_In_RMS >= DGR.Pickup_Threshold_Io) && (PROTECT.Max_Vn_RMS >= DGR.Pickup_Threshold_Vo) && ((DGR.angle_sub >= DGR.angle_low) || (DGR.angle_sub <= DGR.angle_high)))
		{
			if((PROTECT.Max_In_RMS < DGR.Dropout_Threshold_Io) || (PROTECT.Max_Vn_RMS < DGR.Dropout_Threshold_Vo) || ((DGR.diff_angle_deg < DGR.angle_low) && (DGR.diff_angle_deg > DGR.angle_high)))  //under 97%
			{
				if((DGR.op_status == RELAY_DETECT) || (DGR.op_status == RELAY_PICKUP))
				{
					DGR.op_status = RELAY_NORMAL;
					RELAY_STATUS.pickup &= ~F_DGR; //계전요소 alarm OFF
				}
				else if(DGR.op_status == RELAY_TRIP)
				{
					Relay_Off(DGR.do_output);
					DGR.op_status = RELAY_NORMAL; 
					RELAY_STATUS.operation_realtime &= ~F_DGR; //동작 상태 변수 해제
				}
			}
		}
	}
}

void RELAY_SGR(void)
{
	if(SGR.use == 0xaaaa)
	{
		if(PROTECT.Max_I_RMS > SGR.Pickup_Threshold_Vo)
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
					SGR.Op_Ratio	= PROTECT.Max_I_RMS / SGR.Pickup_Threshold_Vo; //배수
					SGR.Op_Phase	= PROTECT.V_Op_Phase; //상
					SGR.Delay_Time = SGR.op_count;
					SGR.Op_Time		= SGR.Delay_Time + SGR.Pickup_Time + TOTAL_DELAY_50; //동작 시간

					RELAY_STATUS.pickup									&= ~F_SGR; //계전요소 alarm OFF
					RELAY_STATUS.operation_realtime			|= F_SGR;  //현재 동작 상태 변수 설정
					RELAY_STATUS.operation_sum_holding	|= F_SGR;  //누적 동작 상태 변수 설정
					
					EVENT.optime = (unsigned long)SGR.Op_Time;
					EVENT.operation |= (F_SGR << 16) + SGR.Op_Phase;
					EVENT.fault_type = F_SGR;
					Phase_Info = (Phase_Info == 0)? EVENT.operation: SGR.Op_Phase;
					Save_Relay_Event(SGR.Op_Ratio * 100.0F);
					Save_Screen_Info(SGR.Op_Phase);		
				}
			}
		}
		else
		{
			if(PROTECT.Max_I_RMS < SGR.Dropout_Threshold_Vo)  //under 99%
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

void PROTECTIVE_RELAY(void)
{
	//-------- 최대 전류 상 저장
	PROTECT.I_Op_Phase = Ia+1;
	PROTECT.Max_I_RMS = MEASUREMENT.rms_value[Ia];
	if(PROTECT.Max_I_RMS  < MEASUREMENT.rms_value[Ib])
	{
		PROTECT.I_Op_Phase = Ib+1;
		PROTECT.Max_I_RMS = MEASUREMENT.rms_value[Ib];
	}
	if(PROTECT.Max_I_RMS < MEASUREMENT.rms_value[Ic])
	{
		PROTECT.I_Op_Phase = Ic+1;
		PROTECT.Max_I_RMS = MEASUREMENT.rms_value[Ic];
	}
	PROTECT.In_Op_Phase = In+1;
	PROTECT.Max_In_RMS = MEASUREMENT.rms_value[In];

	PROTECT.Is_Op_Phase = Is+1;
	PROTECT.Max_Is_RMS = MEASUREMENT.rms_value[Is];
	//-------- 최대 전류 상 저장 END
	
	//-------- 최대 전압 상 저장
	PROTECT.V_Op_Phase = Va+1;
	PROTECT.Max_V_RMS = MEASUREMENT.rms_value[Va];
	if(PROTECT.Max_V_RMS  < MEASUREMENT.rms_value[Vb])
	{
		PROTECT.V_Op_Phase = Vb+1;
		PROTECT.Max_V_RMS = MEASUREMENT.rms_value[Vb];
	}
	if(PROTECT.Max_V_RMS < MEASUREMENT.rms_value[Vc])
	{
		PROTECT.V_Op_Phase = Vc+1;
		PROTECT.Max_V_RMS = MEASUREMENT.rms_value[Vc];
	}
	PROTECT.Vn_Op_Phase = Vn+1;
	PROTECT.Max_Vn_RMS = MEASUREMENT.rms_value[Vn];
	//-------- 최대 전압 상 저장 END

	//-------- 최소 전압 상 저장
	PROTECT.Min_V_Op_Phase = Va+1;
	PROTECT.Min_V_RMS = MEASUREMENT.rms_value[Va];
	if(PROTECT.Min_V_RMS  > MEASUREMENT.rms_value[Vb])
	{
		PROTECT.Min_V_Op_Phase = Vb+1;
		PROTECT.Min_V_RMS = MEASUREMENT.rms_value[Vb];
	}
	if(PROTECT.Min_V_RMS > MEASUREMENT.rms_value[Vc])
	{
		PROTECT.Min_V_Op_Phase = Vc+1;
		PROTECT.Min_V_RMS = MEASUREMENT.rms_value[Vc];
	}
	//-------- 최소 전압 상 저장 END

	//-------- 정상/역상 전압 저장
	PROTECT.V1_RMS = MEASUREMENT.V1_value;
	PROTECT.V2_RMS = MEASUREMENT.V2_value;
	//-------- 정상/역상 전압 저장 END

	OCR_MODE_SET.ocr_di_mask = (DIGITAL_INPUT.di_status & 0x018);

	RELAY_OCR50_1();
	RELAY_OCR50_2();
	RELAY_OCR51_1();
	RELAY_OCR51_2();
	RELAY_OCGR50();
	RELAY_OCGR51();
	RELAY_UVR_1();
	RELAY_UVR_2();
	RELAY_UVR_3();
	RELAY_P47();
	RELAY_N47();
	RELAY_OVR();
	RELAY_OVGR();
	RELAY_DGR();
//	RELAY_SGR();

	SAMPLE.ending = 0;
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


