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
// 최대 about 250us
// 1.3888ms 대비 18.01%

//timer1 -> TINT1_ISR
// 최대 about 7us
// 1ms 대비 0.7%

//main 한바퀴
// 최대 7us

// 기본파
//  0,  1,  2,  3,   4,   5,  6,  7,  8,  9
// Ia, Ib, Ic, In, In2, ZCT, Va, Vb, Vc, Vn

// 0 ~ 9 : I, V
// 13 - tcs
// 14 - ccs
// 15 - ref.
//tcs/ccs
//육상 dc110
//선박 ac220, dc320

interrupt void XINT3_ISR(void)
{
	//메모리 addressing
	// 외부 계측 cpld에서 1.38ms 마다 한번씩 인터럽트를 발생시킴
	// adc 제어 cpld 각 채널별(10ch) 세개 샘플씩 dpram에 쟁겨놓은 후 인터럽트를 발생시킴
	// 근데 메모리 영역이 두 군데이고 번갈아가면서 ad 계측값을 저장해 줌
	// 그래서 인터럽트 발생 시 마다 gpio 신호를 보고 어느 메모리영역인지 파악하여 해당 메모리를 읽어옴
	// #define AD_buffer_low          (int *)0x4000 
  // #define AD_buffer_high         (int *)0x4030
  // 상기 시작주소에서 아래와 같이 배열됨
  //offset -   0,  1,  2,  3,   4,    5,   6,  7,  8,  9
  //          Ia, Ib, Ic, In, In2,  ZCT,  Va, Vb, Vc, Vn
  
  // adc와 dsp사이에 있는 dpram 시작 주소 지정
	if(*GpioIntRegs_GPADAT_Low & 0x0040)	{SAMPLE.dpram = AD_buffer_low;}
	else																	{SAMPLE.dpram = AD_buffer_high;}
	
	// 곱셈연산 줄이기 위해
	// 한번에 세개씩 읽다보니 직전 샘플 등을 계산할 때 편하기위해, 곱셈연산을 줄이기 위해 미리 곱해놈
	DFT.index_count_3times = DFT.index_count * 3;

	//10 채널기준
  // 10채널 중 실제로 사용하는 채널은 8채널 -> 10채널은 H/W장이가 미리 만들어 놓았음
  // F, M 모델 공통으로
  // NCT(OCGR 또는 DGR) 사용 시에는 채널 0, 1, 2, 3, 6, 7, 8, 9 만 사용
  // ZCT(SGR)           사용 시에는 채널 0, 1, 2, 5, 6, 7, 8, 9 만 사용
	for(TIMER0.temp16_1 = 0; TIMER0.temp16_1 < 10; TIMER0.temp16_1++)	// 10채널 반복
	{
		// dpram에 담겨있는 것들 세개를 일단 read
		// rawdata는 signed integer 인데, 아래부분에서 float 계산을 해야하므로 미리 float로 casting하여 저장
		SAMPLE.first  = (float)(SAMPLE.dpram[TIMER0.temp16_1     ] - CALIBRATION.offset[TIMER0.temp16_1]);
		SAMPLE.second = (float)(SAMPLE.dpram[TIMER0.temp16_1 + 16] - CALIBRATION.offset[TIMER0.temp16_1]);
		SAMPLE.third  = (float)(SAMPLE.dpram[TIMER0.temp16_1 + 32] - CALIBRATION.offset[TIMER0.temp16_1]);
		
		//오샘플 제거 알고리즘 적용
		// Himap adc 구성 상 세개 샘플이 동시에 들어오게 되어 있음
		// 노이즈 등 기타 요인이 아닌 정상적인 상황이라면 세개 샘플 중 두번째 샘플은 첫번째-세번째 샘플 평균값에 수렴할 것임
		// 마찬가지로 직전tern의 세번째와 이번 tern 첫번째 두번째 샘플의 관계도 상기 가정에 부합하여야 함
		// 단, peak 치 부근과, 제로크로싱 부근에서는 상기 가정이 맞지 않을 가능성이 크기 때문에 예외 조건이 포함되어 있음
		if(SAMPLE.normal) // calibration 시 또는 debug 시 오샘플제거 알고리즘을 적용하면 이상해질 우려가 있으므로 통상 운용시에만 적용하기위해 플래그 운용
		{
			// 1st, 3rd 평균값 구하기 목적
			SAMPLE.average = SAMPLE.first + SAMPLE.third;
			
			// 분모가 0일경우 cpu exception 회피 목적
			// 분모가 0인상태에서 나눗셈 연산을 할경우 cpu 응답은 두가지 계열로 나뉨
			// 1- cpu 뻣음 -> 하드웨어 FPU 없이 runtime library를 사용하는 cpu에서 나타남
			// 2 - 엉뚱한 결과가 나오는 경우 -> TI dsp 뻣지는 않는데 엄한 결과값이 나올 수도 있음
			// 따라서 분모가 0일 경우 아예 나눗셈을 하지 않게 회피
			// 예를 들어 첫번째 샘플(-10), 두번째 샘플 제로크로싱(0), 세번째 샘플(10)
			// SAMPLE.average가 아래부분에서 분모로 사용함
			if(SAMPLE.average == 0)
			{
				// SAMPLE.average_ratio 를 가지고 실제 입력된 샘플과 계산된 샘플을 크기를 비교하는 것이 목적임
				// SAMPLE.average_ratio가 음수 음수로 표시되는 것을 방지하기 위해 
				// 실제 두번째 샘플(SAMPLE.second)이 음수인 경우 SAMPLE.average를 마이너스로 만든다
				if(SAMPLE.second < 0)	{SAMPLE.average = -2;}
				else									{SAMPLE.average = 2;}
			}
			
			// 1st, 3rd 평균값
			SAMPLE.average /= 2;
			
			// 2nd와 1st, 3rd 평균값 비율을 비교하여 SAMPLE.average_ratio 저장
			SAMPLE.average_ratio = SAMPLE.second / SAMPLE.average;
		
			// 10% 이내 0.9 이상 1.1 이하
			if((SAMPLE.average_ratio >= 0.9) && (SAMPLE.average_ratio <= 1.1))
			{
				//전체 36샘플
				// 0, 1, 2 | 3, 4, 5 | 6, 7, 8 | 9, 10, 11 | 12, 13, 14 | 15, 16, 17 | 18, 19, 20 | 21, 22, 23 | 24, 25, 26 | 27, 28, 29 | 30, 31, 32 | 33, 34, 35
				
				// 상기 SAMPLE.average_ratio = SAMPLE.second / SAMPLE.average; 에서는 현재 tern 에서 비교한 것임
				// 그러나 현재 tern만 비교해서는 진짜 정상적인지 판단하기 힘들어서 직전 tern의 3rd sample과 다시 비교를 하도록 한다
				
				// 이전에 이상 상황이 없었다면...
				if(SAMPLE.abnormal[TIMER0.temp16_1] == 0) // 이상상황 발생 플래그
				{
					// DFT.index_count는 계전용 12샘플 카운터 이다.
					// 0번째라면 상기 나열된 수열에서 직전 tern의 3rd sample은 35번째 sample 이다.
					if(DFT.index_count == 0)	{SAMPLE.pre_value = (float)SAMPLE.pre_36_buffer[TIMER0.temp16_1][35];}
					
					// 그외에는 DFT.index_count_3times(위에서 DFT.index_count의 3배 곱해논 변수)의 빼기 1하면 직전 tern 3rd sample 이다.
					else											{SAMPLE.pre_value = (float)SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times - 1];}
				}
				
				// 만약 이상 있었음
				// 이상 샘플이라 판단되면 SAMPLE.abnormal_value[]에 해당 값을 저장해 놨음. 
				// 이상 값 저장된 것 load
				else	{SAMPLE.pre_value = SAMPLE.abnormal_value[TIMER0.temp16_1];}
				
				// 처음 비교 루틴과 같이 직전샘플로 비교
				// 평균
				SAMPLE.average = SAMPLE.pre_value + SAMPLE.second;
				
				// cpu exception 회피 ->  위의 설명 참조
				if(SAMPLE.average == 0)
				{
					if(SAMPLE.first < 0)	{SAMPLE.average = -2;}
					else									{SAMPLE.average = 2;}
				}
				
				SAMPLE.average /= 2;
				
				SAMPLE.average_ratio = SAMPLE.first / SAMPLE.average; // 위에 설명 참조
				
				// 진짜라고 판정 -> 10% 이내
				if((SAMPLE.average_ratio >= 0.9) && (SAMPLE.average_ratio <= 1.1))
				{
					// 전주기 36샘플 버퍼 저장
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times    ] = (int)SAMPLE.first;
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1] = (int)SAMPLE.second;
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 2] = (int)SAMPLE.third;
					
					// rms 계산용 버퍼 저장
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
					
					// 정상적인 샘플이 들어왔다고 판단했기 때문에 플래그를 초기화
					SAMPLE.abnormal[TIMER0.temp16_1] = 0;
				}
				
				// 한번 더 확인
				else
				{
					SAMPLE.average_ratio = SAMPLE.average - SAMPLE.first;
				
					//zero cross라고 판단
					if((SAMPLE.average_ratio >= -50) && (SAMPLE.average_ratio <= 50))
					{
						// 전주기 36샘플 버퍼 저장
						SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times    ] = (int)SAMPLE.first;
						SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1] = (int)SAMPLE.second;
						SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 2] = (int)SAMPLE.third;
					
						// rms 계산용 버퍼 저장
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
					
					// 여기까지도 정상 판정을 못받았다면 진짜로 이상한 샘플이다.
					else
					{
						// 이번 샘플은 인정 못하니 한주기 전 샘플로 대체한다.
						SAMPLE.rms_12_buffer[TIMER0.temp16_1][DFT.index_count] = SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1];
						
						// 여기의 wave 저장은 한주기 전 sample 값으로 모두 대치
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
					// 전주기 36샘플 버퍼 저장
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times    ] = (int)SAMPLE.first;
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1] = (int)SAMPLE.second;
					SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 2] = (int)SAMPLE.third;
					
					// rms 계산용 버퍼 저장
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
		
		// 초기 또는 calibration 시 -> 오샘플제거 알고리즘 미적용
		else
		{
			// 세개 샘플을 SAMPLE.pre_36_buffer[]에 호로록 저장
			SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times    ] = (int)SAMPLE.first;
			SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 1] = (int)SAMPLE.second;
			SAMPLE.pre_36_buffer[TIMER0.temp16_1][DFT.index_count_3times + 2] = (int)SAMPLE.third;
			
			// 계전용 12샘플(RMS 구하기위해) 저장
			SAMPLE.rms_12_buffer[TIMER0.temp16_1][DFT.index_count] = (int)SAMPLE.second;
			
			// wave normal
			// 사고가 없는 정상적 상황에서 저장
			if(WAVE.post_start != 0x1234)
			{
				WAVE.buffer = (int *)wave_buffer_address[TIMER0.temp16_1];
			
				*(WAVE.buffer + WAVE.pre_count    ) = (int)SAMPLE.first;
				*(WAVE.buffer + WAVE.pre_count + 1) = (int)SAMPLE.second;
				*(WAVE.buffer + WAVE.pre_count + 2) = (int)SAMPLE.third;
			}
			
			// wave fault
			// 계전요소 동작 후 저장
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
		//오샘플 제거 완료

//-------- 필요없는 채널 DFT 계산 제외
		// In 관련 선택
		// 부담저항 1.5k 짜리 0.02In 영역에서 rms오차 차이가 계전에 영향 없음
		// 따라서 In2 취급하지 않음
  	// offset -  0,  1,  2,  3,   4,  5,  6,  7,  8,  9
  	//          Ia, Ib, Ic, In, In2, Is, Va, Vb, Vc, Vn
		
		if(TIMER0.temp16_1 == 3)	//In 채널
		{
			if(CORE.gr_select == ZCT_SELECT)	{goto daum;}	// ZCT 사용 시 In 채널 DFT 계산 않함, 패스
		}
		else if(TIMER0.temp16_1 == 4)				{goto daum;}	// In2 채널 사용 않함, 패스
		else if(TIMER0.temp16_1 == 5)	// Is 채널
		{
			if(CORE.gr_select != ZCT_SELECT)	{goto daum;}	// NCT 사용 시 Is 채널 DFT 계산 않함, 패스
		}
//-------- 필요없는 채널 DFT 계산 제외 END
		
//-------- DFT 계산 시작
		DFT.real_value[TIMER0.temp16_1] = 0;
		DFT.imag_value[TIMER0.temp16_1] = 0;

		//-------- true rms 계산 제거
		//if(DFT.index_count == 11)	{MEASUREMENT.true_rms[TIMER0.temp16_1] = 0;} // true rms 계산은 1주기마다 함
		//-------- true rms 계산 제거 END

		// full window dft 계산법(o)-윈도우가 이동할 때 전체 샘플 값을 가지고 계산
		// recursive dft 계산법(x) - 윈도우가 이동할 때 빠지는 샘플과 새로 유입된 샘플만 고려하여 계산
		for(TIMER0.temp16_2 = 0; TIMER0.temp16_2 < 12; TIMER0.temp16_2++)
		{
			// 2/12 * cos(2pi*(n)) * x[n]			
			DFT.real_value[TIMER0.temp16_1] += (float)(SAMPLE.rms_12_buffer[TIMER0.temp16_1][TIMER0.temp16_2] * cos_table[TIMER0.temp16_2]);
			DFT.imag_value[TIMER0.temp16_1] += (float)(SAMPLE.rms_12_buffer[TIMER0.temp16_1][TIMER0.temp16_2] * sin_table[TIMER0.temp16_2]);

			//-------- true rms 계산 제거
			//if(DFT.index_count == 11) // true rms용, 1주기 값이 채워지면 누적 및 제곱
			//{
			//	TIMER0.temp_float1 = (float)SAMPLE.rms_12_buffer[TIMER0.temp16_1][TIMER0.temp16_2];
			//	MEASUREMENT.true_rms[TIMER0.temp16_1] += TIMER0.temp_float1 * TIMER0.temp_float1;
			//}
			//-------- true rms 계산 제거 END
		}
		// rms 값 계산 
		TIMER0.temp_float2 = sqrt((DFT.real_value[TIMER0.temp16_1] * DFT.real_value[TIMER0.temp16_1]) + (DFT.imag_value[TIMER0.temp16_1] * DFT.imag_value[TIMER0.temp16_1]));
//-------- DFT 계산 END
		
		MEASUREMENT.rms_value[TIMER0.temp16_1] = TIMER0.temp_float2 * CALIBRATION.slope_1st[TIMER0.temp16_1];	// calibration factor 적용, 계전에 사용하는 최종 rms 값
		
		//위상값 계산
		MEASUREMENT.angle[TIMER0.temp16_1] = atan2(DFT.real_value[TIMER0.temp16_1], DFT.imag_value[TIMER0.temp16_1]);	// radian값임 display 할때 degree로 변환
		MEASUREMENT.angle[TIMER0.temp16_1] += CALIBRATION.angle[TIMER0.temp16_1]; // calibration factor 적용, 계전에 사용하는 최종 angle 값
		
		//-------- true rms 계산 제거
		//// 1주기값 채워지면 평균및 제곱근, 후가공 후 display에서 표시함.
		//if(DFT.index_count == 11)
		//{
		//	MEASUREMENT.true_rms[TIMER0.temp16_1] /= 12;
		//	MEASUREMENT.true_rms[TIMER0.temp16_1] = sqrt(MEASUREMENT.true_rms[TIMER0.temp16_1]);
		//}
		//-------- true rms 계산 제거 END

daum:		;
	}

//-------- 정상성분, 역상 상분 계산
	//정상, 역상 계산은 대칭좌표법으로 일일이 계산함
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
	MEASUREMENT.I1_value /= 3; //정상 전류 최종
	
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
	MEASUREMENT.I2_value /= 3; //역상 전류 최종
	
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
	MEASUREMENT.V1_value /= 3; //정상 전압 최종
	
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
	MEASUREMENT.V2_value /= 3; //역상 전압 최종
//-------- 정상성분, 역상 상분 계산 END

	// 전력 계산 
	// P는 일단 무조건 곱한다.	
	// display에서 루트3 까줌
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
	// 전압 90도전 값부터 곱해줌
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

	// 1초 카운터			
	++MEASUREMENT.power_1sec;

	// 1초가 되면
	if(MEASUREMENT.power_1sec == 720)
	{
		// display용 변수에 이동
		MEASUREMENT.Pa_value = MEASUREMENT.Pa_temp;
		MEASUREMENT.Pb_value = MEASUREMENT.Pb_temp;
		MEASUREMENT.Pc_value = MEASUREMENT.Pc_temp;
		
		MEASUREMENT.Qa_value = MEASUREMENT.Qa_temp;
		MEASUREMENT.Qb_value = MEASUREMENT.Qb_temp;
		MEASUREMENT.Qc_value = MEASUREMENT.Qc_temp;
		
		// 변수 초기화
		MEASUREMENT.Pa_temp = 0;
		MEASUREMENT.Pb_temp = 0;
		MEASUREMENT.Pc_temp = 0;
		
		MEASUREMENT.Qa_temp = 0;
		MEASUREMENT.Qb_temp = 0;
		MEASUREMENT.Qc_temp = 0;
		
		// 1초 카운터 초기화
		MEASUREMENT.power_1sec = 0;
		
		// main에서 전력값 계산하라는 flag
		DISPLAY.Power_Up = 0xaaaa;
	}

	//주파수 계산
	// 전주기 real,imag 이번판 real,imag
	// A = (pre_real * real) + (pre_imag * imag)
	// B = (pre_real * imag) - (pre_imag * real)
	// 위상차 = atan(B/A)
	// Df=(DTh*fo)/2PI -> fo/2PI=60/(2*3.14) = 9.549296586
	// Df=(DTh*fo)/2PI -> fo/2PI=50/(2*3.14) = 7.957747155
	// 위상차 * Df
	// 60 또는 50 보태기
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
	
	// DSGR용 Io담기
	// SGR사용할 때는 ZCT channel
	if(CORE.gr_select == ZCT_SELECT)
	{
		MEASUREMENT.sgr_rms_Io = MEASUREMENT.rms_value[Is];
		MEASUREMENT.sgr_angle_Io = MEASUREMENT.angle[Is];
	}
	
	// DGR사용할 때는 NCT channel
	else
	{
		MEASUREMENT.dgr_rms_Io = MEASUREMENT.rms_value[In];
		MEASUREMENT.dgr_angle_Io = MEASUREMENT.angle[In];
	}
	
	// wave 저장 시 di/do 값도 저장함. 1.38ms 마다 1회(12샘플)
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
		// 1초동안 ad된 값 누적
		SUPERVISION.tcs_monitoring += (long)SAMPLE.dpram[13];	// 0x400d, 0x403d
		SUPERVISION.ccs_monitoring += (long)SAMPLE.dpram[14]; // 0x400e, 0x403e
		
		++SUPERVISION.monitor_count;
		
		// 1초가 되면. 0.001388s * 720 = 1s 
		if(SUPERVISION.monitor_count == 720)
		{
			SUPERVISION.monitor_count = 0; // 카운터 초기화
			SUPERVISION.monitor_update = 0x1234; // real_main()에서 추가계산 실시하라는 명령
			
			SUPERVISION.monitoring[0] = SUPERVISION.tcs_monitoring;
			SUPERVISION.monitoring[1] = SUPERVISION.ccs_monitoring;
			
			SUPERVISION.tcs_monitoring = 0;
			SUPERVISION.ccs_monitoring = 0;
		}
	}
	
	// AD converter 건전성 판정용
	//ref 4.096V, 13235
	SYSTEM.adc_ref_monitor = SAMPLE.dpram[15];
	
	// 계전용 sample 배열 카운터
	++DFT.index_count;
	
	// 12번째이면 0
	if(DFT.index_count == 12)
	DFT.index_count = 0;
	
	// wave 저장
	//평상시
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
	//사고후
	else
	{
		if(WAVE.post_count < 5398)
		{
			WAVE.post_count += 3;
			
			++WAVE.post_count_di;
		}
	}
	
	// real_main()에서 protective_relay() 실행하라는 플래그
	SAMPLE.ending = 1;
	
	// 인터럽트 pending 해제 -> 우선순위 낮은 인터럽트 실행가능하게 한다는 뜻이여
	*PieCtrlRegs_PIEACK = PIEACK_GROUP12;
	///////////////////////////
}

// 1ms timer interrupt
interrupt void TINT1_ISR(void)
{
//-------- DO 출력
	// do 부품에 시리얼데이터로 주는데, 15bit 부터 시리얼로 날려줌 (비트위치가 반대로 들어가네)
	// 해당 비트가 1일 때 do가 close됨. 당근 0이면 open
	// 단 86용 접점은 close, open 비트가 따로 있음(do10)
	//                 x L1 H1 L2 H2 L3 H3 L4 H4 L5 H5 L6 H6  x  x  x
	//                00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
	// do1-H1-0x2000   0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  
	// do2-L1-0x4000   0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  
	// do3-H2-0x0800   0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0
	// do4-L2-0x1000   0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0
	// do5-H3-0x0200   0  0  0  0  0  0  1  0  0  0  0  0  0  0  0  0
	// do6-L3-0x0400   0  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0
	// do7-H4-0x0080   0  0  0  0  0  0  0  0  1  0  0  0  0  0  0  0 -> 계전기 상태알람용. 상시 close (부팅 시 항상 킬것)
	// do8-H5-0x0020   0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0
	// do9-H6-0x0008   0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0
	// do10-L6-0x0010  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
	//!do10-L5-0x0040  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
	// do10은 86이기 때문에, 전원이 꺼져도 상태를 유지함
	// 따라서 전원이 켜졌을때 86의 현재 상태를 알아야 함
	// do10 close 되면 gpio53 0
	// do10 open 되면  gpio53 1	-> booting_setting_check() 밑부분 참조
	// 상태에 따라 SYSTEM.do_control 값에 반영함
	
	// 현재do 상태와 SYSTEM.do_control 값이 다르면
	// SYSTEM.do_control - do 제어 변수
	// SYSTEM.do_status - do 현재 상태

	if(SYSTEM.do_status != SYSTEM.do_control)	// 현재상태와 제어변수가 다르면
	{
		TLE6208_CS_LOW;	// do 제어 ic chip select 명령
		for(TIMER1.temp16_1 = 0; TIMER1.temp16_1 < 16; TIMER1.temp16_1++)	// 상기 명시해둔 bitmap에 의거 16bit를 보내야 함
		{
			TLE6208_CLK_HIGH;	// status는 rising edge
			if(SYSTEM.do_control & (0x8000 >> TIMER1.temp16_1))	//bit가 1이면 high
			{
				TLE6208_DI_HIGH;
			}
			else	// bit 0이면 low
			{
				TLE6208_DI_LOW;
			}
			TLE6208_CLK_LOW;	// 출력은 falling edge
		}
		TLE6208_CS_HIGH;	// do 제어 ic chip select 명령해제

		// SYSTEM.do_control 값을  SYSTEM.do_status 저장하고, SYSTEM.do_status를 MMI DO status로 표시해줌
		SYSTEM.do_status = SYSTEM.do_control;
	}
	// 최상위 인터럽트 허용하려고 사용했는데 필요 없는지 확인 필요
	EINT;
//-------- DO 출력 END

//-------- display용 시간 계산
	++TIME.milisecond;
	// 1초
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
					// 밤 12가 되면 RTC 값을 읽어서 시간 보정
					TIME.update = 2;
					TIME.milisecond = 0;
					TIME.hour = 0;
				}
			}
		}
	}
//-------- display용 시간 계산 END

//-------- DI 입력
	// di scan
	SYSTEM.di_present = *DI_CS & 0x00ff; // di는 8채널이고, 변수는 2바이트 이므로 불필요한 상위 바이트 지우기

	// di 8 channel
	for(TIMER1.temp16_1 = 0; TIMER1.temp16_1 < 8; TIMER1.temp16_1++)
	{
		// 비트마스킹용
		TIMER1.temp16_2 = 0x0001;
		
		// LSB에서 MSB 방향으로 1비트씩 이동
		TIMER1.temp16_2 <<= TIMER1.temp16_1;
			
		// 직전 값이랑 다르면
		// SYSTEM.di_present - 방금 di 상태를 scan한 값
		// SYSTEM.di_past - 직전까지 저장된 di 상태값
		// 각각 해당비트만 마스킹하여 비교하여 다르면
		if((SYSTEM.di_past & TIMER1.temp16_2) != (SYSTEM.di_present & TIMER1.temp16_2))
		{
			// 각 채널별 debounce timer
			// SYSTEM.di_debounce_timer[8] - 초기값이 0xffff 인데 이 의미는 결국 di 상태변화가 없었다는 의미
			// 최초 변화 인식된거면 디바운스 타이머 초기화
			if(SYSTEM.di_debounce_timer[TIMER1.temp16_1] == 0xffff)	{SYSTEM.di_debounce_timer[TIMER1.temp16_1] = 0;}
			else	// 두번째 tern 부터
			{
				++SYSTEM.di_debounce_timer[TIMER1.temp16_1];	// 디바운스 타이머 증가
				if(SYSTEM.di_debounce_timer[TIMER1.temp16_1] >= DIGITAL_INPUT.debounce[TIMER1.temp16_1])	// debounce 시간이 설정된 시간 이상이면.
				{
					// DI 상태를 최종 변수에 저장하는데, on/off 를 일일이 따지면 괴로우니 exclusive or를 하면 해당 비트를 조작하여 저장
					DIGITAL_INPUT.di_status ^= TIMER1.temp16_2; // DIGITAL_INPUT.di_status - DI display 변수 최종

					// DI 상태변화 이벤트 저장
					if(DIGITAL_INPUT.di_status & TIMER1.temp16_2)	{EVENT.di_on |= TIMER1.temp16_2;}	// close
					else																	{EVENT.di_off |= TIMER1.temp16_2;}	// open
					
					SYSTEM.di_past ^= TIMER1.temp16_2;	// SYSTEM.di_past - 상태변화 감지용 변수 저장
					SYSTEM.di_debounce_timer[TIMER1.temp16_1] = 0xffff;	// 디바운스 타이머 초기화 
				}
			}
		}
		else	// 노이즈 때문에 채널이 흔들렸을 경우 값이 변동할 수 있으므로...
		{
			SYSTEM.di_debounce_timer[TIMER1.temp16_1] = 0xffff; // 디바운스 타이머 초기화
		}
	}
//-------- DI 입력 END

//-------- 타이머 증가
	// 차단기 trip은 지정되어 있으므로 500ms 넘으면 풀어준다
	++TIMER.cb_open;
	++TIMER.cb_close;

	++TIMER.key; // MMI key scan 타이머
	++TIMER.lcd; // MMI lcd write timer
	++TIMER.led; // MMI led write timer -> H/W 구조상 주기적 계속 써줘야함
	++TIMER.backlight; // LCD 백라이트 3분후 소등용 timer

	++MANAGER.rx_timeout; // 전면 시리얼통신이 데이터가 날라오다가 예를들어 케이블이 끊어졌을 때 내부 변수, 인터럽트 상태를 리셋시키기 위해 통신상태를 감시하는 timer
	++WAVE.timer; // fault wave 저장 시 사용하는 timer
	++TIMER.measurement; // 계측데이터 후가공 시간 간격 timer
	++HARMONICS.timer; // 계측데이터 중 고조파 후가공 시간 간격 timer
	++HIMIX.timer; // himix 송신 시간간격 계산용 timer

	// 계전요소 pickup, op, dropout 계산용 타이머
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

	// R-Hour용 타이머
	// #define DISPLAY_CUT_I_1A        0.021 // (0.04 / 2) * 1.05
  // #define DISPLAY_CUT_I_5A        0.105 // (0.2 / 2) * 1.05
  // 차단기 close 판단의 근거
	// cb close
//	if((DISPLAY.True_RMS[Ia] >= MIN_MAX.current_display) || (DISPLAY.True_RMS[Ib] >= MIN_MAX.current_display) || (DISPLAY.True_RMS[Ic] >= MIN_MAX.current_display))
//	{
//		++TIMER.current_on;
//	}
//	else
//	TIMER.current_on = 0;

//-------- 타이머 증가 END
}

// 전면 시리얼포트를 통해 한 바이트가 수신되면 이리로 인터럽트 발생
interrupt void SCIRXB_ISR(void)
{
	// 일단 dsp 내부 버퍼(*ScibRegs_SCIRXBUF)에서 읽어와서 내부 배열에 저장
	MANAGER.rx_buffer[MANAGER.rx_count] = *ScibRegs_SCIRXBUF & 0xff;
	
	// 다른 인터럽트 허용위해 interrupt pending 해제
	*PieCtrlRegs_PIEACK = PIEACK_GROUP9;
	
	// 최초 시작
	// MANAGER.rx_count - 수신된 바이트 개수 카운터
	// 평상 시 즉, 아무것도 수신이 아니되었으면 0임(초기값)
	if(MANAGER.rx_count == 0)
	{
		// 시리얼통신 타임아웃 카운터 초기화
		MANAGER.rx_timeout = 0;
		
		// 뭔진 모르겠으나 일단 데이타가 들어왔음을 알리는 플래그
		MANAGER.rx_status = 1;
	}
	
	// 헤더, id, function code 2개, 데이타 크기 두개 총 6개는 무조건 들어오게 되어 있음
	// 1byte | 2byte | 3byte | 4byte | 5byte | 6byte ....
	// 헤더  | id    | F/C1  | F/C2  | D/L1  | D/L2  .....
	// 최초 사이즈 결정
	else if(MANAGER.rx_count == 5)
	{
		// data length의 상위 바이트
		MANAGER.rx_length = MANAGER.rx_buffer[4];
		
		MANAGER.rx_length <<= 8;
		
		// data length의 하위 바이트
		MANAGER.rx_length |= MANAGER.rx_buffer[5];
		
		// 전체적으로 들어오는 바이트 개수는 헤더 6개, 맨 뒤에 crc 2개, data length가 지정한 개수만큼 입력을 예상할 수 있다.
		MANAGER.rx_length += 8;	
	}
	
	// 수신 완료?
	// MANAGER.rx_count 가 예측된 데이타 개수(MANAGER.rx_length)와 같으면 하나의 프레임이 모두 들어왔다고 인정
	else if((MANAGER.rx_count + 1) == MANAGER.rx_length)
	MANAGER.rx_status = 2; // 다음 단계를 위해 플래그 변경
		
	++MANAGER.rx_count;	
}

// 전면 시리얼 송신 인터럽트 루틴
interrupt void SCITXINTB_ISR(void)				// PIE9.4 @ 0x000DC6  SCITXINTB (SCI-B)
{
	// *ScibRegs_SCITXBUF - dsp 시리얼 송신 버퍼
	// MANAGER.isr_tx 지정된 주소부터 MANAGER.tx_length 만큼 송신하면 됨
	*ScibRegs_SCITXBUF = *(MANAGER.isr_tx + MANAGER.tx_count++);
	
	// 다보냈으면 송신 인터럽트 사용 중지
	if(MANAGER.tx_count == MANAGER.tx_length)
	*ScibRegs_SCICTL2 &= 0xfffe; // 사용중지
	
	// 송신 인터럽트 펜딩 해제
	*PieCtrlRegs_PIEACK = PIEACK_GROUP9;
}

//himix tx 용
interrupt void SCITXINTC_ISR(void)				// PIE9.4 @ 0x000DC6  SCITXINTc (SCI-c)
{	
	// *ScibRegs_SCITXBUF = MANAGER.tx_buffer[MANAGER.tx_count++];
	*ScicRegs_SCITXBUF = HIMIX.tx_buffer[HIMIX.tx_count++];
	
	// 다보냈으면
	if(HIMIX.tx_count == 61)
	{
		*ScicRegs_SCICTL2 &= 0xfffe;
		
		//485 driver 죽임
		HIMIX_STOP;
	}
	*PieCtrlRegs_PIEACK = PIEACK_GROUP8;
}

