#include "define.h"
#include "extern_variable.h"
#include "extern_prototype.h"

// 설정값 flash 저장
// *ar_temp - 값이 저장된 전역변수
// *ar_address - 값을 저장할 flash 주소
// ar_wordcount - 저장할 word 단위 개수
// return - 저장 양호 여부
unsigned int setting_save(unsigned int *ar_temp, unsigned int *ar_address, unsigned int ar_wordcount)
{
	unsigned int crc;
	unsigned int i;
	
	//crc 계산
	crc = Setting_CRC(ar_temp, ar_wordcount);
	
	// 해당 섹터 지움
	flash_sector_erase(ar_address);
	
	// 지정된 개수만 큼 flash에 저장
	for(i = 0; i < ar_wordcount; i++)
	flash_word_write((ar_address + i), *(ar_temp + i));
	
	// crc 추가로 저장
	flash_word_write((ar_address + i), crc);
	
	// 저장된 값 비교
	for(i = 0; i < ar_wordcount; i++)
	{
		// 하나라도 틀리면 fail
		if(*(ar_temp + i) != *(ar_address + i))
		return(0);
	}
	
// 설정 값에 따른 이벤트 저장
//---------계전요소 설정 이벤트 저장
//	if(ar_address == OCR50_1_USE)				{EVENT.relay_set |= OCR50_1_SET_EVENT;}
//	else if(ar_address == OCR50_2_USE)	{EVENT.relay_set |= OCR50_2_SET_EVENT;}
//	else if(ar_address == OCR51_1_USE)	{EVENT.relay_set |= OCR51_1_SET_EVENT;}
//	else if(ar_address == OCR51_2_USE)	{EVENT.relay_set |= OCR51_2_SET_EVENT;}
//	else if(ar_address == OCGR50_USE)		{EVENT.relay_set |= OCGR50_SET_EVENT;}
//	else if(ar_address == OCGR51_USE)		{EVENT.relay_set |= OCGR51_SET_EVENT;}
//	else if(ar_address == UVR_1_USE)		{EVENT.relay_set |= UVR_1_SET_EVENT;}
//	else if(ar_address == UVR_2_USE)		{EVENT.relay_set |= UVR_2_SET_EVENT;}
//	else if(ar_address == UVR_3_USE)		{EVENT.relay_set |= UVR_3_SET_EVENT;}
//	else if(ar_address == P47_USE)			{EVENT.relay_set |= P47_SET_EVENT;}
//	else if(ar_address == N47_USE)			{EVENT.relay_set |= N47_SET_EVENT;}
//	else if(ar_address == OVR_USE)			{EVENT.relay_set |= OVR_SET_EVENT;}
//	else if(ar_address == OVGR_USE)			{EVENT.relay_set |= OVGR_SET_EVENT;}

//2015.02.24
// 설정 값에 따라 메뉴에서 보이게 안보이게 필요
//	else if(ar_address == DSGR_USE)
//	{
//		if(CORE.gr_select == ZCT_SELECT)
//		EVENT.relay_set |= SGR_SET_EVENT;
//		
//		else
//		EVENT.relay_set |= DGR_SET_EVENT;
//	}
//2015.02.24 END
//	else if(ar_address == SYNCRO_USE)
//	EVENT.relay_set |= SYNCRO_SET_EVENT;
//---------계전요소 설정 이벤트 저장 END
 
//---------시스템요소 설정 이벤트 저장
//	else if(ar_address == CT_PRIMARY)
//	EVENT.system_set |= CT_PT_SET_EVENT;
//	
//	else if(ar_address == DI_DEBOUNCE1)
//	{
//		if(EVENT.group_extra == 0)
//		EVENT.system_set |= DI_DEBOUNCE_EVENT;
//		
//		else
//		EVENT.system_set |= DO_PROPERTY_EVENT;
//	}
//	
//	else if(ar_address == SUPERV_MODE)
//	EVENT.system_set |= SUPERVISION_SET_EVENT;
//	
//	else if(ar_address == PASSWORD1)
//	EVENT.system_set |= PASSWORD_SET_EVENT;
//		
//	else if(ar_address == COMM_ADDRESS)
//	EVENT.system_set |= COMM_SET_EVENT;
//	
//	else if(ar_address == LOCAL_CTRL_USE) //2015.02.24
//	EVENT.system_set |= L_CONTROL_SET_EVENT;
//	
//	else if(ar_address == OCGR_DGR_SEL)
//	{
//		if(EVENT.group_extra == 0)
//		EVENT.system_set |= GR_SET_EVENT;
//		
//		else
//		EVENT.system_set |= ZCT_ANGLE_SET_EVENT;
//	}
//	
//	else if(ar_address == OCR_PROPERTY)
//	EVENT.system_set |= OCR_MODE_SET_EVENT;
//---------시스템요소 설정 이벤트 저장 END	
	
	if(ar_address < CT_PRIMARY)	{event_direct_save(&EVENT.relay_set);}
	else												{event_direct_save(&EVENT.system_set);}
	return(1);
}

// crc가 정상일 경우 사후처리
// *ar_address - flash 시작 주소
// 상기 주소를 가지고 설정 값이 어떤 것인지 판단
// 필요로 하는 내부변수에 반영함
void setting_post_handling(unsigned int *ar_address)
{
	unsigned int i;
	float float_temp1;
	float float_temp2;

	if(ar_address == PT_SECONDARY)
	{
		CPT.pt_primary = CPT.pt_primary_high & 0xffff;
		CPT.pt_primary <<= 16;
		CPT.pt_primary += (CPT.pt_primary_low & 0xffff);
		float_temp1 = (float)CPT.pt_primary;
		float_temp2 = (float)GPT.pt_secondary;
		CPT.pt_ratio = float_temp1 / float_temp2;

		float_temp2 = (float)GPT.pt_tertiary;
		CPT.gpt_ratio = float_temp1 / float_temp2;

//-------- display용 factor
		DISPLAY.multipllier[0] = CPT.ct_ratio;
		DISPLAY.multipllier[1] = CPT.ct_ratio;
		DISPLAY.multipllier[2] = CPT.ct_ratio;
		DISPLAY.multipllier[3] = CPT.nct_ratio;
		DISPLAY.multipllier[4] = CPT.nct_ratio;
		//ZCT
		//DISPLAY.multipllier[5] = 133.33333333333333333333333333333;
		DISPLAY.multipllier[6] = CPT.pt_ratio;
		DISPLAY.multipllier[7] = CPT.pt_ratio;
		DISPLAY.multipllier[8] = CPT.pt_ratio;
		DISPLAY.multipllier[9] = CPT.gpt_ratio;
	}

	else if(ar_address == CT_PRIMARY)
	{
		if(CORE.rated_ct == CT_1A)
		{
			CPT.ct_ratio = (float)CPT.ct_primary;
			CPT.nct_ratio = (float)CPT.nct_primary;
		}
		else //CT_5A
		{
			CPT.ct_ratio = (float)CPT.ct_primary;
			CPT.nct_ratio = (float)CPT.nct_primary;
			CPT.ct_ratio /= 5.0;
			CPT.nct_ratio /= 5.0;
		}
		
		CPT.pt_primary = CPT.pt_primary_high & 0xffff;
		CPT.pt_primary <<= 16;
		CPT.pt_primary += (CPT.pt_primary_low & 0xffff);
		float_temp1 = (float)CPT.pt_primary;
		float_temp2 = (float)GPT.pt_secondary;
		CPT.pt_ratio = float_temp1 / float_temp2;

		float_temp2 = (float)GPT.pt_tertiary;
		CPT.gpt_ratio = float_temp1 / float_temp2;

//-------- display용 factor
		DISPLAY.multipllier[0] = CPT.ct_ratio;
		DISPLAY.multipllier[1] = CPT.ct_ratio;
		DISPLAY.multipllier[2] = CPT.ct_ratio;
		DISPLAY.multipllier[3] = CPT.nct_ratio;
		DISPLAY.multipllier[4] = CPT.nct_ratio;
		//ZCT
		//DISPLAY.multipllier[5] = 133.33333333333333333333333333333;
		DISPLAY.multipllier[6] = CPT.pt_ratio;
		DISPLAY.multipllier[7] = CPT.pt_ratio;
		DISPLAY.multipllier[8] = CPT.pt_ratio;
		DISPLAY.multipllier[9] = CPT.gpt_ratio;

		//누적값들 삭제
//		ACCUMULATION.energy_p = 0;
//		ACCUMULATION.energy_q = 0;
//		ACCUMULATION.energy_rp = 0;
//		ACCUMULATION.energy_rq = 0;
//		float_to_8bit_fram(&ACCUMULATION.energy_p, EP1, 1);
//		float_to_8bit_fram(&ACCUMULATION.energy_q, EQ1, 1);
//		float_to_8bit_fram(&ACCUMULATION.energy_rp, REP1, 1);
//		float_to_8bit_fram(&ACCUMULATION.energy_rq, REQ1, 1);
//		ACCUMULATION.vo_max = 0;						
//		float_to_8bit_fram(&ACCUMULATION.vo_max, VoMAX1, 1);
//		ACCUMULATION.io_max = 0;
//		float_to_8bit_fram(&ACCUMULATION.io_max, IoMAX1, 1);
	}

	else if(ar_address == MOD_ADDR)
	{
		
	}

	else if(ar_address == AUTO_DISP_MODE)
	{
		
	}

	else if(ar_address == PASSWORD1)
	{
		
	}

	else if(ar_address == LOCAL_CTRL_USE)
	{
		
	}

	else if(ar_address == UVR27R_MODE)
	{
		
	}

	else if(ar_address == DISP_3PHASE_USE)
	{
		
	}

	else if(ar_address == MODBUS_BAUDRATE)
	{
		
	}

//	else if(ar_address == DI_DEBOUNCE1)
//	{
//		for(i = 0; i < 8; i++)		
//		SYSTEM.di_debounce_timer[i] = DIGITAL_INPUT.debounce[i];
//		
//	}
		
//	else if(ar_address == SUPERV_MODE)
//	{		
//		SUPERVISION.time[0] = SUPERVISION.tcs_time;
//		
//		if(SUPERVISION.tcs_do_relay & (0x0001 << i))
//		SUPERVISION.do_output[0] |= DO_ON_BIT[i];
//		
//		
//		SUPERVISION.time[1] = SUPERVISION.ccs_time;
//		
//		if(SUPERVISION.ccs_do_relay & (0x0001 << i))
//		SUPERVISION.do_output[1] |= DO_ON_BIT[i];
//		
//		
//		SUPERVISION.status[0] = RELAY_NORMAL;
//		SUPERVISION.status[1] = RELAY_NORMAL;
//		
//		SUPERVISION.tcs_monitoring = 0;
//		SUPERVISION.ccs_monitoring = 0;
//		SUPERVISION.monitor_count  = 0;
//		SUPERVISION.monitor_update = 0;
//	}
		
//	else if(ar_address == COMM_ADDRESS)

//	else if(ar_address == OCR_PROPERTY)
//	{
//		SYSTEM_SET.ocr_di_mask = 0x0001 << SYSTEM_SET.di_number;
//	}
//	else if(ar_address == OCGR_DGR_SEL)
//	{
//		if(CORE.gr_select == ZCT_SELECT)
//		;
//		
//		else
//		{
//			if(SYSTEM_SET.ocgr_dgr == DGR_SELECT)
//			{
//				if(OCGR50.use)
//				{
//					OCGR50.use = 0;
//					setting_save(&OCGR50.use, OCGR50_USE, 5);
//				}
//				
//				if(OCGR51.use)
//				{
//					OCGR51.use = 0;
//					setting_save(&OCGR51.use, OCGR51_USE, 5);
//				}
//				
////			dsgr_load_check();
//			}
//			
//			else
//			{
////2015.02.24
//// DGR, SGR 확인 필요
////				if(DSGR.use)
////				{
////					DSGR.use = 0;
////					setting_save(&DSGR.current_set, DSGR_USE, 6);
////				}
////2015.02.24 END				
////				ocgr_load_check();
//			}
//		}
//	}
//	else if(ar_address == COMM_ADDRESS)
//	{
//		*COMM_2_ADDRESS = COMM.address;
//		*COMM_2_BAUDRATE = COMM.baudrate;
//	}


	else if(ar_address == OCR50_1_USE)
	{
		OCR50_1.Pickup_Threshold = (float)OCR50_1.current_set;
		OCR50_1.Pickup_Threshold *= 0.1;
		
		OCR50_1.Dropout_Threshold = (float)OCR50_1.current_set;
		OCR50_1.Dropout_Threshold *= 0.099; // 0.099 = 0.1 * 0.99
		
		OCR50_1.op_status = RELAY_NORMAL;
		OCR50_1.drop_status = DROPOUT_NORMAL;
		
		OCR50_1.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(OCR50_1.do_relay & (0x0001 << i))
			OCR50_1.do_output |= DO_ON_BIT[i];
		}
		
		if(OCR50_1.mode == DEFINITE)
		{
			OCR50_1.delay_ms = OCR50_1.delay_time * 10;
			OCR50_1.delay_ms -= DEFINITE_PICKUP_LIMIT;
			OCR50_1.pickup_limit = DEFINITE_PICKUP_LIMIT;
		}
		else
		{
			OCR50_1.delay_ms = 7;
			OCR50_1.pickup_limit = INSTANT_PICKUP_LIMIT;
		}
		OCR50_1.event_ready = OCR50_1_SET_EVENT;
		OCR50_1.event_ready |= (unsigned long)(OCR50_1.mode << 8);
		
		RELAY_STATUS.pickup &= ~WAVE_OCR50_1_BIT;
		RELAY_STATUS.operation &= ~WAVE_OCR50_1_BIT;
	}

	else if(ar_address == OCR50_2_USE)
	{
		OCR50_2.Pickup_Threshold = (float)OCR50_2.current_set;
		OCR50_2.Pickup_Threshold *= 0.1;
	
		OCR50_2.Dropout_Threshold = (float)OCR50_2.current_set;
		OCR50_2.Dropout_Threshold *= 0.099; // 0.099 = 0.1 * 0.99
		
		OCR50_2.op_status = RELAY_NORMAL;
		OCR50_2.drop_status = DROPOUT_NORMAL;
		
		OCR50_2.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(OCR50_2.do_relay & (0x0001 << i))
			OCR50_2.do_output |= DO_ON_BIT[i];
		}

		if(OCR50_2.mode == DEFINITE)
		{
			OCR50_2.delay_ms = OCR50_2.delay_time * 10;
			OCR50_2.delay_ms -= DEFINITE_PICKUP_LIMIT;
			OCR50_2.pickup_limit = DEFINITE_PICKUP_LIMIT;
		}
		else
		{
			OCR50_2.delay_ms = 7;
			OCR50_2.pickup_limit = INSTANT_PICKUP_LIMIT;
		}
		OCR50_2.event_ready = OCR50_2_SET_EVENT;
	
		OCR50_2.event_ready |= (unsigned long)(OCR50_2.mode << 8);
		
		RELAY_STATUS.pickup &= ~WAVE_OCR50_2_BIT;
		RELAY_STATUS.operation &= ~WAVE_OCR50_2_BIT;
		
	}

	else if(ar_address == OCR51_1_USE)
	{
		OCR51_1.Pickup_Threshold = (float)OCR51_1.current_set;
		OCR51_1.Pickup_Threshold *= 0.102; // 0.102 = 0.1 * 1.02
	
		OCR51_1.Dropout_Threshold = (float)OCR51_1.current_set;
		OCR51_1.Dropout_Threshold *= 0.097; // 0.097 = 0.1 * 0.97
		
		OCR51_1.op_status = RELAY_NORMAL;
		OCR51_1.drop_status = DROPOUT_NORMAL;
		
		OCR51_1.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(OCR51_1.do_relay & (0x0001 << i))
			OCR51_1.do_output |= DO_ON_BIT[i];
		}
	
		OCR51_1.setting = (float)OCR51_1.current_set;		
		OCR51_1.setting *= 0.1;
		
		if(CORE.rated_ct != CT_1A)
		OCR51_1.setting *= 5;
	
		OCR51_1.Mbyk = (float)OCR51_1.time_lever;
		OCR51_1.Mbyk *= 0.01;
		
		if(OCR51_1.mode == INVERSE)
		{
			OCR51_1.Mbyk *= IEC_INV_k;
			OCR51_1.a = IEC_INV_a;
		}
		else if(OCR51_1.mode == V_INVERSE)
		{
			OCR51_1.Mbyk *= IEC_VINV_k;
			OCR51_1.a = IEC_VINV_a;
		}
		else
		{
			OCR51_1.Mbyk *= IEC_EINV_k;
			OCR51_1.a = IEC_EINV_a;
		}
	
		OCR51_1.event_ready = OCR51_1_SET_EVENT;
		OCR51_1.event_ready |= (unsigned long)(OCR51_1.mode << 8);
		
		RELAY_STATUS.pickup &= ~WAVE_OCR51_1_BIT;
		RELAY_STATUS.operation &= ~WAVE_OCR51_1_BIT;
	}

	else if(ar_address == OCR51_2_USE)
	{
		OCR51_2.Pickup_Threshold = (float)OCR51_2.current_set;
		OCR51_2.Pickup_Threshold *= 0.102; // 0.102 = 0.1 * 1.02
	
		OCR51_2.Dropout_Threshold = (float)OCR51_2.current_set;
		OCR51_2.Dropout_Threshold *= 0.097; // 0.097 = 0.1 * 0.97
		
		OCR51_2.op_status = RELAY_NORMAL;
		OCR51_2.drop_status = DROPOUT_NORMAL;
		
		OCR51_2.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(OCR51_2.do_relay & (0x0001 << i))
			OCR51_2.do_output |= DO_ON_BIT[i];
		}
	
		OCR51_2.setting = (float)OCR51_2.current_set;
		OCR51_2.setting *= 0.1;
		
		if(CORE.rated_ct != CT_1A)
		OCR51_2.setting *= 5;
	
		OCR51_2.Mbyk = (float)OCR51_2.time_lever;
		OCR51_2.Mbyk *= 0.01;
		
		if(OCR51_2.mode == INVERSE)
		{
			OCR51_2.Mbyk *= IEC_INV_k;
			OCR51_2.a = IEC_INV_a;
		}
		else if(OCR51_2.mode == V_INVERSE)
		{
			OCR51_2.Mbyk *= IEC_VINV_k;
			OCR51_2.a = IEC_VINV_a;
		}
		else
		{
			OCR51_2.Mbyk *= IEC_EINV_k;
			OCR51_2.a = IEC_EINV_a;
		}
	
		OCR51_2.event_ready = OCR51_2_SET_EVENT;
		OCR51_2.event_ready |= (unsigned long)(OCR51_2.mode << 8);
		
		RELAY_STATUS.pickup &= ~WAVE_OCR51_2_BIT;
		RELAY_STATUS.operation &= ~WAVE_OCR51_2_BIT;
	}

	else if(ar_address == OCGR50_USE)
	{
		OCGR50.Pickup_Threshold = (float)OCGR50.current_set;
		OCGR50.Pickup_Threshold *= 0.102; // 0.102 = 0.1 * 1.02
		
		OCGR50.Dropout_Threshold = (float)OCGR50.current_set;
		OCGR50.Dropout_Threshold *= 0.097; // 0.097 = 0.1 * 0.97
		
		OCGR50.op_status = RELAY_NORMAL;
		OCGR50.drop_status = DROPOUT_NORMAL;
		
		OCGR50.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(OCGR50.do_relay & (0x0001 << i))
			OCGR50.do_output |= DO_ON_BIT[i];
		}
		
		if(OCGR50.mode == DEFINITE)
		{
			OCGR50.delay_ms = OCGR50.delay_time * 10;
			OCGR50.delay_ms -= DEFINITE_PICKUP_LIMIT;
		
			OCGR50.pickup_limit = DEFINITE_PICKUP_LIMIT;
		}
		else
		{
			OCGR50.delay_ms = 7;
			OCGR50.pickup_limit = INSTANT_PICKUP_LIMIT;
		}
		
		OCGR50.event_ready = OCGR50_SET_EVENT;
		OCGR50.event_ready |= (unsigned long)(OCGR50.mode << 8);
		OCGR50.event_ready |= 0x00000008;
		
		RELAY_STATUS.pickup &= ~WAVE_OCGR50_BIT;
		RELAY_STATUS.operation &= ~WAVE_OCGR50_BIT;
	}

	else if(ar_address == OCGR51_USE)
	{
		OCGR51.Pickup_Threshold = (float)OCGR51.current_set;
		OCGR51.Pickup_Threshold *= 0.0102; // 0.0102 = 0.01 * 1.02
		
		OCGR51.Dropout_Threshold = (float)OCGR51.current_set;
		OCGR51.Dropout_Threshold *= 0.0097; // 0.0097 = 0.01 * 0.97
		
		OCGR51.op_status = RELAY_NORMAL;
		OCGR51.drop_status = DROPOUT_NORMAL;
		
		OCGR51.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(OCGR51.do_relay & (0x0001 << i))
			OCGR51.do_output |= DO_ON_BIT[i];
		}
		
		OCGR51.setting = (float)OCGR51.current_set;
		OCGR51.setting *= 0.01;
		
		if(CORE.rated_ct != CT_1A)
		OCGR51.setting *= 5;
		
		OCGR51.Mbyk = (float)OCGR51.time_lever;
		OCGR51.Mbyk *= 0.01;
		
		if(OCGR51.mode == INVERSE)
		{
			OCGR51.Mbyk *= IEC_INV_k;
			OCGR51.a = IEC_INV_a;
		}
		else if(OCGR51.mode == V_INVERSE)
		{
			OCGR51.Mbyk *= IEC_VINV_k;
			OCGR51.a = IEC_VINV_a;
		}
		else
		{
			OCGR51.Mbyk *= IEC_EINV_k;
			OCGR51.a = IEC_EINV_a;
		}
		
		OCGR51.event_ready = OCGR51_SET_EVENT;
		OCGR51.event_ready |= (unsigned long)(OCGR51.mode << 8);
		OCGR51.event_ready |= 0x00000008;
		
		RELAY_STATUS.pickup &= ~WAVE_OCGR51_BIT;
		RELAY_STATUS.operation &= ~WAVE_OCGR51_BIT;
	}

	else if(ar_address == UVR_1_USE)
	{
		UVR_1.Pickup_Threshold = (float)UVR_1.voltage_set;
		UVR_1.Pickup_Threshold *= 0.097; // 0.097 = 0.1 * 0.97
		
		UVR_1.Dropout_Threshold = (float)UVR_1.voltage_set;
		UVR_1.Dropout_Threshold *= 0.102; // 0.102 = 0.1 * 1.02
		
		UVR_1.Pickup_Threshold *= GPT.pt_secondary;
		UVR_1.Dropout_Threshold *= GPT.pt_secondary;
			
		UVR_1.op_status = RELAY_NORMAL;
		UVR_1.drop_status = DROPOUT_NORMAL;
		
		UVR_1.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(UVR_1.do_relay & (0x0001 << i))
			UVR_1.do_output |= DO_ON_BIT[i];
		}
		
		UVR_1.delay_ms = UVR_1.delay_time * 10;
		UVR_1.delay_ms -=DEFINITE_PICKUP_LIMIT;
		UVR_1.event_ready = UVR_1_SET_EVENT;
		UVR_1.event_ready |= 0x00000100;
		UVR_1.over_volt = 0;
		
		RELAY_STATUS.pickup &= ~WAVE_UVR_1_BIT;
		RELAY_STATUS.operation &= ~WAVE_UVR_1_BIT;
		
		do_release(&UVR_1.do_output_off);
	}

	else if(ar_address == UVR_2_USE)
	{
		UVR_2.Pickup_Threshold = (float)UVR_2.voltage_set;
		UVR_2.Pickup_Threshold *= 0.097; // 0.097 = 0.1 * 0.97
	
		UVR_2.Dropout_Threshold = (float)UVR_2.voltage_set;
		UVR_2.Dropout_Threshold *= 0.102; // 0.102 = 0.1 * 1.02
		
		UVR_2.Pickup_Threshold *= GPT.pt_secondary;
		UVR_2.Dropout_Threshold *= GPT.pt_secondary;
		
		UVR_2.op_status = RELAY_NORMAL;
		UVR_2.drop_status = DROPOUT_NORMAL;
		
		UVR_2.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(UVR_2.do_relay & (0x0001 << i))
			UVR_2.do_output |= DO_ON_BIT[i];
		}
	
		UVR_2.delay_ms = UVR_2.delay_time * 10;
		UVR_2.delay_ms -= DEFINITE_PICKUP_LIMIT;
	
		UVR_2.event_ready = UVR_2_SET_EVENT;
		UVR_2.event_ready |= 0x00000100;
		
		UVR_2.over_volt = 0;
		
		RELAY_STATUS.pickup &= ~WAVE_UVR_2_BIT;
		RELAY_STATUS.operation &= ~WAVE_UVR_2_BIT;
		
		do_release(&UVR_2.do_output_off);
	}

	else if(ar_address == UVR_3_USE)
	{
		UVR_3.Pickup_Threshold = (float)UVR_3.voltage_set;
		UVR_3.Pickup_Threshold *= 0.097; // 0.097 = 0.1 * 0.97
	
		UVR_3.Dropout_Threshold = (float)UVR_3.voltage_set;
		UVR_3.Dropout_Threshold *= 0.102; // 0.102 = 0.1 * 1.02
		
		UVR_3.Pickup_Threshold *= GPT.pt_secondary;
		UVR_3.Dropout_Threshold *= GPT.pt_secondary;
		
		UVR_3.op_status = RELAY_NORMAL;
		UVR_3.drop_status = DROPOUT_NORMAL;
	
		UVR_3.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(UVR_3.do_relay & (0x0001 << i))
			UVR_3.do_output |= DO_ON_BIT[i];
		}
	
		UVR_3.delay_ms = UVR_3.delay_time * 10;
		UVR_3.delay_ms -= DEFINITE_PICKUP_LIMIT;
	
		UVR_3.event_ready = UVR_3_SET_EVENT;
		UVR_3.event_ready |= 0x00000100;
		
		UVR_3.over_volt = 0;
		
		RELAY_STATUS.pickup &= ~WAVE_UVR_3_BIT;
		RELAY_STATUS.operation &= ~WAVE_UVR_3_BIT;
		
		do_release(&UVR_3.do_output_off);
	}

	else if(ar_address == P47_USE)
	{
		P47.Pickup_Threshold = (float)P47.voltage_set;
		P47.Pickup_Threshold *= 0.102; // 0.102 = 0.1 * 1.02
	
		P47.Dropout_Threshold = (float)P47.voltage_set;
		P47.Dropout_Threshold *= 0.097; // 0.097 = 0.1 * 0.97
	
		P47.Pickup_Threshold *= GPT.pt_secondary;
		P47.Dropout_Threshold *= GPT.pt_secondary;
	
		P47.op_status = RELAY_NORMAL;
		P47.drop_status = DROPOUT_NORMAL;
	
		P47.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(P47.do_relay & (0x0001 << i))
			P47.do_output |= DO_ON_BIT[i];
		}
	
		P47.delay_ms = P47.delay_time * 10;
		P47.delay_ms -= DEFINITE_PICKUP_LIMIT;
	
		P47.event_ready = P47_SET_EVENT;
		P47.event_ready |= 0x00000100;
	
		RELAY_STATUS.pickup &= ~WAVE_47P_BIT;
		RELAY_STATUS.operation &= ~WAVE_47P_BIT;
	}

	else if(ar_address == N47_USE)
	{
		N47.Pickup_Threshold = (float)N47.voltage_set;
		N47.Pickup_Threshold *= 0.102; // 0.102 = 0.1 * 1.02

		N47.Dropout_Threshold = (float)N47.voltage_set;
		N47.Dropout_Threshold *= 0.097; // 0.097 = 0.1 * 0.97

		N47.Pickup_Threshold *= GPT.pt_secondary;
		N47.Dropout_Threshold *= GPT.pt_secondary;

		N47.op_status = RELAY_NORMAL;
		N47.drop_status = DROPOUT_NORMAL;

		N47.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(N47.do_relay & (0x0001 << i))
			N47.do_output |= DO_ON_BIT[i];
		}

		N47.delay_ms = N47.delay_time * 10;
		N47.delay_ms -= DEFINITE_PICKUP_LIMIT;

		N47.event_ready = N47_SET_EVENT;
		N47.event_ready |= 0x00000100;

		RELAY_STATUS.pickup &= ~WAVE_47N_BIT;
		RELAY_STATUS.operation &= ~WAVE_47N_BIT;
	}

	else if(ar_address == OVR_USE)
	{
		OVR.Pickup_Threshold = (float)OVR.voltage_set;
		OVR.Pickup_Threshold *= 0.102; // 0.102 = 0.1 * 1.02
	
		OVR.Dropout_Threshold = (float)OVR.voltage_set;
		OVR.Dropout_Threshold *= 0.097; // 0.097 = 0.1 * 0.97
		
		OVR.Pickup_Threshold *= GPT.pt_secondary;
		OVR.Dropout_Threshold *= GPT.pt_secondary;
		
		OVR.op_status = RELAY_NORMAL;
		OVR.drop_status = DROPOUT_NORMAL;
		
		OVR.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(OVR.do_relay & (0x0001 << i))
			OVR.do_output |= DO_ON_BIT[i];
		}
	
		if(OVR.mode == INVERSE)
		{
			OVR.setting = (float)OVR.voltage_set;
			OVR.setting *= 0.1;
			
			OVR.setting *= GPT.pt_secondary;
		
			OVR.Mbyk = (float)OVR.delay_time;
			OVR.Mbyk *= 0.01;
			OVR.Mbyk *= IEC_INV_k;
		
			OVR.pickup_limit = INVERSE_PICKUP_LIMIT;
		}
		else
		{
			OVR.Op_Time_set = (unsigned long)OVR.delay_time;
			OVR.Op_Time_set *= 10;
			OVR.Op_Time_set -= DEFINITE_PICKUP_LIMIT;
		
			OVR.pickup_limit = DEFINITE_PICKUP_LIMIT;
		}
		OVR.event_ready = OVR_SET_EVENT;
		OVR.event_ready |= (unsigned long)(OVR.mode << 8);
		
		RELAY_STATUS.pickup &= ~WAVE_OVR_BIT;
		RELAY_STATUS.operation &= ~WAVE_OVR_BIT;
	}

	else if(ar_address == OVGR_USE)
	{
		OVGR.Pickup_Threshold = (float)OVGR.voltage_set;
		OVGR.Pickup_Threshold *= 0.102; // 0.102 = 0.1 * 1.02
	
		OVGR.Dropout_Threshold = (float)OVGR.voltage_set;
		OVGR.Dropout_Threshold *= 0.097; // 0.097 = 0.1 * 0.97
		
		OVGR.Pickup_Threshold *= GPT.pt_tertiary;
		OVGR.Dropout_Threshold *= GPT.pt_tertiary;
		
		OVGR.op_status = RELAY_NORMAL;
		OVGR.drop_status = DROPOUT_NORMAL;
	
		OVGR.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(OVGR.do_relay & (0x0001 << i))
			OVGR.do_output |= DO_ON_BIT[i];
		}
	
		if(OVGR.mode == INVERSE)
		{
			OVGR.setting = (float)OVGR.voltage_set;
			OVGR.setting *= 0.1;
			
			OVGR.setting *= GPT.pt_tertiary;
		
			OVGR.Mbyk = (float)OVGR.time_lever;
			OVGR.Mbyk *= 0.01;
			OVGR.Mbyk *= IEC_INV_k;
		
			OVGR.pickup_limit = INVERSE_PICKUP_LIMIT;
		}
		else
		{
			OVGR.Op_Time_set = 7;
			OVGR.pickup_limit = INSTANT_PICKUP_LIMIT;
		}
		OVGR.event_ready = OVGR_SET_EVENT;
		OVGR.event_ready |= (unsigned long)(OVGR.mode << 8);
		OVGR.event_ready |= 0x00000008;
		
		RELAY_STATUS.pickup &= ~WAVE_OVGR_BIT;
		RELAY_STATUS.operation &= ~WAVE_OVGR_BIT;
	}

	else if(ar_address == DGR_USE)
	{
		DGR.Pickup_Threshold_Io = (float)DGR.current_set;
		DGR.Pickup_Threshold_Io *= 0.102; // 0.102 = 0.1 * 1.02
		
		DGR.Dropout_Threshold_Io = (float)DGR.current_set;
		DGR.Dropout_Threshold_Io *= 0.097; // 0.097 = 0.1 * 0.97
		
		if(CORE.gr_select != ZCT_SELECT)
		{
			if(CORE.rated_ct != CT_1A)
			{
				DGR.Pickup_Threshold_Io *= 5;
				DGR.Dropout_Threshold_Io *= 5;
			}
		}
		
		DGR.Pickup_Threshold_Vo = (float)DGR.voltage_set;
		DGR.Pickup_Threshold_Vo *= 0.102; // 0.102 = 0.1 * 1.02
		
		DGR.Dropout_Threshold_Vo = (float)DGR.voltage_set;
		DGR.Dropout_Threshold_Vo *= 0.097; // 0.097 = 0.1 * 0.97
		
		DGR.Pickup_Threshold_Vo *= GPT.pt_tertiary;
		DGR.Dropout_Threshold_Vo *= GPT.pt_tertiary;
		
		DGR.angle_low = (float)DGR.angle_set;
		DGR.angle_low -= 60;
		
		if(DGR.angle_low < 0)
		DGR.angle_low += 360;
		
		DGR.angle_high = (float)DGR.angle_set;
		DGR.angle_high += 60;
		
		DGR.op_status = RELAY_NORMAL;
		DGR.drop_status = DROPOUT_NORMAL;
		
		DGR.delay_ms = DGR.delay_time * 10;
		DGR.delay_ms -= DEFINITE_PICKUP_LIMIT;
		
		DGR.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(DGR.do_relay & (0x0001 << i))
			{
				DGR.do_output |= DO_ON_BIT[i];
			}
		}
		if(CORE.gr_select == ZCT_SELECT)
		{
			DGR.event_ready = SGR_SET_EVENT;
		}
		else
		{
			DGR.event_ready = DGR_SET_EVENT;
		}
		DGR.event_ready |= 0x00000100;
		
		RELAY_STATUS.pickup &= ~WAVE_DGR_BIT;
		RELAY_STATUS.operation &= ~WAVE_DGR_BIT;
	}

	else if(ar_address == SGR_USE)
	{
		SGR.Pickup_Threshold_Io = (float)SGR.current_set;
		SGR.Pickup_Threshold_Io *= 0.102; // 0.102 = 0.1 * 1.02
		
		SGR.Dropout_Threshold_Io = (float)SGR.current_set;
		SGR.Dropout_Threshold_Io *= 0.097; // 0.097 = 0.1 * 0.97
		
		if(CORE.gr_select != ZCT_SELECT)
		{
			if(CORE.rated_ct != CT_1A)
			{
				SGR.Pickup_Threshold_Io *= 5;
				SGR.Dropout_Threshold_Io *= 5;
			}
		}
		
		SGR.Pickup_Threshold_Vo = (float)SGR.voltage_set;
		SGR.Pickup_Threshold_Vo *= 0.102; // 0.102 = 0.1 * 1.02
		
		SGR.Dropout_Threshold_Vo = (float)SGR.voltage_set;
		SGR.Dropout_Threshold_Vo *= 0.097; // 0.097 = 0.1 * 0.97
		
		SGR.Pickup_Threshold_Vo *= GPT.pt_tertiary;
		SGR.Dropout_Threshold_Vo *= GPT.pt_tertiary;
		
		SGR.angle_low = (float)SGR.angle_set;
		SGR.angle_low -= 60;
		
		if(SGR.angle_low < 0)
		SGR.angle_low += 360;
		
		SGR.angle_high = (float)SGR.angle_set;
		SGR.angle_high += 60;
		
		SGR.op_status = RELAY_NORMAL;
		SGR.drop_status = DROPOUT_NORMAL;
		
		SGR.delay_ms = SGR.delay_time * 10;
		SGR.delay_ms -= DEFINITE_PICKUP_LIMIT;
		
		SGR.do_output = 0;
		for(i = 0; i < 9; i++)
		{
			if(SGR.do_relay & (0x0001 << i))
			{
				SGR.do_output |= DO_ON_BIT[i];
			}
		}
		if(CORE.gr_select == ZCT_SELECT)
		{
			SGR.event_ready = SGR_SET_EVENT;
		}
		else
		{
			SGR.event_ready = SGR_SET_EVENT;
		}
		SGR.event_ready |= 0x00000100;
		
		RELAY_STATUS.pickup &= ~WAVE_SGR_BIT;
		RELAY_STATUS.operation &= ~WAVE_SGR_BIT;
	}
}

	
// flash read
// *ar_value - flash에서 읽은 설정 값 저장변수
// *ar_address - flash 지정 주소
// ar_wordcount - *ar_address 부터 읽을 word 단위 개수
// return - crc 에러 여부
unsigned int setting_load(unsigned int *ar_value, unsigned int ar_wordcount, unsigned int *ar_address)
{	
	unsigned int crc;
	unsigned int i;
	
	// flash read
	for(i = 0; i < ar_wordcount; i++)
	{
		*(ar_value + i) = *(ar_address + i);
	}
	
	//crc read
	i = *(ar_address + i);
	
	// 일단 crc 계산
	crc = Setting_CRC(ar_value, ar_wordcount);
	
	// crc fail
	if(crc != i)
	return(0);
	
	//crc가 정상일 경우 사후처리
	setting_post_handling(ar_address);
	
	return(1);
}

unsigned int Setting_CRC(unsigned int *ar_address, unsigned int ar_length)
{
	unsigned int temp = 0;
	unsigned int high, low;
	unsigned int crc_hi_temp = 0x00FF;
	unsigned int crc_lo_temp = 0x00FF;
	unsigned int i;
	
	for(i = 0; i < ar_length; i++)
	{	
		temp = *(ar_address + i);
		high = 0;
		low = 0;
		high = temp >> 8;
		low = 0x00ff & temp;
		
		// high
		temp = (crc_hi_temp ^ high);
		crc_hi_temp = (crc_lo_temp ^ CRC_High[temp]);
		crc_lo_temp = CRC_Low[temp];
		
		// low
		temp = (crc_hi_temp ^ low);
		crc_hi_temp = (crc_lo_temp ^ CRC_High[temp]);
		crc_lo_temp = CRC_Low[temp];
	}
	crc_hi_temp <<= 8;
	crc_hi_temp |= crc_lo_temp;
	return(crc_hi_temp);
}

// 이벤트 정보 업데이트, khs, 2015-04-01 오후 5:37:28
void event_info_update(void)
{
	// rollover 아님
	if(EVENT.rollover == 0)
	{
		//시작점 지정
		EVENT.view_start = (int)(EVENT.sp - 1);
		// 현재 보는 부분 지정
		EVENT.view_point = EVENT.view_start;
	}
	// rollover
	else
	{						
		if(EVENT.sp == 0)
		{
			EVENT.view_start = 99;
			EVENT.view_point = 99;
		}
		
		else
		{
			EVENT.view_start = (int)(EVENT.sp - 1);
			EVENT.view_point = EVENT.view_start;
		}
	}	
}

// 이벤트 저장
// *ar_event - 이벤트 저장 변수
void event_direct_save(unsigned long *ar_event)
{
	unsigned int temp;

	event_info_update();//khs, 2015-04-01 오후 5:38:07
	
	// 곱하기 18
	temp = EVENT.sp * 18;
	
	*(EVENT_YEAR + temp)     = TIME.year;  // 연
	*(EVENT_MONTH + temp)    = TIME.month; // 월
	*(EVENT_DAY + temp)      = TIME.day;   // 일
	*(EVENT_HOUR + temp)     = TIME.hour;  // 시
	*(EVENT_MINUTE + temp)   = TIME.minute; // 분
	*(EVENT_SECOND + temp)   = TIME.second; // 초
	*(EVENT_MS1 + temp)      = TIME.milisecond >> 8; //msec 상위바이트
	*(EVENT_MS2 + temp)      = TIME.milisecond;      // msec 하위바이트
	*(EVENT_INDEX1 + temp)   = (unsigned int)(*ar_event >> 24); // 넘겨받은 변수에 이미 index1,2,contents 가 들어있음
	*(EVENT_INDEX2 + temp)   = (unsigned int)(*ar_event >> 16);
	*(EVENT_CONTENT1 + temp) = (unsigned int)(*ar_event >> 8);
	*(EVENT_CONTENT2 + temp) = (unsigned int)*ar_event;
	
	// 넘겨받은 이벤트가 계전동작이면 추가정보를 기록해준다
	// 넘겨받은 것이 정말 계전동작 이벤트라면 넘겨주는 변수 주소는 EVENT.operation
	//if(EVENT.operation & 0x01000000)
	if(ar_event == &EVENT.operation)
	{
		*(EVENT_RATIO1 + temp)   = EVENT.ratio >> 8; // 동작치 배율
		*(EVENT_RATIO2 + temp)   = EVENT.ratio;		
		*(EVENT_OPTIME1 + temp)  = (unsigned int)(EVENT.optime >> 24); //동작시간
		*(EVENT_OPTIME2 + temp)  = (unsigned int)(EVENT.optime >> 16);
		*(EVENT_OPTIME3 + temp)  = (unsigned int)(EVENT.optime >> 8);
		*(EVENT_OPTIME4 + temp)  = (unsigned int)EVENT.optime;
	}
	
	// 이벤트 개수 증가
	++EVENT.sp;
	
	// 200개 다차면 rollover 플래그 셋하고 sp는 0으로 초기화
	if(EVENT.sp == 200)
	{
		EVENT.sp = 0;
		
		EVENT.rollover = 0xaa;
		*EVENT_ROLLOVER = 0xaa;
	}
	
	// fram에 저장
	*EVENT_SP = EVENT.sp;
	
	// 넘겨받은 이벤트 변수 index만 살리고 데이터 초기화
	*ar_event &= 0xff000000;

	event_info_update();//khs, 2015-04-01 오후 5:38:07	
}

// rtc 장치 drivng 함수
void rtc_handling(void)
{
	unsigned int temp[7];
	unsigned int temp16 = 0;
	
	// rtc write mode
	if(TIME.update == 0)
	{
		temp[0] = 0x02;
		i2c_write(0x68, 0x00, 1, temp);
		++TIME.update;
	}
	
	//rtc에 시간을 설정할 때
	else if(TIME.update == 1)
	{
		// decimal을 bcd로 변환하여 써줌
		
		// year
		temp16 = *TIME.buffer / 10;
		temp[6] = temp16;
		temp[6] <<= 4;
		temp16 = *TIME.buffer % 10;
		temp[6] |= temp16;
		
		//month
		temp16 = *(TIME.buffer + 1) / 10;
		temp[5] = temp16;
		temp[5] <<= 4;
		temp16 = *(TIME.buffer + 1) % 10;
		temp[5] |= temp16;
		
		//day
		temp16 = *(TIME.buffer + 2) / 10;
		temp[4] = temp16;
		temp[4] <<= 4;
		temp16 = *(TIME.buffer + 2) % 10;
		temp[4] |= temp16;
		
		//hour
		temp16 = *(TIME.buffer + 3) / 10;
		temp[2] = temp16;
		temp[2] <<= 4;
		temp16 = *(TIME.buffer + 3) % 10;
		temp[2] |= temp16;
		
		//minute
		temp16 = *(TIME.buffer + 4) / 10;
		temp[1] = temp16;
		temp[1] <<= 4;
		temp16 = *(TIME.buffer + 4) % 10;
		temp[1] |= temp16;
		
		//second
		temp16 = *(TIME.buffer + 5) / 10;
		temp[0] = temp16;
		temp[0] <<= 4;
		temp16 = *(TIME.buffer + 5) % 10;
		temp[0] |= temp16;
		
		i2c_write(0x68, 0x02, 7, temp);
		
		++TIME.update;
	}
	// rtc 내부 시간을 읽어올때  read mode로 전환
	else if(TIME.update == 2)
	{
		temp[0] = 0x01;		
		i2c_write(0x68, 0x00, 1, temp);
		++TIME.update;
	}
	// rtc read 동작
	else if(TIME.update == 3)
	{		
		i2c_read(0x68, 0x02, 7);
		++TIME.update;
	}
	// 읽어온 rtc 시간을 conversion & restart
	else if(TIME.update == 4)
	{	
		// i2c_read()에서 읽음완료 후 DSP 수신 FIFO(*I2caRegs_I2CDRR)에서 불러오면 읽기가 최종적으로 완료
		for(temp16 = 0; temp16 < 7; temp16++)
		temp[temp16] = *I2caRegs_I2CDRR;
		
		// RTC 내부는 BCD type으로 시간을 운용하므로 decimal 형태로 변환을 해준다
		// BCD 타입 - 자리수를 nibble 단위로 처리
		// 10월인 경우 - 0x10
		// 31일인 경우 - 0x31
		// year		
		TIME.year = (temp[6] >> 4) * 10;
		TIME.year += (temp[6] & 0x000f);
		
		//month
		TIME.month = (temp[5] >> 4) * 10;
		TIME.month += (temp[5] & 0x000f);
		
		//day
		TIME.day = (temp[4] >> 4) * 10;
		TIME.day += (temp[4] & 0x000f);
				
		//hour
		TIME.hour = (temp[2] >> 4) * 10;
		TIME.hour += (temp[2] & 0x000f);
		
		//minute
		TIME.minute = (temp[1] >> 4) * 10;
		TIME.minute += (temp[1] & 0x000f);
		
		//second
		TIME.second = (temp[0] >> 4) * 10;
		TIME.second += (temp[0] & 0x000f);
		
		//rtc start
		temp[0] = 0x00;
		i2c_write(0x68, 0x00, 1, temp);		
		
		TIME.update = 0xffff;	
	}	
}

void GPT_Default_Settings(void)
{
	GPT.pt_secondary = 110;
	GPT.pt_tertiary = 190;

	if(setting_save(&GPT.pt_secondary, PT_SECONDARY, 2))
	{
		setting_load(&GPT.pt_secondary, 2, PT_SECONDARY);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void CPT_Default_Settings(void)
{
	if(CORE.rated_ct == CT_5A)	{CPT.ct_primary = CT_5A_RATED; CPT.nct_primary = CT_5A_RATED;}
	else												{CPT.ct_primary = CT_1A_RATED; CPT.nct_primary = CT_1A_RATED;}
	
	if(GPT.pt_secondary == 100)			 {CPT.pt_primary_low = 0x86a0; CPT.pt_primary_high = 0x0001;} // 0x186a0 ->100000
	else if(GPT.pt_secondary == 110) {CPT.pt_primary_low = 0xadb0; CPT.pt_primary_high = 0x0001;} // 0x1adb0 ->110000
	else if(GPT.pt_secondary == 120) {CPT.pt_primary_low = 0xd4c0; CPT.pt_primary_high = 0x0001;} // 0x1d4c0 ->120000
	else if(GPT.pt_secondary == 190) {CPT.pt_primary_low = 0xe630; CPT.pt_primary_high = 0x0002;} // 0x2E630 ->190000
		
	if(CORE.rated_ct == CT_5A)	{CPT.rated_current = 50000;}
	else												{CPT.rated_current = 10000;}

	if(setting_save(&CPT.ct_primary, CT_PRIMARY, 5))
	{
		setting_load(&CPT.ct_primary, 5, CT_PRIMARY);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void ADDRESS_Default_Settings(void)
{
	ADDRESS.address = 254;

	if(setting_save(&ADDRESS.address, MOD_ADDR, 1))
	{
		setting_load(&ADDRESS.address, 1, MOD_ADDR);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void AUTO_DISPLAY_Default_Settings(void)
{
	AUTO_DISPLAY.mode = 0;

	if(setting_save(&AUTO_DISPLAY.mode, AUTO_DISP_MODE, 1))
	{
		setting_load(&AUTO_DISPLAY.mode, 1, AUTO_DISP_MODE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void PASSWORD_Default_Settings(void)
{
	PASSWORD.real = 1111;

	if(setting_save(&PASSWORD.real, PASSWORD1, 1))
	{
		setting_load(&PASSWORD.real, 1, PASSWORD1);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void LOCAL_CTRL_Default_Settings(void)
{
	LOCAL_CTRL.use = DISABLE;
	LOCAL_CTRL.close_time = 2;
	LOCAL_CTRL.open_time = 0;

	if(setting_save(&LOCAL_CTRL.use, LOCAL_CTRL_USE, 3))
	{
		setting_load(&LOCAL_CTRL.use, 3, LOCAL_CTRL_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void UVR_MODE_Default_Settings(void)
{
	UVR_MODE.setting1 = MANUAL;
	UVR_MODE.setting2 = MANUAL;
	UVR_MODE.setting3 = MANUAL;

	if(setting_save(&UVR_MODE.setting1, UVR27R_MODE, 3))
	{
		setting_load(&UVR_MODE.setting1, 3, UVR27R_MODE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void DISPLAY_3PHASE_Default_Settings(void)
{
	DISPLAY_3PHASE.use = DISABLE;

	if(setting_save(&DISPLAY_3PHASE.use, DISP_3PHASE_USE, 1))
	{
		setting_load(&DISPLAY_3PHASE.use, 1, DISP_3PHASE_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void MODBUS_Default_Settings(void)
{
	MODBUS.baudrate = 2;  //38400
	MODBUS.delay = 0;  //0ms

	if(setting_save(&MODBUS.baudrate, MODBUS_BAUDRATE, 2))
	{
		setting_load(&MODBUS.baudrate, 2, MODBUS_BAUDRATE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void OCR_MODE_Default_Settings(void)
{
	OCR_MODE_SET.ocr_mode = SELECT;
	OCR_MODE_SET.di_number = 3;

	if(setting_save(&OCR_MODE_SET.ocr_mode, OCR_MODE, 2))
	{
		setting_load(&OCR_MODE_SET.ocr_mode, 2, OCR_MODE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void SYSTEM_ALARM_Default_Settings(void)
{
	SYSTEM_ALARM.use = DISABLE;
	SYSTEM_ALARM.do_relay = 0;

	if(setting_save(&SYSTEM_ALARM.use, SYSTEM_ALARM_USE, 2))
	{
		setting_load(&SYSTEM_ALARM.use, 2, SYSTEM_ALARM_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void OCR50_1_Default_Settings(void)
{
	OCR50_1.use = 0;
	OCR50_1.mode = DEFINITE;
	if(CORE.rated_ct == CT_5A)	{OCR50_1.current_set = OCR50_I_MAX[0];}
	else												{OCR50_1.current_set = OCR50_I_MAX[1];}
	OCR50_1.delay_time = OCR50_T_MAX;
	OCR50_1.do_relay = 0;
	
	if(setting_save(&OCR50_1.use, OCR50_1_USE, 5))
	{
		setting_load(&OCR50_1.use, 5, OCR50_1_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void OCR50_2_Default_Settings(void)
{
	OCR50_2.use = 0;
	OCR50_2.mode = DEFINITE;
	if(CORE.rated_ct == CT_5A)	{OCR50_2.current_set = OCR50_I_MAX[0];}
	else												{OCR50_2.current_set = OCR50_I_MAX[1];}
	OCR50_2.delay_time = OCR50_T_MAX;
	OCR50_2.do_relay = 0;

	if(setting_save(&OCR50_2.use, OCR50_2_USE, 5))
	{
		setting_load(&OCR50_2.use, 5, OCR50_2_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void OCR51_1_Default_Settings(void)
{
	OCR51_1.use = 0;
	OCR51_1.mode = INVERSE;
	if(CORE.rated_ct == CT_5A)	{OCR51_1.current_set = OCR51_I_MAX[0];}
	else												{OCR51_1.current_set = OCR51_I_MAX[1];}
	OCR51_1.time_lever = OCR51_TL_MAX;
	OCR51_1.do_relay = 0;

	if(setting_save(&OCR51_1.use, OCR51_1_USE, 5))
	{
		setting_load(&OCR51_1.use, 5, OCR51_1_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void OCR51_2_Default_Settings(void)
{
	OCR51_2.use = 0;
	OCR51_2.mode = INVERSE;
	if(CORE.rated_ct == CT_5A)	{OCR50_2.current_set = OCR51_I_MAX[0];}
	else												{OCR50_2.current_set = OCR51_I_MAX[1];}
	OCR51_2.time_lever = OCR51_TL_MAX;
	OCR51_2.do_relay = 0;

	if(setting_save(&OCR51_2.use, OCR51_2_USE, 5))
	{
		setting_load(&OCR51_2.use, 5, OCR51_2_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void OCGR50_Default_Settings(void)
{
	OCGR50.use = 0;
	OCGR50.mode = DEFINITE;
	if(CORE.rated_ct == CT_5A)	{OCGR50.current_set = OCGR50_I_MAX[0];}
	else												{OCGR50.current_set = OCGR50_I_MAX[1];}
	OCGR50.delay_time = OCGR50_T_MAX;
	OCGR50.do_relay = 0;

	if(setting_save(&OCGR50.use, OCGR50_USE, 5))
	{
		setting_load(&OCGR50.use, 5, OCGR50_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void OCGR51_Default_Settings(void)
{
	OCGR51.use = 0;
	OCGR51.mode = INVERSE;
	if(CORE.rated_ct == CT_5A)	{OCGR51.current_set = OCGR51_I_MAX[0];}
	else												{OCGR51.current_set = OCGR51_I_MAX[1];}
	OCGR51.time_lever = OCGR51_TL_MAX;
	OCGR51.do_relay = 0;

	if(setting_save(&OCGR51.use, OCGR51_USE, 5))
	{
		setting_load(&OCGR51.use, 5, OCGR51_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void UVR_1_Default_Settings(void)
{
	UVR_1.use = 0;
	if(GPT.pt_secondary == 100)			 {UVR_1.voltage_set = UVR_1_V_MAX[0];}
	else if(GPT.pt_secondary == 110) {UVR_1.voltage_set = UVR_1_V_MAX[1];}
	else if(GPT.pt_secondary == 120) {UVR_1.voltage_set = UVR_1_V_MAX[2];}
	else if(GPT.pt_secondary == 190) {UVR_1.voltage_set = UVR_1_V_MAX[3];}
	UVR_1.delay_time = UVR_T_MAX;
	UVR_1.do_relay = 0;

	if(setting_save(&UVR_1.use, UVR_1_USE, 4))
	{
		setting_load(&UVR_1.use, 4, UVR_1_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void UVR_2_Default_Settings(void)
{
	UVR_2.use = 0;
	if(GPT.pt_secondary == 100)			 {UVR_2.voltage_set = UVR_2_V_MAX[0];}
	else if(GPT.pt_secondary == 110) {UVR_2.voltage_set = UVR_2_V_MAX[1];}
	else if(GPT.pt_secondary == 120) {UVR_2.voltage_set = UVR_2_V_MAX[2];}
	else if(GPT.pt_secondary == 190) {UVR_2.voltage_set = UVR_2_V_MAX[3];}
	UVR_2.delay_time = UVR_T_MAX;
	UVR_2.do_relay = 0;
	
	if(setting_save(&UVR_2.use, UVR_2_USE, 4))
	{
		setting_load(&UVR_2.use, 4, UVR_2_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void UVR_3_Default_Settings(void)
{
	UVR_3.use = 0;
	if(GPT.pt_secondary == 100)			 {UVR_3.voltage_set = UVR_3_V_MAX[0];}
	else if(GPT.pt_secondary == 110) {UVR_3.voltage_set = UVR_3_V_MAX[1];}
	else if(GPT.pt_secondary == 120) {UVR_3.voltage_set = UVR_3_V_MAX[2];}
	else if(GPT.pt_secondary == 190) {UVR_3.voltage_set = UVR_3_V_MAX[3];}
	UVR_3.delay_time = UVR_T_MAX;
	UVR_3.do_relay = 0;

	if(setting_save(&UVR_3.use, UVR_3_USE, 4))
	{
		setting_load(&UVR_3.use, 4, UVR_3_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void P47_Default_Settings(void)
{
	P47.use = 0;
	if(GPT.pt_secondary == 100)			 {P47.voltage_set = P47_V_MAX[0];}
	else if(GPT.pt_secondary == 110) {P47.voltage_set = P47_V_MAX[1];}
	else if(GPT.pt_secondary == 120) {P47.voltage_set = P47_V_MAX[2];}
	else if(GPT.pt_secondary == 190) {P47.voltage_set = P47_V_MAX[3];}
	P47.delay_time = P47_T_MAX;
	P47.do_relay = 0;

	if(setting_save(&P47.use, P47_USE, 4))
	{
		setting_load(&P47.use, 4, P47_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void N47_Default_Settings(void)
{
	N47.use = 0;
	if(GPT.pt_secondary == 100)			 {N47.voltage_set = N47_V_MAX[0];}
	else if(GPT.pt_secondary == 110) {N47.voltage_set = N47_V_MAX[1];}
	else if(GPT.pt_secondary == 120) {N47.voltage_set = N47_V_MAX[2];}
	else if(GPT.pt_secondary == 190) {N47.voltage_set = N47_V_MAX[3];}
	N47.delay_time = N47_T_MAX;
	N47.do_relay = 0;

	if(setting_save(&N47.use, N47_USE, 4))
	{
		setting_load(&N47.use, 4, N47_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void OVR_Default_Settings(void)
{
	OVR.use = 0;
	OVR.mode = DEFINITE;
	if(GPT.pt_secondary == 100)			 {OVR.voltage_set = OVR_V_MAX[0];}
	else if(GPT.pt_secondary == 110) {OVR.voltage_set = OVR_V_MAX[1];}
	else if(GPT.pt_secondary == 120) {OVR.voltage_set = OVR_V_MAX[2];}
	else if(GPT.pt_secondary == 190) {OVR.voltage_set = OVR_V_MAX[3];}
	OVR.delay_time = OVR_T_MAX;
	OVR.time_lever = OVR_TL_MAX;
	OVR.do_relay = 0;
	
	if(setting_save(&OVR.use, OVR_USE, 6))
	{
		setting_load(&OVR.use, 6, OVR_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void OVGR_Default_Settings(void)
{
	OVGR.use = 0;
	OVGR.mode = INVERSE;
	if(GPT.pt_tertiary == 110)			{OVGR.voltage_set = OVGR_V_MAX[0];}
	else if(GPT.pt_tertiary == 120) {OVGR.voltage_set = OVGR_V_MAX[1];}
	else if(GPT.pt_tertiary == 190) {OVGR.voltage_set = OVGR_V_MAX[2];}
	OVGR.time_lever = OVGR_TL_MAX;
	OVGR.do_relay = 0;

	if(setting_save(&OVGR.use, OVGR_USE, 5))
	{
		setting_load(&OVGR.use, 5, OVGR_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void DGR_Default_Settings(void)
{
	DGR.use = 0;
	if(CORE.rated_ct == CT_5A)			{DGR.current_set = DGR_I_MAX[0];}
	else														{DGR.current_set = DGR_I_MAX[1];}
	if(GPT.pt_tertiary == 110)			{DGR.voltage_set = DGR_V_MAX[0];}
	else if(GPT.pt_tertiary == 120) {DGR.voltage_set = DGR_V_MAX[1];}
	else if(GPT.pt_tertiary == 190) {DGR.voltage_set = DGR_V_MAX[2];}
	DGR.angle_set = DGR_A_MAX;
	DGR.delay_time = DGR_T_MAX;
	DGR.do_relay = 0;   

	if(setting_save(&DGR.use, DGR_USE, 6))
	{
		setting_load(&DGR.use, 6, DGR_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void SGR_Default_Settings(void)
{
	SGR.use = 0;
	SGR.current_set = SGR_I_MAX;
	if(GPT.pt_tertiary == 110)			{SGR.voltage_set = SGR_V_MAX[0];}
	else if(GPT.pt_tertiary == 120) {SGR.voltage_set = SGR_V_MAX[1];}
	else if(GPT.pt_tertiary == 190) {SGR.voltage_set = SGR_V_MAX[2];}
	SGR.angle_set = SGR_A_MAX;
	SGR.delay_time = SGR_T_MAX;
	SGR.do_relay = 0;

	if(setting_save(&SGR.use, SGR_USE, 6))
	{
		setting_load(&SGR.use, 6, SGR_USE);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void DI_Default_Settings(void)
{
	DIGITAL_INPUT.debounce[0] = 0;
	DIGITAL_INPUT.debounce[1] = 0;
	DIGITAL_INPUT.debounce[2] = 0;
	DIGITAL_INPUT.debounce[3] = 0;
	DIGITAL_INPUT.debounce[4] = 0;
	DIGITAL_INPUT.debounce[5] = 0;
	DIGITAL_INPUT.debounce[6] = 0;
	DIGITAL_INPUT.debounce[7] = 0;

	if(setting_save(DIGITAL_INPUT.debounce, DI_DEBOUNCE1, 8))
	{
		setting_load(DIGITAL_INPUT.debounce, 8, DI_DEBOUNCE1);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void DO_Default_Settings(void)
{
	DIGITAL_OUTPUT.property = 0x0;

	if(setting_save(&DIGITAL_OUTPUT.property, DO_PROPERTY, 1))
	{
		setting_load(&DIGITAL_OUTPUT.property, 1, DO_PROPERTY);
	}
	else
	{
		//FLASH WRITE ERROR pop up 화면
	}
}

void ClearFLASH(void)
{
	GPT_Default_Settings(); //설정 순서를 CORE. -> GPT. -> CPT. -> 계전요소 순서를 지켜야 함. (순서대로 계속 참조 해야 하므로)
	CPT_Default_Settings();
	ADDRESS_Default_Settings();
	AUTO_DISPLAY_Default_Settings();
	PASSWORD_Default_Settings();
	LOCAL_CTRL_Default_Settings();
	UVR_MODE_Default_Settings();
	DISPLAY_3PHASE_Default_Settings();
	MODBUS_Default_Settings();
	OCR_MODE_Default_Settings();
	SYSTEM_ALARM_Default_Settings();

	OCR50_1_Default_Settings();
	OCR50_2_Default_Settings();
	OCR51_1_Default_Settings();
	OCR51_2_Default_Settings();
	OCGR50_Default_Settings();
	OCGR51_Default_Settings();
	UVR_1_Default_Settings();
	UVR_2_Default_Settings();
	UVR_3_Default_Settings();
	P47_Default_Settings();
	N47_Default_Settings();
	OVR_Default_Settings();
	OVGR_Default_Settings();
	DGR_Default_Settings();
	SGR_Default_Settings();

	DI_Default_Settings();
	DO_Default_Settings();

 	//Fault clear
 	//이벤트 clear
 	//누적값 clear (running hour, vo_max 등)
 	//do 출력 설정 초기화
}



