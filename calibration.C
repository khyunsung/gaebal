#include "define.h"
#include "extern_variable.h"
#include "extern_prototype.h"

// calibration ����
// 1. offset calibration
// 2. ��ä�� �������� HIGH LEVEL (���� 5A : ���� 6A, ���� 110V) (���� 1A   : ���� 2A, ���� 110V) (ZCT 10mA)
// 3. ��ä�� �������� LOW LEVEL  (���� 5A : ���� 1A, ����  10V) (���� 0.2A : ���� 2A, ����  10V) (ZCT  5mA)
// 4. slope ����
// 5. angle ����

// pt �� 380/3 -> �δ����� ����
// ct �� 2000/1 ��δ� 62
//              ����δ� 1.5k
int Offset_Check(unsigned int ar_channel)
{
	unsigned int i;
	unsigned int j;
	long temp_long = 0;
	
	TIMER.lcd = 0;
	
	i = 0;
	
	for(;;)
	{
		if(j != DFT.index_count)
		{
			temp_long += SAMPLE.rms_12_buffer[ar_channel][DFT.index_count];
			j =	DFT.index_count;
			++i;
		}
		if(i == 100)
		break;
	}
	temp_long /= 100;
	return((int)temp_long);
}
	
float DFT_Check(unsigned int ar_channel, float ar_min, float ar_max)
{
	unsigned int i;
	unsigned int j;
	float temp_float = 0.;
	unsigned int temp;
	
X:	i = 0;
	
	for(;;)
	{
		temp = *KEY_CS & 0x0fff;
		delay_us(5000);
		if(temp == UP_KEY || Calibration_Menu_Escape == 1) {
			Calibration_Menu_Escape = 1;
			return 0.0;
		}

		if(j != DFT.index_count)
		{
			if((MEASUREMENT.rms_value[ar_channel] > ar_min) && (MEASUREMENT.rms_value[ar_channel] < ar_max))
			{
				temp_float += MEASUREMENT.rms_value[ar_channel];
				j =	DFT.index_count;
				++i;
			}
			else
			{
				i = 0;
				temp_float = 0;
				goto X;
			}
		}
		if(i == 72)
		break;
	}
	temp_float /= 72.;
	return(temp_float);
}

float Degree_Check(unsigned int ar_channel)
{
	unsigned int i;
	unsigned int j;
	float temp_float = 0.;
	unsigned int temp;
	
	i = 0;
	for(;;)
	{
		temp = *KEY_CS & 0x0fff;
		delay_us(5000);
		if(temp == UP_KEY || Calibration_Menu_Escape == 1) {
			Calibration_Menu_Escape = 1;
			return 0.0;
		}

		if(j != DFT.index_count)
		{
			temp_float += MEASUREMENT.angle[Va] - MEASUREMENT.angle[ar_channel];
			j =	DFT.index_count;
			++i;			
		}
		if(i == 50)
		break;
	}
	temp_float /= 50.;
	return(temp_float);
}

int ADC_Offset_Calibration(void) //Offset Calibration������ Ż�� �ȵǰ� ������ �Ѿ
{
	Uint16 temp = 0;

	SAMPLE.normal = 0;
	
	Calibration_Menu_Escape = 0;

//-------- ���� CALIBRATION �� �ʱ�ȭ	
	for(temp = 0; temp < 10; temp++)
	{
		CALIBRATION.offset_origin[temp] = CALIBRATION.offset[temp];
		CALIBRATION.slope_1st_origin[temp] = CALIBRATION.slope_1st[temp];
		CALIBRATION.angle_origin[temp] = CALIBRATION.angle[temp];	
		
		CALIBRATION.offset[temp] = 0;
		CALIBRATION.slope_1st[temp] = 1.;
		CALIBRATION.angle[temp] = 0.;
	}
//	CALIBRATION.frequency_offset = 0;
//-------- ���� CALIBRATION �� �ʱ�ȭ END

	DFT.index_count = 0;
	for(;;)
	{	
		if(DFT.index_count == 11)
			break;
	}
	delay_us(2000);
	
	//-------- offset ���� ����
	for(temp = 0; temp < 10; temp++)
	{
		CALIBRATION.offset_temp[temp] = Offset_Check(temp);
	}
	return 0;
}

//  0,  1,  2,  3,   4,   5,  6,  7,  8,  9
// Ia, Ib, Ic, In, In2, ZCT, Va, Vb, Vc, Vn
int ADC_High_Calibration(void)
{
	//-------- high level ���� ����
	//124��
	if(CORE.rated_ct == CT_5A)
	{
		DFT_Check(0, 1350, 1650); //�ʱ� DELAY�� ����
		CALIBRATION.high_temp[0] = DFT_Check(0, 1350, 1650); //�Է� 6A (1502~1510)
		CALIBRATION.high_temp[1] = DFT_Check(1, 1350, 1650); //�Է� 6A (1502~1510)
		CALIBRATION.high_temp[2] = DFT_Check(2, 1350, 1650); //�Է� 6A (1502~1510)
		if(CORE.gr_select == NCT_SELECT)	{CALIBRATION.high_temp[3] = DFT_Check(3, 1350, 1650);} //�Է� 6A (1502~1510)
		else															{CALIBRATION.high_temp[5] = DFT_Check(5, 7580, 22741);} //�Է� 10mA
	}
	else
	{
//		CALIBRATION.high_temp[0] = DFT_Check(0, 1.0, 3.0); //�Է� 2A
//		CALIBRATION.high_temp[1] = DFT_Check(1, 1.0, 3.0); //�Է� 2A
//		CALIBRATION.high_temp[2] = DFT_Check(2, 1.0, 3.0); //�Է� 2A
//		if(CORE.gr_select == NCT_SELECT)
//		CALIBRATION.high_temp[3] = DFT_Check(3, 1.0, 3.0); //�Է� 2A
//		else
//		CALIBRATION.high_temp[5] = DFT_Check(5, 5, 15); //�Է� 10mA	
	}
	DFT_Check(6, 5450, 9845); //�ʱ� DELAY�� ����
	CALIBRATION.high_temp[6] = DFT_Check(6, 5450, 9845);  //�Է� 110V (8950~8963)
	CALIBRATION.high_temp[7] = DFT_Check(7, 5450, 9845);  //�Է� 110V (8950~8963)
	CALIBRATION.high_temp[8] = DFT_Check(8, 5450, 9845);  //�Է� 110V (8950~8963)
	CALIBRATION.high_temp[9] = DFT_Check(9, 14310, 17490); //�Է� 110V (15900)

	TIMER.lcd = 0;
	
	for(;;) // 1�� ���
	{
		if(TIMER.lcd > 1000)
		break;
		
		if(Calibration_Menu_Escape == 1)
			return 1;
	}
	
	// ���� ����
	// Vab �������� ��ü ���� ����
	CALIBRATION.angle_temp[6] = 0; //Vab
	CALIBRATION.angle_temp[0] = Degree_Check(0);
	CALIBRATION.angle_temp[1] = Degree_Check(1);
	CALIBRATION.angle_temp[2] = Degree_Check(2);
	CALIBRATION.angle_temp[3] = Degree_Check(3);
	CALIBRATION.angle_temp[5] = Degree_Check(5); //ZCT
	CALIBRATION.angle_temp[7] = Degree_Check(7);
	CALIBRATION.angle_temp[8] = Degree_Check(8);
	CALIBRATION.angle_temp[9] = Degree_Check(9);
	
	return 0;
}

//  0,  1,  2,  3,   4,   5,  6,  7,  8,  9
// Ia, Ib, Ic, In, In2, ZCT, Va, Vb, Vc, Vn
int ADC_low_Calibration(void)
{
	unsigned int i;
	Uint16 temp = 0;
	unsigned int temp1[100];
	void *void_p;
	unsigned int *temp16_p;
	
	//-------- Low level ���� ����
	//125��
	if(CORE.rated_ct == CT_5A)
	{
		DFT_Check(0, 225, 275); //�ʱ� DELAY�� ����
		CALIBRATION.low_temp[0] = DFT_Check(0, 225, 275); //�Է� 1A (249~251)
		CALIBRATION.low_temp[1] = DFT_Check(1, 225, 275); //�Է� 1A (249~251)
		CALIBRATION.low_temp[2] = DFT_Check(2, 225, 275); //�Է� 1A (249~251)
		if(CORE.gr_select == NCT_SELECT)	{CALIBRATION.low_temp[3] = DFT_Check(3, 225, 275);} //�Է� 1A (249~251)
		else															{CALIBRATION.low_temp[5] = DFT_Check(5, 3832, 11496);} //�Է� 5mA
	}
	else
	{
//		CALIBRATION.low_temp[0] = DFT_Check(0, 0.1, 0.3); //�Է� 0.2A
//		CALIBRATION.low_temp[1] = DFT_Check(1, 0.1, 0.3); //�Է� 0.2A
//		CALIBRATION.low_temp[2] = DFT_Check(2, 0.1, 0.3); //�Է� 0.2A
//		if(CORE.gr_select == NCT_SELECT)
//		CALIBRATION.low_temp[3] = DFT_Check(3, 0.1, 0.3); //�Է� 0.2A
//		else
//		CALIBRATION.low_temp[5] = DFT_Check(5, 2, 8); //�Է� 5mA	
	}
	DFT_Check(6, 774, 946); //�ʱ� DELAY�� ����
	CALIBRATION.low_temp[6] = DFT_Check(6, 774, 946); //�Է� 10V (860)
	CALIBRATION.low_temp[7] = DFT_Check(7, 774, 946); //�Է� 10V (815)
	CALIBRATION.low_temp[8] = DFT_Check(8, 774, 946); //�Է� 10V (815)
	CALIBRATION.low_temp[9] = DFT_Check(9, 1296, 1584); //�Է� 10V (1440)

//-------- �⺻�� ������ ��� �� CALIBRATION FACTOR ����
	for(temp = 0; temp < 6; temp++) //���� slope
	{
		CALIBRATION.slope_temp[temp] = 5.0 / (CALIBRATION.high_temp[temp] - CALIBRATION.low_temp[temp]);
		CALIBRATION.slope_1st[temp] = CALIBRATION.slope_temp[temp];
	}
	for(temp = 6; temp < 10; temp++) //���� slope
	{
		CALIBRATION.slope_temp[temp] = 100.0 / (CALIBRATION.high_temp[temp] - CALIBRATION.low_temp[temp]);
		CALIBRATION.slope_1st[temp] = CALIBRATION.slope_temp[temp];
	}

	for(temp = 0; temp < 10; temp++) //offset
	{
		CALIBRATION.offset[temp] = CALIBRATION.offset_temp[temp];
	}

	CALIBRATION.angle[6] = CALIBRATION.angle_temp[6]; //angle
	CALIBRATION.angle[0] = CALIBRATION.angle_temp[0];
	CALIBRATION.angle[1] = CALIBRATION.angle_temp[1];
	CALIBRATION.angle[2] = CALIBRATION.angle_temp[2];
	CALIBRATION.angle[3] = CALIBRATION.angle_temp[3];
	CALIBRATION.angle[5] = CALIBRATION.angle_temp[5];
	CALIBRATION.angle[7] = CALIBRATION.angle_temp[7];
	CALIBRATION.angle[8] = CALIBRATION.angle_temp[8];
	CALIBRATION.angle[9] = CALIBRATION.angle_temp[9];
//-------- �⺻�� ������ ��� �� CALIBRATION FACTOR ���� END

	if(Calibration_Menu_Escape == 1)
		return 1;

//-------- EEROM ����
	eerom_control(4, 0x80);
	eerom_control(4, 0xc0);
	
	for(i = 0; i < 10; i++)  //offset ����
	{
		eerom_write(i, &CALIBRATION.offset[i]);
		temp1[i] = CALIBRATION.offset[i]; //[0]~[9]
	}
	for(i = 0; i < 10; i++) //slope ����
	{
		void_p = &CALIBRATION.slope_1st[i];
		temp16_p = (unsigned int *)void_p;
		eerom_write(0x10 + (i << 1), temp16_p);
		eerom_write(0x11 + (i << 1), temp16_p + 1);
		temp1[10 + (i << 1)] = *temp16_p;  //[10]~[29]
		temp1[11 + (i << 1)] = *(temp16_p + 1);
	}
	for(i = 0; i < 10; i++) //angle ����
	{
		void_p = &CALIBRATION.angle[i];
		temp16_p = (unsigned int *)void_p;
		eerom_write(0x30 + (i << 1), temp16_p);
		eerom_write(0x31 + (i << 1), temp16_p + 1);
		temp1[30 + (i << 1)] = *temp16_p; //[30]~[49]
		temp1[31 + (i << 1)] = *(temp16_p + 1);
	}
	
	i = Setting_CRC(temp1, 50);
	eerom_write(0xa0, &i);
	
	return 0;
//-------- EEROM ���� END
}

//void ADC_Calibration_Control(void)
//{
//	unsigned int i;
//	
//	Uint16 temp = 0;
//	unsigned int temp1[100];
//
//	void *void_p;
//	unsigned int *temp16_p;
//	
//	float low[10];
//	float high[10];
//	float slope[10];
//	
//	float trms_high[10];
//	float trms_low[10];
//	float trms_slope[10];
//	
//	float p_high[3];
//	float p_low[3];
//	float p_slope[3];
//	
//	float PVa;
//	float PVb;
//	float PVc;
//	
//	float QVa;
//	float QVb;
//	float QVc;
//	
//	float freq_temp = 0;
//	unsigned int freq_index = 0;
//	unsigned int freq_count = 0;
//
//	//�Է����� ���ÿ� ȭ�� �Ѹ�
//	TIMER.lcd = 0;
//	
//	// 1�� ���
//	for(;;)
//	{
//		if(TIMER.lcd > 1000)
//		break;		
//	}
//	
////-------- calibration start
//	SAMPLE.normal = 0;
//	
//	for(temp = 0; temp < 10; temp++)
//	{
//		CALIBRATION.offset[temp] = 0;
//		CALIBRATION.slope_1st[temp] = 1.;
//		CALIBRATION.slope_trms[temp] = 1.;
//		CALIBRATION.angle[temp] = 0;
//	}
//	CALIBRATION.frequency_offset = 0;
//	DFT.index_count = 0;
//	
//	for(;;)
//	{	
//		if(DFT.index_count == 11)
//		break;
//	}
//	delay_us(2000);
//	
//	//-------- offset ���� ����
//	for(temp = 0; temp < 10; temp++)
//	{
//		CALIBRATION.offset[temp] = Offset_Check(temp);
//	}
//
//	
//	//-------- high level ���� ����
//	//124�� 
//	high[0] = DFT_Check(0, 1000, 3000);
//	high[1] = DFT_Check(1, 1000, 3000);
//	high[2] = DFT_Check(2, 1000, 3000);
//	
//	//ZCT
//	if(CORE.gr_select == ZCT_SELECT)
//	high[5] = DFT_Check(5, 7580, 22741);
//	else
//	high[3] = DFT_Check(3, 1200, 3600);
//	
//	high[6] = DFT_Check(6, 5450, 16350);
//	high[7] = DFT_Check(7, 5450, 16350);
//	high[8] = DFT_Check(8, 5450, 16350);
//	high[9] = DFT_Check(9, 10890, 32680);
//		
//	trms_high[0] = TRMS_Check(&MEASUREMENT.true_rms[Ia]);
//	trms_high[1] = TRMS_Check(&MEASUREMENT.true_rms[Ib]);
//	trms_high[2] = TRMS_Check(&MEASUREMENT.true_rms[Ic]);
//	trms_high[3] = TRMS_Check(&MEASUREMENT.true_rms[In]);
//	
//	//ZCT
//	trms_high[5] = TRMS_Check(&MEASUREMENT.true_rms[Is]);
//	
//	trms_high[6] = TRMS_Check(&MEASUREMENT.true_rms[Va]);
//	trms_high[7] = TRMS_Check(&MEASUREMENT.true_rms[Vb]);
//	trms_high[8] = TRMS_Check(&MEASUREMENT.true_rms[Vc]);
//	trms_high[9] = TRMS_Check(&MEASUREMENT.true_rms[Vn]);
//	
//	trms_high[0] *= INTERNAL_CT_RATIO;
//	trms_high[1] *= INTERNAL_CT_RATIO;
//	trms_high[2] *= INTERNAL_CT_RATIO;
//	trms_high[3] *= INTERNAL_CT_RATIO;
//    
//	//ZCT
//	trms_high[5] *= INTERNAL_ZCT_RATIO;
//	
//	trms_high[6] *= INTERNAL_PT_RATIO;
//	trms_high[7] *= INTERNAL_PT_RATIO;
//	trms_high[8] *= INTERNAL_PT_RATIO;
//	trms_high[9] *= INTERNAL_GPT_RATIO;
//	
//	p_high[0] = P_Check(&MEASUREMENT.Pa_value);
//	p_high[1] = P_Check(&MEASUREMENT.Pb_value);
//	p_high[2] = P_Check(&MEASUREMENT.Pc_value);
//	
//	p_high[0] *= INTERNAL_CPT_RATIO;
//	p_high[1] *= INTERNAL_CPT_RATIO;
//	p_high[2] *= INTERNAL_CPT_RATIO;
//	
//	TIMER.lcd = 0;
//	
//	// 1�� ���
//	for(;;)
//	{
//		if(TIMER.lcd > 1000)
//		break;		
//	}
//	
//	// ���� ����
//	// Va �������� ��ü ���� ����
//	CALIBRATION.angle[6] = 0;
//	CALIBRATION.angle[0] = Degree_Check(0);
//	CALIBRATION.angle[1] = Degree_Check(1);
//	CALIBRATION.angle[2] = Degree_Check(2);
//	CALIBRATION.angle[3] = Degree_Check(3);
//	//ZCT
//	CALIBRATION.angle[5] = Degree_Check(5);
//	CALIBRATION.angle[7] = Degree_Check(7);
//	CALIBRATION.angle[8] = Degree_Check(8);
//	CALIBRATION.angle[9] = Degree_Check(9);
//	
//	// ���ļ� ����
//	freq_index = DFT.index_count;
//	for(;;)
//	{
//		if(freq_index != DFT.index_count)
//		{
//			freq_index = DFT.index_count;
//			freq_temp += MEASUREMENT.frequency;
//			++freq_count;
//			if(freq_count == 100)
//			break;
//		}
//	}
//	freq_temp /= 100;
//	CALIBRATION.frequency_offset = freq_temp;
//    
//	//-------- Low level ���� ����
//	// 10V, 1A
//	if(CORE.rated_ct == CT_1A)
//	{
////		if(CORE.gr_select == ZCT_SELECT)
////		lcd_refresh_drive(calibration9);
////		else
////		lcd_refresh_drive(calibration7);
//	}
//	else
//	{
////		if(CORE.gr_select == ZCT_SELECT)
////		lcd_refresh_drive(calibration5);
////		else
////		lcd_refresh_drive(calibration3);
//	}
//	TIMER.lcd = 0;
//	
//	// 1�� ���
//	for(;;)
//	{
//		if(TIMER.lcd > 1000)
//		break;		
//	}
//
//	//150��
////	low[0] = DFT_Check(0, 156, 468);
////	low[1] = DFT_Check(1, 156, 468);
////	low[2] = DFT_Check(2, 156, 468);
//	
//	//125��
//	low[0] = DFT_Check(0, 120, 400);
//	low[1] = DFT_Check(1, 120, 400);
//	low[2] = DFT_Check(2, 120, 400);
//	
//	//ZCT
//	if(CORE.gr_select == ZCT_SELECT)
//	low[5] = DFT_Check(5, 3832, 11496);
//	else	
//	low[3] = DFT_Check(3, 150, 500);
//	
//	low[6] = DFT_Check(6, 412, 1236);
//	low[7] = DFT_Check(7, 412, 1236);
//	low[8] = DFT_Check(8, 412, 1236);
//	low[9] = DFT_Check(9, 890, 2671);
//	
//	trms_low[0] = TRMS_Check(&MEASUREMENT.true_rms[Ia]);
//	trms_low[1] = TRMS_Check(&MEASUREMENT.true_rms[Ib]); 
//	trms_low[2] = TRMS_Check(&MEASUREMENT.true_rms[Ic]); 
//	trms_low[3] = TRMS_Check(&MEASUREMENT.true_rms[In]); 
//	
//	//ZCT
//	trms_low[5] = TRMS_Check(&MEASUREMENT.true_rms[Is]);
//    
//	trms_low[6] = TRMS_Check(&MEASUREMENT.true_rms[Va]); 
//	trms_low[7] = TRMS_Check(&MEASUREMENT.true_rms[Vb]); 
//	trms_low[8] = TRMS_Check(&MEASUREMENT.true_rms[Vc]); 
//	trms_low[9] = TRMS_Check(&MEASUREMENT.true_rms[Vn]); 
//	 
//	trms_low[0] *= INTERNAL_CT_RATIO;
//	trms_low[1] *= INTERNAL_CT_RATIO;
//	trms_low[2] *= INTERNAL_CT_RATIO;
//	trms_low[3] *= INTERNAL_CT_RATIO;
//   	
//	//ZCT
//	trms_low[5] *= INTERNAL_ZCT_RATIO;
//	
//	trms_low[6] *= INTERNAL_PT_RATIO;
//	trms_low[7] *= INTERNAL_PT_RATIO;
//	trms_low[8] *= INTERNAL_PT_RATIO;
//	trms_low[9] *= INTERNAL_GPT_RATIO;
//
//	p_low[0] = P_Check(&MEASUREMENT.Pa_value);
//	p_low[1] = P_Check(&MEASUREMENT.Pb_value);
//	p_low[2] = P_Check(&MEASUREMENT.Pc_value);
//	
//	p_low[0] *= INTERNAL_CPT_RATIO;
//	p_low[1] *= INTERNAL_CPT_RATIO;
//	p_low[2] *= INTERNAL_CPT_RATIO;
//
////-------- �⺻�� ������ ���
//	for(temp = 0; temp < 6; temp++)
//	{
//		slope[temp] = 5 / (high[temp] - low[temp]);
//		CALIBRATION.slope_1st[temp] = slope[temp];
//		trms_slope[temp] = 5 / (trms_high[temp] - trms_low[temp]);
//		CALIBRATION.slope_trms[temp] = trms_slope[temp];
//	}
//	for(temp = 6; temp < 10; temp++)
//	{
//		slope[temp] = 100 / (high[temp] - low[temp]);
//		CALIBRATION.slope_1st[temp] = slope[temp];
//		trms_slope[temp] = 100 / (trms_high[temp] - trms_low[temp]);
//		CALIBRATION.slope_trms[temp] = trms_slope[temp];
//	}
//	for(temp = 0; temp < 3; temp++)
//	{
//		p_slope[temp] = ((110 * 6) - (10 * 1)) / (p_high[temp] - p_low[temp]);
//		CALIBRATION.slope_power[temp] = p_slope[temp];
//	}
//	
//	// ùȸ ��ٸ�	
//	for(;;)
//	{
//		if(DISPLAY.Power_Up == 0xaaaa)
//		{
//			DISPLAY.Power_Up = 0;
//			break;
//		}
//	}
//	for(;;)
//	{
//		if(DISPLAY.Power_Up == 0xaaaa)
//		{	
//			// raw ��
//			PVa = MEASUREMENT.Pa_value;
//			PVb = MEASUREMENT.Pb_value;
//			PVc = MEASUREMENT.Pc_value;
//				
//			QVa = MEASUREMENT.Qa_value;
//			QVb = MEASUREMENT.Qb_value;
//			QVc = MEASUREMENT.Qc_value;
//				
//			DISPLAY.Power_Up = 0;
//				
//			break;
//		}
//	}
//	
//	PVa /= 720;
//	PVb /= 720;
//	PVc /= 720;
//	
//	QVa /= 720;
//	QVb /= 720;
//	QVc /= 720;
//	
//	PVa *= CALIBRATION.slope_power[0];
//	PVb *= CALIBRATION.slope_power[1];
//	PVc *= CALIBRATION.slope_power[2];
//	
//	QVa *= CALIBRATION.slope_power[0];
//	QVb *= CALIBRATION.slope_power[1];
//	QVc *= CALIBRATION.slope_power[2];
//	
//	PVa *= INTERNAL_CPT_RATIO;
//	PVb *= INTERNAL_CPT_RATIO;
//	PVc *= INTERNAL_CPT_RATIO;
//	
//	QVa *= INTERNAL_CPT_RATIO;
//	QVb *= INTERNAL_CPT_RATIO;
//	QVc *= INTERNAL_CPT_RATIO;
//	
//	CALIBRATION.Power_Cos[0] =  ((10 * 1) * PVa) / ((PVa * PVa) + (QVa * QVa));
//	CALIBRATION.Power_Sin[0] = -((10 * 1) * QVa) / ((PVa * PVa) + (QVa * QVa));
//	
//	CALIBRATION.Power_Cos[1] =  ((10 * 1) * PVb) / ((PVb * PVb) + (QVb * QVb));		
//	CALIBRATION.Power_Sin[1] = -((10 * 1) * QVb) / ((PVb * PVb) + (QVb * QVb));
//	
//	CALIBRATION.Power_Cos[2] =  ((10 * 1) * PVc) / ((PVc * PVc) + (QVc * QVc));		
//	CALIBRATION.Power_Sin[2] = -((10 * 1) * QVc) / ((PVc * PVc) + (QVc * QVc));
//
////-------- EEROM ����
//	eerom_control(4, 0x80);
//	eerom_control(4, 0xc0);
//	
//	for(i = 0; i < 10; i++)
//	{
//		eerom_write(i, &CALIBRATION.offset[i]);
//		temp1[i] = CALIBRATION.offset[i];
//	}
//	
//	for(i = 0; i < 10; i++)
//	{
//		void_p = &CALIBRATION.slope_1st[i];
//		temp16_p = (unsigned int *)void_p;
//		eerom_write(0x10 + (i << 1), temp16_p);
//		eerom_write(0x11 + (i << 1), temp16_p + 1);
//		
//		temp1[10 + (i << 1)] = *temp16_p;
//		temp1[11 + (i << 1)] = *(temp16_p + 1);
//	}
//	
//	for(i = 0; i < 10; i++)
//	{
//		void_p = &CALIBRATION.slope_trms[i];
//		temp16_p = (unsigned int *)void_p;
//		eerom_write(0x30 + (i << 1), temp16_p);
//		eerom_write(0x31 + (i << 1), temp16_p + 1);
//		
//		temp1[30 + (i << 1)] = *temp16_p;
//		temp1[31 + (i << 1)] = *(temp16_p + 1);
//	}
//	
//	for(i = 0; i < 10; i++)
//	{
//		void_p = &CALIBRATION.angle[i];
//		temp16_p = (unsigned int *)void_p;
//		eerom_write(0x50 + (i << 1), temp16_p);
//		eerom_write(0x51 + (i << 1), temp16_p + 1);
//		
//		temp1[50 + (i << 1)] = *temp16_p;
//		temp1[51 + (i << 1)] = *(temp16_p + 1);
//	}
//	
//	for(i = 0; i < 3; i++)
//	{
//		void_p = &CALIBRATION.slope_power[i];
//		temp16_p = (unsigned int *)void_p;
//		eerom_write(0x70 + (i << 1), temp16_p);
//		eerom_write(0x71 + (i << 1), temp16_p + 1);
//		
//		temp1[70 + (i << 1)] = *temp16_p;
//		temp1[71 + (i << 1)] = *(temp16_p + 1);
//	}
//	
//	for(i = 0; i < 3; i++)
//	{
//		void_p = &CALIBRATION.Power_Cos[i];
//		temp16_p = (unsigned int *)void_p;
//		eerom_write(0x80 + (i << 1), temp16_p);
//		eerom_write(0x81 + (i << 1), temp16_p + 1);
//		
//		temp1[76 + (i << 1)] = *temp16_p;
//		temp1[77 + (i << 1)] = *(temp16_p + 1);
//	}
//	
//	for(i = 0; i < 3; i++)
//	{
//		void_p = &CALIBRATION.Power_Sin[i];
//		temp16_p = (unsigned int *)void_p;
//		eerom_write(0x86 + (i << 1), temp16_p);
//		eerom_write(0x87 + (i << 1), temp16_p + 1);
//		
//		temp1[82 + (i << 1)] = *temp16_p;
//		temp1[83 + (i << 1)] = *(temp16_p + 1);
//	}
//	
//	void_p = &CALIBRATION.frequency_offset;
//	temp16_p = (unsigned int *)void_p;
//	eerom_write(0x90 + (i << 1), temp16_p);
//	eerom_write(0x91 + (i << 1), temp16_p + 1);
//	
//	temp1[88] = *temp16_p;
//	temp1[89] = *(temp16_p + 1);
//	
//	i = Setting_CRC(temp1, 90);
//	eerom_write(0xa0, &i);
////-------- EEROM ���� END
//
//// TURN OFF �޼���
////	lcd_refresh_drive(calibration10);
//	
//	// turn off
//	for(;;)
//	;
//}

