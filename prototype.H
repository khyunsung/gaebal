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

//void manager_handling(void);

void i2c_read(unsigned int, unsigned int, unsigned int);
void i2c_write(unsigned int, unsigned int, unsigned int, unsigned int *);

void lcd_character_write(char);
void lcd_control_write(char);

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

unsigned int setting_save(unsigned int *, unsigned int *, unsigned int);
unsigned int setting_load(unsigned int *, unsigned int, unsigned int *);
unsigned int Setting_CRC(unsigned int *, unsigned int);

void delay_us(unsigned long);
void booting_setting_check(void);
unsigned int setting_min_max_check(unsigned int *, unsigned int, unsigned int);
unsigned int pt_setting_min_max_check(unsigned long *, unsigned int, unsigned long);
unsigned int setting_min_max_check_angle(int *, int, int);

void rtc_handling(void);

unsigned long Inverse_GetDelayTime(int ,float ,float);

void eerom_control(unsigned int, unsigned int);
void eerom_write(unsigned int, void *);
void eerom_read(unsigned int, void *);

unsigned int aux_do_check(unsigned int *);

//unsigned int COMM_CRC(unsigned int *, unsigned int);
void make_crc_send(unsigned int *, unsigned int *, unsigned int);
void serial_write(unsigned int, unsigned int *, unsigned int *);
void serial_ok_nak_send(unsigned int);

void PROTECTIVE_RELAY(void);
void Check_47P_10V(void);
void RELAY_OCR50_1(void);
void RELAY_OCR50_2(void);
void RELAY_OCR51_1(void);
void RELAY_OCR51_2(void);
void RELAY_OCGR50(void);

void RELAY_OCGR51(void);
void RELAY_UVR_1(void);
void RELAY_UVR_2(void);
void RELAY_UVR_3(void);
void RELAY_P47(void);

void RELAY_N47(void);
void RELAY_OVR(void);
void RELAY_OVGR(void);
void RELAY_DGR(void);
void RELAY_SGR(void);

void flash_sector_erase(unsigned int *);
void flash_word_write(unsigned int *, unsigned int);

void measure_display(void);
void measure2_display(void);

void setting_post_handling(unsigned int *);
void GPT_Default_Settings(void);
void CPT_Default_Settings(void);
void ADDRESS_Default_Settings(void);
void AUTO_DISPLAY_Default_Settings(void);
void PASSWORD_Default_Settings(void);
void LOCAL_CTRL_Default_Settings(void);
void DSGR_ANGLE_Default_Settings(void);
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

void Save_Relay_Event(float);
void relay_dropout_to_normal(unsigned int);

//void supervision_relay(unsigned int);

void float_to_8bit_fram(void *, unsigned int *, unsigned int);
void float_to_integer(float, unsigned int *, float);
void float_to_integer2(float, unsigned int *, float);

void wave_dump_serial_sram(unsigned int *, unsigned int, unsigned int);
void himix_drive(void);
void himix_crc(unsigned int);

void Relay_On(unsigned int);
void Relay_Off(unsigned int);

void comm_drive(void);

void menu_init(void);
void VFD_cursor(char, char);
void VFD_Single_Line_dump(char, const char *);

void DO_Output(unsigned int);
void Event_Item_Display(void);		//khs, 2015-04-01 오후 3:31:35
void Event_Time_Display(void);		//khs, 2015-04-01 오후 3:31:35
void Save_Screen_Info(unsigned int);
void Restore_Screen_Info(void);
