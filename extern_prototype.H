extern void cpu_setup(void);
extern void interrupt_setup(void);
extern void Peripheral_Clock_setup(void);
extern void pll_setup(void);
extern void InitFlash(void);
extern void GPIO_setup(void);
extern void Xinterface_setup(void);
extern void interrupt_control(void);
extern void InitTimer(void);
extern void real_main(void);
extern void i2c_setup(void);
extern void sci_setup(void);

extern interrupt void XINT3_ISR(void);
extern interrupt void TINT1_ISR(void);
//extern interrupt void TINT0_ISR(void);
//extern interrupt void TINT2_ISR(void);
extern interrupt void SCIRXB_ISR(void);
extern interrupt void SCITXINTB_ISR(void);
extern interrupt void SCITXINTC_ISR(void);

//extern void manager_handling(void);

extern void i2c_read(unsigned int, unsigned int, unsigned int);
extern void i2c_write(unsigned int, unsigned int, unsigned int, unsigned int *);

extern void lcd_setup(void);
extern void key_drive(void);

extern void lcd_character_write(char);
extern void lcd_control_write(char);

//extern void ADC_Calibration_Control(void);
extern int ADC_Offset_Calibration(void);
extern int ADC_High_Calibration(void);
extern int ADC_low_Calibration(void);
extern int Offset_Check(unsigned int);
extern float DFT_Check(unsigned int, float, float);
//extern float TRMS_Check(float *);
//extern float P_Check(float *);

extern void power_update(void);
extern float Degree_Check(unsigned int);

extern unsigned int setting_save(unsigned int *, unsigned int *, unsigned int);
extern unsigned int setting_load(unsigned int *, unsigned int, unsigned int *);
extern unsigned int Setting_CRC(unsigned int *, unsigned int);

extern void delay_us(unsigned long);
extern void booting_setting_check(void);
extern unsigned int setting_min_max_check(unsigned int *, unsigned int, unsigned int);
extern unsigned int pt_setting_min_max_check(unsigned long *, unsigned int, unsigned long);
extern unsigned int setting_min_max_check_angle(int *, int, int);

extern void rtc_handling(void);

extern unsigned long Inverse_GetDelayTime(int, float, float);

extern void eerom_control(unsigned int, unsigned int);
extern void eerom_write(unsigned int, void *);
extern void eerom_read(unsigned int, void *);

extern unsigned int aux_do_check(unsigned int *);

//extern unsigned int COMM_CRC(unsigned int *, unsigned int);
extern void make_crc_send(unsigned int *, unsigned int *, unsigned int);
extern void serial_write(unsigned int, unsigned int *, unsigned int *);
extern void serial_ok_nak_send(unsigned int);

extern void PROTECTIVE_RELAY(void);
extern void Check_47P_10V(void);
extern void RELAY_OCR50_1(void);
extern void RELAY_OCR50_2(void);
extern void RELAY_OCR51_1(void);
extern void RELAY_OCR51_2(void);
extern void RELAY_OCGR50(void);

extern void RELAY_OCGR51(void);
extern void RELAY_UVR_1(void);
extern void RELAY_UVR_2(void);
extern void RELAY_UVR_3(void);
extern void RELAY_P47(void);

extern void RELAY_N47(void);
extern void RELAY_OVR(void);
extern void RELAY_OVGR(void);
extern void RELAY_DGR(void);
extern void RELAY_SGR(void);

extern void flash_sector_erase(unsigned int *);
extern void flash_word_write(unsigned int *, unsigned int);

extern void measure_display(void);
extern void measure2_display(void);

extern void setting_post_handling(unsigned int *);
extern void GPT_Default_Settings(void);
extern void CPT_Default_Settings(void);
extern void ADDRESS_Default_Settings(void);
extern void AUTO_DISPLAY_Default_Settings(void);
extern void PASSWORD_Default_Settings(void);
extern void LOCAL_CTRL_Default_Settings(void);
extern void DSGR_ANGLE_Default_Settings(void);
extern void UVR_MODE_Default_Settings(void);
extern void DISPLAY_3PHASE_Default_Settings(void);
extern void MODBUS_Default_Settings(void);
extern void OCR_MODE_Default_Settings(void);
extern void SYSTEM_ALARM_Default_Settings(void);

extern void OCR50_1_Default_Settings(void);
extern void OCR50_2_Default_Settings(void);
extern void OCR51_1_Default_Settings(void);
extern void OCR51_2_Default_Settings(void);
extern void OCGR50_Default_Settings(void);
extern void OCGR51_Default_Settings(void);
extern void UVR_1_Default_Settings(void);
extern void UVR_2_Default_Settings(void);
extern void UVR_3_Default_Settings(void);
extern void P47_Default_Settings(void);
extern void N47_Default_Settings(void);
extern void OVR_Default_Settings(void);
extern void OVGR_Default_Settings(void);
extern void DGR_Default_Settings(void);
extern void SGR_Default_Settings(void);

extern void DI_Default_Settings(void);
extern void DO_Default_Settings(void); 

extern void ClearFLASH(void);

extern void event_direct_save(unsigned long *);

extern void wave_save_process(void);
extern void wave_flash_word_write(unsigned int *, unsigned int);
extern void wave_vi_initial_pre1(void);
extern void wave_vi_initial_pre2(void);
extern void wave_vi_initial_post(void);
extern void wave_di_initial_pre1(void);
extern void wave_di_initial_pre2(void);
extern void wave_di_initial_post(void);
extern void wave_flash_sector_erase(unsigned int *);

extern void led_handling(void);

extern void measure_view_detail(float, unsigned int, unsigned int);
extern void power_view(float, unsigned int, unsigned int);
extern void harmonics(void);
extern void harmonic_sample_backup(unsigned int);
extern void harmonic_real_imag_calculation(float *, float const *);

extern void Save_Relay_Event(float);
extern void relay_dropout_to_normal(unsigned int);

//extern void supervision_relay(unsigned int);

extern void float_to_8bit_fram(void *, unsigned int *, unsigned int);
extern void float_to_integer(float, unsigned int *, float);
extern void float_to_integer2(float, unsigned int *, float);


extern void wave_dump_serial_sram(unsigned int *, unsigned int, unsigned int);
extern void himix_drive(void);
extern void himix_crc(unsigned int);

extern void Relay_On(unsigned int);
extern void Relay_Off(unsigned int);

extern void comm_drive(void);

extern void menu_init(void);
extern void menu_drive(void);

extern void VFD_cursor(char, char);
extern void VFD_Single_Line_dump(char, const char *);

extern void DO_Output(unsigned int);
extern void Event_Item_Display(void);
extern void Event_Time_Display(void);
extern void Save_Screen_Info(unsigned int);
extern void Restore_Screen_Info(void);
