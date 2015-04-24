#include "define.h"
#include "extern_variable.h"
#include "extern_prototype.h"

//-----------------------------------------------------------------
//	< Cal_RMS = Sampling Time Calculation >
//	12 [Samples/Cycle] (Total: 16channel)
//	-> 1 / (60[Hz] x 12[Samples]) = 1.3888[msec]
//	-> 1 / (50[Hz] x 12[Samples]) = 1.6667[msec]
//-------------------------------------------------------------------

//timer0 -> XINT3_ISR
// �ִ� about 250us
// 1.3888ms ��� 18.01%

//timer1 -> TINT1_ISR
// �ִ� about 7us
// 1ms ��� 0.7%

//main �ѹ���
// �ִ� 7us

// �⺻��
//  0,  1,  2,  3,   4,   5,  6,  7,  8,  9
// Ia, Ib, Ic, In, In2, ZCT, Va, Vb, Vc, Vn

// 0 ~ 9 : I, V
// 13 - tcs
// 14 - ccs
// 15 - ref.
//tcs/ccs
//���� dc110
//���� ac220, dc320

interrupt void XINT3_ISR(void)
{
	//�޸� addressing
	// �ܺ� ���� cpld���� 1.38ms ���� �ѹ��� ���ͷ�Ʈ�� �߻���Ŵ
	// adc ���� cpld �� ä�κ�(10ch) ���� ���þ� dpram�� ��ܳ��� �� ���ͷ�Ʈ�� �߻���Ŵ
	// �ٵ� �޸� ������ �� �����̰� �����ư��鼭 ad �������� ������ ��
	// �׷��� ���ͷ�Ʈ �߻� �� ���� gpio ��ȣ�� ���� ��� �޸𸮿������� �ľ��Ͽ� �ش� �޸𸮸� �о��
	// #define AD_buffer_low          (int *)0x4000 
  // #define AD_buffer_high         (int *)0x4030
  // ��� �����ּҿ��� �Ʒ��� ���� �迭��
  //offset -   0,  1,  2,  3,   4,    5,   6,  7,  8,  9
  //          Ia, Ib, Ic, In, In2,  ZCT,  Va, Vb, Vc, Vn
  
  // adc�� dsp���̿� �ִ� dpram ���� �ּ� ����
	if(*GpioIntRegs_GPADAT_Low & 0x0040)	{SAMPLE.dpram = AD_buffer_low;}
	else																	{SAMPLE.dpram = AD_buffer_high;}
	
	// �������� ���̱� ����
	// �ѹ��� ������ �дٺ��� ���� ���� ���� ����� �� ���ϱ�����, ���������� ���̱� ���� �̸� ���س�
	DFT.index_count_3times = DFT.index_count * 3;

	//10 ä�α���
  // 10ä�� �� ������ ����ϴ� ä���� 8ä�� -> 10ä���� H/W���̰� �̸� ����� ������
  // F, M �� ��������
  // NCT(OCGR �Ǵ� DGR) ��� �ÿ��� ä�� 0, 1, 2, 3, 6, 7, 8, 9 �� ���
  // ZCT(SGR)           ��� �ÿ��� ä�� 0, 1, 2, 5, 6, 7, 8, 9 �� ���
	for(TIMER0.temp16_1 = 0; TIMER0.temp16_1 < 10; TIMER0.temp16_1++)	// 10ä�� �ݺ�
	{
		// dpram�� ����ִ� �͵� ������ �ϴ� read
		// rawdata�� signed integer �ε�, �Ʒ��κп��� float ����� �ؾ��ϹǷ� �̸� float�� casting�Ͽ� ����
		SAMPLE.first  = (float)(SAMPLE.dpram[TIMER0.temp16_1     ] - CALIBRATION.offset[TIMER0.temp16_1]);
		SAMPLE.second = (float)(SAMPLE.dpram[TIMER0.temp16_1 + 16] - CALIBRATION.offset[TIMER0.temp16_1]);
		SAMPLE.third  = (float)(SAMPLE.dpram[TIMER0.temp16_1 + 32] - CALIBRATION.offset[TIMER0.temp16_1]);
		
		//������ ���� �˰����� ����
		// Himap adc ���� �� ���� ������ ���ÿ� ������ �Ǿ� ����
		// ������ �� ��Ÿ ������ �ƴ� �������� ��Ȳ�̶�� ���� ���� �� �ι�° ������ ù��°-����° ���� ��հ��� ������ ����
		// ���������� ����tern�� ����°�� �̹� tern ù��° �ι�° ������ ���赵 ��� ������ �����Ͽ��� ��
		// ��, peak ġ �αٰ�, ����ũ�ν� �αٿ����� ��� ������ ���� ���� ���ɼ��� ũ�� ������ ���� ������ ���ԵǾ� ����
		if(SAMPLE.normal) // calibration �� �Ǵ� debug �� ���������� �˰������� �����ϸ� �̻����� ����� �����Ƿ� ��� ���ÿ��� �����ϱ����� �÷��� ���
		{
			// 1st, 3rd ��հ� ���ϱ� ����
			SAMPLE.average = SAMPLE.first + SAMPLE.third;
			
			// �и� 0�ϰ�� cpu exception ȸ�� ����
			// �и� 0�λ��¿��� ������ ������ �Ұ�� cpu ������ �ΰ��� �迭�� ����
			// 1- cpu ���� -> �ϵ���� FPU ���� runtime library�� ����ϴ� cpu���� ��Ÿ��
			// 2 - ������ ����� ������ ��� -> TI dsp ������ �ʴµ� ���� ������� ���� ���� ����
			// ���� �и� 0�� ��� �ƿ� �������� ���� �ʰ� ȸ��
			// ���� ��� ù��° ����(-10), �ι�° ���� ����ũ�ν�(0), ����° ����(10)
			// SAMPLE.average�� �Ʒ��κп��� �и�� �����
			if(SAMPLE.average == 0)
			{
				// SAMPLE.average_ratio �� ������ ���� �Էµ� ���ð� ���� ������ ũ�⸦ ���ϴ� ���� ������
				// SAMPLE.average_ratio�� ���� ������ ǥ�õǴ� ���� �����ϱ� ���� 
				// ���� �ι�° ����(SAMPLE.second)�� ������ ��� SAMPLE.average�� ���̳ʽ��� �����
				if(SAMPLE.second < 0)	{SAMPLE.average = -2;}
				else									{SAMPLE.average = 2;}
			}
			
			// 1st, 3rd ��հ�
			SAMPLE.average /= 2;
			
			// 2nd�� 1st, 3rd ��հ� ������ ���Ͽ� SAMPLE.average_ratio ����
			SAMPLE.average_ratio = SAMPLE.second / SAMPLE.average;
		
			// 10% �̳� 0.9 �̻� 1.1 ����
			if((SAMPLE.average_ratio >= 0.9) && (SAMPLE.average_ratio <= 1.1))
			{
				//��ü 36����
				// 0, 1, 2 | 3, 4, 5 | 6, 7, 8 | 9, 10, 11 | 12, 13, 14 | 15, 16, 17 | 18, 19, 20 | 21, 22, 23 | 24, 25, 26 | 27, 28, 29 | 30, 31, 32 | 33, 34, 35
				
				// ��� SAMPLE.average_ratio = SAMPLE.second / SAMPLE.average; ������ ���� tern ���� ���� ����
				// �׷��� ���� tern�� ���ؼ��� ��¥ ���������� �Ǵ��ϱ� ���� ���� tern�� 3rd sample�� �ٽ� �񱳸� �ϵ��� �Ѵ�
				
				// ������ �̻� ��Ȳ�� �����ٸ�...
				if(SAMPLE.abnormal[TIMER0.temp16_1] == 0) // �̻��Ȳ �߻� �÷���
				{
					// DFT.index_count�� ������ 12���� ī���� �̴�.
					// 0��°��� ��� ������ �������� ���� tern�� 3rd sample�� 35��° sample �̴�.
					if(DFT.index_count == 0)	{SAMPLE.pre_value = (float)SAMPLE.pre_36_buffer[TIMER0.temp16_1][35];}
					
					// �׿ܿ��� DFT.index_count_3times(������ DFT.index_count�� 3�� ���س� ����)�� ���� 1�ϸ� ���� tern 3rd sample �̴�.
					else											{SAMPLE.pre_value = (float)SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times - 1];}
				}
				
				// ���� �̻� �־���
				// �̻� �����̶� �ǴܵǸ� SAMPLE.abnormal_value[]�� �ش� ���� ������ ����. 
				// �̻� �� ����� �� load
				else	{SAMPLE.pre_value = SAMPLE.abnormal_value[TIMER0.temp16_1];}
				
				// ó�� �� ��ƾ�� ���� �������÷� ��
				// ���
				SAMPLE.average = SAMPLE.pre_value + SAMPLE.second;
				
				// cpu exception ȸ�� ->  ���� ���� ����
				if(SAMPLE.average == 0)
				{
					if(SAMPLE.first < 0)	{SAMPLE.average = -2;}
					else									{SAMPLE.average = 2;}
				}
				
				SAMPLE.average /= 2;
				
				SAMPLE.average_ratio = SAMPLE.first / SAMPLE.average; // ���� ���� ����
				
				// ��¥��� ���� -> 10% �̳�
				if((SAMPLE.average_ratio >= 0.9) && (SAMPLE.average_ratio <= 1.1))
				{
					// ���ֱ� 36���� ���� ����
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times    ] = (int)SAMPLE.first;
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1] = (int)SAMPLE.second;
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 2] = (int)SAMPLE.third;
					
					// rms ���� ���� ����
					SAMPLE.rms_12_buffer[TIMER0.temp16_1][DFT.index_count] = (int)SAMPLE.second;
					
					// wave normal
					if(WAVE.post_start != 0x1234)
					{
						WAVE.buffer = (int *)wave_buffer_address[TIMER0.temp16_1];
			
						*(WAVE.buffer + WAVE.pre_count    ) = (int)SAMPLE.first;
						*(WAVE.buffer + WAVE.pre_count + 1) = (int)SAMPLE.second;
						*(WAVE.buffer + WAVE.pre_count + 2) = (int)SAMPLE.third;
					}
			
					// wave fault
					else
					{
						if(WAVE.post_count != 5400)
						{
							WAVE.buffer = (int *)wave_buffer_post_address[TIMER0.temp16_1];
					
							*(WAVE.buffer + WAVE.post_count    ) = (int)SAMPLE.first;
							*(WAVE.buffer + WAVE.post_count + 1) = (int)SAMPLE.second;
							*(WAVE.buffer + WAVE.post_count + 2) = (int)SAMPLE.third;
						}
					}
					
					// �������� ������ ���Դٰ� �Ǵ��߱� ������ �÷��׸� �ʱ�ȭ
					SAMPLE.abnormal[TIMER0.temp16_1] = 0;
				}
				
				// �ѹ� �� Ȯ��
				else
				{
					SAMPLE.average_ratio = SAMPLE.average - SAMPLE.first;
				
					//zero cross��� �Ǵ�
					if((SAMPLE.average_ratio >= -50) && (SAMPLE.average_ratio <= 50))
					{
						// ���ֱ� 36���� ���� ����
						SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times    ] = (int)SAMPLE.first;
						SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1] = (int)SAMPLE.second;
						SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 2] = (int)SAMPLE.third;
					
						// rms ���� ���� ����
						SAMPLE.rms_12_buffer[TIMER0.temp16_1][DFT.index_count] = (int)SAMPLE.second;
						
						// wave normal
						if(WAVE.post_start != 0x1234)
						{
							WAVE.buffer = (int *)wave_buffer_address[TIMER0.temp16_1];
			
							*(WAVE.buffer + WAVE.pre_count    ) = (int)SAMPLE.first;
							*(WAVE.buffer + WAVE.pre_count + 1) = (int)SAMPLE.second;
							*(WAVE.buffer + WAVE.pre_count + 2) = (int)SAMPLE.third;
						}
			
						// wave fault
						else
						{
							if(WAVE.post_count != 5400)
							{
								WAVE.buffer = (int *)wave_buffer_post_address[TIMER0.temp16_1];
					
								*(WAVE.buffer + WAVE.post_count    ) = (int)SAMPLE.first;
								*(WAVE.buffer + WAVE.post_count + 1) = (int)SAMPLE.second;
								*(WAVE.buffer + WAVE.post_count + 2) = (int)SAMPLE.third;
							}
						}
						SAMPLE.abnormal[TIMER0.temp16_1] = 0;
					}
					
					// ��������� ���� ������ ���޾Ҵٸ� ��¥�� �̻��� �����̴�.
					else
					{
						// �̹� ������ ���� ���ϴ� ���ֱ� �� ���÷� ��ü�Ѵ�.
						SAMPLE.rms_12_buffer[TIMER0.temp16_1][DFT.index_count] = SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1];
						
						// ������ wave ������ ���ֱ� �� sample ������ ��� ��ġ
						// wave normal
						if(WAVE.post_start != 0x1234)
						{
							WAVE.buffer = (int *)wave_buffer_address[TIMER0.temp16_1];
			
							*(WAVE.buffer + WAVE.pre_count    ) = SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times    ];
							*(WAVE.buffer + WAVE.pre_count + 1) = SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1];
							*(WAVE.buffer + WAVE.pre_count + 2) = SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 2];
						}
			
						// wave fault
						else
						{
							if(WAVE.post_count != 5400)
							{
								WAVE.buffer = (int *)wave_buffer_post_address[TIMER0.temp16_1];
					
								*(WAVE.buffer + WAVE.post_count    ) = SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times    ];
								*(WAVE.buffer + WAVE.post_count + 1) = SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1];
								*(WAVE.buffer + WAVE.post_count + 2) = SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 2];
							}
						}
						SAMPLE.abnormal[TIMER0.temp16_1] = 1;
						SAMPLE.abnormal_value[TIMER0.temp16_1] = SAMPLE.third;
					}
				}				
			}
			else
			{
				SAMPLE.average_ratio = SAMPLE.average - SAMPLE.second;
				
				//zero cross
				if((SAMPLE.average_ratio >= -50) && (SAMPLE.average_ratio <= 50))
				{
					// ���ֱ� 36���� ���� ����
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times    ] = (int)SAMPLE.first;
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1] = (int)SAMPLE.second;
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 2] = (int)SAMPLE.third;
					
					// rms ���� ���� ����
					SAMPLE.rms_12_buffer[TIMER0.temp16_1][DFT.index_count] = (int)SAMPLE.second;
					
					// wave normal
					if(WAVE.post_start != 0x1234)
					{
						WAVE.buffer = (int *)wave_buffer_address[TIMER0.temp16_1];
			
						*(WAVE.buffer + WAVE.pre_count    ) = (int)SAMPLE.first;
						*(WAVE.buffer + WAVE.pre_count + 1) = (int)SAMPLE.second;
						*(WAVE.buffer + WAVE.pre_count + 2) = (int)SAMPLE.third;
					}
			
					// wave fault
					else
					{
						if(WAVE.post_count != 5400)
						{
							WAVE.buffer = (int *)wave_buffer_post_address[TIMER0.temp16_1];
					
							*(WAVE.buffer + WAVE.post_count    ) = (int)SAMPLE.first;
							*(WAVE.buffer + WAVE.post_count + 1) = (int)SAMPLE.second;
							*(WAVE.buffer + WAVE.post_count + 2) = (int)SAMPLE.third;
						}
					}
					SAMPLE.abnormal[TIMER0.temp16_1] = 0;
				}
				
				else
				{
					SAMPLE.rms_12_buffer[TIMER0.temp16_1][DFT.index_count] = SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1];
				
					SAMPLE.abnormal[TIMER0.temp16_1] = 1;
						
					SAMPLE.abnormal_value[TIMER0.temp16_1] = SAMPLE.third;					
				}
			}
		}
		
		// �ʱ� �Ǵ� calibration �� -> ���������� �˰����� ������
		else
		{
			// ���� ������ SAMPLE.pre_36_buffer[]�� ȣ�η� ����
			SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times    ] = (int)SAMPLE.first;
			SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1] = (int)SAMPLE.second;
			SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 2] = (int)SAMPLE.third;
			
			// ������ 12����(RMS ���ϱ�����) ����
			SAMPLE.rms_12_buffer[TIMER0.temp16_1][DFT.index_count] = (int)SAMPLE.second;
			
			// wave normal
			// ����� ���� ������ ��Ȳ���� ����
			if(WAVE.post_start != 0x1234)
			{
				WAVE.buffer = (int *)wave_buffer_address[TIMER0.temp16_1];
			
				*(WAVE.buffer + WAVE.pre_count    ) = (int)SAMPLE.first;
				*(WAVE.buffer + WAVE.pre_count + 1) = (int)SAMPLE.second;
				*(WAVE.buffer + WAVE.pre_count + 2) = (int)SAMPLE.third;
			}
			
			// wave fault
			// ������� ���� �� ����
			else
			{
				if(WAVE.post_count != 5400)
				{
					WAVE.buffer = (int *)wave_buffer_post_address[TIMER0.temp16_1];
					
					*(WAVE.buffer + WAVE.post_count    ) = (int)SAMPLE.first;
					*(WAVE.buffer + WAVE.post_count + 1) = (int)SAMPLE.second;
					*(WAVE.buffer + WAVE.post_count + 2) = (int)SAMPLE.third;
				}
			}
		}
		//������ ���� �Ϸ�

//-------- �ʿ���� ä�� DFT ��� ����
		// In ���� ����
		// �δ����� 1.5k ¥�� 0.02In �������� rms���� ���̰� ������ ���� ����
		// ���� In2 ������� ����
  	// offset -  0,  1,  2,  3,   4,  5,  6,  7,  8,  9
  	//          Ia, Ib, Ic, In, In2, Is, Va, Vb, Vc, Vn
		
		if(TIMER0.temp16_1 == 3)	//In ä��
		{
			if(CORE.gr_select == ZCT_SELECT)	{goto daum;}	// ZCT ��� �� In ä�� DFT ��� ����, �н�
		}
		else if(TIMER0.temp16_1 == 4)				{goto daum;}	// In2 ä�� ��� ����, �н�
		else if(TIMER0.temp16_1 == 5)	// Is ä��
		{
			if(CORE.gr_select != ZCT_SELECT)	{goto daum;}	// NCT ��� �� Is ä�� DFT ��� ����, �н�
		}
//-------- �ʿ���� ä�� DFT ��� ���� END
		
//-------- DFT ��� ����
		DFT.real_value[TIMER0.temp16_1] = 0;
		DFT.imag_value[TIMER0.temp16_1] = 0;

		//-------- true rms ��� ����
		//if(DFT.index_count == 11)	{MEASUREMENT.true_rms[TIMER0.temp16_1] = 0;} // true rms ����� 1�ֱ⸶�� ��
		//-------- true rms ��� ���� END

		// full window dft ����(o)-�����찡 �̵��� �� ��ü ���� ���� ������ ���
		// recursive dft ����(x) - �����찡 �̵��� �� ������ ���ð� ���� ���Ե� ���ø� �����Ͽ� ���
		for(TIMER0.temp16_2 = 0; TIMER0.temp16_2 < 12; TIMER0.temp16_2++)
		{
			// 2/12 * cos(2pi*(n)) * x[n]			
			DFT.real_value[TIMER0.temp16_1] += (float)(SAMPLE.rms_12_buffer[TIMER0.temp16_1][TIMER0.temp16_2] * cos_table[TIMER0.temp16_2]);
			DFT.imag_value[TIMER0.temp16_1] += (float)(SAMPLE.rms_12_buffer[TIMER0.temp16_1][TIMER0.temp16_2] * sin_table[TIMER0.temp16_2]);

			//-------- true rms ��� ����
			//if(DFT.index_count == 11) // true rms��, 1�ֱ� ���� ä������ ���� �� ����
			//{
			//	TIMER0.temp_float1 = (float)SAMPLE.rms_12_buffer[TIMER0.temp16_1][TIMER0.temp16_2];
			//	MEASUREMENT.true_rms[TIMER0.temp16_1] += TIMER0.temp_float1 * TIMER0.temp_float1;
			//}
			//-------- true rms ��� ���� END
		}
		// rms �� ��� 
		TIMER0.temp_float2 = sqrt((DFT.real_value[TIMER0.temp16_1] * DFT.real_value[TIMER0.temp16_1]) + (DFT.imag_value[TIMER0.temp16_1] * DFT.imag_value[TIMER0.temp16_1]));
//-------- DFT ��� END
		
		MEASUREMENT.rms_value[TIMER0.temp16_1] = TIMER0.temp_float2 * CALIBRATION.slope_1st[TIMER0.temp16_1];	// calibration factor ����, ������ ����ϴ� ���� rms ��
		
		//���� ���
		MEASUREMENT.angle[TIMER0.temp16_1] = atan2(DFT.real_value[TIMER0.temp16_1], DFT.imag_value[TIMER0.temp16_1]);	// radian���� display �Ҷ� degree�� ��ȯ
		MEASUREMENT.angle[TIMER0.temp16_1] += CALIBRATION.angle[TIMER0.temp16_1]; // calibration factor ����, ������ ����ϴ� ���� angle ��
		
		//-------- true rms ��� ����
		//// 1�ֱⰪ ä������ ��չ� ������, �İ��� �� display���� ǥ����.
		//if(DFT.index_count == 11)
		//{
		//	MEASUREMENT.true_rms[TIMER0.temp16_1] /= 12;
		//	MEASUREMENT.true_rms[TIMER0.temp16_1] = sqrt(MEASUREMENT.true_rms[TIMER0.temp16_1]);
		//}
		//-------- true rms ��� ���� END

daum:		;
	}

//-------- ���󼺺�, ���� ��� ���
	//����, ���� ����� ��Ī��ǥ������ ������ �����
	MEASUREMENT.real_I1_a = MEASUREMENT.rms_value[Ia] * cos(MEASUREMENT.angle[Ia]);
	MEASUREMENT.imag_I1_a = MEASUREMENT.rms_value[Ia] * sin(MEASUREMENT.angle[Ia]);
	
	MEASUREMENT.real_I1_b = MEASUREMENT.rms_value[Ib] * cos(2.094395103 + MEASUREMENT.angle[Ib]);
	MEASUREMENT.imag_I1_b = MEASUREMENT.rms_value[Ib] * sin(2.094395103 + MEASUREMENT.angle[Ib]);
	
	MEASUREMENT.real_I1_c = MEASUREMENT.rms_value[Ic] * cos(4.188790205 + MEASUREMENT.angle[Ic]);
	MEASUREMENT.imag_I1_c = MEASUREMENT.rms_value[Ic] * sin(4.188790205 + MEASUREMENT.angle[Ic]);
	
	MEASUREMENT.real_I1_sum = MEASUREMENT.real_I1_a + MEASUREMENT.real_I1_b + MEASUREMENT.real_I1_c;
	MEASUREMENT.imag_I1_sum = MEASUREMENT.imag_I1_a + MEASUREMENT.imag_I1_b + MEASUREMENT.imag_I1_c;
	
	MEASUREMENT.I1_value = sqrt((MEASUREMENT.real_I1_sum * MEASUREMENT.real_I1_sum) + 
	                            (MEASUREMENT.imag_I1_sum * MEASUREMENT.imag_I1_sum));
	MEASUREMENT.I1_value /= 3; //���� ���� ����
	
	MEASUREMENT.real_I2_a = MEASUREMENT.real_I1_a;
	MEASUREMENT.imag_I2_a = MEASUREMENT.imag_I1_a;
	
	MEASUREMENT.real_I2_b = MEASUREMENT.rms_value[Ib] * cos(4.188790205 + MEASUREMENT.angle[Ib]);
	MEASUREMENT.imag_I2_b = MEASUREMENT.rms_value[Ib] * sin(4.188790205 + MEASUREMENT.angle[Ib]);
	
	MEASUREMENT.real_I2_c = MEASUREMENT.rms_value[Ic] * cos(2.094395103 + MEASUREMENT.angle[Ic]);
	MEASUREMENT.imag_I2_c = MEASUREMENT.rms_value[Ic] * sin(2.094395103 + MEASUREMENT.angle[Ic]);
	
	MEASUREMENT.real_I2_sum = MEASUREMENT.real_I2_a + MEASUREMENT.real_I2_b + MEASUREMENT.real_I2_c;
	MEASUREMENT.imag_I2_sum = MEASUREMENT.imag_I2_a + MEASUREMENT.imag_I2_b + MEASUREMENT.imag_I2_c;
	
	MEASUREMENT.I2_value = sqrt((MEASUREMENT.real_I2_sum * MEASUREMENT.real_I2_sum) + 
	                            (MEASUREMENT.imag_I2_sum * MEASUREMENT.imag_I2_sum));
	MEASUREMENT.I2_value /= 3; //���� ���� ����
	
	MEASUREMENT.real_V1_a = MEASUREMENT.rms_value[Va] * cos(MEASUREMENT.angle[Va]);
	MEASUREMENT.imag_V1_a = MEASUREMENT.rms_value[Va] * sin(MEASUREMENT.angle[Va]);
	
	MEASUREMENT.real_V1_b = MEASUREMENT.rms_value[Vb] * cos(2.094395103 + MEASUREMENT.angle[Vb]);
	MEASUREMENT.imag_V1_b = MEASUREMENT.rms_value[Vb] * sin(2.094395103 + MEASUREMENT.angle[Vb]);
	
	MEASUREMENT.real_V1_c = MEASUREMENT.rms_value[Vc] * cos(4.188790205 + MEASUREMENT.angle[Vc]);
	MEASUREMENT.imag_V1_c = MEASUREMENT.rms_value[Vc] * sin(4.188790205 + MEASUREMENT.angle[Vc]);
	
	MEASUREMENT.real_V1_sum = MEASUREMENT.real_V1_a + MEASUREMENT.real_V1_b + MEASUREMENT.real_V1_c;
	MEASUREMENT.imag_V1_sum = MEASUREMENT.imag_V1_a + MEASUREMENT.imag_V1_b + MEASUREMENT.imag_V1_c;
	
	MEASUREMENT.V1_value = sqrt((MEASUREMENT.real_V1_sum * MEASUREMENT.real_V1_sum) + 
	                            (MEASUREMENT.imag_V1_sum * MEASUREMENT.imag_V1_sum));
	MEASUREMENT.V1_value /= 3; //���� ���� ����
	
	MEASUREMENT.real_V2_a = MEASUREMENT.real_V1_a;
	MEASUREMENT.imag_V2_a = MEASUREMENT.imag_V1_a;
	
	MEASUREMENT.real_V2_b = MEASUREMENT.rms_value[Vb] * cos(4.188790205 + MEASUREMENT.angle[Vb]);
	MEASUREMENT.imag_V2_b = MEASUREMENT.rms_value[Vb] * sin(4.188790205 + MEASUREMENT.angle[Vb]);
	
	MEASUREMENT.real_V2_c = MEASUREMENT.rms_value[Vc] * cos(2.094395103 + MEASUREMENT.angle[Vc]);
	MEASUREMENT.imag_V2_c = MEASUREMENT.rms_value[Vc] * sin(2.094395103 + MEASUREMENT.angle[Vc]);
	
	MEASUREMENT.real_V2_sum = MEASUREMENT.real_V2_a + MEASUREMENT.real_V2_b + MEASUREMENT.real_V2_c;
	MEASUREMENT.imag_V2_sum = MEASUREMENT.imag_V2_a + MEASUREMENT.imag_V2_b + MEASUREMENT.imag_V2_c;
	
	MEASUREMENT.V2_value = sqrt((MEASUREMENT.real_V2_sum * MEASUREMENT.real_V2_sum) + 
	                            (MEASUREMENT.imag_V2_sum * MEASUREMENT.imag_V2_sum));
	MEASUREMENT.V2_value /= 3; //���� ���� ����
//-------- ���󼺺�, ���� ��� ��� END

	// ���� ��� 
	// P�� �ϴ� ������ ���Ѵ�.	
	// display���� ��Ʈ3 ����
  // Active P
	TIMER0.temp_float1 = (float)SAMPLE.rms_12_buffer[Va][DFT.index_count];
	TIMER0.temp_float2 = (float)SAMPLE.rms_12_buffer[Ia][DFT.index_count];
	MEASUREMENT.Pa_temp += TIMER0.temp_float1 * TIMER0.temp_float2;
	
	TIMER0.temp_float1 = (float)SAMPLE.rms_12_buffer[Vb][DFT.index_count];
	TIMER0.temp_float2 = (float)SAMPLE.rms_12_buffer[Ib][DFT.index_count];
	MEASUREMENT.Pb_temp += TIMER0.temp_float1 * TIMER0.temp_float2;
	
	TIMER0.temp_float1 = (float)SAMPLE.rms_12_buffer[Vc][DFT.index_count];
	TIMER0.temp_float2 = (float)SAMPLE.rms_12_buffer[Ic][DFT.index_count];
	MEASUREMENT.Pc_temp += TIMER0.temp_float1 * TIMER0.temp_float2;
	
	// Reactive Q
	// ���� 90���� ������ ������
	if(DFT.index_count < 3)
	MEASUREMENT.power_adder = 9;
		
	else
	MEASUREMENT.power_adder = -3;
	
	TIMER0.temp_float1 = (float)SAMPLE.rms_12_buffer[Va][DFT.index_count + MEASUREMENT.power_adder];
	TIMER0.temp_float2 = (float)SAMPLE.rms_12_buffer[Ia][DFT.index_count];
	MEASUREMENT.Qa_temp += TIMER0.temp_float1 * TIMER0.temp_float2;
	
	TIMER0.temp_float1 = (float)SAMPLE.rms_12_buffer[Vb][DFT.index_count + MEASUREMENT.power_adder];
	TIMER0.temp_float2 = (float)SAMPLE.rms_12_buffer[Ib][DFT.index_count];
	MEASUREMENT.Qb_temp += TIMER0.temp_float1 * TIMER0.temp_float2;
	
	TIMER0.temp_float1 = (float)SAMPLE.rms_12_buffer[Vc][DFT.index_count + MEASUREMENT.power_adder];
	TIMER0.temp_float2 = (float)SAMPLE.rms_12_buffer[Ic][DFT.index_count];
	MEASUREMENT.Qc_temp += TIMER0.temp_float1 * TIMER0.temp_float2;

	// 1�� ī����			
	++MEASUREMENT.power_1sec;

	// 1�ʰ� �Ǹ�
	if(MEASUREMENT.power_1sec == 720)
	{
		// display�� ������ �̵�
		MEASUREMENT.Pa_value = MEASUREMENT.Pa_temp;
		MEASUREMENT.Pb_value = MEASUREMENT.Pb_temp;
		MEASUREMENT.Pc_value = MEASUREMENT.Pc_temp;
		
		MEASUREMENT.Qa_value = MEASUREMENT.Qa_temp;
		MEASUREMENT.Qb_value = MEASUREMENT.Qb_temp;
		MEASUREMENT.Qc_value = MEASUREMENT.Qc_temp;
		
		// ���� �ʱ�ȭ
		MEASUREMENT.Pa_temp = 0;
		MEASUREMENT.Pb_temp = 0;
		MEASUREMENT.Pc_temp = 0;
		
		MEASUREMENT.Qa_temp = 0;
		MEASUREMENT.Qb_temp = 0;
		MEASUREMENT.Qc_temp = 0;
		
		// 1�� ī���� �ʱ�ȭ
		MEASUREMENT.power_1sec = 0;
		
		// main���� ���°� ����϶�� flag
		DISPLAY.Power_Up = 0xaaaa;
	}

	//���ļ� ���
	// ���ֱ� real,imag �̹��� real,imag
	// A = (pre_real * real) + (pre_imag * imag)
	// B = (pre_real * imag) - (pre_imag * real)
	// ������ = atan(B/A)
	// Df=(DTh*fo)/2PI -> fo/2PI=60/(2*3.14) = 9.549296586
	// Df=(DTh*fo)/2PI -> fo/2PI=50/(2*3.14) = 7.957747155
	// ������ * Df
	// 60 �Ǵ� 50 ���±�
	if(DFT.index_count < 5)
	TIMER0.temp16_1 = 7 + DFT.index_count;
	
	else
	TIMER0.temp16_1 = DFT.index_count - 5;
	
	TIMER0.temp16_2 = TIMER0.temp16_1;
	
	TIMER0.temp_float1 = (MEASUREMENT.freq_real[TIMER0.temp16_1] * DFT.real_value[Va]) + (MEASUREMENT.freq_imag[TIMER0.temp16_1] * DFT.imag_value[Va]);
	TIMER0.temp_float2 = (MEASUREMENT.freq_real[TIMER0.temp16_1] * DFT.imag_value[Va]) - (MEASUREMENT.freq_imag[TIMER0.temp16_1] * DFT.real_value[Va]);
		
	MEASUREMENT.frequency_temp[MEASUREMENT.frequency_count] = atan(TIMER0.temp_float2 / TIMER0.temp_float1);
		
	MEASUREMENT.freq_real[TIMER0.temp16_1] = DFT.real_value[Va];
	MEASUREMENT.freq_imag[TIMER0.temp16_1] = DFT.imag_value[Va];
	
	
	MEASUREMENT.frequency = 0;
	
	MEASUREMENT.frequency += MEASUREMENT.frequency_temp[0];
	MEASUREMENT.frequency += MEASUREMENT.frequency_temp[1];
	MEASUREMENT.frequency += MEASUREMENT.frequency_temp[2];
	MEASUREMENT.frequency += MEASUREMENT.frequency_temp[3];
	MEASUREMENT.frequency += MEASUREMENT.frequency_temp[4];
	MEASUREMENT.frequency += MEASUREMENT.frequency_temp[5];
		
	MEASUREMENT.frequency /= 6;	
	
	if(MEASUREMENT.rms_value[Va] > 5)
	{
		if(CORE.Hz == Hz50)
		{
			MEASUREMENT.frequency *= -7.957747155;
			MEASUREMENT.frequency += 50;
		}
		
		else
		{
			MEASUREMENT.frequency *= -9.549296586;
			MEASUREMENT.frequency += 60;
		}
	}
	
	else
	MEASUREMENT.frequency = 0;
	
	++MEASUREMENT.frequency_count;
	if(MEASUREMENT.frequency_count == 6)
	MEASUREMENT.frequency_count = 0;
	
	// DSGR�� Io���
	// SGR����� ���� ZCT channel
	if(CORE.gr_select == ZCT_SELECT)
	{
		MEASUREMENT.sgr_rms_Io = MEASUREMENT.rms_value[Is];
		MEASUREMENT.sgr_angle_Io = MEASUREMENT.angle[Is];
	}
	
	// DGR����� ���� NCT channel
	else
	{
		MEASUREMENT.dgr_rms_Io = MEASUREMENT.rms_value[In];
		MEASUREMENT.dgr_angle_Io = MEASUREMENT.angle[In];
	}
	
	// wave ���� �� di/do ���� ������. 1.38ms ���� 1ȸ(12����)
	// digital  normal
	if(WAVE.post_start != 0x1234)
	{
		*(Pre_relay_wave_buffer + WAVE.pre_count_di) = WAVE.relay;
		*(Pre_DI_wave_buffer + WAVE.pre_count_di)    = DIGITAL_INPUT.di_status;
		*(Pre_DO_wave_buffer + WAVE.pre_count_di)    = DIGITAL_OUTPUT.do_status;
	}
			
	// digital fault
	else
	{
		if(WAVE.post_count != 5400)
		{
			*(Post_relay_wave_buffer + WAVE.post_count_di) = WAVE.relay;
			*(Post_DI_wave_buffer + WAVE.post_count_di)    = DIGITAL_INPUT.di_status;
			*(Post_DO_wave_buffer + WAVE.post_count_di)    = DIGITAL_OUTPUT.do_status;
		}
	}
	
	//tcs,ccs
	if(SUPERVISION.mode)
	{
		// 1�ʵ��� ad�� �� ����
		SUPERVISION.tcs_monitoring += (long)SAMPLE.dpram[13];	// 0x400d, 0x403d
		SUPERVISION.ccs_monitoring += (long)SAMPLE.dpram[14]; // 0x400e, 0x403e
		
		++SUPERVISION.monitor_count;
		
		// 1�ʰ� �Ǹ�. 0.001388s * 720 = 1s 
		if(SUPERVISION.monitor_count == 720)
		{
			SUPERVISION.monitor_count = 0; // ī���� �ʱ�ȭ
			SUPERVISION.monitor_update = 0x1234; // real_main()���� �߰���� �ǽ��϶�� ����
			
			SUPERVISION.monitoring[0] = SUPERVISION.tcs_monitoring;
			SUPERVISION.monitoring[1] = SUPERVISION.ccs_monitoring;
			
			SUPERVISION.tcs_monitoring = 0;
			SUPERVISION.ccs_monitoring = 0;
		}
	}
	
	// AD converter ������ ������
	//ref 4.096V, 13235
	SYSTEM.adc_ref_monitor = SAMPLE.dpram[15];
	
	// ������ sample �迭 ī����
	++DFT.index_count;
	
	// 12��°�̸� 0
	if(DFT.index_count == 12)
	DFT.index_count = 0;
	
	// wave ����
	//����
	if(WAVE.post_start != 0x1234)
	{
		WAVE.pre_count += 3;
		
		++WAVE.pre_count_di;
		
		if(WAVE.pre_count == 5400)
		{
			WAVE.pre_count = 0;
			
			WAVE.pre_count_di = 0;
		}
	}
	//�����
	else
	{
		if(WAVE.post_count < 5398)
		{
			WAVE.post_count += 3;
			
			++WAVE.post_count_di;
		}
	}
	
	// real_main()���� protective_relay() �����϶�� �÷���
	SAMPLE.ending = 1;
	
	// ���ͷ�Ʈ pending ���� -> �켱���� ���� ���ͷ�Ʈ ���డ���ϰ� �Ѵٴ� ���̿�
	*PieCtrlRegs_PIEACK = PIEACK_GROUP12;
	///////////////////////////
}

// 1ms timer interrupt
interrupt void TINT1_ISR(void)
{
//-------- DO ���
	// do ��ǰ�� �ø������ͷ� �ִµ�, 15bit ���� �ø���� ������ (��Ʈ��ġ�� �ݴ�� ����)
	// �ش� ��Ʈ�� 1�� �� do�� close��. ��� 0�̸� open
	// �� 86�� ������ close, open ��Ʈ�� ���� ����(do10)
	//                 x L1 H1 L2 H2 L3 H3 L4 H4 L5 H5 L6 H6  x  x  x
	//                00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
	// do1-H1-0x2000   0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  
	// do2-L1-0x4000   0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  
	// do3-H2-0x0800   0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0
	// do4-L2-0x1000   0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0
	// do5-H3-0x0200   0  0  0  0  0  0  1  0  0  0  0  0  0  0  0  0
	// do6-L3-0x0400   0  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0
	// do7-H4-0x0080   0  0  0  0  0  0  0  0  1  0  0  0  0  0  0  0 -> ������ ���¾˶���. ��� close (���� �� �׻� ų��)
	// do8-H5-0x0020   0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0
	// do9-H6-0x0008   0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0
	// do10-L6-0x0010  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
	//!do10-L5-0x0040  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
	// do10�� 86�̱� ������, ������ ������ ���¸� ������
	// ���� ������ �������� 86�� ���� ���¸� �˾ƾ� ��
	// do10 close �Ǹ� gpio53 0
	// do10 open �Ǹ�  gpio53 1	-> booting_setting_check() �غκ� ����
	// ���¿� ���� SYSTEM.do_control ���� �ݿ���
	
	// ����do ���¿� SYSTEM.do_control ���� �ٸ���
	// SYSTEM.do_control - do ���� ����
	// SYSTEM.do_status - do ���� ����

	if(SYSTEM.do_status != SYSTEM.do_control)	// ������¿� ������� �ٸ���
	{
		TLE6208_CS_LOW;	// do ���� ic chip select ����
		for(TIMER1.temp16_1 = 0; TIMER1.temp16_1 < 16; TIMER1.temp16_1++)	// ��� �����ص� bitmap�� �ǰ� 16bit�� ������ ��
		{
			TLE6208_CLK_HIGH;	// status�� rising edge
			if(SYSTEM.do_control & (0x8000 >> TIMER1.temp16_1))	//bit�� 1�̸� high
			{
				TLE6208_DI_HIGH;
			}
			else	// bit 0�̸� low
			{
				TLE6208_DI_LOW;
			}
			TLE6208_CLK_LOW;	// ����� falling edge
		}
		TLE6208_CS_HIGH;	// do ���� ic chip select ��������

		// SYSTEM.do_control ����  SYSTEM.do_status �����ϰ�, SYSTEM.do_status�� MMI DO status�� ǥ������
		SYSTEM.do_status = SYSTEM.do_control;
	}
	// �ֻ��� ���ͷ�Ʈ ����Ϸ��� ����ߴµ� �ʿ� ������ Ȯ�� �ʿ�
	EINT;
//-------- DO ��� END

//-------- display�� �ð� ���
	++TIME.milisecond;
	// 1��
	if(TIME.milisecond >= 1000)//khs
	{
		TIME.milisecond = 0;
		++TIME.second;
		if(TIME.second >= 60)//khs
		{
			TIME.second = 0;
			++TIME.minute;
			if(TIME.minute >= 60)//khs
			{
				TIME.minute = 0;
				++TIME.hour;
				if(TIME.hour >= 24)//khs
				{
					// �� 12�� �Ǹ� RTC ���� �о �ð� ����
					TIME.update = 2;
					TIME.milisecond = 0;
					TIME.hour = 0;
				}
			}
		}
	}
//-------- display�� �ð� ��� END

//-------- DI �Է�
	// di scan
	SYSTEM.di_present = *DI_CS & 0x00ff; // di�� 8ä���̰�, ������ 2����Ʈ �̹Ƿ� ���ʿ��� ���� ����Ʈ �����

	// di 8 channel
	for(TIMER1.temp16_1 = 0; TIMER1.temp16_1 < 8; TIMER1.temp16_1++)
	{
		// ��Ʈ����ŷ��
		TIMER1.temp16_2 = 0x0001;
		
		// LSB���� MSB �������� 1��Ʈ�� �̵�
		TIMER1.temp16_2 <<= TIMER1.temp16_1;
			
		// ���� ���̶� �ٸ���
		// SYSTEM.di_present - ��� di ���¸� scan�� ��
		// SYSTEM.di_past - �������� ����� di ���°�
		// ���� �ش��Ʈ�� ����ŷ�Ͽ� ���Ͽ� �ٸ���
		if((SYSTEM.di_past & TIMER1.temp16_2) != (SYSTEM.di_present & TIMER1.temp16_2))
		{
			// �� ä�κ� debounce timer
			// SYSTEM.di_debounce_timer[8] - �ʱⰪ�� 0xffff �ε� �� �ǹ̴� �ᱹ di ���º�ȭ�� �����ٴ� �ǹ�
			// ���� ��ȭ �νĵȰŸ� ��ٿ Ÿ�̸� �ʱ�ȭ
			if(SYSTEM.di_debounce_timer[TIMER1.temp16_1] == 0xffff)	{SYSTEM.di_debounce_timer[TIMER1.temp16_1] = 0;}
			else	// �ι�° tern ����
			{
				++SYSTEM.di_debounce_timer[TIMER1.temp16_1];	// ��ٿ Ÿ�̸� ����
				if(SYSTEM.di_debounce_timer[TIMER1.temp16_1] >= DIGITAL_INPUT.debounce[TIMER1.temp16_1])	// debounce �ð��� ������ �ð� �̻��̸�.
				{
					// DI ���¸� ���� ������ �����ϴµ�, on/off �� ������ ������ ���ο�� exclusive or�� �ϸ� �ش� ��Ʈ�� �����Ͽ� ����
					DIGITAL_INPUT.di_status ^= TIMER1.temp16_2; // DIGITAL_INPUT.di_status - DI display ���� ����

					// DI ���º�ȭ �̺�Ʈ ����
					if(DIGITAL_INPUT.di_status & TIMER1.temp16_2)	{EVENT.di_on |= TIMER1.temp16_2;}	// close
					else																	{EVENT.di_off |= TIMER1.temp16_2;}	// open
					
					SYSTEM.di_past ^= TIMER1.temp16_2;	// SYSTEM.di_past - ���º�ȭ ������ ���� ����
					SYSTEM.di_debounce_timer[TIMER1.temp16_1] = 0xffff;	// ��ٿ Ÿ�̸� �ʱ�ȭ 
				}
			}
		}
		else	// ������ ������ ä���� ������ ��� ���� ������ �� �����Ƿ�...
		{
			SYSTEM.di_debounce_timer[TIMER1.temp16_1] = 0xffff; // ��ٿ Ÿ�̸� �ʱ�ȭ
		}
	}
//-------- DI �Է� END

//-------- Ÿ�̸� ����
	// ���ܱ� trip�� �����Ǿ� �����Ƿ� 500ms ������ Ǯ���ش�
	++TIMER.cb_open;
	++TIMER.cb_close;

	++TIMER.key; // MMI key scan Ÿ�̸�
	++TIMER.lcd; // MMI lcd write timer
	++TIMER.led; // MMI led write timer -> H/W ������ �ֱ��� ��� �������
	++TIMER.backlight; // LCD �����Ʈ 3���� �ҵ�� timer

	++MANAGER.rx_timeout; // ���� �ø�������� �����Ͱ� ������ٰ� ������� ���̺��� �������� �� ���� ����, ���ͷ�Ʈ ���¸� ���½�Ű�� ���� ��Ż��¸� �����ϴ� timer
	++WAVE.timer; // fault wave ���� �� ����ϴ� timer
	++TIMER.measurement; // ���������� �İ��� �ð� ���� timer
	++HARMONICS.timer; // ���������� �� ������ �İ��� �ð� ���� timer
	++HIMIX.timer; // himix �۽� �ð����� ���� timer

	// ������� pickup, op, dropout ���� Ÿ�̸�
	// OCR50-1
	++OCR50_1.Pickup_Count;
	++OCR50_1.Dropout_Count;

	// OCR50-2
	++OCR50_2.Pickup_Count;	
	++OCR50_2.Dropout_Count;

	// OCR51-1
	++OCR51_1.Pickup_Count;
	++OCR51_1.Dropout_Count;
	
	// OCR51-2
	++OCR51_2.Pickup_Count;
	++OCR51_2.Dropout_Count;

	// OCGR50
	++OCGR50.Pickup_Count;
	++OCGR50.Dropout_Count;

	// OCGR51
	++OCGR51.Pickup_Count;	
	++OCGR51.Dropout_Count;

	// UVR-1
	++UVR_1.Pickup_Count;
	++UVR_1.Dropout_Count;

	// UVR-2
	++UVR_2.Pickup_Count;	
	++UVR_2.Dropout_Count;
	
	// UVR-3
	++UVR_3.Pickup_Count;
	++UVR_3.Dropout_Count;
	
	// 47P
	++P47.Pickup_Count;	
	++P47.Dropout_Count;
	
	// 47N
	++N47.Pickup_Count;
	++N47.Dropout_Count;
	
	// OVR
	++OVR.Pickup_Count;
	++OVR.Dropout_Count;
	
	// OVGR
	++OVGR.Pickup_Count;	
	++OVGR.Dropout_Count;

	// DGR
	++DGR.Pickup_Count;
	++DGR.Dropout_Count;

	// SGR
	++SGR.Pickup_Count;
	++SGR.Dropout_Count;

	// R-Hour�� Ÿ�̸�
	// #define DISPLAY_CUT_I_1A        0.021 // (0.04 / 2) * 1.05
  // #define DISPLAY_CUT_I_5A        0.105 // (0.2 / 2) * 1.05
  // ���ܱ� close �Ǵ��� �ٰ�
	// cb close
//	if((DISPLAY.True_RMS[Ia] >= MIN_MAX.current_display) || (DISPLAY.True_RMS[Ib] >= MIN_MAX.current_display) || (DISPLAY.True_RMS[Ic] >= MIN_MAX.current_display))
//	{
//		++TIMER.current_on;
//	}
//	else
//	TIMER.current_on = 0;

//-------- Ÿ�̸� ���� END
}

// ���� �ø�����Ʈ�� ���� �� ����Ʈ�� ���ŵǸ� �̸��� ���ͷ�Ʈ �߻�
interrupt void SCIRXB_ISR(void)
{
	// �ϴ� dsp ���� ����(*ScibRegs_SCIRXBUF)���� �о�ͼ� ���� �迭�� ����
	MANAGER.rx_buffer[MANAGER.rx_count] = *ScibRegs_SCIRXBUF & 0xff;
	
	// �ٸ� ���ͷ�Ʈ ������� interrupt pending ����
	*PieCtrlRegs_PIEACK = PIEACK_GROUP9;
	
	// ���� ����
	// MANAGER.rx_count - ���ŵ� ����Ʈ ���� ī����
	// ��� �� ��, �ƹ��͵� ������ �ƴϵǾ����� 0��(�ʱⰪ)
	if(MANAGER.rx_count == 0)
	{
		// �ø������ Ÿ�Ӿƿ� ī���� �ʱ�ȭ
		MANAGER.rx_timeout = 0;
		
		// ���� �𸣰����� �ϴ� ����Ÿ�� �������� �˸��� �÷���
		MANAGER.rx_status = 1;
	}
	
	// ���, id, function code 2��, ����Ÿ ũ�� �ΰ� �� 6���� ������ ������ �Ǿ� ����
	// 1byte | 2byte | 3byte | 4byte | 5byte | 6byte ....
	// ���  | id    | F/C1  | F/C2  | D/L1  | D/L2  .....
	// ���� ������ ����
	else if(MANAGER.rx_count == 5)
	{
		// data length�� ���� ����Ʈ
		MANAGER.rx_length = MANAGER.rx_buffer[4];
		
		MANAGER.rx_length <<= 8;
		
		// data length�� ���� ����Ʈ
		MANAGER.rx_length |= MANAGER.rx_buffer[5];
		
		// ��ü������ ������ ����Ʈ ������ ��� 6��, �� �ڿ� crc 2��, data length�� ������ ������ŭ �Է��� ������ �� �ִ�.
		MANAGER.rx_length += 8;	
	}
	
	// ���� �Ϸ�?
	// MANAGER.rx_count �� ������ ����Ÿ ����(MANAGER.rx_length)�� ������ �ϳ��� �������� ��� ���Դٰ� ����
	else if((MANAGER.rx_count + 1) == MANAGER.rx_length)
	MANAGER.rx_status = 2; // ���� �ܰ踦 ���� �÷��� ����
		
	++MANAGER.rx_count;	
}

// ���� �ø��� �۽� ���ͷ�Ʈ ��ƾ
interrupt void SCITXINTB_ISR(void)				// PIE9.4 @ 0x000DC6  SCITXINTB (SCI-B)
{
	// *ScibRegs_SCITXBUF - dsp �ø��� �۽� ����
	// MANAGER.isr_tx ������ �ּҺ��� MANAGER.tx_length ��ŭ �۽��ϸ� ��
	*ScibRegs_SCITXBUF = *(MANAGER.isr_tx + MANAGER.tx_count++);
	
	// �ٺ������� �۽� ���ͷ�Ʈ ��� ����
	if(MANAGER.tx_count == MANAGER.tx_length)
	*ScibRegs_SCICTL2 &= 0xfffe; // �������
	
	// �۽� ���ͷ�Ʈ ��� ����
	*PieCtrlRegs_PIEACK = PIEACK_GROUP9;
}

//himix tx ��
interrupt void SCITXINTC_ISR(void)				// PIE9.4 @ 0x000DC6  SCITXINTc (SCI-c)
{	
	// *ScibRegs_SCITXBUF = MANAGER.tx_buffer[MANAGER.tx_count++];
	*ScicRegs_SCITXBUF = HIMIX.tx_buffer[HIMIX.tx_count++];
	
	// �ٺ�������
	if(HIMIX.tx_count == 61)
	{
		*ScicRegs_SCICTL2 &= 0xfffe;
		
		//485 driver ����
		HIMIX_STOP;
	}
	*PieCtrlRegs_PIEACK = PIEACK_GROUP8;
}
