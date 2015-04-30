// 시스템 정보
//  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
//  

//					                //12345678901234567890
//char*  const code_spec[2]={"H4'FS MD 5/60 V01.00",
// 		  			       "14.03.27    00000000"};
//
//					                 //12345678901234567890
//char*  const version_f1[2]={"FMN 110/190V,5A,60Hz",
// 		  			          "14.03.27 V00 0000000"};

//  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0
// 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93
// c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf d0 d1 d2 d3
 
 
											//12345678901234567890
char*  const clear[2]={"",
											 ""};
//khs, 2015-04-03 오후 6:57:30  전체 수정 
//root
														 //12345678901234567890
char*  const root_window1[2]={"F/OCGR        NO.000",
															""};
														 //12345678901234567890
char*  const root_window2[2]={"F/DGR         NO.000",
															""};
														 //12345678901234567890
char*  const root_window3[2]={"F/SGR         NO.000",
															""};
														 //12345678901234567890
char*  const root_window4[2]={"M/OCGR        NO.000",
															""};
														 //12345678901234567890
char*  const root_window5[2]={"M/DGR         NO.000",
															""};
														 //12345678901234567890
char*  const root_window6[2]={"M/SGR         NO.000",
															""};

//display root
												//12345678901234567890
const char* display1[2]={"  1.MEASUREMENT",
												 "  2.DI/DO/CB STATUS"};
												//12345678901234567890
const char* display2[2]={"  3.RELAY SET VIEW",
												 "  4.CT/PT SET VIEW"};
												//12345678901234567890
const char* display3[2]={"  5.EVENT  VIEW",
												 "  6.SYSTEM SET VIEW"};
												//12345678901234567890
const char* display4[2]={"  7.ETC VIEW",
												 ""};

//measuremnt root
													 //12345678901234567890
const char* measuremnt1[2]={"  1.VOLTAGE VIEW",
														"  2.CURRENT VIEW"};
													 //12345678901234567890
const char* measuremnt2[2]={"  3.POWER     VIEW",
														"  4.HARMONICS VIEW"};
													 //12345678901234567890
const char* measuremnt3[2]={"  5.THERMAL VIEW",
														""};
													 //12345678901234567890
const char* measuremnt4[2]={"  5.25 Vdf/Ddf/Fdf",
														""};

// Voltage
                     //12345678901234567890
const char* volt1[2]={"Vab        Vbc",
                      "Vca        Vo "};
                     //12345678901234567890
const char* volt2[2]={"Va:        Vb:",
                      "Vc:        Vo:"};
                     //12345678901234567890
const char* volt3[2]={" Vab:          V",
                      " Vbc:          V"};
                     //12345678901234567890
const char* volt4[2]={" Vca:          V",
                      " Vo :          V"};
                     //12345678901234567890
const char* volt5[2]={" Va :          V",
                      " Vb :          V"};
                     //12345678901234567890
const char* volt6[2]={" Vc :          V",
                      " Vo :          V"};
                     //12345678901234567890
const char* volt7[2]={"Va:  0.0 Vb:",
                      "Vc:"};
                     //12345678901234567890
const char* volt8[2]={" Vps:          V",
                      " Vns:          V"};
                     //12345678901234567890
const char* volt9[2]={" Vo :          V",
                      " Vom:          V"};

// current
                        //12345678901234567890
const char* current1[2]={"Ia:        Ib:",
                         "Ic:        Io:"};
                        //12345678901234567890
const char* current2[2]={" Ia :          A",
                         " Ib :          A"};
                        //12345678901234567890
const char* current3[2]={" Ic :          A",
                         " Io :          A"};
                        //12345678901234567890
const char* current4[2]={" Ic :          A",
                         " Io :          mA"};
                        //12345678901234567890
const char* current5[2]={"Va:  0.0 Ia:",
                         "Ib:      Ic:"};
                        //12345678901234567890
const char* current6[2]={" Ips:          A",
                         " Ins:          A"};
                        //12345678901234567890
const char* current7[2]={" Io :          A",
                         " Iom:          A"};

//power
                      //12345678901234567890
const char* power1[2]={"Freq:          Hz",
                       " PF :"};
                      //12345678901234567890
const char* power2[2]={"  P :          kW",
                       "  Q :          kVar"};
                      //12345678901234567890
const char* power3[2]={" Ep :          kWh",
                       " Eq :          kVarh"};
                      //12345678901234567890
const char* power4[2]={"R.Ep:          kWh",
                       "R.Eq:          kVarh"};

//harmonics
                          //12345678901234567890
const char* harmonics1[2]={"Ia3:       Ia5:",
                           "Ia7:       Ia9:"};
                          //12345678901234567890
const char* harmonics2[2]={"THDa:",
                           "TDDa:"};
                          //12345678901234567890
const char* harmonics3[2]={"Ib3:       Ib5:",
                           "Ib7:       Ib9:"};
                          //12345678901234567890
const char* harmonics4[2]={"THDb:",
                           "TDDb:"};
                          //12345678901234567890
const char* harmonics5[2]={"Ic3:       Ic5:",
                           "Ic7:       Ic9:"};
                          //12345678901234567890
const char* harmonics6[2]={"THDc:",
                           "TDDc:"};
                          //12345678901234567890
const char* delta_vdf1[2]={"Vdf:      Ddf:",
                           "Fdf:"};
                       //12345678901234567890
const char* q_value[2]={"     THERMAL Q",
                        "  Q :           %"};

//DI/DO/CB STATUS root
											 //12345678901234567890
const char* didocb1[2]={"  1.DI/DO STATUS",
												"  2.DI/DO SET VIEW"};
											 //12345678901234567890
const char* didocb2[2]={"  3.CB SUPERVSION",
												""};
									 //12345678901234567890
const char* di1[2]={"DI 1 2 3 4 5 6 7 8",
                    "ST"};
									 //12345678901234567890
const char* do1[2]={"DO 1 2 3 4 5 6 7 8 9",
                    "ST"};
									 //12345678901234567890
const char* do2[2]={"DO 1 2 3 4 5 6 7",
                    "MD"};

//RELAY SET VIEW root
											//12345678901234567890
const char* relay1[2]={"  1.OCR  VIEW",
											 "  2.GR   VIEW"};
											//12345678901234567890
const char* relay2[2]={"  3.OVR  VIEW",
											 "  4.OVGR VIEW"};
											//12345678901234567890
const char* relay3[2]={"  5.UVR  VIEW",
											 "  6.SOVR VIEW"};
											//12345678901234567890
const char* relay4[2]={"  7.SYNCRO VIEW",
											 ""};
											//12345678901234567890
const char* relay5[2]={"  3.UCR   VIEW",
											 "  4.NSOCR VIEW"};
											//12345678901234567890
const char* relay6[2]={"  5.UVR  VIEW",
											 "  6.THR  VIEW"};
											//12345678901234567890
const char* relay7[2]={"  7.50H  VIEW",
											 "  8.S/L  VIEW"};
											//12345678901234567890
const char* relay8[2]={"  9.NCH  VIEW",
											 ""};

//CT/PT SET VIEW root
										//12345678901234567890
const char* cpt1[2]={"  1.CT/PT SET VIEW",
										 ""};

											//12345678901234567890
const char* relay9[2]={"  1.OCR  SET",
											 "  2.GR   SET"};
											 //12345678901234567890
const char* relay10[2]={"  3.OVR  SET",
											  "  4.OVGR SET"};
											 //12345678901234567890
const char* relay11[2]={"  5.UVR  SET",
											  "  6.SOVR SET"};
											 //12345678901234567890
const char* relay12[2]={"  7.SYNCRO SET",
											  ""};
											 //12345678901234567890
const char* relay13[2]={"  3.UCR   SET",
											  "  4.NSOCR SET"};
											 //12345678901234567890
const char* relay14[2]={"  5.UVR  SET",
											  "  6.THR  SET"};
											 //12345678901234567890
const char* relay15[2]={"  7.50H  SET",
											  "  8.S/L  SET"};
											 //12345678901234567890
const char* relay16[2]={"  9.NCH SET",
											  ""};

												//12345678901234567890
const char* setting1[2]={"  1.RELAY SET",
												 "  2.CT/PT SET"};
												//12345678901234567890
const char* setting2[2]={"  3.DI/DO/CB SET",
												 "  4.SYSTEM   SET"};
												//12345678901234567890
const char* setting3[2]={"  5.DATA RESET",
												 "  6.ETC"};

										//12345678901234567890
const char* cpt2[2]={"  1.CT/PT SET",
										 ""};

										 //12345678901234567890
const char* docb1[2]={"  1.DI DEBOUNCE TIME",
											"  2.DO MODE SET"};
										 //12345678901234567890
const char* docb2[2]={"  3.SUPERVISION SET",
											""};

                               //12345678901234567890
const char* cb_status_view1[2]={"CB STATUS :",
                                "TCS:       CCS:"};
                               //12345678901234567890
const char* cb_status_view2[2]={"   CB CLOSE TIME",
                                "             HOUR"};
                                //12345678901234567890
const char* supervison_view1[2]={"TC/CC:",
                                 "  Vs :       V"};
                                //12345678901234567890
const char* supervison_view2[2]={" %tc>:       %",
                                 " Ttc :       s"};
                                //12345678901234567890
const char* supervison_view3[2]={" %cc>:       %",
                                 " Tcc :       s"};
                               //12345678901234567890
const char* supervison_set1[2]={" TC/CC SUPERVISION",
                                "    USE    NOT"};
                               //12345678901234567890
const char* supervison_set2[2]={" SUPERVISION VOLTAGE",
                                "  Vs :       V"};
                               //12345678901234567890
const char* supervison_set3[2]={"    TCS ON LEVEL",
                                " %tc>:       %"};
                               //12345678901234567890
const char* supervison_set4[2]={"    TCS OFF TIME",
                                "  Ttc:       s"};
                               //12345678901234567890
const char* supervison_set5[2]={"    CCS ON LEVEL",
                                " %cc>:       %"};
                               //12345678901234567890
const char* supervison_set6[2]={"    CCS OFF TIME",
                                "  Tcc:       s"};

												 //12345678901234567890
const char* ocr_view1[2]={"  1.50-1 VIEW",
													"  2.50-2 VIEW"};
												 //12345678901234567890
const char* ocr_view2[2]={"  3.51-1 VIEW",
													"  4.51-2 VIEW"};
												 //12345678901234567890
const char* ocr_view3[2]={"  1.50-1 SET",
													"  2.50-2 SET"};
												 //12345678901234567890
const char* ocr_view4[2]={"  3.51-1 SET",
													"  4.51-2 SET"};
												 //12345678901234567890
const char* ocr_view5[2]={"  1.50 VIEW",
													""};
												 //12345678901234567890
const char* ocr_view6[2]={"  1.50 SET",
													""};

												//12345678901234567890
const char* gr_view1[2]={"  1.50G VIEW",
												 "  2.51G VIEW"};
												//12345678901234567890
const char* gr_view2[2]={"  1.67N VIEW",
												 ""};
												//12345678901234567890
const char* gr_view3[2]={"  1.67G VIEW",
												 ""};
												//12345678901234567890
const char* gr_view4[2]={"  1.50G SET",
												 "  2.51G SET"};
												//12345678901234567890
const char* gr_view5[2]={"  1.67N SET",
												 ""};
												//12345678901234567890
const char* gr_view6[2]={"  1.67G SET",
												 ""};

												 //12345678901234567890
const char* ovr_view4[2]={"  1.59 VIEW",
													""};
												 //12345678901234567890
const char* ovr_view5[2]={"  1.59 SET",
													""};

													//12345678901234567890
const char* ovgr_view4[2]={"  1.64 VIEW",
													 ""};
													//12345678901234567890
const char* ovgr_view5[2]={"  1.64 SET",
													 ""};
													//12345678901234567890
const char* ovgr_view6[2]={"  1.64 VIEW",
													 "    25 USE"};
													//12345678901234567890
const char* ovgr_view7[2]={"  1.64 SET",
													 "    25 USE"};

												 //12345678901234567890
const char* ucr_view3[2]={"  1.37 VIEW",
													""};
												 //12345678901234567890
const char* ucr_view4[2]={"  1.37 SET",
													""};
													 //12345678901234567890
const char* nsocr_view4[2]={"  1.46 VIEW",
														""};
													 //12345678901234567890
const char* nsocr_view5[2]={"  1.46 SET",
														""};

												 //12345678901234567890
const char* thr_view4[2]={"  1.49 VIEW",
													""};
												 //12345678901234567890
const char* thr_view5[2]={"  1.49 SET",
													""};

												 //12345678901234567890
const char* h50_view1[2]={"  1.50H VIEW",
													""};
												 //12345678901234567890
const char* h50_view2[2]={"  1.50H SET",
													""};

												//12345678901234567890
const char* sl_view1[2]={"  1.48/51LR VIEW",
												 ""};
												//12345678901234567890
const char* sl_view2[2]={"  1.48/51LR SET",
												 ""};

												 //12345678901234567890
const char* nch_view4[2]={"  1.66 VIEW",
													""};
												 //12345678901234567890
const char* nch_view5[2]={"  1.66 SET",
													""};

												 //12345678901234567890
const char* uvr_view1[2]={"  1.27-1 VIEW",
													"  2.27-2 VIEW"};
												 //12345678901234567890
const char* uvr_view2[2]={"  3.27-3 VIEW",
													""};
												 //12345678901234567890
const char* uvr_view4[2]={"  1.27-1 SET",
													"  2.27-2 SET"};
												 //12345678901234567890
const char* uvr_view5[2]={"  3.27-3 SET",
													""};

													//12345678901234567890
const char* sovr_view1[2]={"  1.47P VIEW",
													 "  2.47N VIEW"};
													//12345678901234567890
const char* sovr_view2[2]={"  1.47P SET",
													 "  2.47N SET"};

														//12345678901234567890
const char* syncro_view1[2]={"  1.25 VIEW",
														 ""};
														//12345678901234567890
const char* syncro_view2[2]={"  1.25 SET",
														 ""};
														//12345678901234567890
const char* syncro_view3[2]={"  1.25 VIEW",
														 "    64 USE"};
														//12345678901234567890
const char* syncro_view4[2]={"  1.25 SET",
														 "    64 USE"};

												 //12345678901234567890
const char* uvr_view3[2]={"  1.27 VIEW",
													""};
												 //12345678901234567890
const char* uvr_view6[2]={"  1.27 SET",
													""};

                            //12345678901234567890
const char* relay_onoff1[2]={"       ON/OFF SET",
                             "        ON     OFF"};

													 //12345678901234567890
const char* relay_mode1[2]={"       MODE   SET",
                            "        INST   DEF"};
													 //12345678901234567890
const char* relay_mode2[2]={"       MODE   SET",
                            "  INV   VINV   EINV"};
													 //12345678901234567890
const char* relay_mode3[2]={"       MODE   SET",
                            "        DEF    INV"};
													 //12345678901234567890
const char* relay_mode4[2]={"       MODE   SET",
                            "        INST   INV"};

													//12345678901234567890
const char* delay_set1[2]={"      TIME DELAY SET",
                           "  Td :          s"};
											 //12345678901234567890
const char* tl_set1[2]={"      TIME LEVER SET",
                        "  T/L:"};

                               //12345678901234567890
const char* relay_set_view1[2]={"       USE :",
                                "      MODE :"};
                               
const char* relay_curve[6]={ "INST",
	                         "DEF ",
	                         "INV ",
	                         "VINV",
	                         "EINV",
	                         "----"};

//                          //12345678901234567890
const char* relay_relay1[2]={"   1 2 3 4 5 6 7 8 9",
                             "DO"};

                           //12345678901234567890
const char* ocr50_set1[2]={ "       CURRENT SET",
                            "  I>>:          In"};
                           //12345678901234567890
const char* ocr50_view1[2]={"  I>>:          In",
                            ""};
                           //12345678901234567890
const char* ocr50_view2[2]={"  I>>:          In",
                            "  Td :          s"};
                            
													//12345678901234567890
const char* ocr51_set1[2]={"       CURRENT SET",
                           "  I> :          In"};
													 //12345678901234567890
const char* ocr51_view1[2]={"  I> :          In",
                            "  T/L:"};
													 //12345678901234567890
const char* ocgr50_set1[2]={"       CURRENT SET",
                            " Io>>:          In"};
                            //12345678901234567890
const char* ocgr50_view1[2]={" Io>>:          In",
                             ""};
                            //12345678901234567890
const char* ocgr50_view2[2]={" Io>>:          In",
                             "  Td :          s"};
													 //12345678901234567890
const char* ocgr51_set1[2]={"       CURRENT SET",
                            "  Io>:          In"};
														//12345678901234567890
const char* ocgr51_view1[2]={"  Io>:          In",
                             "  T/L:"};

                          //12345678901234567890
const char* dsgr_view1[2]={"  Io>:          In",
                           "  Vo>:          V"};
                          //12345678901234567890
const char* dsgr_view2[2]={"  Io>:          mA",
                           "  Vo>:          V"};
                          //12345678901234567890
const char* dsgr_view3[2]={"ANGLE:          ",
                           "  Td :          s"};

												 //12345678901234567890
const char* dsgr_set1[2]={"       CURRENT SET",
                          "  Io>:          In"};
												 //12345678901234567890
const char* dsgr_set2[2]={"       CURRENT SET",
                          "  Io>:          mA"};
												 //12345678901234567890
const char* dsgr_set3[2]={"       VOLTAGE SET",
                          "  Vo>:          Vn"};
												 //12345678901234567890
const char* dsgr_set4[2]={"       ANGLE   SET",
                          "ANGLE:          "};
                           
												//12345678901234567890
const char* ovr_set1[2]={"       VOLTAGE SET",
                         "  V> :         Vn"};
                         //12345678901234567890
const char* ovr_view1[2]={"  V> :          Vn",
                          "  Td :          s"};
                         //12345678901234567890
const char* ovr_view2[2]={"  V> :          Vn",
                          "  T/L:"};
												 //12345678901234567890
const char* ovgr_set1[2]={"       VOLTAGE SET",
                          "  Vo>:          Vn"};

                          //12345678901234567890
const char* ovgr_view1[2]={"  Vo>:          Vn",
                           ""};
                          //12345678901234567890
const char* ovgr_view2[2]={"  Vo>:          Vn",
                           "  T/L:"};


												//12345678901234567890
const char* uvr_set1[2]={"       VOLTAGE SET",
                         "  V< :          Vn"};
												//12345678901234567890
const char* uvr_set2[2]={"     OP. MODE SET",
                         "      AUTO  MANUAL"};

                             //12345678901234567890
const char* uvr_set_view1[2]={"  V< :          Vn",
                              "  Td :          s"};
                             //12345678901234567890
const char* uvr_set_view2[2]={"      OP. MODE",
                              "        AUTO"};
                             //12345678901234567890
const char* uvr_set_view3[2]={"      OP. MODE",
                              "       MANUAL"};


                         //12345678901234567890
const char* p47_set1[2]={ "       VOLTAGE SET",
                          " Vps>:          Vn"};
                         //12345678901234567890
const char* p47_view1[2]={" Vps>:          Vn",
                          "  Td :          s"};

                         //12345678901234567890
const char* n47_set1[2]={ "       VOLTAGE SET",
                          " Vns>:          Vn"};
                         //12345678901234567890
const char* n47_view1[2]={" Vns>:          Vn",
                          "  Td :          s"};


                         //12345678901234567890
const char* sync_set1[2]={"25 DIFF. VOLTAGE SET",
                          " Vdf>:          V"};
                         //12345678901234567890
const char* sync_set2[2]={"25 DIFF. PHASE  SET",
                          " Ddf>:          "};

                         //12345678901234567890
const char* sync_set3[2]={"25 DIFF. FREQ.  SET",
                          " Fdf>:          Hz"};
                         //12345678901234567890
const char* sync_set4[2]={"25 LIVE VOLTAGE SET",
                          " Vlv>:          V"};

                          //12345678901234567890
const char* sync_view1[2]={" Vdf>:          V",
                           " Ddf>:          "};
                          //12345678901234567890
const char* sync_view2[2]={" Fdf>:          Hz",
                           " Vlv>:          V"};
                          //12345678901234567890
const char* sync_view3[2]={"  Td :          s",
                           ""};

                         //12345678901234567890
const char* ucr_set1[2]={ "37 HIGH CURRENT SET",
                          "  Ih<:          In"};
                         //12345678901234567890
const char* ucr_set2[2]={ "37 LOW  CURRENT SET",
                          "  Il<:          In"};
                         //12345678901234567890
const char* ucr_view1[2]={"  Ih<:          In",
                          "  Il<:          A"};
                         //12345678901234567890
const char* ucr_view2[2]={"  Td :          s",
                          ""};

                           //12345678901234567890
const char* nsocr_set1[2]={ "46     CURRENT SET",
                            " Ins>:          In"};
                           //12345678901234567890
const char* nsocr_view1[2]={" Ins>:          In",
                            "  Td :          s"};

                           //12345678901234567890
const char* motor_view1[2]={" Ifl :          In",
                            " Tms :          s"};
                           //12345678901234567890
const char* motor_view2[2]={"   k :",
                            ""};

                          //12345678901234567890
const char* motor_set1[2]={"  FULL LOAD CURRENT",
                           " Ifl :          In"};
                          //12345678901234567890
const char* motor_set2[2]={"  MOTOR START TIME",
                           " Tms :          s"};
                          //12345678901234567890
const char* motor_set3[2]={"   MOTOR k FACTOR",
                           "   k :"};

                         //12345678901234567890
const char* thr_view1[2]={"  Tc :          s",
                          "  TCR:"};

                        //12345678901234567890
const char* thr_set1[2]={"    MOTOR COLD TIME",
                         "  Tc :          s"};
                        //12345678901234567890
const char* thr_set2[2]={"    TIME CONSTANT",
                         "  TCR:"};

                     //12345678901234567890
const char* commx[2]={"  NOT INSERT COMM.",
                      "   PRESS ANY KEY"};
                          //12345678901234567890
const char* comm_view1[2]={"ADDR.:",
                           "BAUD :          bps"};

                         //12345678901234567890
const char* comm_set1[2]={" COMM. ADDRESS SET",
                          "ADDR.:"};
                         //12345678901234567890
const char* comm_set2[2]={" COMM. BAUDRATE SET",
                          "BAUD :          bps"};

                         //12345678901234567890
const char* l_cb_open[2]={"      CB OPEN?",
                          "      YES   NO"};
                          //12345678901234567890
const char* l_cb_close[2]={"      CB CLOSE?",
                           "      YES   NO"};
                        //12345678901234567890
const char* cb_good1[2]={" CB CLOSE OPERATION",
                         "   PRESS ANY KEY"};
                        //12345678901234567890
const char* cb_good2[2]={" CB OPEN  OPERATION",
                         "   PRESS ANY KEY"};

                        //12345678901234567890
const char* cb_fail1[2]={"  CAN NOT CB CLOSE",
                         "   PRESS ANY KEY"};
                        //12345678901234567890
const char* cb_fail2[2]={"  CAN NOT CB OPEN",
                         "   PRESS ANY KEY"};
                          //12345678901234567890
const char* cb_cancel1[2]={" CB CLOSE CANCELED",
                           "   PRESS ANY KEY"};
                          //12345678901234567890
const char* cb_cancel2[2]={" CB OPEN  CANCELED",
                           "   PRESS ANY KEY"};

												 //12345678901234567890
const char* h50_view3[2]={"  I>>:          In",
													" MODE:  "};

												//12345678901234567890
const char* h50_set1[2]={"     OP. MODE SET",
												 "     KEPCO  NORMAL"};

												 //12345678901234567890
const char* s_l_view1[2]={" Ilk>:          Ifl",
													" T/L :"};
												 //12345678901234567890
const char* s_l_view2[2]={" Ist>:          Ifl",
													"  Td :          s"};

                        //12345678901234567890
const char* s_l_set1[2]={"     LOCK CURRENT",
                         " Ilk>:          Ifl"};
                        //12345678901234567890
const char* s_l_set2[2]={"     STALL CURRENT",
                         " Ist>:          Ifl"};

                        //12345678901234567890
const char* nch_set1[2]={"66 MOTOR START ALLOW",
                         " Tma :           min"};
                        //12345678901234567890
const char* nch_set2[2]={"66  NUMBER OF START",
                         " Nst :"};
                        //12345678901234567890
const char* nch_set3[2]={"66  MOTOR BLOCK TIME",
                         " Tbk :          min"};
                        //12345678901234567890
const char* nch_set4[2]={"66  THERMAL LIMIT",
                         "  Ql :          %"};

                         //12345678901234567890
const char* nch_view1[2]={" Tma :          min",
                          " Nst :"};
                         //12345678901234567890
const char* nch_view2[2]={" Tbk :          min",
                          " Ql  :          %"};

                            //12345678901234567890
const char* system_view1[2]={"  1.COMM. SET VIEW",
                             "  2.LOCAL CONTROL"};
                            //12345678901234567890
const char* system_view2[2]={"  3.OCGR/DGR SELECT",
                             "  4.OCR MODE"};
                            //12345678901234567890
const char* system_view3[2]={"  3.OCGR/DGR SELECT",
                             "  4.MOTOR SET VIEW"};
                            //12345678901234567890
const char* system_view4[2]={"  3.ZCT ANGLE SET",
                             "  4.OCR MODE"};
                            //12345678901234567890
const char* system_view5[2]={"  3.ZCT ANGLE SET",
                             "  4.MOTOR SET VIEW"};

                           //12345678901234567890
const char* system_set1[2]={"  1.PASSWORD  SET",
                            "  2.SYS. TIME SET"};
                           //12345678901234567890
const char* system_set2[2]={"  3.COMM. SET",
                            "  4.LOCAL CONTROL"};
                           //12345678901234567890
const char* system_set3[2]={"  5.OCGR/DGR SELECT",
                            "  6.OCR MODE SET"};
                           //12345678901234567890
const char* system_set4[2]={"  5.OCGR/DGR SELECT",
                            "  6.MOTOR INFO. SET"};
                           //12345678901234567890
const char* system_set5[2]={"  5.ZCT ANGLE SET",
                            "  6.OCR MODE SET"};
                           //12345678901234567890
const char* system_set6[2]={"  5.ZCT ANGLE SET",
                            "  6.MOTOR INFO. SET"};

                              //12345678901234567890
const char* ct_ratio_view1[2]={" CT :       /5 A",
                               "NCT :       /5 A"};
                              //12345678901234567890
const char* ct_ratio_view2[2]={" CT :       /1 A",
                               "NCT :       /1 A"};
                              //12345678901234567890
const char* ct_ratio_view3[2]={" CT :       /5.0  A",
                               "ZCT :    200/1.5 mA"};
                              //12345678901234567890
const char* ct_ratio_view4[2]={" CT :       /1.0  A",
                               "ZCT :    200/1.5 mA"};

                              //12345678901234567890
const char* pt_ratio_view1[2]={" PT :       /100 V",
                               "GPT :       /100 V"};
                              //12345678901234567890
const char* pt_ratio_view2[2]={" PT :       /100 V",
                               "GPT :       /110 V"};
                              //12345678901234567890
const char* pt_ratio_view3[2]={" PT :       /100 V",
                               "GPT :       /120 V"};
                              //12345678901234567890
const char* pt_ratio_view4[2]={" PT :       /100 V",
                               "GPT :       /190 V"};
                              //12345678901234567890
const char* pt_ratio_view5[2]={" PT :       /110 V",
                               "GPT :       /100 V"};
                              //12345678901234567890
const char* pt_ratio_view6[2]={" PT :       /110 V",
                               "GPT :       /110 V"};
                              //12345678901234567890
const char* pt_ratio_view7[2]={" PT :       /110 V",
                               "GPT :       /120 V"};
                              //12345678901234567890
const char* pt_ratio_view8[2]={" PT :       /110 V",
                               "GPT :       /190 V"};
                               //12345678901234567890
const char* pt_ratio_view9[2]= {" PT :       /120 V",
                                "GPT :       /100 V"};
                              //12345678901234567890
const char* pt_ratio_view10[2]={" PT :       /120 V",
                                "GPT :       /110 V"};
                              //12345678901234567890
const char* pt_ratio_view11[2]={" PT :       /120 V",
                                "GPT :       /120 V"};
                              //12345678901234567890
const char* pt_ratio_view12[2]={" PT :       /120 V",
                                "GPT :       /190 V"};
                               //12345678901234567890
const char* pt_ratio_view13[2]={" PT :       /190 V",
                                "GPT :       /100 V"};
                              //12345678901234567890
const char* pt_ratio_view14[2]={" PT :       /190 V",
                                "GPT :       /110 V"};
                              //12345678901234567890
const char* pt_ratio_view15[2]={" PT :       /190 V",
                                "GPT :       /120 V"};
                              //12345678901234567890
const char* pt_ratio_view16[2]={" PT :       /190 V",
                                "GPT :       /190 V"};

                               //12345678901234567890
const char* pt_wiring_view1[2]={"      PT WIRING",
                                ""};

                             //12345678901234567890
const char* ct_ratio_set1[2]={" CT  PRIMARY SET",
                              " CT :       /5 A"};
                             //12345678901234567890
const char* ct_ratio_set2[2]={" CT  PRIMARY SET",
                              " CT :       /1 A"};
                             //12345678901234567890
const char* ct_ratio_set3[2]={"NCT  PRIMARY SET",
                              "NCT :       /5 A"};
                             //12345678901234567890
const char* ct_ratio_set4[2]={"NCT  PRIMARY SET",
                              "NCT :       /1 A"};
                             //12345678901234567890
const char* pt_ratio_set1[2]={" PT  PRIMARY   SET",
                              " PT :        V"};
                             //12345678901234567890
const char* pt_ratio_set2[2]={" PT  SECONDARY SET",
                              " 100  110  120  190"};
                             //12345678901234567890
const char* pt_ratio_set3[2]={" GPT SECONDARY SET",
                              " 100  110  120  190"};
                              //12345678901234567890
const char* pt_wiring_set1[2]={"   PT WIRING SET",
                               "    3P3W   3P4W"};

											 //12345678901234567890
const char* di_set1[2]={" DI #1 DEBOUNCE TIME",
												" Tde1:       ms"};
											 //12345678901234567890
const char* di_set2[2]={" DI #2 DEBOUNCE TIME",
												" Tde2:       ms"};
											 //12345678901234567890
const char* di_set3[2]={" DI #3 DEBOUNCE TIME",
												" Tde3:       ms"};
											 //12345678901234567890
const char* di_set4[2]={" DI #4 DEBOUNCE TIME",
												" Tde4:       ms"};
											 //12345678901234567890
const char* di_set5[2]={" DI #5 DEBOUNCE TIME",
												" Tde5:       ms"};
											 //12345678901234567890
const char* di_set6[2]={" DI #6 DEBOUNCE TIME",
												" Tde6:       ms"};
											 //12345678901234567890
const char* di_set7[2]={" DI #7 DEBOUNCE TIME",
												" Tde7:       ms"};
											 //12345678901234567890
const char* di_set8[2]={" DI #8 DEBOUNCE TIME",
												" Tde8:       ms"};

                            //12345678901234567890
const char* di_set_view1[2]={" Tde1:       ms",
                             " Tde2:       ms"};
                            //12345678901234567890
const char* di_set_view2[2]={" Tde3:       ms",
                             " Tde4:       ms"};
                            //12345678901234567890
const char* di_set_view3[2]={" Tde5:       ms",
                             " Tde6:       ms"};
                            //12345678901234567890
const char* di_set_view4[2]={" Tde7:       ms",
                             " Tde8:       ms"};

                               //12345678901234567890
const char* password_input1[2]={"   PASSWORD INPUT",
                                ""};
                               //12345678901234567890
const char* password_input2[2]={" PASSWORD INPUT FAIL",
                                "   PRESS ANY KEY"};
                              //12345678901234567890
const char* password_set1[2]={ "   PASSWORD SET",
                               ""};

                        //12345678901234567890
const char* gr_set1[2]={ "   OCGR/DGR SELECT",
                         "    OCGR   DGR"};
                        //12345678901234567890
const char* zct_set1[2]={"    ZCT ANGLE SET",
                         ""};
                         //12345678901234567890
const char* zct_view1[2]={"    ZCT ANGLE VIEW",
                          ""};

														//12345678901234567890
const char* gr_set_view1[2]={"OCGR/DGR SELECT VIEW",
                             ""};

														//12345678901234567890
const char* ocr_pro_set1[2]={"    OCR MODE SET",
                             "    TURN   NORMAL"};
														//12345678901234567890
const char* ocr_pro_set2[2]={"   OCR TURN DI SET",
                             "      DI :"};
                             //12345678901234567890
const char* ocr_pro_view1[2]={"    OCR MODE VIEW",
                              "        NORMAL"};
                             //12345678901234567890
const char* ocr_pro_view2[2]={"    OCR MODE VIEW",
                              "       TURN/DI"};

											 //12345678901234567890
const char* lc_set1[2]={"  CB LOCAL CONTROL",
												"    USE    NOT"};
											 //12345678901234567890
const char* lc_set2[2]={"    CB OPEN TIME",
												"  To :       s"};
											 //12345678901234567890
const char* lc_set3[2]={"    CB CLOSE TIME",
												"  Tc :       s"};

												//12345678901234567890
const char* lc_view1[2]={"  CB LOCAL CONTROL",
												 ""};
           
                           //12345678901234567890
const char* data_reset1[2]={"  1.EVENT  CLEAR",
                            "  2.ENERGY CLEAR"};
                           //12345678901234567890
const char* data_reset2[2]={"  3.Vo MAX CLEAR",
                            "  4.Io MAX CLEAR"};
                           //12345678901234567890
const char* data_reset3[2]={"  5.RUN TIME CLEAR",
                            ""};

                    //12345678901234567890
const char* etc1[2]={"  1.WAVE CAPTURE",
                     "  2.DO TEST"};
                    //12345678901234567890
const char* etc2[2]={"  3.KEY/LED TEST",
                     ""};
                    //12345678901234567890
const char* etc3[2]={"  3.KEY/LED TEST",
                     "  4.THERMAL Q RESET"};

                     //12345678901234567890
const char* wave1[2]={"    WAVE CAPTURE ?",
                      "      YES   NO"};
                     //12345678901234567890
const char* wave2[2]={"   WAVE CAPTURING",
                      "   WAIT  A SECOND"};
                     //12345678901234567890
const char* wave3[2]={" WAVE CAPTURE FINISH",
                      "   PRESS ANY KEY"};

                      //12345678901234567890
const char* resetq[2]={" WNARNING! Q CLEAR?",
                       "      YES   NO"};

                      //12345678901234567890
const char* reset1[2]={"        DATA CLEAR?",
                       "      YES   NO"};
                      //12345678901234567890
const char* reset2[2]={" DATA CLEAR COMPLETE",
                       "    PRESS ANY KEY"};
                      //12345678901234567890
const char* reset3[2]={" DATA CLEAR CANCELED",
                       "    PRESS ANY KEY"};

                     //12345678901234567890
const char* save1[2]={"      DATA SET?",
                      "      YES   NO"};

                            //12345678901234567890
const char* save_cancel1[2]={"      SET CANCELED",
                             "    PRESS ANY KEY"};

                             //12345678901234567890
const char* save_confirm1[2]={"      SET COMPLETE",
                              "    PRESS ANY KEY"};

													//12345678901234567890
const char* save_fail1[2]={"      SET FAIL !",
													 "    PRESS ANY KEY"};
												//12345678901234567890
const char* save_no1[2]={"      SET NO CHANGE",
												 "    PRESS ANY KEY"};

														//12345678901234567890
const char* view_format1[3]={"  %1.0f ",
                             "  %1.1f ",
                             "  %1.2f "};
														 //12345678901234567890
const char* view_format10[3]={" %2.0f ",
                              " %2.1f ",
                              " %2.2f "};
															//12345678901234567890
const char* view_format100[3]={"%3.0f ",
                               "%3.1f ",
                               "%3.2f "};

														 //12345678901234567890
const char* view_format1k[3]={"  %1.0f ",
                              "  %1.1fk",
                              "  %1.2fk"};
															//12345678901234567890
const char* view_format10k[3]={" %2.0f ",
                               " %2.1fk",
                               " %2.2fk"};
															 //12345678901234567890
const char* view_format100k[3]={"%3.0f ",
                                "%3.1fk",
                                "%3.2fk"};

														 //12345678901234567890
const char* view_format1m[3]={"  %1.0f ",
                              "  %1.1fM",
                              "  %1.2fM"};
															//12345678901234567890
const char* view_format10m[3]={" %2.0f ",
                               " %2.1fM",
                               " %2.2fM"};
															 //12345678901234567890
const char* view_format100m[3]={"%3.0f ",
                                "%3.1fM",
                                "%3.2fM"};

														 //12345678901234567890
const char* view_format1g[3]={"  %1.0f ",
                              "  %1.1fG",
                              "  %1.2fG"};
															//12345678901234567890
const char* view_format10g[3]={" %2.0f ",
                               " %2.1fG",
                               " %2.2fG"};
															 //12345678901234567890
const char* view_format100g[3]={"%3.0f ",
                                "%3.1fG",
                                "%3.2fG"};

															//12345678901234567890
const char* p_view_format1[2]={"   %1.2f  ",
                               "  -%1.2f  "};
															 //12345678901234567890
const char* p_view_format10[2]={"  %2.2f  ",
                                " -%2.2f  "};
																//12345678901234567890
const char* p_view_format100[2]={" %3.2f  ",
                                 "-%3.2f  "};
															 //12345678901234567890
const char* p_view_format1k[2]={"   %1.2f k",
                                "  -%1.2f k"};
																//12345678901234567890
const char* p_view_format10k[2]={"  %2.2f k",
                                 " -%2.2f k"};
																 //12345678901234567890
const char* p_view_format100k[2]={" %3.2f k",
                                  "-%3.2f k"};
															 //12345678901234567890
const char* p_view_format1m[2]={"   %1.2f M",
                                "  -%1.2f M"};
																//12345678901234567890
const char* p_view_format10m[2]={"  %2.2f M",
                                 " -%2.2f M"};
																 //12345678901234567890
const char* p_view_format100m[2]={" %3.2f M",
                                  "-%3.2f M"};
															 //12345678901234567890
const char* p_view_format1g[2]={"   %1.2f G",
                                "  -%1.2f G"};
																//12345678901234567890
const char* p_view_format10g[2]={"  %2.2f G",
                                 " -%2.2f G"};
																 //12345678901234567890
const char* p_view_format100g[2]={" %3.2f G",
                                  "-%3.2f G"};

const char* set_integer[6]={ "     %1.0f",
                             "    %2.0f",
                             "   %3.0f",
                             "  %4.0f",
                             " %5.0f",
                             "%6.0f"};

const char* six_point_2[6]={ "     %1.2f",
                             "    %2.2f",
                             "   %3.2f",
                             "  %4.2f",
                             " %5.2f",
                             "%6.2f"};

                      
                      //12345678901234567890
const char* eventx[2]={"   ] I DON'T KNOW  ",
                       "                    "};

                      //12345678901234567890
const char* event1[2]={"   ] RELAY PICKUP   ",
                       "                    "};
                      //12345678901234567890
const char* event2[2]={"   ] RELAY OPERATION",
                       "                    "};
                      //12345678901234567890
const char* event3[2]={"   ] RELAY SETTING  ",
                       "                    "};
                      //12345678901234567890
const char* event4[2]={"   ] SYSTEM SETTING ",
                       "                    "};
                      //12345678901234567890
const char* event5[2]={"   ] DATA CLEAR     ",
                       "                    "};
                      //12345678901234567890
const char* event6[2]={"   ] DIGITAL INPUT  ",
                       "                    "};
                      //12345678901234567890
//const char* event7[2]={"   ] DI CLOSE       ",
//                       "                    "};
                      //12345678901234567890
const char* event8[2]={"   ] DO OPEN        ",
                       "                    "};
                      //12345678901234567890
const char* event9[2]={"   ] DO CLOSE       ",
                       "                    "};
                       //12345678901234567890
const char* event10[2]={"   ] SYSTEM REBOOT  ",
                        "                    "};
                       //12345678901234567890
const char* event11[2]={"   ] MODE CHANGE",
                        "                    "};
                       //12345678901234567890
const char* event12[2]={"   ] CONTROL",
                        "                    "};

                       //12345678901234567890
const char* event_t[2]={"   ] 20             ",
                        "                    "};
                       //12345678901234567890
const char* event_d[2]={"   ] RELAY OPERATION",
                        "RATIO:              "};

const char* event_phase[9]={ "",
	                         "A ",
	                         "B ",
	                         "AB",
	                         "C ",
	                         "AC",
	                         "BC",
	                         "ABC",
	                         "N"};

const char* event_io[10]={ "1 ",
	                         "2 ",
	                         "3 ",
	                         "4 ",
	                         "5 ",
	                         "6 ",
	                         "7 ",
	                         "8 ",
	                         "9 ",
	                         "10"};

const char* event_relay[27]=   {"50-1",
                                "50-2",
                                "51-1",
                                "51-2",
                                "50G",
                                "51G",
                                "67N",
                                "67G",
                                "59",
                                "64",
                                "27-1",
                                "27-2",
                                "27-3",
                                "47P",
                                "47N",
                                "25",
                                "50",
                                "37",
																"46",
																"27",
																"49",
																"50H",
																"S/L",
																"66",
																"51LR",
																"48",
																""};

const char* event_set[13]={ "CT/PT",
                            "DI DEBOUNCE",
                            "DO MODE",
                            "SUPERVISION",
                            "PASSWORD",
                            "TIME",
                            "COMM.",
                            "LOCAL CONTROL",
                            "GR SELECT",
                            "ZCT ANGLE",
                            "OCR MODE",
                            "CAPTURE",
                            "DO TEST"};

const char* event_reset[5]={  "EVENT",
                              "ENERGY",
                              "Vo MAX",
                              "Io MAX",
                              "CB CLOSE TIME"};

const char* event_mode_change[7]={  "ACK KEY",
                                    "LOCAL->REMOTE",
                                    "REMOTE->LOCAL",
                                    "TCS FAIL->GOOD",
                                    "CCS FAIL->GOOD",
                                    "TCS GOOD->FAIL",
                                    "CCS GOOD->FAIL"
                                    };

const char* event_control[4]={  "LOCAL CB OPEN",
                                "LOCAL CB CLOSE",
                                "REMOTE CB OPEN",
                                "REMOTE CB CLOSE"
                                };

const char* event_test[1]={ "DO RELAY"};

                            //12345678901234567890
const char* relay_popup1[2]={"",
                             "R:        T:"};
                            //12345678901234567890
const char* relay_popup2[2]={"      A",
                             "R:        T:"};
                            //12345678901234567890
const char* relay_popup3[2]={"      B",
                             "R:        T:"};
                            //12345678901234567890
const char* relay_popup4[2]={"      AB",
                             "R:        T:"};
                            //12345678901234567890
const char* relay_popup5[2]={"      C",
                             "R:        T:"};
                            //12345678901234567890
const char* relay_popup6[2]={"      AC",
                             "R:        T:"};
                            //12345678901234567890
const char* relay_popup7[2]={"      BC",
                             "R:        T:"};
                            //12345678901234567890
const char* relay_popup8[2]={"      ABC",
                             "R:        T:"};
                            //12345678901234567890
const char* relay_popup9[2]={"      N",
                             "R:        T:"};

                           //12345678901234567890
const char* relay_test1[2]={"  REALLY DO TEST ?",
                            "      YES   NO"};
                           //12345678901234567890
const char* relay_test2[2]={"    DO TEST SELECT",
                            "      AUTO  MANUAL"};
                           //12345678901234567890
const char* relay_test3[2]={"   DO AUTO TEST ?",
                            "      YES   NO"};
                           //12345678901234567890
const char* relay_test4[2]={" 1 2 3 4 5 6 7 8 9 0",
                            ""};
                           //12345678901234567890
const char* relay_test5[2]={"  DO TEST COMPLETE",
                            "   PRESS ANY KEY"};

                         //12345678901234567890
const char* led_test1[2]={"     KEY/LED TEST",
                          "     KEY : "};

                     //12345678901234567890
const char* test2[2]={"  TEST COMPLETE !",
                      "  PRESS ANY KEY !"};
                     //12345678901234567890
const char* test3[2]={"  TEST CANCELED !",
                      "  PRESS ANY KEY !"};
                     //12345678901234567890
const char* test4[2]={"  PRESS ENTER KEY",
                      "   TO RESET !"};

                       //12345678901234567890
const char* hidden1[2]={"  1.RATED VALUE SET",
                        "  2.C/PT CALIBRATION"};

                         //12345678901234567890
const char* etc_view1[2]={"  1.SYSTEM TIME",
                          "  2.RUN    TIME"};
                         //12345678901234567890
const char* etc_view2[2]={"  3.VERSION",
                          "  4.DIAGNOSTIC"};

                        //12345678901234567890
const char* sys_time[2]={"    SYSTEM TIME",
                         ""};

                        //12345678901234567890
const char* run_time[2]={"      RUN TIME",
                         "  Tr :          HOUR"};

                         //12345678901234567890
const char* time_set1[2]={"      YEAR SET",
                          " YEAR:"};
                         //12345678901234567890
const char* time_set2[2]={"     MONTH SET",
                          "MONTH:"};
                         //12345678901234567890
const char* time_set3[2]={"       DAY SET",
                          " DAY :"};
                         //12345678901234567890
const char* time_set4[2]={"      HOUR SET",
                          " HOUR:"};
                         //12345678901234567890
const char* time_set5[2]={"    MINUTE SET",
                          " MIN.:"};
                         //12345678901234567890
const char* time_set6[2]={"    SECOND SET",
                          " SEC.:"};
                         //12345678901234567890
const char* time_set7[2]={"      TIME SET",
                          ""};
                         //12345678901234567890
const char* time_set8[2]={"      TIME SET?",
                          "      YES   NO"};

                     //12345678901234567890
const char* diag1[2]={"CALIBRATION -",
                      "FLASH ROM   -"};
                     //12345678901234567890
const char* diag2[2]={"SRAM        -",
                      "FRAM        -"};
                     //12345678901234567890
const char* diag3[2]={"A/D CONV.   -",
                      ""};

                         //12345678901234567890
const char* east_egg1[2]={"DSP, MMI - KIM JOONG PYO",
                          "COMM. - KIM HYUN SEONG"};
                         //12345678901234567890
const char* east_egg2[2]={"H/W - KIM YOON HEO",
                          "COMM. - KIM HYUN SEONG"};

                         //12345678901234567890
const char* east_egg3[2]={"SPECIAL THANKS",
                          "LEE BYUNG HO"};

                         //12345678901234567890
const char* Trip_Message[14][2]={
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "},
													{" 50-1 SHORT CIRCUIT ",
                           " [Fault Phase: Ia ] "}
                         };
