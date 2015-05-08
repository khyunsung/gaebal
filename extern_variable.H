extern struct
{
	unsigned int pre_count;
	unsigned int post_count;
	unsigned int post_start;
	
	int *buffer;
	
	unsigned int save_index;
	unsigned int pre_count_di;
	unsigned int post_count_di;
	
	unsigned int relay;
	unsigned int timer;
	unsigned int hold;
} WAVE;

extern struct
{
	unsigned int destination_count;
	unsigned int source_count;
	unsigned int source_count_end;
	unsigned int end_flag;
} FLASH;

extern struct
{
	float first;
	float second;
	float third;
	float average;
	float average_ratio;
	
	unsigned int abnormal[10];
	int abnormal_value[10];
	
	int rms_12_buffer[10][12];
	int pre_36_buffer[10][36];
	int pre_value;
	
	unsigned int wave_count;
	
	unsigned int normal;
	int *dpram;
	
	unsigned int ending;
} SAMPLE;

extern struct
{
	unsigned int index_count;
	unsigned int index_count_3times;
	
	float real_value[10];
	float imag_value[10];
} DFT;

extern struct
{
	float rms_value[10];
	float true_rms[10];
	float angle[10];
	
	float real_I1_a;
	float real_I1_b;
	float real_I1_c;
	float imag_I1_a;
	float imag_I1_b;
	float imag_I1_c;
	float real_I1_sum;
	float imag_I1_sum;
	float I1_value;
	
	float real_I2_a;
	float real_I2_b;
	float real_I2_c;
	float imag_I2_a;
	float imag_I2_b;
	float imag_I2_c;
	float real_I2_sum;
	float imag_I2_sum;
	float I2_value;
	
	float real_V1_a;
	float real_V1_b;
	float real_V1_c;
	float imag_V1_a;
	float imag_V1_b;
	float imag_V1_c;
	float real_V1_sum;
	float imag_V1_sum;
	float V1_value;
	
	float real_V2_a;
	float real_V2_b;
	float real_V2_c;
	float imag_V2_a;
	float imag_V2_b;
	float imag_V2_c;
	float real_V2_sum;
	float imag_V2_sum;
	float V2_value;
	
	unsigned int power_1sec;
	int power_adder;
	
	float Pa_temp;
	float Pb_temp;
	float Pc_temp;
	
	float Qa_temp;
	float Qb_temp;
	float Qc_temp;
	
	float Pa_value;
	float Pb_value;
	float Pc_value;
	
	float Qa_value;
	float Qb_value;
	float Qc_value;
	
	float freq_real[12];
	float freq_imag[12];
	float frequency;
	float frequency_temp[6];
	unsigned int frequency_count;
	
	float freq_real_vn[12];
	float freq_imag_vn[12];
	float frequency_vn;
	float frequency_temp_vn[6];
	
	float dgr_rms_Io;
	float dgr_angle_Io;
	float sgr_rms_Io;
	float sgr_angle_Io;
} MEASUREMENT;

extern struct
{
	float True_RMS[10];
	float True_RMS_sum[10];
	float line2line[3];
	float line2line_real[3];
	float line2line_imag[3];

	float multipllier[10];
	float p_multipllier[3];
	unsigned int index;
	unsigned int sum_count;
	unsigned int switching;
	unsigned int switching_index;
	unsigned int Power_Up;
	float power_s[3];
	float power_p[3];
	float power_q[3];
	float pf[3];
	float p3;
	float q3;
	float pf3;

	float rms_value[10];
	float rms_value_temp[10];
	float rms_value_sum[10];
	float rms_Iavg_value;
	float rms_Vavg_value;
	float V1_value;
	float V2_value;
	float I1_value;
	float I2_value;
	float anlge[8];
} DISPLAY;

extern struct
{
	int offset[10];
	float slope[10];
	float intercept[10];
	float angle[10];

	float slope_power[3];
	float Power_Cos[3];
	float Power_Sin[3];
	float frequency_offset;

	float low_temp[10];
	float high_temp[10];
	float slope_temp1[10];
	float slope_temp2[10];

	int offset_temp[10];
	long slope_temp[10];
	float angle_temp[10];

	int offset_origin[10];
	float slope_origin[10];
	float intercept_origin[10];
	float angle_origin[10];
} CALIBRATION;

extern struct
{
	unsigned int key;
	unsigned int led;
	unsigned int lcd;
	
	unsigned long backlight;
	
	unsigned int cb_open;
	unsigned int cb_close;
	
	unsigned int measurement;
	
	unsigned long current_on;
} TIMER;

extern struct
{
	unsigned int pushed_key;
	unsigned int pushed_key_count;
	
	unsigned long position;
	unsigned long return_position;
	
	unsigned int remote1_local0;
	
//unsigned int do_status;
	unsigned int do_control;
	unsigned int do_out;
	unsigned int di_present;
	unsigned int di_past;
	unsigned int di_debounce_timer[8];
//unsigned int do_status_backup;
	
	unsigned int led_on;
	unsigned int led_mode;
	
	unsigned int diagnostic;
	int adc_ref_monitor;
	unsigned int adc_ref_monitor_count;
} SYSTEM;

extern struct
{
	// lcd 전체
	unsigned int refresh_status;
	unsigned int refresh_count;
	char *line_buffer1;
	char *line_buffer2;	
	
	char *twinkle_adder;
	unsigned int twinkle;
	unsigned int twinkle_status;
	unsigned int twinkle_addressing1;
	unsigned int twinkle_count;
	
	char twinkle_data1[10];

	// 붙여쓰기
	char line_data1[20];
	char line_data2[20];
	char line_data3[20];
	char line_data4[20];
	char *line_1st_adder;
	char *line_2nd_adder;
	char *line_3rd_adder;
	char *line_4th_adder;
	unsigned int line_1st_count;
	unsigned int line_2nd_count;
	unsigned int line_3rd_count;
	unsigned int line_4th_count;
	unsigned int line_1st_addressing;
	unsigned int line_2nd_addressing;
	unsigned int line_3rd_addressing;
	unsigned int line_4th_addressing;
	unsigned int line_1st_status;
	unsigned int line_2nd_status;
	unsigned int line_3rd_status;
	unsigned int line_4th_status;
} LCD;

extern struct
{
	unsigned int debounce[8];
	unsigned int property;
	unsigned int di_status;
	unsigned int do_status;
} DIDO;

extern struct
{
	unsigned int rx_buffer[0xff];
	unsigned int rx_count;
	unsigned int rx_timeout;
	unsigned int rx_length;
	unsigned int rx_status;
	
	unsigned int tx_buffer[0xff];
	unsigned int tx_length;
	unsigned int tx_count;
	
	unsigned int *isr_tx;
	
	unsigned int temp[10];
	
	unsigned int event_sp_backup;
	unsigned int event_rollover_backup;
	unsigned int event_send_block;
} MANAGER;

extern struct
{
	unsigned int Op_Phase;
	float Max_RMS;	
} PROTECT;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int mode;
	unsigned int current_set;
	unsigned int delay_time;
	unsigned int do_relay;
	
	unsigned int delay_ms;
	unsigned int do_output;
	unsigned int do_output_off;
	
	// 내부운용	
	float Pickup_Threshold;
	float Dropout_Threshold;
	
	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;

	// 추가 부분
	unsigned int use_temp;
	unsigned int mode_temp;
	unsigned int current_set_temp;
	unsigned int delay_time_temp;
	unsigned int do_relay_temp;	

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} OCR50_1;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int mode;
	unsigned int current_set;
	unsigned int delay_time;
	unsigned int do_relay;
	
	unsigned int delay_ms;
	unsigned int do_output;
	unsigned int do_output_off;
	
	// 내부운용	
	float Pickup_Threshold;
	float Dropout_Threshold;
	
	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;

	// 추가 부분
	unsigned int use_temp;
	unsigned int mode_temp;
	unsigned int current_set_temp;
	unsigned int delay_time_temp;
	unsigned int do_relay_temp;	

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} OCR50_2;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int mode;
	unsigned int current_set;
	unsigned int time_lever;
	unsigned int do_relay;

	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	unsigned long Op_Time_set;
	float setting;
	float Mbyk;
	float a;
	
	// 내부운용	
	float Pickup_Threshold; // 순서 지킬것
	float Dropout_Threshold; // 순서 지킬것

	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;

	// 추가 부분
	unsigned int use_temp;
	unsigned int mode_temp;
	unsigned int current_set_temp;
	unsigned int time_lever_temp;
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} OCR51_1;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int mode;
	unsigned int current_set;
	unsigned int time_lever;
	unsigned int do_relay;

	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	unsigned long Op_Time_set;
	float setting;
	float Mbyk;
	float a;
	
	// 내부운용	
	float Pickup_Threshold; // 순서 지킬것
	float Dropout_Threshold; // 순서 지킬것

	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;

	// 추가 부분
	unsigned int use_temp;
	unsigned int mode_temp;
	unsigned int current_set_temp;
	unsigned int time_lever_temp;
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;   
} OCR51_2;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int mode;
	unsigned int current_set;
	unsigned int delay_time;
	unsigned int do_relay;
	
	unsigned int delay_ms;
	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	// 내부운용	
	float Pickup_Threshold; // 순서 지킬것
	float Dropout_Threshold; // 순서 지킬것

	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;

	// 추가 부분
	unsigned int use_temp;
	unsigned int mode_temp;
	unsigned int current_set_temp;
	unsigned int delay_time_temp;
	unsigned int do_relay_temp;
	
	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} OCGR50;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int mode;
	unsigned int current_set;
	unsigned int time_lever;
	unsigned int do_relay;

	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	unsigned long Op_Time_set;
	float setting;
	float Mbyk;
	float a;
	
	// 내부운용	
	float Pickup_Threshold; // 순서 지킬것
	float Dropout_Threshold; // 순서 지킬것

	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;   //순서엄수

	// 추가 부분
	unsigned int use_temp;
	unsigned int mode_temp;
	unsigned int current_set_temp;
	unsigned int time_lever_temp;
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} OCGR51;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int voltage_set;
	unsigned int delay_time;
	unsigned int do_relay;
	
	unsigned int delay_ms;
	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	// 내부운용	
	float Pickup_Threshold; // 순서 지킬것
	float Dropout_Threshold; // 순서 지킬것

	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;   //순서엄수
	unsigned int over_volt;

	// 추가 부분
	unsigned int use_temp;
	unsigned int voltage_set_temp;
	unsigned int voltage_set_temp2;
	unsigned int delay_time_temp;
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} UVR_1;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int voltage_set;
	unsigned int delay_time;
	unsigned int do_relay;
	
	unsigned int delay_ms;
	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	// 내부운용	
	float Pickup_Threshold; // 순서 지킬것
	float Dropout_Threshold; // 순서 지킬것

	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;   //순서엄수
	unsigned int over_volt;

	// 추가 부분
	unsigned int use_temp;
	unsigned int voltage_set_temp;
	unsigned int voltage_set_temp2;
	unsigned int delay_time_temp;
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} UVR_2;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int voltage_set;
	unsigned int delay_time;
	unsigned int do_relay;
	
	unsigned int delay_ms;
	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	// 내부운용	
	float Pickup_Threshold; // 순서 지킬것
	float Dropout_Threshold; // 순서 지킬것

	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;   //순서엄수
	unsigned int over_volt;

	// 추가 부분
	unsigned int use_temp;
	unsigned int voltage_set_temp;
	unsigned int voltage_set_temp2;
	unsigned int delay_time_temp;
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} UVR_3;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int voltage_set;
	unsigned int delay_time;
	unsigned int do_relay;
	
	unsigned int delay_ms;
	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	// 내부운용	
	float Pickup_Threshold; // 순서 지킬것
	float Dropout_Threshold; // 순서 지킬것

	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;   //순서엄수

	// 추가 부분
	unsigned int use_temp;
	unsigned int voltage_set_temp;
	unsigned int voltage_set_temp2;
	unsigned int delay_time_temp;
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} P47;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int voltage_set;
	unsigned int delay_time;
	unsigned int do_relay;
	
	unsigned int delay_ms;
	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	// 내부운용	
	float Pickup_Threshold; // 순서 지킬것
	float Dropout_Threshold; // 순서 지킬것

	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;   //순서엄수

	// 추가 부분
	unsigned int use_temp;
	unsigned int voltage_set_temp;
	unsigned int voltage_set_temp2;
	unsigned int delay_time_temp;
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} N47;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int mode;
	unsigned int voltage_set;
	unsigned int delay_time;
	unsigned int time_lever;
	unsigned int do_relay;

	unsigned int delay_ms;
	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	float setting;
	float Mbyk;
		
	unsigned long Op_Time_set;
	
	// 내부운용	
	float Pickup_Threshold; // 순서 지킬것
	float Dropout_Threshold; // 순서 지킬것

	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;   //순서엄수

	// 추가 부분
	unsigned int use_temp;
	unsigned int mode_temp;
	unsigned int voltage_set_temp;
	unsigned int voltage_set_temp2;
	unsigned int delay_time_temp;
	unsigned int time_lever_temp;
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} OVR;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int mode;
	unsigned int voltage_set;
	unsigned int time_lever;	
	unsigned int do_relay;

	unsigned int delay_ms;
	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	float setting;
	float Mbyk;
	
	unsigned long Op_Time_set;
	
	// 내부운용	
	float Pickup_Threshold; // 순서 지킬것
	float Dropout_Threshold; // 순서 지킬것

	unsigned long op_count;
	unsigned int pickup_limit;
	unsigned long event_ready;
	unsigned int op_status;   //순서엄수

	// 추가 부분
	unsigned int use_temp;
	unsigned int mode_temp;
	unsigned int voltage_set_temp;
	unsigned int voltage_set_temp2;
	unsigned int time_lever_temp;	
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} OVGR;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int current_set;
	unsigned int voltage_set;
	unsigned int angle_set;
	unsigned int delay_time;
	unsigned int do_relay;
	
	unsigned int delay_ms;
	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	// 내부운용	
	float Pickup_Threshold_Io; // 순서 지킬것
	float Dropout_Threshold_Io; // 순서 지킬것
	float Pickup_Threshold_Vo; // 순서 지킬것
	float Dropout_Threshold_Vo; // 순서 지킬것

	unsigned int pickup_limit;
	unsigned long op_count;
	int angle_low;
	int angle_high;
	float angle_sub;
	unsigned long event_ready;
	unsigned int op_status;

	// 추가 부분
	unsigned int use_temp;
	unsigned int current_set_temp; 
	unsigned int current_set_temp2;
	unsigned int voltage_set_temp; 
	unsigned int voltage_set_temp2;
	unsigned int angle_set_temp;
	unsigned int delay_time_temp;
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} DGR;

extern struct
{
	// 설정치들
	unsigned int use;
	unsigned int current_set;
	unsigned int voltage_set;
	unsigned int angle_set;
	unsigned int delay_time;
	unsigned int do_relay;
	
	unsigned int delay_ms;
	unsigned int do_output;// 순서 지킬것
	unsigned int do_output_off;// 순서 지킬것
	
	// 내부운용	
	float Pickup_Threshold_Io; // 순서 지킬것
	float Dropout_Threshold_Io; // 순서 지킬것
	float Pickup_Threshold_Vo; // 순서 지킬것
	float Dropout_Threshold_Vo; // 순서 지킬것

	unsigned int pickup_limit;
	unsigned long op_count;
	int angle_low;
	int angle_high;
	float angle_sub;
	unsigned long event_ready;
	unsigned int op_status;

	// 추가 부분
	unsigned int use_temp;
	unsigned int current_set_temp; 
	unsigned int current_set_temp2;
	unsigned int voltage_set_temp; 
	unsigned int voltage_set_temp2;
	unsigned int angle_set_temp;
	unsigned int delay_time_temp;
	unsigned int do_relay_temp;

	float Op_Ratio;
	unsigned int Op_Phase;
	float Op_Time;

	unsigned long Pickup_Time;
	unsigned long Delay_Time;
} SGR;

extern struct
{
	unsigned int pickup;	
	unsigned int operation_realtime;
	unsigned int operation_sum_holding;

	unsigned long popup_property;
	float popup_ratio;
	unsigned long popup_optime;
	unsigned int popup_mask;

} RELAY_STATUS;

//--------- SYSTEM VARIABLE
extern struct
{
	unsigned int ct_primary;
	unsigned int nct_primary;
	unsigned int pt_primary_high;
	unsigned int pt_primary_low;
	unsigned int rated_current;
	
	float ct_ratio;
	float nct_ratio;
	float pt_ratio;
	float gpt_ratio;

	unsigned int ct_primary_temp;
	unsigned int nct_primary_temp;
	unsigned long pt_primary;
	unsigned long pt_primary_temp;
	unsigned int rated_current_temp;
} CPT;

extern struct
{
	unsigned int manual_relay_test;
	unsigned int manual_do_out;
} AUX_RELAY_TEST;

extern struct
{
	unsigned int address;
	unsigned int address_temp;
} ADDRESS;

extern struct
{
	unsigned int mode;
	unsigned int mode_temp;
} AUTO_DISPLAY;

extern struct
{
	unsigned int real;
	unsigned int real_temp;
} PASSWORD;

extern struct
{
	unsigned int pt_secondary;
	unsigned int pt_tertiary;
	
	unsigned int pt_secondary_temp;
	unsigned int pt_tertiary_temp;
} GPT;

extern struct
{	
	unsigned int mode;
	unsigned int source_level;
	unsigned int tcs_level;
	unsigned int tcs_time;
	unsigned int tcs_do_relay;
	unsigned int ccs_level;
	unsigned int ccs_time;
	unsigned int ccs_do_relay;
	
	//0이 tcs, 1이 ccs
	unsigned int do_output[2];
	unsigned int do_output_off[2];
	long monitoring[2];
	unsigned int pickup_count[2];
	unsigned int pickup[2];
	unsigned int status[2];
	unsigned int time[2];
	
	unsigned int monitor_count;
	unsigned int monitor_update;
	long tcs_monitoring;
	long ccs_monitoring;	
	unsigned int tcs_ccs_status;
	unsigned int cb_close_time;
} SUPERVISION;

extern struct
{
	unsigned int use;
	unsigned int close_time;
	unsigned int open_time;

	unsigned int use_temp;
	unsigned int close_time_temp;
	unsigned int open_time_temp;
} LOCAL_CTRL;

extern struct
{
	int dgr_angle;
	int sgr_angle;

	int dgr_angle_temp;
	int sgr_angle_temp;
} DSGR_ANGLE;

extern struct
{
	unsigned int setting1;
	unsigned int setting2;
	unsigned int setting3;

	unsigned int setting1_temp;
	unsigned int setting2_temp;
	unsigned int setting3_temp;
} UVR_MODE;

extern struct
{
	unsigned int use;

	unsigned int use_temp;
} DISPLAY_3PHASE;

extern struct
{
	unsigned int baudrate;
	unsigned int delay;

	unsigned int baudrate_temp;
	unsigned int delay_temp;
} MODBUS;

extern struct
{
	unsigned int ocr_mode;
	unsigned int di_number;

	unsigned int ocr_di_mask;

	unsigned int ocr_mode_temp;
	unsigned int di_number_temp;
} OCR_MODE_SET;

extern struct
{
	unsigned int use;
	unsigned int do_relay;

	unsigned int use_temp;
	unsigned int do_relay_temp;
} SYSTEM_ALARM;

extern struct
{
	unsigned int Hz;
	unsigned int rated_ct;	
	unsigned int gr_select;
//unsigned int model;	//2015.02.25	
} CORE;

extern struct
{
	unsigned int debounce[8];
	unsigned int di_status; //DI 상태
	
	unsigned int debounce_temp[8];
} DIGITAL_INPUT;

extern struct
{
	unsigned int property;
	unsigned int do_status; //DO 상태. 기존에 기능 없음

	unsigned int property_temp;
} DIGITAL_OUTPUT;
//--------- SYSTEM VARIABLE END

extern struct
{
	unsigned int use;
	unsigned int address;
	unsigned int baudrate;
	unsigned int index;
} COMM;

extern struct
{
	unsigned int mode;	
} LOCAL_CONTROL;

extern struct
{
	int dgr;
	int sgr;
} GR_ANGLE;

extern struct
{
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned int milisecond;

	unsigned int update;
	unsigned int *buffer;

	unsigned int year_temp;
	unsigned int month_temp;
	unsigned int day_temp;
	unsigned int hour_temp;
	unsigned int minute_temp;
	unsigned int second_temp;
	unsigned int milisecond_temp;

	unsigned int year_temp2;
	unsigned int month_temp2;
	unsigned int day_temp2;
	unsigned int hour_temp2;
	unsigned int minute_temp2;
	unsigned int second_temp2;
	unsigned int milisecond_temp2;
} TIME;

extern struct
{
	unsigned int sp;
	
	int view_point;
	int view_start;
	unsigned int rollover;
	
	unsigned int temp;
	
	unsigned long pickup;
	unsigned long operation;
	
	unsigned long relay_set;
	unsigned long system_set;
	unsigned long data_reset;
	unsigned long di_off;
	unsigned long di_on;
	unsigned long do_off;
	unsigned long do_on;
	unsigned long mode_change;
	unsigned long control;
	
	unsigned int ratio;
	unsigned long optime;
	
	unsigned int group_extra;
} EVENT;

extern struct
{
	unsigned int tx_buffer[61];
	unsigned int tx_count;
	
	unsigned int index;
	
	unsigned int timer;
	
	unsigned long temp32;
	unsigned int temp16;
	
	unsigned int number_start;
	unsigned int minus_flag;
	
	unsigned int buffer_index;
	
	unsigned int crc[2];
	unsigned int crc_count;
} HIMIX;

extern struct
{
	unsigned int temp16_1;
	unsigned int temp16_2;
	
	float temp_float1;
	float temp_float2;
} TIMER0;

extern struct
{
	unsigned int temp16_1;
	unsigned int temp16_2;
} TIMER1;

extern struct
{
	unsigned int ocgr_dgr;
	unsigned int zct_angle;
	
	unsigned int ocr_mode;
	unsigned int di_number;
	unsigned int ocr_di_mask;
} SYSTEM_SET;

extern struct
{
	float current_display;
} MIN_MAX;

extern struct
{
	int sample[36];
	float base;
	float ia_max;
	float ib_max;
	float ic_max;
	
	float ia_max_save;
	float ib_max_save;
	float ic_max_save;
	
	float ia[6];
	float ib[6];
	float ic[6];
	float ftemp[4];
	
	float real;
	float imag;
	float float_temp;
	
	unsigned int index;
	unsigned int timer;
	
	unsigned int timer_15min;
} HARMONICS;

extern struct
{
	float energy_p;
	float energy_q;
	float energy_rp;
	float energy_rq;
	
	float vo_max;
	float io_max;
} ACCUMULATION;

typedef struct screen_move_info {
	unsigned int x;
	unsigned int y;
	unsigned int select;
	unsigned int data_change;
} Screen_Position_Info;
extern Screen_Position_Info Screen_Position, Cursor_Position, Save_Current_Screen;

extern unsigned int const DO_ON_BIT[8];
extern unsigned int const DO_OFF_BIT[8];
//extern unsigned int const ON_BIT[9];
//extern unsigned int const OFF_BIT[9];

extern float cos_table[12];
extern float sin_table[12];

// 2/N * sum(2pi * n/N) * x[n]
extern float const cos_60Hz[12];
extern float const sin_60Hz[12];
extern float const cos_50Hz[12];
extern float const sin_50Hz[12];

extern float const harmonics_1st_cos[36];
extern float const harmonics_1st_sin[36];
extern float const harmonics_3rd_cos[36];
extern float const harmonics_3rd_sin[36];
extern float const harmonics_5th_cos[36];
extern float const harmonics_5th_sin[36];
extern float const harmonics_7th_cos[36];
extern float const harmonics_7th_sin[36];
extern float const harmonics_9th_cos[36];
extern float const harmonics_9th_sin[36];

extern unsigned int const CRC_High[256];
extern unsigned int const CRC_Low[256];
extern unsigned long const wave_buffer_address[10];
extern unsigned long const wave_buffer_post_address[10];
extern unsigned int const do_control[7];

extern unsigned int const himix_number[10];

extern const unsigned int OCR50_I_MIN[2];
extern const unsigned int OCR50_I_MAX[2];
extern const unsigned int OCR50_T_MIN;
extern const unsigned int OCR50_T_MAX;

extern const unsigned int OCR51_I_MIN[2];
extern const unsigned int OCR51_I_MAX[2];
extern const unsigned int OCR51_TL_MIN;
extern const unsigned int OCR51_TL_MAX;

extern const unsigned int OCGR50_I_MIN[2];
extern const unsigned int OCGR50_I_MAX[2];
extern const unsigned int OCGR50_T_MIN;
extern const unsigned int OCGR50_T_MAX;

extern const unsigned int OCGR51_I_MIN[2];
extern const unsigned int OCGR51_I_MAX[2];
extern const unsigned int OCGR51_TL_MIN;
extern const unsigned int OCGR51_TL_MAX;

extern const unsigned int UVR_1_V_MIN[4];
extern const unsigned int UVR_1_V_MAX[4];
extern const unsigned int UVR_2_V_MIN[4];
extern const unsigned int UVR_2_V_MAX[4];
extern const unsigned int UVR_3_V_MIN[4];
extern const unsigned int UVR_3_V_MAX[4];
extern const unsigned int UVR_T_MIN;
extern const unsigned int UVR_T_MAX;

extern const unsigned int P47_V_MIN[4];
extern const unsigned int P47_V_MAX[4];
extern const unsigned int P47_T_MIN;
extern const unsigned int P47_T_MAX;

extern const unsigned int N47_V_MIN[4];
extern const unsigned int N47_V_MAX[4];
extern const unsigned int N47_T_MIN;
extern const unsigned int N47_T_MAX;

extern const unsigned int OVR_V_MIN[4];
extern const unsigned int OVR_V_MAX[4];
extern const unsigned int OVR_T_MIN;
extern const unsigned int OVR_T_MAX;
extern const unsigned int OVR_TL_MIN;
extern const unsigned int OVR_TL_MAX;

extern const unsigned int OVGR_V_MIN[3];
extern const unsigned int OVGR_V_MAX[3];
extern const unsigned int OVGR_TL_MIN;
extern const unsigned int OVGR_TL_MAX;

extern const unsigned int DGR_I_MIN[2];
extern const unsigned int DGR_I_MAX[2];
extern const unsigned int DGR_V_MIN[3];
extern const unsigned int DGR_V_MAX[3];
extern const unsigned int DGR_T_MIN;
extern const unsigned int DGR_T_MAX;
extern const unsigned int DGR_A_MIN;
extern const unsigned int DGR_A_MAX;

extern const unsigned int SGR_I_MIN;
extern const unsigned int SGR_I_MAX;
extern const unsigned int SGR_V_MIN[3];
extern const unsigned int SGR_V_MAX[3];
extern const unsigned int SGR_T_MIN;
extern const unsigned int SGR_T_MAX;
extern const unsigned int SGR_A_MIN;
extern const unsigned int SGR_A_MAX;

extern const unsigned int CT_MIN;
extern const unsigned int CT_MAX;
extern const unsigned int PT_MIN;
extern const unsigned long PT_MAX;
extern const unsigned int CT_5A_RATED;
extern const unsigned int CT_1A_RATED;

extern const unsigned int DEBOUNCE_MIN;
extern const unsigned int DEBOUNCE_MAX;

extern const long CALIB_I_MIN;
extern const long CALIB_I_MAX;
extern const long CALIB_In_MIN;
extern const long CALIB_In_MAX;
extern const long CALIB_V_MIN;
extern const long CALIB_V_MAX;
extern const long CALIB_Vo_MIN;
extern const long CALIB_Vo_MAX;

extern unsigned int KHS_Key_Press;
extern unsigned int Calibration_Menu_Escape;
