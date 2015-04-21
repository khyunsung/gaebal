//system setup 관련
void cpu_setup(void);
void interrupt_setup(void);
void Peripheral_Clock_setup(void);
void pll_setup(void);
void InitFlash(void);
void GPIO_setup(void);
void Xinterface_setup(void);
void interrupt_control(void);
void InitTimer(void);
void real_main(void);
void i2c_setup(void);
void sci_setup(void);
void lcd_setup(void);

// interrupt routine
interrupt void XINT3_ISR(void);
interrupt void TINT1_ISR(void);
//interrupt void TINT0_ISR(void);
//interrupt void TINT2_ISR(void);
interrupt void SCIRXB_ISR(void);
interrupt void SCITXINTB_ISR(void);
interrupt void SCITXINTC_ISR(void);

// manager 대응
//void manager_handling(void);

// rtc 대응
void i2c_read(unsigned int, unsigned int, unsigned int);
void i2c_write(unsigned int, unsigned int, unsigned int, unsigned int *);

// lcd handling
void lcd_character_write(char);
void lcd_control_write(char);

// mmi 대응
void key_drive(void);
void menu_drive(void);

//calibration 관련
//void ADC_Calibration_Control(void);
int ADC_Offset_Calibration(void);
int ADC_High_Calibration(void);
int ADC_low_Calibration(void);
int Offset_Check(unsigned int);
float DFT_Check(unsigned int, float, float);
//float TRMS_Check(float *);
//float P_Check(float *);
float Degree_Check(unsigned int);

// display 계측
void power_update(void);

void setting_view(float, unsigned int, unsigned int, unsigned int);
void display_aux_relay(unsigned int);
void point_setting_left_right_action(unsigned int, unsigned int);
void point_setting_up_down_action(unsigned int *, unsigned int, unsigned int, unsigned int);
void ocr50_ocgr50_mode_view(char* const *, unsigned int, unsigned int);
void ocr50_ocgr50_I_T_view(char* const *, unsigned int, unsigned int, unsigned int);
void ocr51_ocgr51_mode_view(char* const *, unsigned int, unsigned int);
void ocr51_ocgr51_I_T_view(char* const *, unsigned int, unsigned int, unsigned int);
void aux_relay_setting(unsigned int *);
unsigned int setting_save(unsigned int *, unsigned int *, unsigned int);
unsigned int setting_load(unsigned int *, unsigned int, unsigned int *);
unsigned int Setting_CRC(unsigned int *, unsigned int);

void delay_us(unsigned long);
void booting_setting_check(void);
unsigned int setting_min_max_check(unsigned int *, unsigned int, unsigned int);
unsigned int pt_setting_min_max_check(unsigned long *, unsigned int, unsigned long);

void voltage_set_view(char* const *, unsigned int, unsigned int);
void relay_use_display(char* const *, unsigned int, unsigned int, unsigned int);
void integer_view(float, unsigned int);
void integer_setting_left_right_action(unsigned int, unsigned int);
void integer_setting_up_down_action(void *, unsigned long, unsigned long, unsigned int);
void display_time(unsigned int *);
void rtc_handling(void);

unsigned long Inverse_Op_Timeset(unsigned int, float, float, float);

void eerom_control(unsigned int, unsigned int);
void eerom_write(unsigned int, void *);
void eerom_read(unsigned int, void *);

unsigned int aux_do_check(unsigned int *);

//unsigned int COMM_CRC(unsigned int *, unsigned int);
void make_crc_send(unsigned int *, unsigned int *, unsigned int);
void serial_write(unsigned int, unsigned int *, unsigned int *);
void serial_ok_nak_send(unsigned int);


void protective_relay(void);
void OCR50_1_RELAY(void);

void flash_sector_erase(unsigned int *);
void flash_word_write(unsigned int *, unsigned int);

void core_set_drive(void);

void relay_use_mode_view(unsigned int, unsigned int, unsigned int);
void relay_settings_view(char* const *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void relay_setting_view_handing(unsigned long, unsigned long, unsigned long);
void relay_use_set_view(unsigned int, unsigned int *, unsigned int);
void relay_mode_set_view(unsigned int, unsigned int);
void relay_set_value_view(unsigned int, char* const *, unsigned int, unsigned int, unsigned int);

void lcd_twinkle_display(char *, unsigned int);
void integer_value_set_handling(unsigned int *, unsigned int, unsigned int, unsigned int, unsigned long);
void integer_set_value_view(char* const *, float, unsigned int);
void setting_confirm(unsigned long, unsigned int, unsigned int *, unsigned long, unsigned int *);

void long_value_set_handling(unsigned long *, unsigned int, unsigned int, unsigned int, unsigned long, unsigned long);
void pt_set_view(char* const *, unsigned int);
void pt_set_handling(unsigned int *);

void display_do_property(unsigned int);
void do_property_setting(unsigned int *);
void di_do_status(unsigned int);

void load_set_value(unsigned int *, unsigned int);

void measure_display(void);
void measure2_display(void);

void setting_post_handling(unsigned int *);
void GPT_Default_Settings(void);
void CPT_Default_Settings(void);
void ADDRESS_Default_Settings(void);
void AUTO_DISPLAY_Default_Settings(void);
void PASSWORD_Default_Settings(void);
void LOCAL_CTRL_Default_Settings(void);
void UVR_MODE_Default_Settings(void);
void DISPLAY_3PHASE_Default_Settings(void);
void MODBUS_Default_Settings(void);
void OCR_MODE_Default_Settings(void);
void SYSTEM_ALARM_Default_Settings(void);

void OCR50_1_Default_Settings(void);
void OCR50_2_Default_Settings(void);
void OCR51_1_Default_Settings(void);
void OCR51_2_Default_Settings(void);
void OCGR50_Default_Settings(void);
void OCGR51_Default_Settings(void);
void UVR_1_Default_Settings(void);
void UVR_2_Default_Settings(void);
void UVR_3_Default_Settings(void);
void P47_Default_Settings(void);
void N47_Default_Settings(void);
void OVR_Default_Settings(void);
void OVGR_Default_Settings(void);
void DGR_Default_Settings(void);
void SGR_Default_Settings(void);

void DI_Default_Settings(void);
void DO_Default_Settings(void); 

void ClearFLASH(void);

void event_direct_save(unsigned long *);

void free_integer_display(char *, unsigned int, unsigned int);

void wave_save_process(void);
void wave_flash_word_write(unsigned int *, unsigned int);
void wave_vi_initial_pre1(void);
void wave_vi_initial_pre2(void);
void wave_vi_initial_post(void);
void wave_di_initial_pre1(void);
void wave_di_initial_pre2(void);
void wave_di_initial_post(void);
void wave_flash_sector_erase(unsigned int *);

void led_handling(void);

void measure_view_detail(float, unsigned int, unsigned int);
void power_view(float, unsigned int, unsigned int);
void harmonics(void);
void harmonic_sample_backup(unsigned int);
void harmonic_real_imag_calculation(float *, float const *);

void relay_normal_to_detect(unsigned long *, unsigned int *);
void relay_detect_to_pickup(unsigned long *, unsigned int *, unsigned long, unsigned int);
void relay_pickup_to_operation(unsigned int *, unsigned int, float, unsigned long, unsigned int *);
void relay_opeartion_to_dropout(unsigned int *, unsigned int *);
void relay_dropout_to_normal(unsigned long *, unsigned int *, unsigned int, unsigned int *);

//void h50_pickup_process(void);
//void h50_dropout_process(void);

void supervision_relay(unsigned int);

void float_to_8bit_fram(void *, unsigned int *, unsigned int);


void relay_use_view(unsigned int, unsigned int);

void wave_dump_serial_sram(unsigned int *, unsigned int, unsigned int);
//void do_test_check(unsigned int);
void himix_drive(void);
void himix_crc(unsigned int);

void relay_value_set_handling(unsigned int *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void do_release(unsigned int *);
float over_phase_value_check(unsigned int, float, unsigned long *);
float under_phase_value_check(unsigned int, float, unsigned long *);

void comm_drive(void);

void menu_init(void);
void VFD_cursor(char, char);
void VFD_Single_Line_dump(char, const char *);

void DO_Output(unsigned int);
void Event_Item_Display(void);		//khs, 2015-04-01 오후 3:31:35
void Event_Time_Display(void);		//khs, 2015-04-01 오후 3:31:35
