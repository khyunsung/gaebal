/*
 * menu.c
 *
 *  Created on: 2015. 1. 27.
 *      Author: hhi
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	//memset, ...

#include "define.h"
#include "extern_variable.h"
#include "extern_prototype.h"
#include "display.h"

typedef char	int8;
typedef unsigned char	uint8;
typedef unsigned short uint16;
typedef unsigned long	uint32;

typedef void (*Screen_Function_Pointer)(unsigned int, int);

enum {
	NORMAL_MENU,
	REALTIME_MENU,
	HIDDEN_MENU,
	POPUP_MENU
};

typedef struct screen_move_info {
	uint16 x;
	uint16 y;
	uint16 select;
	uint16 data_change;
} Screen_Position_Info;
Screen_Position_Info Screen_Position, Cursor_Position, Save_Current_Screen;

int Button_Press;
unsigned short Set_Password = 0xffff;
unsigned int KHS_Key_Press;
unsigned int Data_Change = 1;
unsigned long Data_Change_Count = 0;

void cursor_move(unsigned int y, unsigned int x)
{
	VFD_cursor(y, x);
	Cursor_Position.y = y;
	Cursor_Position.x = x;
}

void cursor_move_last(void)
{
	VFD_cursor(Cursor_Position.y, Cursor_Position.x);
}


void screen_frame(char *str[2])
{
	VFD_Single_Line_dump(LCD_L1_00, str[0]);
	VFD_Single_Line_dump(LCD_L2_00, str[1]);

	cursor_move_last();
}

void screen_frame2(char str[2][22])
{
	VFD_Single_Line_dump(LCD_L1_00, str[0]);
	VFD_Single_Line_dump(LCD_L2_00, str[1]);

	cursor_move_last();
}

void screen_frame3(const char *str[2])
{
	VFD_Single_Line_dump(LCD_L1_00, str[0]);
	VFD_Single_Line_dump(LCD_L2_00, str[1]);

	cursor_move_last();
}

void menu_00_00(unsigned int value, int display)
{
	char string[22];

	if(display == 1) {
		sprintf(string, "    [ HIMAP-FI ]    \0");
		VFD_Single_Line_dump(LCD_L1_00, string);
		sprintf(string, " %02d:%02d:%02d     NO.%d%d \0", TIME.hour, TIME.minute, TIME.second, ADDRESS.address/10, ADDRESS.address%10);
		VFD_Single_Line_dump(LCD_L2_00, string);

		Screen_Position.select = 0; //init.
		Screen_Position.data_change = REALTIME_MENU;
		cursor_move(0, 0);
		return;
	} else if(display == 2) {
		sprintf(string, " %02d:%02d:%02d     NO.%d%d \0", TIME.hour, TIME.minute, TIME.second, ADDRESS.address/10, ADDRESS.address%10);
		VFD_Single_Line_dump(LCD_L2_00, string);
		return;
	}

	if(value == DISPLAY_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 1;
		Screen_Position.data_change = NORMAL_MENU;
	} else if(value == SET_KEY) {
		Screen_Position.y = 42;
		Screen_Position.x = 1;
		Screen_Position.select = 3;
		Screen_Position.data_change = NORMAL_MENU;
		Set_Password = 0x8000 + 1111;
	}
}

void menu_00_01(unsigned int value, int display)
{
	const char *str[2] = {
			"    < DISPLAY >    \1\0",
			"[METER]?  [SET  ]? \2\0" };
	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 7);
		} else {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 0;
		cursor_move(0, 0);//cursor off
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 1;
	}	else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 0;
			Screen_Position.x = 2;
		} else {
//			Screen_Position.y = 4;
//			Screen_Position.x = 2;
		}
	}
}

void menu_00_02(unsigned int value, int display)
{
	const char *str[2] = {
			"[Volt] ?  [Curr] ? \1\0",
			"[Pwr ] ?  [D/I ] ? \2\0" };
	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 7);
		} else if (Screen_Position.select == 1) {
			cursor_move(0, 17);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 7);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 4;
	} else if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 1;
		cursor_move(0, 0);
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 0;
			Screen_Position.x = 3;
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 1;
			Screen_Position.x = 3;
		} else if(Screen_Position.select == 2) {
			Screen_Position.y = 2;
			Screen_Position.x = 3;
		} else if(Screen_Position.select == 3) {
			Screen_Position.y = 3;
			Screen_Position.x = 3;
		}
		Screen_Position.select = 0;
		Screen_Position.data_change = REALTIME_MENU;
		cursor_move(0, 0);
	}
}

void menu_00_03(unsigned int value, int display)
{
	char string[22];

	const char *str[2] = {
			" Vab]         [V]  \1\0",
			" Vbc]         [V]  \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%7ld\0", (unsigned long)(DISPLAY.rms_value[Va]));
		VFD_Single_Line_dump(LCD_L1_06, string);
		sprintf(string, "%7ld\0", (unsigned long)(DISPLAY.rms_value[Vb]));
		VFD_Single_Line_dump(LCD_L2_06, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 4;
	}
}

void menu_00_04(unsigned int value, int display)
{
	char string[22];

	const char *str[2] = {
			" Vca]         [V]  \1\0",
			" Vo ]         [V]  \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%7ld\0", (unsigned long)(DISPLAY.rms_value[Vc]));
		VFD_Single_Line_dump(LCD_L1_06, string);
		sprintf(string, "%7ld\0", (unsigned long)(DISPLAY.rms_value[Vn]));
		VFD_Single_Line_dump(LCD_L2_06, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 5;
	}
}

void menu_00_05(unsigned int value, int display)
{
	char string[22];

	const char *str[2] = {
			" Vom]         [V]  \1\0",
			" Vavg]        [V]  \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%7ld\0", (unsigned long)(ACCUMULATION.vo_max));
		VFD_Single_Line_dump(LCD_L1_06, string);
		sprintf(string, "%7ld\0", (unsigned long)(DISPLAY.rms_Vavg_value));
		VFD_Single_Line_dump(LCD_L2_06, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 6;
	}
}

void menu_00_06(unsigned int value, int display)
{
	char string[22];

	const char *str[2] = {
			" Vps]         [V]  \1\0",
			" Vns]         [V]  \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%7ld\0", (unsigned long)(DISPLAY.V1_value));
		VFD_Single_Line_dump(LCD_L1_06, string);
		sprintf(string, "%7ld\0", (unsigned long)(DISPLAY.V2_value));
		VFD_Single_Line_dump(LCD_L2_06, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 7;
	}
}

void menu_00_07(unsigned int value, int display)
{
	char string[22];

	const char *str[2] = {
			"\5Vab]              \1\0",
			"\5Vbc]              \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%7.1f %c\0", DISPLAY.anlge[0], DGREE);
		VFD_Single_Line_dump(LCD_L1_06, string);
		sprintf(string, "%7.1f %c\0", DISPLAY.anlge[1], DGREE);
		VFD_Single_Line_dump(LCD_L2_06, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 8;
	}
}

void menu_00_08(unsigned int value, int display)
{
	char string[22];

	const char *str[2] = {
			"\5Vca]              \1\0",
			"\5Vo ]              \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%7.1f %c\0", DISPLAY.anlge[2], DGREE);
		VFD_Single_Line_dump(LCD_L1_06, string);
		sprintf(string, "%7.1f %c\0", DISPLAY.anlge[3], DGREE);
		VFD_Single_Line_dump(LCD_L2_06, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 3;
	}
}

void menu_01_01(unsigned int value, int display)
{
	const char *str[2] = {
			"    < DISPLAY >    \1\0",
			"[EVENT]?  [TCS  ]? \2\0" };
	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 7);
		} else {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 0;
		cursor_move(0, 0);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 2;
		Screen_Position.x = 1;
	}	else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) { //EVENT
			Screen_Position.y = 29;//khs, 2015-03-31 오후 4:09:03
			Screen_Position.x = 2;
			cursor_move(0, 0);//cursor off
			EVENT.view_point = EVENT.view_start;//khs, 2015-04-03 오후 5:07:27
		} else { //TCS
//			Screen_Position.y = 46;
//			Screen_Position.x = 3;
		}
	}
}

void menu_01_03(unsigned int value, int display)
{
	char string[22];

	const char *str[2] = {
			" Ia]          [A]  \1\0",
			" Ib]          [A]  \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%7.1f\0", DISPLAY.rms_value[Ia]);
		VFD_Single_Line_dump(LCD_L1_06, string);
		sprintf(string, "%7.1f\0", DISPLAY.rms_value[Ib]);
		VFD_Single_Line_dump(LCD_L2_06, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 4;
	}
}

void menu_01_04(unsigned int value, int display)
{
	char string[22];

	if(display == 1) {
		sprintf(string, " Ic]          [A]  \1\0");
		VFD_Single_Line_dump(LCD_L1_00, string);
		if(CORE.gr_select == NCT_SELECT)
		{
			sprintf(string, " Io]          [A]  \2\0");
			VFD_Single_Line_dump(LCD_L2_00, string);
		}
		else
		{
			sprintf(string, " Io]          [mA] \2\0");
			VFD_Single_Line_dump(LCD_L2_00, string);
		}
		return;
	} else if(display == 2) {
		sprintf(string, "%7.1f\0", DISPLAY.rms_value[Ic]);
		VFD_Single_Line_dump(LCD_L1_06, string);
		if(CORE.gr_select == NCT_SELECT)
		{
			sprintf(string, "%7.1f\0", DISPLAY.rms_value[In]);
			VFD_Single_Line_dump(LCD_L2_06, string);
		}
		else
		{
			sprintf(string, "%7.1f\0", DISPLAY.rms_value[Is]);
			VFD_Single_Line_dump(LCD_L2_06, string);
		}		
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 5;
	}
}

void menu_01_05(unsigned int value, int display)
{
	char string[22];

	const char *str[2] = {
			" Iavg]        [A]  \1\0",
			"                   \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%7.1f\0", DISPLAY.rms_Iavg_value);
		VFD_Single_Line_dump(LCD_L1_06, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU; //2015.02.10
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 6;
	}
}

void menu_01_06(unsigned int value, int display)
{
	char string[22];

	const char *str[2] = {
			" Ips]         [A]  \1\0",
			" Ins]         [A]  \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%7.1f\0", DISPLAY.I1_value);
		VFD_Single_Line_dump(LCD_L1_06, string);
		sprintf(string, "%7.1f\0", DISPLAY.I2_value);
		VFD_Single_Line_dump(LCD_L2_06, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 7;
	}
}

void menu_01_07(unsigned int value, int display)
{
	float ftest_value1 = 1.2;//temporary test valu
	float ftest_value2 = 2.3;//temporary test valu
	float ftest_value3 = 3.4;//temporary test valu
	char string[22];

	const char *str[2] = {
			"    3rd   5th   7th~\0",
			"Ia  0.0   0.0   0.0 \0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%3.1f\0", ftest_value1);//1
		VFD_Single_Line_dump(LCD_L2_04, string);
		sprintf(string, "%3.1f\0", ftest_value2);//2
		VFD_Single_Line_dump(LCD_L2_10, string);
		sprintf(string, "%3.1f\0", ftest_value3);//3
		VFD_Single_Line_dump(LCD_L2_16, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 9;
	} else if(value == RIGHT_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 8;
	}
}

void menu_01_08(unsigned int value, int display)
{
	float ftest_value1 = 1.2;//temporary test valu
	float ftest_value2 = 2.3;//temporary test valu
	unsigned int test_value1 = 4;//temporary test 
	unsigned int test_value2 = 5;//temporary test 
	char string[22];

	const char *str[2] = {
			"   9th 11th THD TDD",
			"Ia  0.0  0.0   0   0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%3.1f\0", ftest_value1);//1
		VFD_Single_Line_dump(LCD_L2_04, string);
		sprintf(string, "%3.1f\0", ftest_value2);//2
		VFD_Single_Line_dump(LCD_L2_09, string);
		sprintf(string, "%3d\0", test_value1);//3
		VFD_Single_Line_dump(LCD_L2_13, string);
		sprintf(string, "%3d\0", test_value2);//4
		VFD_Single_Line_dump(LCD_L2_17, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 9;
	} else if(value == LEFT_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 7;
	}
}

void menu_01_09(unsigned int value, int display)
{
	float ftest_value1 = 1.2;//temporary test valu
	float ftest_value2 = 2.3;//temporary test valu
	float ftest_value3 = 3.4;//temporary test valu
	char string[22];

	const char *str[2] = {
			"    3rd   5th   7th~\0",
			"Ib  0.0   0.0   0.0 \0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%3.1f\0", ftest_value1);
		VFD_Single_Line_dump(LCD_L2_04, string);
		sprintf(string, "%3.1f\0", ftest_value2);
		VFD_Single_Line_dump(LCD_L2_10, string);
		sprintf(string, "%3.1f\0", ftest_value3);
		VFD_Single_Line_dump(LCD_L2_16, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 11;
	} else if(value == RIGHT_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 10;
	}
}

void menu_01_10(unsigned int value, int display)
{
	float ftest_value1 = 4.5;//temporary test valu
	float ftest_value2 = 6.7;//temporary test valu
	unsigned int test_value1 = 8;//temporary test 
	unsigned int test_value2 = 9;//temporary test 
	char string[22];

	const char *str[2] = {
			"   9th 11th THD TDD",
			"Ib  0.0  0.0   0   0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%3.1f\0", ftest_value1);
		VFD_Single_Line_dump(LCD_L2_04, string);
		sprintf(string, "%3.1f\0", ftest_value2);
		VFD_Single_Line_dump(LCD_L2_09, string);
		sprintf(string, "%3d\0", test_value1);
		VFD_Single_Line_dump(LCD_L2_13, string);
		sprintf(string, "%3d\0", test_value2);
		VFD_Single_Line_dump(LCD_L2_17, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 11;
	} else if(value == LEFT_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 9;
	}
}

void menu_01_11(unsigned int value, int display)
{
	float ftest_value1 = 4.5;//temporary test value
	float ftest_value2 = 5.6;//temporary test value
	float ftest_value3 = 6.7;//temporary test value
	char string[22];

	const char *str[2] = {
			"    3rd   5th   7th~\0",
			"Ic  0.0   0.0   0.0 \0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%3.1f\0", ftest_value1);
		VFD_Single_Line_dump(LCD_L2_04, string);
		sprintf(string, "%3.1f\0", ftest_value2);
		VFD_Single_Line_dump(LCD_L2_10, string);
		sprintf(string, "%3.1f\0", ftest_value3);
		VFD_Single_Line_dump(LCD_L2_16, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 13;
	} else if(value == RIGHT_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 12;
	}	
}

void menu_01_12(unsigned int value, int display)
{
	float ftest_value1 = 5.6;//temporary test valu
	float ftest_value2 = 7.8;//temporary test valu
	unsigned int test_value1 =101;//temporary test
	unsigned int test_value2 =20;//temporary test 
	char string[22];

	const char *str[2] = {
			"   9th 11th THD TDD",
			"Ic  0.0  0.0   0   0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%3.1f\0", ftest_value1);
		VFD_Single_Line_dump(LCD_L2_04, string);
		sprintf(string, "%3.1f\0", ftest_value2);
		VFD_Single_Line_dump(LCD_L2_09, string);
		sprintf(string, "%3d\0", test_value1);
		VFD_Single_Line_dump(LCD_L2_13, string);
		sprintf(string, "%3d\0", test_value2);
		VFD_Single_Line_dump(LCD_L2_17, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 13;
	} else if(value == LEFT_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 11;
	}	
}

void menu_01_13(unsigned int value, int display)
{
	char string[22];

	const char *str[2] = {
			"\5Ia]               \1\0",
			"\5Ib]               \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%7.1f %c\0", DISPLAY.anlge[4], DGREE);
		VFD_Single_Line_dump(LCD_L1_06, string);
		sprintf(string, "%7.1f %c\0", DISPLAY.anlge[5], DGREE);
		VFD_Single_Line_dump(LCD_L2_06, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 14;
	}
}

void menu_01_14(unsigned int value, int display)
{
	char string[22];

	const char *str[2] = {
			"\5Ic]               \1\0",
			"\5Io]               \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%7.1f %c\0", DISPLAY.anlge[6], DGREE);
		VFD_Single_Line_dump(LCD_L1_06, string);
		sprintf(string, "%7.1f %c\0", DISPLAY.anlge[7], DGREE);
		VFD_Single_Line_dump(LCD_L2_06, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 3;
	}
}

void menu_02_01(unsigned int value, int display)
{
	const char *str[2] = {
			"    < DISPLAY >    \1\0",
			"[R-HOUR]?          \2\0" };
	if(display) {
		screen_frame3(str);
		cursor_move(1, 8);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 0;
		cursor_move(0, 0);//cursor off
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 1;
	}	else if(value == ENTER_KEY) {
			Screen_Position.y = 30;
			Screen_Position.x = 2;
			cursor_move(0, 0);//cursor off
	}
}

void menu_02_03(unsigned int value, int display)
{
	float ftest_value1 = 123456.7;//temporary test
	float ftest_value2 = 568.9;//temporary test va
	char string[22];

	const char *str[2] = {
			" kW  ]        0.0  \1\0",
			" kVar]        0.0  \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%8.1f\0", ftest_value1);
		VFD_Single_Line_dump(LCD_L1_09, string);
		sprintf(string, "%8.1f\0", ftest_value2);
		VFD_Single_Line_dump(LCD_L2_09, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 2;
		Screen_Position.x = 4;
	}
}

void menu_02_04(unsigned int value, int display)
{
	float ftest_value1 = 123.45;//temporary test value
	float ftest_value2 = 12.456;//temporary test value
	char sign_c = '-'; //temporary test value
	int	powerfactor=0, frequency=0; //test
	char string[22];

	const char *str[2] = {
			" PF ] +  0.00 [%]  \1\0",
			" Fre]  ------ [Hz] \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		if(powerfactor)
		{
			sprintf(string, "%c%6.2f\0",sign_c,ftest_value1);
			VFD_Single_Line_dump(LCD_L1_06, string);
		}
		else
		{
			sprintf(string, " ----  \0");
			VFD_Single_Line_dump(LCD_L1_06, string);	
		}
		if(frequency)
		{
			sprintf(string, "%6.3f\0", ftest_value2);
			VFD_Single_Line_dump(LCD_L2_07, string);
		}
		else
		{
			sprintf(string, "------\0");
			VFD_Single_Line_dump(LCD_L2_07, string);	
		}
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 2;
		Screen_Position.x = 5;
	}
}

void menu_02_05(unsigned int value, int display)
{
	long test_value1 = 12345678;//temporary test v
	long test_value2 = 9;//temporary test value
	char string[22];

	const char *str[2] = {
			" kWh  ]         0  \1\0",
			" kVarh]         0  \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%8ld\0", test_value1);
		VFD_Single_Line_dump(LCD_L1_09, string);
		sprintf(string, "%8ld\0", test_value2);
		VFD_Single_Line_dump(LCD_L2_09, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 2;
		Screen_Position.x = 6;
	}
}

void menu_02_06(unsigned int value, int display)
{
	long test_value1 = 12345678;//temporary test value
	long test_value2 = 9;//temporary test value
	char string[22];

	const char *str[2] = {
			" RkWh  ]        0  \1\0",
			" RkVarh]        0  \2\0" };

	if(display == 1) {
		screen_frame3(str);
		return;
	} else if(display == 2) {
		sprintf(string, "%8ld\0", test_value1);
		VFD_Single_Line_dump(LCD_L1_09, string);
		sprintf(string, "%8ld\0", test_value2);
		VFD_Single_Line_dump(LCD_L2_09, string);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 2;
		Screen_Position.x = 3;
	}
}

void menu_03_03(unsigned int value, int display)
{
	char str[2][22];

	if(display == 1) {
		sprintf(str[0], "DI 1 2 3 4 5 6 7 8 \1\0");
		sprintf(str[1], "ST                  \0");
		screen_frame2(str);
		return;
	} else if(display == 2) {
		sprintf(str[1], "%c %c %c %c %c %c %c %c  \0",
			(DIGITAL_INPUT.di_status & 0x01)? FULLDOT: ' ',	//DI 01
			(DIGITAL_INPUT.di_status & 0x02)? FULLDOT: ' ',	//DI 02
			(DIGITAL_INPUT.di_status & 0x04)? FULLDOT: ' ',	//DI 03
			(DIGITAL_INPUT.di_status & 0x08)? FULLDOT: ' ',	//DI 04
			(DIGITAL_INPUT.di_status & 0x10)? FULLDOT: ' ',	//DI 05
			(DIGITAL_INPUT.di_status & 0x20)? FULLDOT: ' ',	//DI 06
			(DIGITAL_INPUT.di_status & 0x40)? FULLDOT: ' ',	//DI 07
			(DIGITAL_INPUT.di_status & 0x80)? FULLDOT: ' '); //DI 08
		VFD_Single_Line_dump(LCD_L2_03, str[1]);				
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 2;
		Screen_Position.data_change = NORMAL_MENU;
		cursor_move(0, 7);
	}
}

void menu_04_02(unsigned int value, int display)
{
	const char *str[2] = {
			"    < DISPLAY >    \1\0",
			" [RLYS] ?  [SYS] ?  " };
	if(display) {
		screen_frame3(str);
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select = 0;
		cursor_move(13, 40);
	} else if(value == RIGHT_KEY) {
		Screen_Position.select = 1;
		cursor_move(13, 50);
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 0;
			Screen_Position.x = 2;
		} else {
			Screen_Position.y = 1;
			Screen_Position.x = 2;
		}
	} else if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 1;
	}
}

void menu_04_03(unsigned int value, int display)
{
	const char *str[2] = {
			"<DISP #1> [50-1] ? \1\0",
			"[50-2] ?  [51-1] ? \2\0" };
	if(display==1) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 7);
		} else if(Screen_Position.select == 2) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select == 0)	{Screen_Position.select  = 2;}
		else														{Screen_Position.select -= 1;}
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 3;
	} else if(value == UP_KEY) {
		Screen_Position.y = 4;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 5;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 4;
			Screen_Position.x = 4;
			(OCR50_1.mode == INSTANT) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 43;
			Screen_Position.x = 4;
			(OCR50_2.mode == INSTANT) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
		} else if(Screen_Position.select == 2) {
			Screen_Position.y = 45;
			Screen_Position.x = 4;
			(OCR51_1.mode == INVERSE) ? (Screen_Position.select = 0) : (OCR51_1.mode == V_INVERSE) ? (Screen_Position.select = 1) : (Screen_Position.select = 2) ;
		}
	}
}

void menu_29_02(unsigned int value, int display)
{
	char str[22];
	
	if(display) {
		Event_Item_Display();
		
		if(EVENT.view_start > 0) {
			lcd_control_write(LCD_L1_19);
			delay_us(1000);
			lcd_character_write(UPARROW);
		}
		
		delay_us(1000);
		sprintf(str, "%3d\0", EVENT.view_start - EVENT.view_point + 1);
		VFD_Single_Line_dump(LCD_L1_00, str);
				
		return;
	}

	if(value == LEFT_KEY) {
		if(EVENT.rollover)
		{
			if(EVENT.view_point == 199)	{EVENT.view_point = 0;}
			else												{++EVENT.view_point;}
		}
		else
		{
			if(EVENT.view_point == EVENT.view_start)	{EVENT.view_point = 0;}
			else																			{++EVENT.view_point;}
		}
	} else if(value == RIGHT_KEY) {
		if(EVENT.rollover)
		{
			if(EVENT.view_point == 0)	{EVENT.view_point = 199;}
			else											{--EVENT.view_point;}
		}
		else
		{
			if(EVENT.view_point == 0)	{EVENT.view_point = EVENT.view_start;}
			else											{--EVENT.view_point;}
		}
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 29;
		Screen_Position.x = 3;
	} else if(value == UP_KEY) {
		Screen_Position.y = 1;
		Screen_Position.x = 1;
	}
}

void menu_29_03(unsigned int value, int display)
{
	if(display) {
		Event_Time_Display();
		return;
	}

	if(value) {
		Screen_Position.y = 29;
		Screen_Position.x = 2;
	}
}

void menu_41_02(unsigned int value, int display)
{
	const char *str[2] = {
			"    < SETTING >    \1\0",
			" [RLYS] ?  [SYS] ?  \0" };
	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 8);
		} else {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == UP_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 0;
		cursor_move(0, 0);//cursor off
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 41;
			Screen_Position.x = 3;
		} else {
			Screen_Position.y = 80;
			Screen_Position.x = 3;
		}
		Screen_Position.select = 0;
	}
}

void menu_41_03(unsigned int value, int display)
{
	const char *str[2] = {
			"<SET  #1> [50-1] ? \1\0",
			"[50-2] ?  [51-1] ? \2\0" };
	if(display==1) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 7);
		} else if(Screen_Position.select == 2) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select == 0)	{Screen_Position.select  = 2;}
		else														{Screen_Position.select -= 1;}
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 3;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		cursor_move(0, 0);//cursor off
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 42;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 41;
			Screen_Position.x = 4;
			(OCR50_1.mode == INSTANT) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 43;
			Screen_Position.x = 4;
			(OCR50_2.mode == INSTANT) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
		} else if(Screen_Position.select == 2) {
			Screen_Position.y = 45;
			Screen_Position.x = 4;
			(OCR51_1.mode == INVERSE) ? (Screen_Position.select = 0) : (OCR51_1.mode == V_INVERSE) ? (Screen_Position.select = 1) : (Screen_Position.select = 2) ;
		}
	}
}

void menu_41_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		if((OCR50_1.mode != INSTANT) && (OCR50_1.mode != DEFINITE))
		{
			OCR50_1.mode = DEFINITE;
		}
		sprintf(str[0],"50-1    MODE : %s \0", OCR50_1.mode == INSTANT? "INST" : "DEF ");  
		sprintf(str[1],"  INST?       DEF?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 6);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			OCR50_1.mode_temp = INSTANT;
			Screen_Position.y = 41;
			Screen_Position.x = 5;
		} else if(Screen_Position.select == 1) {
			OCR50_1.mode_temp = DEFINITE;
			Screen_Position.y = 42;
			Screen_Position.x = 5;
		}
		if(CORE.rated_ct == CT_5A)	{Screen_Position.select = 2;}
		else												{Screen_Position.select = 3;}
		OCR50_1.current_set_temp = OCR50_1.current_set;
	}
}

void menu_41_05(unsigned int value, int display)
{
//const unsigned int number_digit[3] = {100, 10, 1}; //2015.4.27
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		const unsigned int number_digit[3] = {100, 10, 1}; //2015.4.27
		sprintf(str[0],"PRE-SET  : %5.1f [A]\0",(float)(OCR50_1.current_set*0.1));
		sprintf(str[1],"CURRENT  : %5.1f [A]\0",(float)(OCR50_1.current_set_temp*0.1));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCR50_1.current_set_temp += number_digit[Screen_Position.select % 3];
			if(OCR50_1.current_set_temp >= OCR50_I_MAX[0]) OCR50_1.current_set_temp = OCR50_I_MAX[0];
		} else if(value == DOWN_KEY) {
			OCR50_1.current_set_temp -= number_digit[Screen_Position.select % 3];
			if(OCR50_1.current_set_temp <= OCR50_I_MIN[0] || OCR50_1.current_set_temp >= 60000)	OCR50_1.current_set_temp = OCR50_I_MIN[0];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 41;
				Screen_Position.x = 6;
				Screen_Position.select = 0;
				OCR50_1.do_relay_temp = OCR50_1.do_relay;
		}
	} else { //1A
//2015.4.27
		const unsigned int number_digit[4] = {1000, 100, 10, 1};
		sprintf(str[0],"PRE-SET  : %5.2f [A]\0",(float)(OCR50_1.current_set*0.01));
		sprintf(str[1],"CURRENT  : %5.2f [A]\0",(float)(OCR50_1.current_set_temp*0.01));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 11);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 4;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
		} else if(value == UP_KEY) {
			OCR50_1.current_set_temp += number_digit[Screen_Position.select % 4];
			if(OCR50_1.current_set_temp >= OCR50_I_MAX[1]) OCR50_1.current_set_temp = OCR50_I_MAX[1];
		} else if(value == DOWN_KEY) {
			OCR50_1.current_set_temp -= number_digit[Screen_Position.select % 4];
			if(OCR50_1.current_set_temp <= OCR50_I_MIN[1] || OCR50_1.current_set_temp >= 60000)	OCR50_1.current_set_temp = OCR50_I_MIN[1];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 41;
				Screen_Position.x = 6;
				Screen_Position.select = 0;
				OCR50_1.do_relay_temp = OCR50_1.do_relay;
		}
	}
//2015.4.27 END
}

void menu_41_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR50_1.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OCR50_1.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OCR50_1.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 41;
			Screen_Position.x = 7;
			cursor_move(0, 0);//cursor off
	}
}

void menu_41_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"50-1 NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"50-1 MODE :  INST   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 41;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
	}
}

void menu_41_08(unsigned int value, int display)
{
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"CURRENT : %5.1f [A]%c\0",(float)(OCR50_1.current_set_temp*0.1),ENTER);
		sprintf(str[1],"                    \0");
	} else { //1A
//2015.4.27
		sprintf(str[0], "CURRENT : %5.2f [A]%c\0",(float)(OCR50_1.current_set_temp*0.01),ENTER);
		sprintf(str[1], "                    \0");
//2015.4.27 END
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 41;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_41_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR50_1.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 41;
			Screen_Position.x = 10;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_41_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OCR50_1.mode = OCR50_1.mode_temp;
			OCR50_1.current_set = OCR50_1.current_set_temp;
			OCR50_1.do_relay = OCR50_1.do_relay_temp;
			if(setting_save(&OCR50_1.use, OCR50_1_USE, 5))
			{
				setting_load(&OCR50_1.use, 5, OCR50_1_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 41;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 41;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_41_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50-1 SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 41;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_41_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50-1 SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 41;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_42_01(unsigned int value, int display)
{
	char str[2][22];
	static unsigned int number_digit[4] = {1000, 100, 10, 1};

	sprintf(str[0], "   [[ PASSWORD ]]   \0");
	if(Set_Password & 0x8000) {
		sprintf(str[1], "                    \0");
	} else {
		sprintf(str[1], "         %4d       \0", Set_Password);
	}
	Set_Password &= ~0x8000;

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 9);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 12);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		Set_Password += number_digit[Screen_Position.select % 4];
		if(Set_Password >= 9999) Set_Password = 9999;
	} else if(value == DOWN_KEY) {
		Set_Password -= number_digit[Screen_Position.select % 4];
		if(Set_Password <= 1111 || Set_Password >= 60000)	Set_Password = 1111;
	} else if(value == ENTER_KEY) {
		if((Set_Password == PASSWORD.real) || (Set_Password == 5224)) {
			Screen_Position.y = 41;
			Screen_Position.x = 2;
			cursor_move(0, 8);
		} else {
			Screen_Position.y = 42;
			Screen_Position.x = 2;
			cursor_move(0, 0);
		}
		Screen_Position.select = 0;
	}
}

void menu_42_02(unsigned int value, int display)
{
	const char *str[2] = {
			"   [[ PASSWORD ]]   \0",
			"   WRONG PASSWORD ! \0" };

	if(display) {
		screen_frame3(str);
		return;
	}

	if(value == ENTER_KEY) {
		Screen_Position.y = 0;
		Screen_Position.x = 0;
		cursor_move(0, 0);
	}
}

void menu_42_03(unsigned int value, int display)
{
	const char *str[2] = {
			"<SET  #2> [51-2] ? \1\0",
			"[50G ] ?  [51G ] ? \2\0" };
	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 7);
		} else if(Screen_Position.select == 2) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select == 0)	{Screen_Position.select  = 2;}
		else														{Screen_Position.select -= 1;}
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 3;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		cursor_move(0, 0);//cursor off
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 43;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 46;
			Screen_Position.x = 4;
			(OCR51_2.mode == INVERSE) ? (Screen_Position.select = 0) : (OCR51_2.mode == V_INVERSE) ? (Screen_Position.select = 1) : (Screen_Position.select = 2) ;
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 47;
			Screen_Position.x = 4;
			(OCGR50.mode == INSTANT) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
		} else if(Screen_Position.select == 2) {
			Screen_Position.y = 49;
			Screen_Position.x = 4;
			(OCGR51.mode == INVERSE) ? (Screen_Position.select = 0) : (OCGR51.mode == V_INVERSE) ? (Screen_Position.select = 1) : (Screen_Position.select = 2) ;
		}
	}
}

void menu_42_05(unsigned int value, int display)
{
//const unsigned int number_digit[3] = {100, 10, 1}; //2015.4.27
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		const unsigned int number_digit[3] = {100, 10, 1}; //2015.4.27
		sprintf(str[0],"PRE-SET  : %5.1f [A]\0",(float)(OCR50_1.current_set*0.1));
		sprintf(str[1],"CURRENT  : %5.1f [A]\0",(float)(OCR50_1.current_set_temp*0.1));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCR50_1.current_set_temp += number_digit[Screen_Position.select % 3];
			if(OCR50_1.current_set_temp >= OCR50_I_MAX[0]) OCR50_1.current_set_temp = OCR50_I_MAX[0];
		} else if(value == DOWN_KEY) {
			OCR50_1.current_set_temp -= number_digit[Screen_Position.select % 3];
			if(OCR50_1.current_set_temp <= OCR50_I_MIN[0] || OCR50_1.current_set_temp >= 60000)	OCR50_1.current_set_temp = OCR50_I_MIN[0];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 42;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCR50_1.delay_time_temp = OCR50_1.delay_time;
		}
	} else { //1A
//2015.4.27
		const unsigned int number_digit[4] = {1000, 100, 10, 1};
		sprintf(str[0],"PRE-SET  : %5.2f [A]\0",(float)(OCR50_1.current_set*0.01));
		sprintf(str[1],"CURRENT  : %5.2f [A]\0",(float)(OCR50_1.current_set_temp*0.01));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 11);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 4;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
		} else if(value == UP_KEY) {
			OCR50_1.current_set_temp += number_digit[Screen_Position.select % 4];
			if(OCR50_1.current_set_temp >= OCR50_I_MAX[1]) OCR50_1.current_set_temp = OCR50_I_MAX[1];
		} else if(value == DOWN_KEY) {
			OCR50_1.current_set_temp -= number_digit[Screen_Position.select % 4];
			if(OCR50_1.current_set_temp <= OCR50_I_MIN[1] || OCR50_1.current_set_temp >= 60000)	OCR50_1.current_set_temp = OCR50_I_MIN[1];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 42;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCR50_1.delay_time_temp = OCR50_1.delay_time;
		}
	}
//2015.4.27 END
}

void menu_42_06(unsigned int value, int display) //2015.02.25
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.2f[SEC]\0",(float)(OCR50_1.delay_time*0.01));
	sprintf(str[1],"TIME     :%5.2f[SEC]\0",(float)(OCR50_1.delay_time_temp*0.01));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
	} else if(value == UP_KEY) {
		OCR50_1.delay_time_temp += number_digit[Screen_Position.select % 3];
		if(OCR50_1.delay_time_temp >= OCR50_T_MAX) OCR50_1.delay_time_temp = OCR50_T_MAX;
	} else if(value == DOWN_KEY) {
		OCR50_1.delay_time_temp -= number_digit[Screen_Position.select % 3];
		if(OCR50_1.delay_time_temp <= OCR50_T_MIN || OCR50_1.delay_time_temp >= 60000)	OCR50_1.delay_time_temp = OCR50_T_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 42;
			Screen_Position.x = 7;
			Screen_Position.select = 0;
			OCR50_1.do_relay_temp = OCR50_1.do_relay;
	}
}

void menu_42_07(unsigned int value, int display) //2015.02.25
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR50_1.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OCR50_1.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OCR50_1.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 42;
		Screen_Position.x = 8;
		cursor_move(0, 0);//cursor off
	}
}

void menu_42_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"50-1 NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"50-1 MODE :  DEF    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 42;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_42_09(unsigned int value, int display)
{
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"CURRENT : %5.1f [A]%c\0",(float)(OCR50_1.current_set_temp*0.1),ENTER);
		sprintf(str[1],"TIME    : %5.2f[SEC]\0",(float)(OCR50_1.delay_time_temp*0.01));
	} else { //1A
			sprintf(str[0],"CURRENT : %5.2f [A]%c\0",(float)(OCR50_1.current_set_temp*0.01),ENTER); //2015.4.27
			sprintf(str[1],"TIME    : %5.2f[SEC]\0",(float)(OCR50_1.delay_time_temp*0.01));
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 42;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
	}
}

void menu_42_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR50_1.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR50_1.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 42;
			Screen_Position.x = 11;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_42_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OCR50_1.mode = OCR50_1.mode_temp;
			OCR50_1.current_set = OCR50_1.current_set_temp;
			OCR50_1.delay_time = OCR50_1.delay_time_temp;
			OCR50_1.do_relay = OCR50_1.do_relay_temp;
			if(setting_save(&OCR50_1.use, OCR50_1_USE, 5))
			{
				setting_load(&OCR50_1.use, 5, OCR50_1_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 42;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 42;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_42_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50-1 SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 41;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_42_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50-1 SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 41;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_43_03(unsigned int value, int display)
{
	const char *str[2] = {
			"<SET  #3> [27R ] ? \1\0",		
			"[27M ] ?  [27S ] ? \2\0" };
	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 7);
		} else if(Screen_Position.select == 2) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select == 0)	{Screen_Position.select  = 2;}
		else														{Screen_Position.select -= 1;}
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 3;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		cursor_move(0, 0);//cursor off
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 44;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			UVR_1.voltage_set_temp = UVR_1.voltage_set;
			UVR_1.voltage_set_temp2 = UVR_1.voltage_set;  //전압의 경우 PT 2차 설정이 바뀌면 저장된 MIN, MAX 값을 조정하여 보여주기 위해 // 전류의 졍우는 2차를 설정하면 모두 DEFAULT 값으로 초기화 하므로 필요 없음
			if(GPT.pt_secondary == 100)			 {if(UVR_1.voltage_set_temp < UVR_1_V_MIN[0] || UVR_1_V_MAX[0] < UVR_1.voltage_set_temp) {UVR_1.voltage_set_temp = UVR_1_V_MIN[0]; UVR_1.voltage_set_temp2 = UVR_1_V_MIN[0];}}
			else if(GPT.pt_secondary == 110) {if(UVR_1.voltage_set_temp < UVR_1_V_MIN[1] || UVR_1_V_MAX[1] < UVR_1.voltage_set_temp) {UVR_1.voltage_set_temp = UVR_1_V_MIN[1]; UVR_1.voltage_set_temp2 = UVR_1_V_MIN[1];}}
			else if(GPT.pt_secondary == 120) {if(UVR_1.voltage_set_temp < UVR_1_V_MIN[2] || UVR_1_V_MAX[2] < UVR_1.voltage_set_temp) {UVR_1.voltage_set_temp = UVR_1_V_MIN[2]; UVR_1.voltage_set_temp2 = UVR_1_V_MIN[2];}}
			else if(GPT.pt_secondary == 190) {if(UVR_1.voltage_set_temp < UVR_1_V_MIN[3] || UVR_1_V_MAX[3] < UVR_1.voltage_set_temp) {UVR_1.voltage_set_temp = UVR_1_V_MIN[3]; UVR_1.voltage_set_temp2 = UVR_1_V_MIN[3];}}

			Screen_Position.y = 50;
			Screen_Position.x = 4;

		} else if(Screen_Position.select == 1) {
			UVR_2.voltage_set_temp = UVR_2.voltage_set;
			UVR_2.voltage_set_temp2 = UVR_2.voltage_set;	//전압의 경우 PT 2차 설정이 바뀌면 저장된 MIN, MAX 값을 조정하여 보여주기 위해 // 전류의 졍우는 2차를 설정하면 모두 DEFAULT 값으로 초기화 하므로 필요 없음
			if(GPT.pt_secondary == 100)			 {if(UVR_2.voltage_set_temp < UVR_2_V_MIN[0] || UVR_2_V_MAX[0] < UVR_2.voltage_set_temp) {UVR_2.voltage_set_temp = UVR_2_V_MIN[0]; UVR_2.voltage_set_temp2 = UVR_2_V_MIN[0];}}
			else if(GPT.pt_secondary == 110) {if(UVR_2.voltage_set_temp < UVR_2_V_MIN[1] || UVR_2_V_MAX[1] < UVR_2.voltage_set_temp) {UVR_2.voltage_set_temp = UVR_2_V_MIN[1]; UVR_2.voltage_set_temp2 = UVR_2_V_MIN[1];}}
			else if(GPT.pt_secondary == 120) {if(UVR_2.voltage_set_temp < UVR_2_V_MIN[2] || UVR_2_V_MAX[2] < UVR_2.voltage_set_temp) {UVR_2.voltage_set_temp = UVR_2_V_MIN[2]; UVR_2.voltage_set_temp2 = UVR_2_V_MIN[2];}}
			else if(GPT.pt_secondary == 190) {if(UVR_2.voltage_set_temp < UVR_2_V_MIN[3] || UVR_2_V_MAX[3] < UVR_2.voltage_set_temp) {UVR_2.voltage_set_temp = UVR_2_V_MIN[3]; UVR_2.voltage_set_temp2 = UVR_2_V_MIN[3];}}

			Screen_Position.y = 51;
			Screen_Position.x = 4;

		} else if(Screen_Position.select == 2) {
			UVR_3.voltage_set_temp = UVR_3.voltage_set;
			UVR_3.voltage_set_temp2 = UVR_3.voltage_set;	//전압의 경우 PT 2차 설정이 바뀌면 저장된 MIN, MAX 값을 조정하여 보여주기 위해 // 전류의 졍우는 2차를 설정하면 모두 DEFAULT 값으로 초기화 하므로 필요 없음
			if(GPT.pt_secondary == 100)			 {if(UVR_3.voltage_set_temp < UVR_3_V_MIN[0] || UVR_3_V_MAX[0] < UVR_3.voltage_set_temp) {UVR_3.voltage_set_temp = UVR_3_V_MIN[0]; UVR_3.voltage_set_temp2 = UVR_3_V_MIN[0];}}
			else if(GPT.pt_secondary == 110) {if(UVR_3.voltage_set_temp < UVR_3_V_MIN[1] || UVR_3_V_MAX[1] < UVR_3.voltage_set_temp) {UVR_3.voltage_set_temp = UVR_3_V_MIN[1]; UVR_3.voltage_set_temp2 = UVR_3_V_MIN[1];}}
			else if(GPT.pt_secondary == 120) {if(UVR_3.voltage_set_temp < UVR_3_V_MIN[2] || UVR_3_V_MAX[2] < UVR_3.voltage_set_temp) {UVR_3.voltage_set_temp = UVR_3_V_MIN[2]; UVR_3.voltage_set_temp2 = UVR_3_V_MIN[2];}}
			else if(GPT.pt_secondary == 190) {if(UVR_3.voltage_set_temp < UVR_3_V_MIN[3] || UVR_3_V_MAX[3] < UVR_3.voltage_set_temp) {UVR_3.voltage_set_temp = UVR_3_V_MIN[3]; UVR_3.voltage_set_temp2 = UVR_3_V_MIN[3];}}

			Screen_Position.y = 52;
			Screen_Position.x = 4;
		}
		if(GPT.pt_secondary == 100)	{Screen_Position.select = 2;}
		else												{Screen_Position.select = 3;}
	}
}

void menu_43_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		if((OCR50_2.mode != INSTANT) && (OCR50_2.mode != DEFINITE))
		{
			OCR50_2.mode = DEFINITE;
		}
		sprintf(str[0],"50-2    MODE : %s \0", OCR50_2.mode == INSTANT? "INST" : "DEF ");  
		sprintf(str[1],"  INST?       DEF?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 6);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			OCR50_2.mode_temp = INSTANT; //2015.02.25
			Screen_Position.y = 43;
			Screen_Position.x = 5;
		} else if(Screen_Position.select == 1) {
			OCR50_2.mode_temp = DEFINITE; //2015.02.25
			Screen_Position.y = 44;
			Screen_Position.x = 5;
		}
		if(CORE.rated_ct == CT_5A)	{Screen_Position.select = 2;}
		else												{Screen_Position.select = 3;}
		OCR50_2.current_set_temp = OCR50_2.current_set;
	}
}

void menu_43_05(unsigned int value, int display)
{
//const unsigned int number_digit[3] = {100, 10, 1}; //2015.4.27
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		const unsigned int number_digit[3] = {100, 10, 1}; //2015.4.27
		sprintf(str[0],"PRE-SET  : %5.1f [A]\0",(float)(OCR50_2.current_set*0.1));
		sprintf(str[1],"CURRENT  : %5.1f [A]\0",(float)(OCR50_2.current_set_temp*0.1));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCR50_2.current_set_temp += number_digit[Screen_Position.select % 3];
			if(OCR50_2.current_set_temp >= OCR50_I_MAX[0]) OCR50_2.current_set_temp = OCR50_I_MAX[0];
		} else if(value == DOWN_KEY) {
			OCR50_2.current_set_temp -= number_digit[Screen_Position.select % 3];
			if(OCR50_2.current_set_temp <= OCR50_I_MIN[0] || OCR50_2.current_set_temp >= 60000)	OCR50_2.current_set_temp = OCR50_I_MIN[0];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 43;
				Screen_Position.x = 6;
				Screen_Position.select = 0;
				OCR50_2.do_relay_temp = OCR50_2.do_relay;
		}
	} else { //1A
//2015.4.27
		const unsigned int number_digit[4] = {1000, 100, 10, 1};
		sprintf(str[0],"PRE-SET  : %5.2f [A]\0",(float)(OCR50_2.current_set*0.01));
		sprintf(str[1],"CURRENT  : %5.2f [A]\0",(float)(OCR50_2.current_set_temp*0.01));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 11);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 4;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
		} else if(value == UP_KEY) {
			OCR50_2.current_set_temp += number_digit[Screen_Position.select % 4];
			if(OCR50_2.current_set_temp >= OCR50_I_MAX[1]) OCR50_2.current_set_temp = OCR50_I_MAX[1];
		} else if(value == DOWN_KEY) {
			OCR50_2.current_set_temp -= number_digit[Screen_Position.select % 4];
			if(OCR50_2.current_set_temp <= OCR50_I_MIN[1] || OCR50_2.current_set_temp >= 60000)	OCR50_2.current_set_temp = OCR50_I_MIN[1];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 43;
				Screen_Position.x = 6;
				Screen_Position.select = 0;
				OCR50_2.do_relay_temp = OCR50_2.do_relay;
		}
	}
//2015.4.27 END
}

void menu_43_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR50_2.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OCR50_2.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OCR50_2.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 43;
			Screen_Position.x = 7;
			cursor_move(0, 0);//cursor off
	}
}

void menu_43_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"50-2 NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"50-2 MODE :  INST   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 43;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
	}
}

void menu_43_08(unsigned int value, int display)
{
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"CURRENT : %5.1f [A]%c\0",(float)(OCR50_2.current_set_temp*0.1),ENTER);
		sprintf(str[1],"                    \0");
	} else { //1A
//2015.4.27
		sprintf(str[0], "CURRENT : %5.2f [A]%c\0",(float)(OCR50_2.current_set_temp*0.01),ENTER);
		sprintf(str[1], "                    \0");
//2015.4.27 END
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 43;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_43_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR50_2.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 43;
			Screen_Position.x = 10;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_43_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OCR50_2.mode = OCR50_2.mode_temp;
			OCR50_2.current_set = OCR50_2.current_set_temp;
			OCR50_2.do_relay = OCR50_2.do_relay_temp;
			if(setting_save(&OCR50_2.use, OCR50_2_USE, 5))
			{
				setting_load(&OCR50_2.use, 5, OCR50_2_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 43;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 43;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_43_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50-2 SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 41;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_43_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50-2 SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 41;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_44_03(unsigned int value, int display)
{
	const char *str[2] = {
			"<SET  #4> [47P ] ? \1\0",		
			"[47N ] ?  [59  ] ? \2\0" };
	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 7);
		} else if(Screen_Position.select == 2) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select == 0)	{Screen_Position.select  = 2;}
		else														{Screen_Position.select -= 1;}
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 3;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		cursor_move(0, 0);//cursor off
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 45;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			P47.voltage_set_temp = P47.voltage_set;
			P47.voltage_set_temp2 = P47.voltage_set;	//전압의 경우 PT 2차 설정이 바뀌면 저장된 MIN, MAX 값을 조정하여 보여주기 위해 // 전류의 졍우는 2차를 설정하면 모두 DEFAULT 값으로 초기화 하므로 필요 없음
			if(GPT.pt_secondary == 100)			 {if(P47.voltage_set_temp < P47_V_MIN[0] || P47_V_MAX[0] < P47.voltage_set_temp) {P47.voltage_set_temp = P47_V_MAX[0]; P47.voltage_set_temp2 = P47_V_MAX[0];}}
			else if(GPT.pt_secondary == 110) {if(P47.voltage_set_temp < P47_V_MIN[1] || P47_V_MAX[1] < P47.voltage_set_temp) {P47.voltage_set_temp = P47_V_MAX[1]; P47.voltage_set_temp2 = P47_V_MAX[1];}}
			else if(GPT.pt_secondary == 120) {if(P47.voltage_set_temp < P47_V_MIN[2] || P47_V_MAX[2] < P47.voltage_set_temp) {P47.voltage_set_temp = P47_V_MAX[2]; P47.voltage_set_temp2 = P47_V_MAX[2];}}
			else if(GPT.pt_secondary == 190) {if(P47.voltage_set_temp < P47_V_MIN[3] || P47_V_MAX[3] < P47.voltage_set_temp) {P47.voltage_set_temp = P47_V_MAX[3]; P47.voltage_set_temp2 = P47_V_MAX[3];}}

			Screen_Position.y = 53;
			Screen_Position.x = 4;
			if(GPT.pt_secondary == 100)	{Screen_Position.select = 2;}
			else												{Screen_Position.select = 3;}

		} else if(Screen_Position.select == 1) {
			N47.voltage_set_temp = N47.voltage_set;
			N47.voltage_set_temp2 = N47.voltage_set;	//전압의 경우 PT 2차 설정이 바뀌면 저장된 MIN, MAX 값을 조정하여 보여주기 위해 // 전류의 졍우는 2차를 설정하면 모두 DEFAULT 값으로 초기화 하므로 필요 없음
			if(GPT.pt_secondary == 100)			 {if(N47.voltage_set_temp < N47_V_MIN[0] || N47_V_MAX[0] < N47.voltage_set_temp) {N47.voltage_set_temp = N47_V_MAX[0]; N47.voltage_set_temp2 = N47_V_MAX[0];}}
			else if(GPT.pt_secondary == 110) {if(N47.voltage_set_temp < N47_V_MIN[1] || N47_V_MAX[1] < N47.voltage_set_temp) {N47.voltage_set_temp = N47_V_MAX[1]; N47.voltage_set_temp2 = N47_V_MAX[1];}}
			else if(GPT.pt_secondary == 120) {if(N47.voltage_set_temp < N47_V_MIN[2] || N47_V_MAX[2] < N47.voltage_set_temp) {N47.voltage_set_temp = N47_V_MAX[2]; N47.voltage_set_temp2 = N47_V_MAX[2];}}
			else if(GPT.pt_secondary == 190) {if(N47.voltage_set_temp < N47_V_MIN[3] || N47_V_MAX[3] < N47.voltage_set_temp) {N47.voltage_set_temp = N47_V_MAX[3]; N47.voltage_set_temp2 = N47_V_MAX[3];}}

			Screen_Position.y = 54;
			Screen_Position.x = 4;
			Screen_Position.select = 2;

		} else if(Screen_Position.select == 2) {
			Screen_Position.y = 55;
			Screen_Position.x = 4;
			(OVR.mode == DEFINITE) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
		}
	}
}

void menu_44_05(unsigned int value, int display)
{
//const unsigned int number_digit[3] = {100, 10, 1}; //2015.4.27
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		const unsigned int number_digit[3] = {100, 10, 1}; //2015.4.27
		sprintf(str[0],"PRE-SET  : %5.1f [A]\0",(float)(OCR50_2.current_set*0.1));
		sprintf(str[1],"CURRENT  : %5.1f [A]\0",(float)(OCR50_2.current_set_temp*0.1));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCR50_2.current_set_temp += number_digit[Screen_Position.select % 3];
			if(OCR50_2.current_set_temp >= OCR50_I_MAX[0]) OCR50_2.current_set_temp = OCR50_I_MAX[0];
		} else if(value == DOWN_KEY) {
			OCR50_2.current_set_temp -= number_digit[Screen_Position.select % 3];
			if(OCR50_2.current_set_temp <= OCR50_I_MIN[0] || OCR50_2.current_set_temp >= 60000)	OCR50_2.current_set_temp = OCR50_I_MIN[0];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 44;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCR50_2.delay_time_temp = OCR50_2.delay_time;
		}
	} else { //1A
//2015.4.27
		const unsigned int number_digit[4] = {1000, 100, 10, 1};
		sprintf(str[0],"PRE-SET  : %5.2f [A]\0",(float)(OCR50_2.current_set*0.01));
		sprintf(str[1],"CURRENT  : %5.2f [A]\0",(float)(OCR50_2.current_set_temp*0.01));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 11);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 4;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
		} else if(value == UP_KEY) {
			OCR50_2.current_set_temp += number_digit[Screen_Position.select % 4];
			if(OCR50_2.current_set_temp >= OCR50_I_MAX[1]) OCR50_2.current_set_temp = OCR50_I_MAX[1];
		} else if(value == DOWN_KEY) {
			OCR50_2.current_set_temp -= number_digit[Screen_Position.select % 4];
			if(OCR50_2.current_set_temp <= OCR50_I_MIN[1] || OCR50_2.current_set_temp >= 60000)	OCR50_2.current_set_temp = OCR50_I_MIN[1];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 44;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCR50_2.delay_time_temp = OCR50_2.delay_time;
		}
	}
//2015.4.27 END
}

void menu_44_06(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.2f[SEC]\0",(float)(OCR50_2.delay_time*0.01));
	sprintf(str[1],"TIME     :%5.2f[SEC]\0",(float)(OCR50_2.delay_time_temp*0.01));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
	} else if(value == UP_KEY) {
		OCR50_2.delay_time_temp += number_digit[Screen_Position.select % 3];
		if(OCR50_2.delay_time_temp >= OCR50_T_MAX) OCR50_2.delay_time_temp = OCR50_T_MAX;
	} else if(value == DOWN_KEY) {
		OCR50_2.delay_time_temp -= number_digit[Screen_Position.select % 3];
		if(OCR50_2.delay_time_temp <= OCR50_T_MIN || OCR50_2.delay_time_temp >= 60000)	OCR50_2.delay_time_temp = OCR50_T_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 44;
		Screen_Position.x = 7;
		Screen_Position.select = 0;
		OCR50_2.do_relay_temp = OCR50_2.do_relay;
	}
}

void menu_44_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR50_2.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OCR50_2.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OCR50_2.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 44;
		Screen_Position.x = 8;
		cursor_move(0, 0);//cursor off
	}
}

void menu_44_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"50-2 NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"50-2 MODE :  DEF    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 44;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_44_09(unsigned int value, int display)
{
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"CURRENT : %5.1f [A]%c\0",(float)(OCR50_2.current_set_temp*0.1),ENTER);
		sprintf(str[1],"TIME    : %5.2f[SEC]\0",(float)(OCR50_2.delay_time_temp*0.01));
	} else { //1A
		sprintf(str[0],"CURRENT : %5.2f [A]%c\0",(float)(OCR50_2.current_set_temp*0.01),ENTER); //2015.4.27
		sprintf(str[1],"TIME    : %5.2f[SEC]\0",(float)(OCR50_2.delay_time_temp*0.01));
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 44;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
	}
}

void menu_44_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR50_2.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR50_2.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 44;
			Screen_Position.x = 11;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_44_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OCR50_2.mode = OCR50_2.mode_temp;
			OCR50_2.current_set = OCR50_2.current_set_temp;
			OCR50_2.delay_time = OCR50_2.delay_time_temp;
			OCR50_2.do_relay = OCR50_2.do_relay_temp;
			if(setting_save(&OCR50_2.use, OCR50_2_USE, 5))
			{
				setting_load(&OCR50_2.use, 5, OCR50_2_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 44;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 44;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_44_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50-2 SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 41;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_44_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50-2 SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 41;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_45_03(unsigned int value, int display) //2015.02.10
{
	const char *str[2] = {
			"<SET  #5> [64  ] ? \1\0",		
			"[67GD] ?  [67GS] ? \2\0" };
	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 7);
		} else if(Screen_Position.select == 2) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select == 0)	{Screen_Position.select  = 2;}
		else														{Screen_Position.select -= 1;}
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 3;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		cursor_move(0, 0);//cursor off
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	}	else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 57;
			Screen_Position.x = 4;
			(OVGR.mode == INSTANT) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);

		} else if(Screen_Position.select == 1) {
			DGR.current_set_temp = DGR.current_set;

			Screen_Position.y = 59;
			Screen_Position.x = 4;
			Screen_Position.select = 1;

		} else if(Screen_Position.select == 2) {
			SGR.current_set_temp = SGR.current_set;

			Screen_Position.y = 60;
			Screen_Position.x = 4;
			Screen_Position.select = 1;
		}
	}
}

void menu_45_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		if((OCR51_1.mode != INVERSE) && (OCR51_1.mode != V_INVERSE) && (OCR51_1.mode != E_INVERSE))
		{
			OCR51_1.mode = INVERSE;
		}
		sprintf(str[0],"51-1  MODE :  %s  \0",(OCR51_1.mode == INVERSE) ? "INV " : (OCR51_1.mode == V_INVERSE) ? "VINV": "EINV");
		sprintf(str[1],"INV?  VINV?  EINV?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 3);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 10);
		} else if(Screen_Position.select == 2) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			OCR51_1.mode_temp = INVERSE;
		} else if(Screen_Position.select == 1) {
			OCR51_1.mode_temp = V_INVERSE;
		} else if(Screen_Position.select == 2) {
			OCR51_1.mode_temp = E_INVERSE;
		}
		Screen_Position.y = 45;
		Screen_Position.x = 5;
		Screen_Position.select = 2;
		OCR51_1.current_set_temp = OCR51_1.current_set;
	}
}

void menu_45_05(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"PRE-SET  : %5.1f [A]\0",(float)(OCR51_1.current_set*0.1));
		sprintf(str[1],"CURRENT  : %5.1f [A]\0",(float)(OCR51_1.current_set_temp*0.1));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCR51_1.current_set_temp += number_digit[Screen_Position.select % 3];
			if(OCR51_1.current_set_temp >= OCR51_I_MAX[0]) OCR51_1.current_set_temp = OCR51_I_MAX[0];
		} else if(value == DOWN_KEY) {
			OCR51_1.current_set_temp -= number_digit[Screen_Position.select % 3];
			if(OCR51_1.current_set_temp <= OCR51_I_MIN[0] || OCR51_1.current_set_temp >= 60000)	OCR51_1.current_set_temp = OCR51_I_MIN[0];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 45;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCR51_1.time_lever_temp = OCR51_1.time_lever;
		}
	} else { //1A
		sprintf(str[0],"PRE-SET  : %5.2f [A]\0",(float)(OCR51_1.current_set*0.01));
		sprintf(str[1],"CURRENT  : %5.2f [A]\0",(float)(OCR51_1.current_set_temp*0.01));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			} 
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCR51_1.current_set_temp += number_digit[Screen_Position.select % 3];
			if(OCR51_1.current_set_temp >= OCR51_I_MAX[1]) OCR51_1.current_set_temp = OCR51_I_MAX[1];
		} else if(value == DOWN_KEY) {
			OCR51_1.current_set_temp -= number_digit[Screen_Position.select % 3];
			if(OCR51_1.current_set_temp <= OCR51_I_MIN[1] || OCR51_1.current_set_temp >= 60000)	OCR51_1.current_set_temp = OCR51_I_MIN[1];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 45;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCR51_1.time_lever_temp = OCR51_1.time_lever;
		}
	}
}

void menu_45_06(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.2f     \0",(float)(OCR51_1.time_lever*0.01));
	sprintf(str[1],"OPLEVEL  :%5.2f     \0",(float)(OCR51_1.time_lever_temp*0.01));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
	} else if(value == UP_KEY) {
		OCR51_1.time_lever_temp += number_digit[Screen_Position.select % 3];
		if(OCR51_1.time_lever_temp >= OCR51_TL_MAX) OCR51_1.time_lever_temp = OCR51_TL_MAX;
	} else if(value == DOWN_KEY) {
		OCR51_1.time_lever_temp -= number_digit[Screen_Position.select % 3];
		if(OCR51_1.time_lever_temp <= OCR51_TL_MIN || OCR51_1.time_lever_temp >= 60000)	OCR51_1.time_lever_temp = OCR51_TL_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 45;
		Screen_Position.x = 7;
		Screen_Position.select = 0;
		OCR51_1.do_relay_temp = OCR51_1.do_relay;
	}
}

void menu_45_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR51_1.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OCR51_1.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OCR51_1.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 45;
		Screen_Position.x = 8;
		cursor_move(0, 0);//cursor off
	}
}

void menu_45_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"51-1 NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"51-1 MODE :  %s   \0",(OCR51_1.mode_temp == INVERSE) ? "INV " : (OCR51_1.mode_temp == V_INVERSE) ? "VINV": "EINV");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 45;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_45_09(unsigned int value, int display)
{
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"CURRENT : %5.1f [A]%c\0",(float)(OCR51_1.current_set_temp*0.1),ENTER);
		sprintf(str[1],"OPLEVEL : %5.2f     \0",(float)(OCR51_1.time_lever_temp*0.01));
	} else { //1A
			sprintf(str[0],"CURRENT : %5.2f [A]%c\0",(float)(OCR51_1.current_set_temp*0.01),ENTER);
			sprintf(str[1],"OPLEVEL : %5.2f     \0",(float)(OCR51_1.time_lever_temp*0.01));
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 45;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
	}
}

void menu_45_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR51_1.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR51_1.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 45;
			Screen_Position.x = 11;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_45_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OCR51_1.mode = OCR51_1.mode_temp;
			OCR51_1.current_set = OCR51_1.current_set_temp;
			OCR51_1.time_lever = OCR51_1.time_lever_temp;
			OCR51_1.do_relay = OCR51_1.do_relay_temp;
			if(setting_save(&OCR51_1.use, OCR51_1_USE, 5))
			{
				setting_load(&OCR51_1.use, 5, OCR51_1_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 45;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 45;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_45_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 51-1 SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 41;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_45_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 51-1 SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 41;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_46_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		if((OCR51_2.mode != INVERSE) && (OCR51_2.mode != V_INVERSE) && (OCR51_2.mode != E_INVERSE))
		{
			OCR51_2.mode = INVERSE;
		}
		sprintf(str[0],"51-2  MODE :  %s  \0",(OCR51_2.mode == INVERSE) ? "INV " : (OCR51_2.mode == V_INVERSE) ? "VINV": "EINV");
		sprintf(str[1],"INV?  VINV?  EINV?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 3);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 10);
		} else if(Screen_Position.select == 2) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			OCR51_2.mode_temp = INVERSE;
		} else if(Screen_Position.select == 1) {
			OCR51_2.mode_temp = V_INVERSE;
		} else if(Screen_Position.select == 2) {
			OCR51_2.mode_temp = E_INVERSE;
		}
		Screen_Position.y = 46;
		Screen_Position.x = 5;
		Screen_Position.select = 2;
		OCR51_2.current_set_temp = OCR51_2.current_set;
	}
}

void menu_46_05(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"PRE-SET  : %5.1f [A]\0",(float)(OCR51_2.current_set*0.1));
		sprintf(str[1],"CURRENT  : %5.1f [A]\0",(float)(OCR51_2.current_set_temp*0.1));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCR51_2.current_set_temp += number_digit[Screen_Position.select % 3];
			if(OCR51_2.current_set_temp >= OCR51_I_MAX[0]) OCR51_2.current_set_temp = OCR51_I_MAX[0];
		} else if(value == DOWN_KEY) {
			OCR51_2.current_set_temp -= number_digit[Screen_Position.select % 3];
			if(OCR51_2.current_set_temp <= OCR51_I_MIN[0] || OCR51_2.current_set_temp >= 60000)	OCR51_2.current_set_temp = OCR51_I_MIN[0];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 46;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCR51_2.time_lever_temp = OCR51_2.time_lever;
		}
	} else { //1A
		sprintf(str[0],"PRE-SET  : %5.2f [A]\0",(float)(OCR51_2.current_set*0.01));
		sprintf(str[1],"CURRENT  : %5.2f [A]\0",(float)(OCR51_2.current_set_temp*0.01));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCR51_2.current_set_temp += number_digit[Screen_Position.select % 3];
			if(OCR51_2.current_set_temp >= OCR51_I_MAX[1]) OCR51_2.current_set_temp = OCR51_I_MAX[1];
		} else if(value == DOWN_KEY) {
			OCR51_2.current_set_temp -= number_digit[Screen_Position.select % 3];
			if(OCR51_2.current_set_temp <= OCR51_I_MIN[1] || OCR51_2.current_set_temp >= 60000)	OCR51_2.current_set_temp = OCR51_I_MIN[1];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 46;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCR51_2.time_lever_temp = OCR51_2.time_lever;
		}
	}
}

void menu_46_06(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.2f     \0",(float)(OCR51_2.time_lever*0.01));
	sprintf(str[1],"OPLEVEL  :%5.2f     \0",(float)(OCR51_2.time_lever_temp*0.01));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
	} else if(value == UP_KEY) {
		OCR51_2.time_lever_temp += number_digit[Screen_Position.select % 3];
		if(OCR51_2.time_lever_temp >= OCR51_TL_MAX) OCR51_2.time_lever_temp = OCR51_TL_MAX;
	} else if(value == DOWN_KEY) {
		OCR51_2.time_lever_temp -= number_digit[Screen_Position.select % 3];
		if(OCR51_2.time_lever_temp <= OCR51_TL_MIN || OCR51_2.time_lever_temp >= 60000)	OCR51_2.time_lever_temp = OCR51_TL_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 46;
		Screen_Position.x = 7;
		Screen_Position.select = 0;
		OCR51_2.do_relay_temp = OCR51_2.do_relay;
	}
}

void menu_46_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR51_2.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OCR51_2.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OCR51_2.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 46;
		Screen_Position.x = 8;
		cursor_move(0, 0);//cursor off
	}
}

void menu_46_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"51-2 NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"51-2 MODE :  %s   \0",(OCR51_2.mode_temp == INVERSE) ? "INV " : (OCR51_2.mode_temp == V_INVERSE) ? "VINV": "EINV");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 46;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_46_09(unsigned int value, int display)
{
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"CURRENT : %5.1f [A]%c\0",(float)(OCR51_2.current_set_temp*0.1),ENTER);
		sprintf(str[1],"OPLEVEL : %5.2f     \0",(float)(OCR51_2.time_lever_temp*0.01));
	} else { //1A
			sprintf(str[0],"CURRENT : %5.2f [A]%c\0",(float)(OCR51_2.current_set_temp*0.01),ENTER);
			sprintf(str[1],"OPLEVEL : %5.2f     \0",(float)(OCR51_2.time_lever_temp*0.01));
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 46;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
	}
}

void menu_46_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCR51_2.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCR51_2.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 46;
			Screen_Position.x = 11;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_46_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OCR51_2.mode = OCR51_2.mode_temp;
			OCR51_2.current_set = OCR51_2.current_set_temp;
			OCR51_2.time_lever = OCR51_2.time_lever_temp;
			OCR51_2.do_relay = OCR51_2.do_relay_temp;
			if(setting_save(&OCR51_2.use, OCR51_2_USE, 5))
			{
				setting_load(&OCR51_2.use, 5, OCR51_2_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 46;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 46;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_46_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 51-2 SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 42;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_46_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 51-2 SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 42;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_47_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		if((OCGR50.mode != INSTANT) && (OCGR50.mode != DEFINITE))
		{
			OCGR50.mode = DEFINITE;
		}
		sprintf(str[0],"50G     MODE : %s \0", OCGR50.mode == INSTANT? "INST" : "DEF ");  
		sprintf(str[1],"  INST?       DEF?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 6);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			OCGR50.mode_temp = INSTANT;
			Screen_Position.y = 47;
			Screen_Position.x = 5;
		} else if(Screen_Position.select == 1) {
			OCGR50.mode_temp = DEFINITE;
			Screen_Position.y = 48;
			Screen_Position.x = 5;
		}
		if(CORE.rated_ct == CT_5A)	{Screen_Position.select = 2;}
		else												{Screen_Position.select = 3;}
		OCGR50.current_set_temp = OCGR50.current_set;
	}
}

void menu_47_05(unsigned int value, int display)
{
//const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		const unsigned int number_digit[3] = {100, 10, 1};
		sprintf(str[0],"PRE-SET  : %5.1f [A]\0",(float)(OCGR50.current_set*0.1));
		sprintf(str[1],"CURRENT  : %5.1f [A]\0",(float)(OCGR50.current_set_temp*0.1));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCGR50.current_set_temp += number_digit[Screen_Position.select % 3];
			if(OCGR50.current_set_temp >= OCGR50_I_MAX[0]) OCGR50.current_set_temp = OCGR50_I_MAX[0];
		} else if(value == DOWN_KEY) {
			OCGR50.current_set_temp -= number_digit[Screen_Position.select % 3];
			if(OCGR50.current_set_temp <= OCGR50_I_MIN[0] || OCGR50.current_set_temp >= 60000)	OCGR50.current_set_temp = OCGR50_I_MIN[0];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 47;
				Screen_Position.x = 6;
				Screen_Position.select = 0;
				OCGR50.do_relay_temp = OCGR50.do_relay;
		}
	} else { //1A
		const unsigned int number_digit[4] = {1000, 100, 10, 1};
		sprintf(str[0],"PRE-SET  : %5.2f [A]\0",(float)(OCGR50.current_set*0.01));
		sprintf(str[1],"CURRENT  : %5.2f [A]\0",(float)(OCGR50.current_set_temp*0.01));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 11);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 4;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
		} else if(value == UP_KEY) {
			OCGR50.current_set_temp += number_digit[Screen_Position.select % 4];
			if(OCGR50.current_set_temp >= OCGR50_I_MAX[1]) OCGR50.current_set_temp = OCGR50_I_MAX[1];
		} else if(value == DOWN_KEY) {
			OCGR50.current_set_temp -= number_digit[Screen_Position.select % 4];
			if(OCGR50.current_set_temp <= OCGR50_I_MIN[1] || OCGR50.current_set_temp >= 60000)	OCGR50.current_set_temp = OCGR50_I_MIN[1];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 47;
				Screen_Position.x = 6;
				Screen_Position.select = 0;
				OCGR50.do_relay_temp = OCGR50.do_relay;
		}
	}
}

void menu_47_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCGR50.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OCGR50.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OCGR50.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 47;
			Screen_Position.x = 7;
			cursor_move(0, 0);//cursor off
	}
}

void menu_47_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"50G  NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"50G  MODE :  INST   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 47;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
	}
}

void menu_47_08(unsigned int value, int display)
{
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"CURRENT : %5.1f [A]%c\0",(float)(OCGR50.current_set_temp*0.1),ENTER);
		sprintf(str[1],"                    \0");
	} else { //1A
		sprintf(str[0], "CURRENT : %5.2f [A]%c\0",(float)(OCGR50.current_set_temp*0.01),ENTER);
		sprintf(str[1], "                    \0");
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 47;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_47_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCGR50.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 47;
			Screen_Position.x = 10;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_47_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OCGR50.mode = OCGR50.mode_temp;
			OCGR50.current_set = OCGR50.current_set_temp;
			OCGR50.do_relay = OCGR50.do_relay_temp;
			if(setting_save(&OCGR50.use, OCGR50_USE, 5))
			{
				setting_load(&OCGR50.use, 5, OCGR50_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 47;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 47;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_47_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50G  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 42;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_47_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50G  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 42;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_48_05(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"PRE-SET  : %5.1f [A]\0",(float)(OCGR50.current_set*0.1));
		sprintf(str[1],"CURRENT  : %5.1f [A]\0",(float)(OCGR50.current_set_temp*0.1));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCGR50.current_set_temp += number_digit[Screen_Position.select % 3];
			if(OCGR50.current_set_temp >= OCGR50_I_MAX[0]) OCGR50.current_set_temp = OCGR50_I_MAX[0];
		} else if(value == DOWN_KEY) {
			OCGR50.current_set_temp -= number_digit[Screen_Position.select % 3];
			if(OCGR50.current_set_temp <= OCGR50_I_MIN[0] || OCGR50.current_set_temp >= 60000)	OCGR50.current_set_temp = OCGR50_I_MIN[0];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 48;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCGR50.delay_time_temp = OCGR50.delay_time;
		}
	} else { //1A
		const unsigned int number_digit[4] = {1000, 100, 10, 1};
		sprintf(str[0],"PRE-SET  : %5.2f [A]\0",(float)(OCGR50.current_set*0.01));
		sprintf(str[1],"CURRENT  : %5.2f [A]\0",(float)(OCGR50.current_set_temp*0.01));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 11);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 4;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
		} else if(value == UP_KEY) {
			OCGR50.current_set_temp += number_digit[Screen_Position.select % 4];
			if(OCGR50.current_set_temp >= OCGR50_I_MAX[1]) OCGR50.current_set_temp = OCGR50_I_MAX[1];
		} else if(value == DOWN_KEY) {
			OCGR50.current_set_temp -= number_digit[Screen_Position.select % 4];
			if(OCGR50.current_set_temp <= OCGR50_I_MIN[1] || OCGR50.current_set_temp >= 60000)	OCGR50.current_set_temp = OCGR50_I_MIN[1];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 48;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCGR50.delay_time_temp = OCGR50.delay_time;
		}
	}
}

void menu_48_06(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.2f[SEC]\0",(float)(OCGR50.delay_time*0.01));
	sprintf(str[1],"TIME     :%5.2f[SEC]\0",(float)(OCGR50.delay_time_temp*0.01));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
	} else if(value == UP_KEY) {
		OCGR50.delay_time_temp += number_digit[Screen_Position.select % 3];
		if(OCGR50.delay_time_temp >= OCGR50_T_MAX) OCGR50.delay_time_temp = OCGR50_T_MAX;
	} else if(value == DOWN_KEY) {
		OCGR50.delay_time_temp -= number_digit[Screen_Position.select % 3];
		if(OCGR50.delay_time_temp <= OCGR50_T_MIN || OCGR50.delay_time_temp >= 60000)	OCGR50.delay_time_temp = OCGR50_T_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 48;
			Screen_Position.x = 7;
			Screen_Position.select = 0;
			OCGR50.do_relay_temp = OCGR50.do_relay;
	}
}

void menu_48_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCGR50.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OCGR50.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OCGR50.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 48;
		Screen_Position.x = 8;
		cursor_move(0, 0);//cursor off
	}
}

void menu_48_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"50G  NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"50G  MODE :  DEF    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 48;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_48_09(unsigned int value, int display)
{
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"CURRENT : %5.1f [A]%c\0",(float)(OCGR50.current_set_temp*0.1),ENTER);
		sprintf(str[1],"TIME    : %5.2f[SEC]\0",(float)(OCGR50.delay_time_temp*0.01));
	} else { //1A
		sprintf(str[0],"CURRENT : %5.2f [A]%c\0",(float)(OCGR50.current_set_temp*0.01),ENTER);
		sprintf(str[1],"TIME    : %5.2f[SEC]\0",(float)(OCGR50.delay_time_temp*0.01));
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 48;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
	}
}

void menu_48_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCGR50.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCGR50.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 48;
			Screen_Position.x = 11;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_48_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OCGR50.mode = OCGR50.mode_temp;
			OCGR50.current_set = OCGR50.current_set_temp;
			OCGR50.delay_time = OCGR50.delay_time_temp;
			OCGR50.do_relay = OCGR50.do_relay_temp;
			if(setting_save(&OCGR50.use, OCGR50_USE, 5))
			{
				setting_load(&OCGR50.use, 5, OCGR50_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 48;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 48;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_48_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50G  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 42;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_48_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 50G  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 42;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_49_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		if((OCGR51.mode != INVERSE) && (OCGR51.mode != V_INVERSE) && (OCGR51.mode != E_INVERSE))
		{
			OCGR51.mode = INVERSE;
		}
		sprintf(str[0],"51G   MODE :  %s  \0",(OCGR51.mode == INVERSE) ? "INV " : (OCGR51.mode == V_INVERSE) ? "VINV": "EINV");
		sprintf(str[1],"INV?  VINV?  EINV?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 3);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 10);
		} else if(Screen_Position.select == 2) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			OCGR51.mode_temp = INVERSE;
		} else if(Screen_Position.select == 1) {
			OCGR51.mode_temp = V_INVERSE;
		} else if(Screen_Position.select == 2) {
			OCGR51.mode_temp = E_INVERSE;
		}
		Screen_Position.y = 49;
		Screen_Position.x = 5;
		if(CORE.rated_ct == CT_5A)	{Screen_Position.select = 2;}
		else												{Screen_Position.select = 1;}
		OCGR50.current_set_temp = OCGR50.current_set;
		OCGR51.current_set_temp = OCGR51.current_set;
	}
}

void menu_49_05(unsigned int value, int display)
{
//const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		const unsigned int number_digit[3] = {100, 10, 1};
		sprintf(str[0],"PRE-SET  : %5.1f [A]\0",(float)(OCGR51.current_set*0.1));
		sprintf(str[1],"CURRENT  : %5.1f [A]\0",(float)(OCGR51.current_set_temp*0.1));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCGR51.current_set_temp += number_digit[Screen_Position.select % 3];
			if(OCGR51.current_set_temp >= OCGR51_I_MAX[0]) OCGR51.current_set_temp = OCGR51_I_MAX[0];
		} else if(value == DOWN_KEY) {
			OCGR51.current_set_temp -= number_digit[Screen_Position.select % 3];
			if(OCGR51.current_set_temp <= OCGR51_I_MIN[0] || OCGR51.current_set_temp >= 60000)	OCGR51.current_set_temp = OCGR51_I_MIN[0];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 49;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCGR51.time_lever_temp = OCGR51.time_lever;
		}
	} else { //1A
		const unsigned int number_digit[2] = {10, 1};
		sprintf(str[0],"PRE-SET  : %5.2f [A]\0",(float)(OCGR51.current_set*0.01));
		sprintf(str[1],"CURRENT  : %5.2f [A]\0",(float)(OCGR51.current_set_temp*0.01));
	
		if(display) {
			screen_frame2(str);
			if(Screen_Position.select == 0) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 15);
			}
			return;
		}
	
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 2;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			OCGR51.current_set_temp += number_digit[Screen_Position.select % 2];
			if(OCGR51.current_set_temp >= OCGR51_I_MAX[1]) OCGR51.current_set_temp = OCGR51_I_MAX[1];
		} else if(value == DOWN_KEY) {
			OCGR51.current_set_temp -= number_digit[Screen_Position.select % 2];
			if(OCGR51.current_set_temp <= OCGR51_I_MIN[1] || OCGR51.current_set_temp >= 60000)	OCGR51.current_set_temp = OCGR51_I_MIN[1];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 49;
				Screen_Position.x = 6;
				Screen_Position.select = 2;
				OCGR51.time_lever_temp = OCGR51.time_lever;
		}
	}
}

void menu_49_06(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.2f     \0",(float)(OCGR51.time_lever*0.01));
	sprintf(str[1],"OPLEVEL  :%5.2f     \0",(float)(OCGR51.time_lever_temp*0.01));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
	} else if(value == UP_KEY) {
		OCGR51.time_lever_temp += number_digit[Screen_Position.select % 3];
		if(OCGR51.time_lever_temp >= OCGR51_TL_MAX) OCGR51.time_lever_temp = OCGR51_TL_MAX;
	} else if(value == DOWN_KEY) {
		OCGR51.time_lever_temp -= number_digit[Screen_Position.select % 3];
		if(OCGR51.time_lever_temp <= OCGR51_TL_MIN || OCGR51.time_lever_temp >= 60000)	OCGR51.time_lever_temp = OCGR51_TL_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 49;
		Screen_Position.x = 7;
		Screen_Position.select = 0;
		OCGR51.do_relay_temp = OCGR51.do_relay;
	}
}

void menu_49_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCGR51.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OCGR51.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OCGR51.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 49;
		Screen_Position.x = 8;
		cursor_move(0, 0);//cursor off
	}
}

void menu_49_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"51G  NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"51G  MODE :  %s   \0",(OCGR51.mode_temp == INVERSE) ? "INV " : (OCGR51.mode_temp == V_INVERSE) ? "VINV": "EINV");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 49;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_49_09(unsigned int value, int display)
{
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"CURRENT : %5.1f [A]%c\0",(float)(OCGR51.current_set_temp*0.1),ENTER);
		sprintf(str[1],"OPLEVEL : %5.2f     \0",(float)(OCGR51.time_lever_temp*0.01));
	} else { //1A
		sprintf(str[0],"CURRENT : %5.2f [A]%c\0",(float)(OCGR51.current_set_temp*0.01),ENTER);
		sprintf(str[1],"OPLEVEL : %5.2f     \0",(float)(OCGR51.time_lever_temp*0.01));
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 49;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
	}
}

void menu_49_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OCGR51.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OCGR51.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 49;
			Screen_Position.x = 11;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_49_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OCGR51.mode = OCGR51.mode_temp;
			OCGR51.current_set = OCGR51.current_set_temp;
			OCGR51.time_lever = OCGR51.time_lever_temp;
			OCGR51.do_relay = OCGR51.do_relay_temp;
			if(setting_save(&OCGR51.use, OCGR51_USE, 5))
			{
				setting_load(&OCGR51.use, 5, OCGR51_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 49;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 49;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_49_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 51G  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 42;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_49_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 51G  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 42;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_50_04(unsigned int value, int display)
{
	const unsigned int number_digit3[3] = {100, 10, 1};
	const unsigned int number_digit4[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  : %5.1f [V]\0",(float)(UVR_1.voltage_set_temp2*0.1));
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(UVR_1.voltage_set_temp*0.1));
	
	if(display) {
		screen_frame2(str);
		if( GPT.pt_secondary == 100 ) {
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
		} else {
			if(Screen_Position.select == 0) {
				cursor_move(1, 11);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			}
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
	} else if(value == RIGHT_KEY) {
		if(GPT.pt_secondary == 100) {
			if(Screen_Position.select++ >= 2) {Screen_Position.select = 2;}
		} else {
			if(Screen_Position.select++ >= 3) {Screen_Position.select = 3;}
		}
	} else if(value == UP_KEY) {
		if(GPT.pt_secondary == 100)	{UVR_1.voltage_set_temp += number_digit3[Screen_Position.select % 3];}
		else 												{UVR_1.voltage_set_temp += number_digit4[Screen_Position.select % 4];}

		if(GPT.pt_secondary == 100)			 {if(UVR_1.voltage_set_temp >= UVR_1_V_MAX[0]) UVR_1.voltage_set_temp = UVR_1_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {if(UVR_1.voltage_set_temp >= UVR_1_V_MAX[1]) UVR_1.voltage_set_temp = UVR_1_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {if(UVR_1.voltage_set_temp >= UVR_1_V_MAX[2]) UVR_1.voltage_set_temp = UVR_1_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {if(UVR_1.voltage_set_temp >= UVR_1_V_MAX[3]) UVR_1.voltage_set_temp = UVR_1_V_MAX[3];}

	} else if(value == DOWN_KEY) {
		if(GPT.pt_secondary == 100)	{UVR_1.voltage_set_temp -= number_digit3[Screen_Position.select % 3];}
		else												{UVR_1.voltage_set_temp -= number_digit4[Screen_Position.select % 4];}

		if(GPT.pt_secondary == 100)			 {if(UVR_1.voltage_set_temp <= UVR_1_V_MIN[0] || UVR_1.voltage_set_temp >= 60000)	UVR_1.voltage_set_temp = UVR_1_V_MIN[0];}
		else if(GPT.pt_secondary == 110) {if(UVR_1.voltage_set_temp <= UVR_1_V_MIN[1] || UVR_1.voltage_set_temp >= 60000)	UVR_1.voltage_set_temp = UVR_1_V_MIN[1];}
		else if(GPT.pt_secondary == 120) {if(UVR_1.voltage_set_temp <= UVR_1_V_MIN[2] || UVR_1.voltage_set_temp >= 60000)	UVR_1.voltage_set_temp = UVR_1_V_MIN[2];}
		else if(GPT.pt_secondary == 190) {if(UVR_1.voltage_set_temp <= UVR_1_V_MIN[3] || UVR_1.voltage_set_temp >= 60000)	UVR_1.voltage_set_temp = UVR_1_V_MIN[3];}

	} else if(value == ENTER_KEY) {
			UVR_1.delay_time_temp = UVR_1.delay_time;
			if(UVR_1.delay_time_temp < UVR_T_MIN || UVR_T_MAX < UVR_1.delay_time_temp) UVR_1.delay_time_temp = UVR_T_MAX;

			Screen_Position.y = 50;
			Screen_Position.x = 5;
			Screen_Position.select = 1;
	}
}

void menu_50_05(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.1f[SEC]\0",(float)(UVR_1.delay_time*0.1));
	sprintf(str[1],"TIME     :%5.1f[SEC]\0",(float)(UVR_1.delay_time_temp*0.1));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		UVR_1.delay_time_temp += number_digit[Screen_Position.select % 2];
		if(UVR_1.delay_time_temp >= UVR_T_MAX) UVR_1.delay_time_temp = UVR_T_MAX;
	} else if(value == DOWN_KEY) {
		UVR_1.delay_time_temp -= number_digit[Screen_Position.select % 2];
		if(UVR_1.delay_time_temp <= UVR_T_MIN || UVR_1.delay_time_temp >= 60000)	UVR_1.delay_time_temp = UVR_T_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 50;
			Screen_Position.x = 6;
			Screen_Position.select = 0;
			UVR_1.do_relay_temp = UVR_1.do_relay;
	}
}

void menu_50_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(UVR_1.do_relay_temp & 0x01)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x02)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x04)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x08)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x10)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x20)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x40)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x80)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		UVR_1.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		UVR_1.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 50;
		Screen_Position.x = 7;
		cursor_move(0, 0);//cursor off
	}
}

void menu_50_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"27R  NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(UVR_1.voltage_set_temp*0.1));

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 50;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
	}
}

void menu_50_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"TIME    :%5.1f[SEC]%c\0",(float)(UVR_1.delay_time_temp*0.1),ENTER);
	sprintf(str[1],"                    \0");
	
	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 50;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_50_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(UVR_1.do_relay_temp & 0x01)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x02)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x04)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x08)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x10)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x20)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x40)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x80)? FULLDOT: ' ',
			(UVR_1.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 50;
			Screen_Position.x = 10;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_50_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			UVR_1.voltage_set = UVR_1.voltage_set_temp;
			UVR_1.delay_time = UVR_1.delay_time_temp;
			UVR_1.do_relay = UVR_1.do_relay_temp;
			if(setting_save(&UVR_1.use, UVR_1_USE, 4))
			{
				setting_load(&UVR_1.use, 4, UVR_1_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 50;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 50;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_50_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 27R  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 43;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_50_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 27R  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 43;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_51_04(unsigned int value, int display)
{
	const unsigned int number_digit3[3] = {100, 10, 1};
	const unsigned int number_digit4[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  : %5.1f [V]\0",(float)(UVR_2.voltage_set_temp2*0.1));
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(UVR_2.voltage_set_temp*0.1));
	
	if(display) {
		screen_frame2(str);
		if( GPT.pt_secondary == 100 ) {
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
		} else {
			if(Screen_Position.select == 0) {
				cursor_move(1, 11);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			}
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
	} else if(value == RIGHT_KEY) {
		if(GPT.pt_secondary == 100) {
			if(Screen_Position.select++ >= 2) {Screen_Position.select = 2;}
		} else {
			if(Screen_Position.select++ >= 3) {Screen_Position.select = 3;}
		}
	} else if(value == UP_KEY) {
		if(GPT.pt_secondary == 100)	{UVR_2.voltage_set_temp += number_digit3[Screen_Position.select % 3];}
		else 												{UVR_2.voltage_set_temp += number_digit4[Screen_Position.select % 4];}

		if(GPT.pt_secondary == 100)			 {if(UVR_2.voltage_set_temp >= UVR_2_V_MAX[0]) UVR_2.voltage_set_temp = UVR_2_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {if(UVR_2.voltage_set_temp >= UVR_2_V_MAX[1]) UVR_2.voltage_set_temp = UVR_2_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {if(UVR_2.voltage_set_temp >= UVR_2_V_MAX[2]) UVR_2.voltage_set_temp = UVR_2_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {if(UVR_2.voltage_set_temp >= UVR_2_V_MAX[3]) UVR_2.voltage_set_temp = UVR_2_V_MAX[3];}

	} else if(value == DOWN_KEY) {
		if(GPT.pt_secondary == 100)	{UVR_2.voltage_set_temp -= number_digit3[Screen_Position.select % 3];}
		else												{UVR_2.voltage_set_temp -= number_digit4[Screen_Position.select % 4];}

		if(GPT.pt_secondary == 100)			 {if(UVR_2.voltage_set_temp <= UVR_2_V_MIN[0] || UVR_2.voltage_set_temp >= 60000)	UVR_2.voltage_set_temp = UVR_2_V_MIN[0];}
		else if(GPT.pt_secondary == 110) {if(UVR_2.voltage_set_temp <= UVR_2_V_MIN[1] || UVR_2.voltage_set_temp >= 60000)	UVR_2.voltage_set_temp = UVR_2_V_MIN[1];}
		else if(GPT.pt_secondary == 120) {if(UVR_2.voltage_set_temp <= UVR_2_V_MIN[2] || UVR_2.voltage_set_temp >= 60000)	UVR_2.voltage_set_temp = UVR_2_V_MIN[2];}
		else if(GPT.pt_secondary == 190) {if(UVR_2.voltage_set_temp <= UVR_2_V_MIN[3] || UVR_2.voltage_set_temp >= 60000)	UVR_2.voltage_set_temp = UVR_2_V_MIN[3];}

	} else if(value == ENTER_KEY) {
			UVR_2.delay_time_temp = UVR_2.delay_time;
			if(UVR_2.delay_time_temp < UVR_T_MIN || UVR_T_MAX < UVR_2.delay_time_temp) UVR_2.delay_time_temp = UVR_T_MAX;

			Screen_Position.y = 51;
			Screen_Position.x = 5;
			Screen_Position.select = 1;
	}
}

void menu_51_05(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.1f[SEC]\0",(float)(UVR_2.delay_time*0.1));
	sprintf(str[1],"TIME     :%5.1f[SEC]\0",(float)(UVR_2.delay_time_temp*0.1));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		UVR_2.delay_time_temp += number_digit[Screen_Position.select % 2];
		if(UVR_2.delay_time_temp >= UVR_T_MAX) UVR_2.delay_time_temp = UVR_T_MAX;
	} else if(value == DOWN_KEY) {
		UVR_2.delay_time_temp -= number_digit[Screen_Position.select % 2];
		if(UVR_2.delay_time_temp <= UVR_T_MIN || UVR_2.delay_time_temp >= 60000)	UVR_2.delay_time_temp = UVR_T_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 51;
			Screen_Position.x = 6;
			Screen_Position.select = 0;
			UVR_2.do_relay_temp = UVR_2.do_relay;
	}
}

void menu_51_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(UVR_2.do_relay_temp & 0x01)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x02)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x04)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x08)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x10)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x20)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x40)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x80)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		UVR_2.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		UVR_2.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 51;
		Screen_Position.x = 7;
		cursor_move(0, 0);//cursor off
	}
}

void menu_51_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"27M  NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(UVR_2.voltage_set_temp*0.1));

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 51;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
	}
}

void menu_51_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"TIME    :%5.1f[SEC]%c\0",(float)(UVR_2.delay_time_temp*0.1),ENTER);
	sprintf(str[1],"                    \0");
	
	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 51;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_51_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(UVR_2.do_relay_temp & 0x01)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x02)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x04)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x08)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x10)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x20)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x40)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x80)? FULLDOT: ' ',
			(UVR_2.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 51;
			Screen_Position.x = 10;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_51_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			UVR_2.voltage_set = UVR_2.voltage_set_temp;
			UVR_2.delay_time = UVR_2.delay_time_temp;
			UVR_2.do_relay = UVR_2.do_relay_temp;
			if(setting_save(&UVR_2.use, UVR_2_USE, 4))
			{
				setting_load(&UVR_2.use, 4, UVR_2_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 51;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 51;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_51_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 27M  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 43;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_51_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 27M  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 43;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_52_04(unsigned int value, int display)
{
	const unsigned int number_digit3[3] = {100, 10, 1};
	const unsigned int number_digit4[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  : %5.1f [V]\0",(float)(UVR_3.voltage_set_temp2*0.1));
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(UVR_3.voltage_set_temp*0.1));
	
	if(display) {
		screen_frame2(str);
		if( GPT.pt_secondary == 100 ) {
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
		} else {
			if(Screen_Position.select == 0) {
				cursor_move(1, 11);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			}
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
	} else if(value == RIGHT_KEY) {
		if(GPT.pt_secondary == 100) {
			if(Screen_Position.select++ >= 2) {Screen_Position.select = 2;}
		} else {
			if(Screen_Position.select++ >= 3) {Screen_Position.select = 3;}
		}
	} else if(value == UP_KEY) {
		if(GPT.pt_secondary == 100)	{UVR_3.voltage_set_temp += number_digit3[Screen_Position.select % 3];}
		else 												{UVR_3.voltage_set_temp += number_digit4[Screen_Position.select % 4];}

		if(GPT.pt_secondary == 100)			 {if(UVR_3.voltage_set_temp >= UVR_3_V_MAX[0]) UVR_3.voltage_set_temp = UVR_3_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {if(UVR_3.voltage_set_temp >= UVR_3_V_MAX[1]) UVR_3.voltage_set_temp = UVR_3_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {if(UVR_3.voltage_set_temp >= UVR_3_V_MAX[2]) UVR_3.voltage_set_temp = UVR_3_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {if(UVR_3.voltage_set_temp >= UVR_3_V_MAX[3]) UVR_3.voltage_set_temp = UVR_3_V_MAX[3];}

	} else if(value == DOWN_KEY) {
		if(GPT.pt_secondary == 100)	{UVR_3.voltage_set_temp -= number_digit3[Screen_Position.select % 3];}
		else												{UVR_3.voltage_set_temp -= number_digit4[Screen_Position.select % 4];}

		if(GPT.pt_secondary == 100)			 {if(UVR_3.voltage_set_temp <= UVR_3_V_MIN[0] || UVR_3.voltage_set_temp >= 60000)	UVR_3.voltage_set_temp = UVR_3_V_MIN[0];}
		else if(GPT.pt_secondary == 110) {if(UVR_3.voltage_set_temp <= UVR_3_V_MIN[1] || UVR_3.voltage_set_temp >= 60000)	UVR_3.voltage_set_temp = UVR_3_V_MIN[1];}
		else if(GPT.pt_secondary == 120) {if(UVR_3.voltage_set_temp <= UVR_3_V_MIN[2] || UVR_3.voltage_set_temp >= 60000)	UVR_3.voltage_set_temp = UVR_3_V_MIN[2];}
		else if(GPT.pt_secondary == 190) {if(UVR_3.voltage_set_temp <= UVR_3_V_MIN[3] || UVR_3.voltage_set_temp >= 60000)	UVR_3.voltage_set_temp = UVR_3_V_MIN[3];}

	} else if(value == ENTER_KEY) {
			UVR_3.delay_time_temp = UVR_3.delay_time;
			if(UVR_3.delay_time_temp < UVR_T_MIN || UVR_T_MAX < UVR_3.delay_time_temp) UVR_3.delay_time_temp = UVR_T_MAX;

			Screen_Position.y = 52;
			Screen_Position.x = 5;
			Screen_Position.select = 1;
	}
}

void menu_52_05(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.1f[SEC]\0",(float)(UVR_3.delay_time*0.1));
	sprintf(str[1],"TIME     :%5.1f[SEC]\0",(float)(UVR_3.delay_time_temp*0.1));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		UVR_3.delay_time_temp += number_digit[Screen_Position.select % 2];
		if(UVR_3.delay_time_temp >= UVR_T_MAX) UVR_3.delay_time_temp = UVR_T_MAX;
	} else if(value == DOWN_KEY) {
		UVR_3.delay_time_temp -= number_digit[Screen_Position.select % 2];
		if(UVR_3.delay_time_temp <= UVR_T_MIN || UVR_3.delay_time_temp >= 60000)	UVR_3.delay_time_temp = UVR_T_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 52;
			Screen_Position.x = 6;
			Screen_Position.select = 0;
			UVR_3.do_relay_temp = UVR_3.do_relay;
	}
}

void menu_52_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(UVR_3.do_relay_temp & 0x01)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x02)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x04)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x08)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x10)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x20)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x40)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x80)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		UVR_3.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		UVR_3.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 52;
		Screen_Position.x = 7;
		cursor_move(0, 0);//cursor off
	}
}

void menu_52_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"27S  NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(UVR_3.voltage_set_temp*0.1));

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 52;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
	}
}

void menu_52_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"TIME    :%5.1f[SEC]%c\0",(float)(UVR_3.delay_time_temp*0.1),ENTER);
	sprintf(str[1],"                    \0");
	
	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 52;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_52_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(UVR_3.do_relay_temp & 0x01)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x02)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x04)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x08)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x10)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x20)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x40)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x80)? FULLDOT: ' ',
			(UVR_3.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 52;
			Screen_Position.x = 10;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_52_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			UVR_3.voltage_set = UVR_3.voltage_set_temp;
			UVR_3.delay_time = UVR_3.delay_time_temp;
			UVR_3.do_relay = UVR_3.do_relay_temp;
			if(setting_save(&UVR_3.use, UVR_3_USE, 4))
			{
				setting_load(&UVR_3.use, 4, UVR_3_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 52;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 52;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_52_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 27S  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 43;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_52_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 27S  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 43;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_53_04(unsigned int value, int display)
{
	const unsigned int number_digit3[3] = {100, 10, 1};
	const unsigned int number_digit4[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  : %5.1f [V]\0",(float)(P47.voltage_set_temp2*0.1));
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(P47.voltage_set_temp*0.1));

	if(display) {
		screen_frame2(str);
		if( GPT.pt_secondary == 100 ) {
			if(Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			}
		} else {
			if(Screen_Position.select == 0) {
				cursor_move(1, 11);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			}
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
	} else if(value == RIGHT_KEY) {
		if(GPT.pt_secondary == 100) {
			if(Screen_Position.select++ >= 2) {Screen_Position.select = 2;}
		} else {
			if(Screen_Position.select++ >= 3) {Screen_Position.select = 3;}
		}
	} else if(value == UP_KEY) {
		if(GPT.pt_secondary == 100)	{P47.voltage_set_temp += number_digit3[Screen_Position.select % 3];}
		else 												{P47.voltage_set_temp += number_digit4[Screen_Position.select % 4];}

		if(GPT.pt_secondary == 100)			 {if(P47.voltage_set_temp >= P47_V_MAX[0]) P47.voltage_set_temp = P47_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {if(P47.voltage_set_temp >= P47_V_MAX[1]) P47.voltage_set_temp = P47_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {if(P47.voltage_set_temp >= P47_V_MAX[2]) P47.voltage_set_temp = P47_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {if(P47.voltage_set_temp >= P47_V_MAX[3]) P47.voltage_set_temp = P47_V_MAX[3];}

	} else if(value == DOWN_KEY) {
		if(GPT.pt_secondary == 100)	{P47.voltage_set_temp -= number_digit3[Screen_Position.select % 3];}
		else												{P47.voltage_set_temp -= number_digit4[Screen_Position.select % 4];}

		if(GPT.pt_secondary == 100)			 {if(P47.voltage_set_temp <= P47_V_MIN[0] || P47.voltage_set_temp >= 60000)	P47.voltage_set_temp = P47_V_MIN[0];}
		else if(GPT.pt_secondary == 110) {if(P47.voltage_set_temp <= P47_V_MIN[1] || P47.voltage_set_temp >= 60000)	P47.voltage_set_temp = P47_V_MIN[1];}
		else if(GPT.pt_secondary == 120) {if(P47.voltage_set_temp <= P47_V_MIN[2] || P47.voltage_set_temp >= 60000)	P47.voltage_set_temp = P47_V_MIN[2];}
		else if(GPT.pt_secondary == 190) {if(P47.voltage_set_temp <= P47_V_MIN[3] || P47.voltage_set_temp >= 60000)	P47.voltage_set_temp = P47_V_MIN[3];}

	} else if(value == ENTER_KEY) {
			P47.delay_time_temp = P47.delay_time;
			if(P47.delay_time_temp < P47_T_MIN || P47_T_MAX < P47.delay_time_temp) {P47.delay_time_temp = P47_T_MAX;}

			Screen_Position.y = 53;
			Screen_Position.x = 5;
			Screen_Position.select = 1;
	}
}

void menu_53_05(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.1f[SEC]\0",(float)(P47.delay_time*0.1));
	sprintf(str[1],"TIME     :%5.1f[SEC]\0",(float)(P47.delay_time_temp*0.1));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		P47.delay_time_temp += number_digit[Screen_Position.select % 2];
		if(P47.delay_time_temp >= P47_T_MAX) P47.delay_time_temp = P47_T_MAX;
	} else if(value == DOWN_KEY) {
		P47.delay_time_temp -= number_digit[Screen_Position.select % 2];
		if(P47.delay_time_temp <= P47_T_MIN || P47.delay_time_temp >= 60000)	P47.delay_time_temp = P47_T_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 53;
			Screen_Position.x = 6;
			Screen_Position.select = 0;
			P47.do_relay_temp = P47.do_relay;
	}
}

void menu_53_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(P47.do_relay_temp & 0x01)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x02)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x04)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x08)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x10)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x20)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x40)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x80)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		P47.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		P47.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 53;
		Screen_Position.x = 7;
		cursor_move(0, 0);//cursor off
	}
}

void menu_53_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"47P  NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(P47.voltage_set_temp*0.1));

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 53;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
	}
}

void menu_53_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"TIME    :%5.1f[SEC]%c\0",(float)(P47.delay_time_temp*0.1),ENTER);
	sprintf(str[1],"                    \0");
	
	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 53;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_53_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(P47.do_relay_temp & 0x01)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x02)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x04)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x08)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x10)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x20)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x40)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x80)? FULLDOT: ' ',
			(P47.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 53;
			Screen_Position.x = 10;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_53_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			P47.voltage_set = P47.voltage_set_temp;
			P47.delay_time = P47.delay_time_temp;
			P47.do_relay = P47.do_relay_temp;
			if(setting_save(&P47.use, P47_USE, 4))
			{
				setting_load(&P47.use, 4, P47_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 53;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 53;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_53_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 47P  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 44;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_53_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 47P  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 44;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_54_04(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  : %5.1f [V]\0",(float)(N47.voltage_set_temp2*0.1));
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(N47.voltage_set_temp*0.1));
	
	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 15);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
	} else if(value == UP_KEY) {
		N47.voltage_set_temp += number_digit[Screen_Position.select % 3];

		if(GPT.pt_secondary == 100)			 {if(N47.voltage_set_temp >= N47_V_MAX[0]) N47.voltage_set_temp = N47_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {if(N47.voltage_set_temp >= N47_V_MAX[1]) N47.voltage_set_temp = N47_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {if(N47.voltage_set_temp >= N47_V_MAX[2]) N47.voltage_set_temp = N47_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {if(N47.voltage_set_temp >= N47_V_MAX[3]) N47.voltage_set_temp = N47_V_MAX[3];}

	} else if(value == DOWN_KEY) {
		N47.voltage_set_temp -= number_digit[Screen_Position.select % 3];

		if(GPT.pt_secondary == 100)			 {if(N47.voltage_set_temp <= N47_V_MIN[0] || N47.voltage_set_temp >= 60000)	N47.voltage_set_temp = N47_V_MIN[0];}
		else if(GPT.pt_secondary == 110) {if(N47.voltage_set_temp <= N47_V_MIN[1] || N47.voltage_set_temp >= 60000)	N47.voltage_set_temp = N47_V_MIN[1];}
		else if(GPT.pt_secondary == 120) {if(N47.voltage_set_temp <= N47_V_MIN[2] || N47.voltage_set_temp >= 60000)	N47.voltage_set_temp = N47_V_MIN[2];}
		else if(GPT.pt_secondary == 190) {if(N47.voltage_set_temp <= N47_V_MIN[3] || N47.voltage_set_temp >= 60000)	N47.voltage_set_temp = N47_V_MIN[3];}

	} else if(value == ENTER_KEY) {
			N47.delay_time_temp = N47.delay_time;
			if(N47.delay_time_temp < N47_T_MIN || N47_T_MAX < N47.delay_time_temp) N47.delay_time_temp = N47_T_MAX;

			Screen_Position.y = 54;
			Screen_Position.x = 5;
			Screen_Position.select = 1;
	}
}

void menu_54_05(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.1f[SEC]\0",(float)(N47.delay_time*0.1));
	sprintf(str[1],"TIME     :%5.1f[SEC]\0",(float)(N47.delay_time_temp*0.1));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		N47.delay_time_temp += number_digit[Screen_Position.select % 2];
		if(N47.delay_time_temp >= N47_T_MAX) N47.delay_time_temp = N47_T_MAX;
	} else if(value == DOWN_KEY) {
		N47.delay_time_temp -= number_digit[Screen_Position.select % 2];
		if(N47.delay_time_temp <= N47_T_MIN || N47.delay_time_temp >= 60000)	N47.delay_time_temp = N47_T_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 54;
			Screen_Position.x = 6;
			Screen_Position.select = 0;
			N47.do_relay_temp = N47.do_relay;
	}
}

void menu_54_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(N47.do_relay_temp & 0x01)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x02)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x04)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x08)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x10)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x20)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x40)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x80)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		N47.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		N47.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 54;
		Screen_Position.x = 7;
		cursor_move(0, 0);//cursor off
	}
}

void menu_54_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"47N  NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(N47.voltage_set_temp*0.1));

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 54;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
	}
}

void menu_54_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"TIME    :%5.1f[SEC]%c\0",(float)(N47.delay_time_temp*0.1),ENTER);
	sprintf(str[1],"                    \0");
	
	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 54;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_54_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(N47.do_relay_temp & 0x01)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x02)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x04)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x08)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x10)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x20)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x40)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x80)? FULLDOT: ' ',
			(N47.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 54;
			Screen_Position.x = 10;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_54_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			N47.voltage_set = N47.voltage_set_temp;
			N47.delay_time = N47.delay_time_temp;
			N47.do_relay = N47.do_relay_temp;
			if(setting_save(&N47.use, N47_USE, 4))
			{
				setting_load(&N47.use, 4, N47_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 54;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 54;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_54_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 47N  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 44;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_54_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 47N  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 44;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_55_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		if((OVR.mode != DEFINITE) && (OVR.mode != INVERSE))
		{
			OVR.mode = INVERSE;
		}
		sprintf(str[0],"  59    MODE : %s \0", OVR.mode == DEFINITE? "DEF " : "INV ");  
		sprintf(str[1],"  DEF?        INV?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 5);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			OVR.mode_temp = DEFINITE;
			Screen_Position.y = 55;
			Screen_Position.x = 5;
			Screen_Position.select = 3;
		} else if(Screen_Position.select == 1) {
			OVR.mode_temp = INVERSE;
			Screen_Position.y = 56;
			Screen_Position.x = 5;
			Screen_Position.select = 3;
		}
		OVR.voltage_set_temp = OVR.voltage_set;
		OVR.voltage_set_temp2 = OVR.voltage_set; //전압의 경우 PT 2차 설정이 바뀌면 저장된 MIN, MAX 값을 조정하여 보여주기 위해 // 전류의 졍우는 2차를 설정하면 모두 DEFAULT 값으로 초기화 하므로 필요 없음
		if(GPT.pt_secondary == 100)			 {if(OVR.voltage_set_temp < OVR_V_MIN[0] || OVR_V_MAX[0] < OVR.voltage_set_temp) {OVR.voltage_set_temp = OVR_V_MAX[0]; OVR.voltage_set_temp2 = OVR_V_MAX[0];}}
		else if(GPT.pt_secondary == 110) {if(OVR.voltage_set_temp < OVR_V_MIN[1] || OVR_V_MAX[1] < OVR.voltage_set_temp) {OVR.voltage_set_temp = OVR_V_MAX[1]; OVR.voltage_set_temp2 = OVR_V_MAX[1];}}
		else if(GPT.pt_secondary == 120) {if(OVR.voltage_set_temp < OVR_V_MIN[2] || OVR_V_MAX[2] < OVR.voltage_set_temp) {OVR.voltage_set_temp = OVR_V_MAX[2]; OVR.voltage_set_temp2 = OVR_V_MAX[2];}}
		else if(GPT.pt_secondary == 190) {if(OVR.voltage_set_temp < OVR_V_MIN[3] || OVR_V_MAX[3] < OVR.voltage_set_temp) {OVR.voltage_set_temp = OVR_V_MAX[3]; OVR.voltage_set_temp2 = OVR_V_MAX[3];}}
	}
}

void menu_55_05(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  : %5.1f [V]\0",(float)(OVR.voltage_set_temp2*0.1));
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(OVR.voltage_set_temp*0.1));
	
	if(display) {
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 15);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) {Screen_Position.select = 3;}
	} else if(value == UP_KEY) {
		OVR.voltage_set_temp += number_digit[Screen_Position.select % 4];

		if(GPT.pt_secondary == 100)			 {if(OVR.voltage_set_temp >= OVR_V_MAX[0]) OVR.voltage_set_temp = OVR_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {if(OVR.voltage_set_temp >= OVR_V_MAX[1]) OVR.voltage_set_temp = OVR_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {if(OVR.voltage_set_temp >= OVR_V_MAX[2]) OVR.voltage_set_temp = OVR_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {if(OVR.voltage_set_temp >= OVR_V_MAX[3]) OVR.voltage_set_temp = OVR_V_MAX[3];}

	} else if(value == DOWN_KEY) {
		OVR.voltage_set_temp -= number_digit[Screen_Position.select % 4];

		if(GPT.pt_secondary == 100)			 {if(OVR.voltage_set_temp <= OVR_V_MIN[0] || OVR.voltage_set_temp >= 60000)	OVR.voltage_set_temp = OVR_V_MIN[0];}
		else if(GPT.pt_secondary == 110) {if(OVR.voltage_set_temp <= OVR_V_MIN[1] || OVR.voltage_set_temp >= 60000)	OVR.voltage_set_temp = OVR_V_MIN[1];}
		else if(GPT.pt_secondary == 120) {if(OVR.voltage_set_temp <= OVR_V_MIN[2] || OVR.voltage_set_temp >= 60000)	OVR.voltage_set_temp = OVR_V_MIN[2];}
		else if(GPT.pt_secondary == 190) {if(OVR.voltage_set_temp <= OVR_V_MIN[3] || OVR.voltage_set_temp >= 60000)	OVR.voltage_set_temp = OVR_V_MIN[3];}

	} else if(value == ENTER_KEY) {
			OVR.delay_time_temp = OVR.delay_time;
			if(OVR.delay_time_temp < OVR_T_MIN || OVR_T_MAX < OVR.delay_time_temp) OVR.delay_time_temp = OVR_T_MAX;

			Screen_Position.y = 55;
			Screen_Position.x = 6;
			Screen_Position.select = 1;
	}
}

void menu_55_06(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.1f[SEC]\0",(float)(OVR.delay_time*0.1));
	sprintf(str[1],"TIME     :%5.1f[SEC]\0",(float)(OVR.delay_time_temp*0.1));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		OVR.delay_time_temp += number_digit[Screen_Position.select % 2];
		if(OVR.delay_time_temp >= OVR_T_MAX) OVR.delay_time_temp = OVR_T_MAX;
	} else if(value == DOWN_KEY) {
		OVR.delay_time_temp -= number_digit[Screen_Position.select % 2];
		if(OVR.delay_time_temp <= OVR_T_MIN || OVR.delay_time_temp >= 60000)	OVR.delay_time_temp = OVR_T_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 55;
			Screen_Position.x = 7;
			Screen_Position.select = 0;
			OVR.do_relay_temp = OVR.do_relay;
	}
}

void menu_55_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OVR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OVR.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OVR.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 55;
		Screen_Position.x = 8;
		cursor_move(0, 0);//cursor off
	}
}

void menu_55_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"59   NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"59   MODE :  DEF    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 55;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_55_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"VOLTAGE : %5.1f [V]%c\0",(float)(OVR.voltage_set_temp*0.1),ENTER);
	sprintf(str[1],"TIME    : %5.1f[SEC]\0",(float)(OVR.delay_time_temp*0.1));

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 55;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
	}
}

void menu_55_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OVR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 55;
			Screen_Position.x = 11;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_55_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OVR.mode = OVR.mode_temp;
			OVR.voltage_set = OVR.voltage_set_temp;
			OVR.delay_time = OVR.delay_time_temp;
			OVR.do_relay = OVR.do_relay_temp;
			if(setting_save(&OVR.use, OVR_USE, 6))
			{
				setting_load(&OVR.use, 6, OVR_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 55;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 55;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_55_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  59  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 44;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_55_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  59  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 44;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_56_05(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  : %5.1f [V]\0",(float)(OVR.voltage_set_temp2*0.1));
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(OVR.voltage_set_temp*0.1));
	
	if(display) {
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 15);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) {Screen_Position.select = 3;}
	} else if(value == UP_KEY) {
		OVR.voltage_set_temp += number_digit[Screen_Position.select % 4];

		if(GPT.pt_secondary == 100)			 {if(OVR.voltage_set_temp >= OVR_V_MAX[0]) OVR.voltage_set_temp = OVR_V_MAX[0];}
		else if(GPT.pt_secondary == 110) {if(OVR.voltage_set_temp >= OVR_V_MAX[1]) OVR.voltage_set_temp = OVR_V_MAX[1];}
		else if(GPT.pt_secondary == 120) {if(OVR.voltage_set_temp >= OVR_V_MAX[2]) OVR.voltage_set_temp = OVR_V_MAX[2];}
		else if(GPT.pt_secondary == 190) {if(OVR.voltage_set_temp >= OVR_V_MAX[3]) OVR.voltage_set_temp = OVR_V_MAX[3];}

	} else if(value == DOWN_KEY) {
		OVR.voltage_set_temp -= number_digit[Screen_Position.select % 4];

		if(GPT.pt_secondary == 100)			 {if(OVR.voltage_set_temp <= OVR_V_MIN[0] || OVR.voltage_set_temp >= 60000)	OVR.voltage_set_temp = OVR_V_MIN[0];}
		else if(GPT.pt_secondary == 110) {if(OVR.voltage_set_temp <= OVR_V_MIN[1] || OVR.voltage_set_temp >= 60000)	OVR.voltage_set_temp = OVR_V_MIN[1];}
		else if(GPT.pt_secondary == 120) {if(OVR.voltage_set_temp <= OVR_V_MIN[2] || OVR.voltage_set_temp >= 60000)	OVR.voltage_set_temp = OVR_V_MIN[2];}
		else if(GPT.pt_secondary == 190) {if(OVR.voltage_set_temp <= OVR_V_MIN[3] || OVR.voltage_set_temp >= 60000)	OVR.voltage_set_temp = OVR_V_MIN[3];}

	} else if(value == ENTER_KEY) {
			OVR.time_lever_temp = OVR.time_lever;
			if(OVR.time_lever_temp < OVR_TL_MIN || OVR_TL_MAX < OVR.time_lever_temp) OVR.time_lever_temp = OVR_TL_MAX;

			Screen_Position.y = 56;
			Screen_Position.x = 6;
			Screen_Position.select = 1;
	}
}

void menu_56_06(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.2f     \0",(float)(OVR.time_lever*0.01));
	sprintf(str[1],"OPLEVEL  :%5.2f     \0",(float)(OVR.time_lever_temp*0.01));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
	} else if(value == UP_KEY) {
		OVR.time_lever_temp += number_digit[Screen_Position.select % 3];
		if(OVR.time_lever_temp >= OVR_TL_MAX) OVR.time_lever_temp = OVR_TL_MAX;
	} else if(value == DOWN_KEY) {
		OVR.time_lever_temp -= number_digit[Screen_Position.select % 3];
		if(OVR.time_lever_temp <= OVR_TL_MIN || OVR.time_lever_temp >= 60000)	OVR.time_lever_temp = OVR_TL_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 56;
		Screen_Position.x = 7;
		Screen_Position.select = 0;
		OVR.do_relay_temp = OVR.do_relay;
	}
}

void menu_56_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OVR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OVR.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OVR.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 56;
		Screen_Position.x = 8;
		cursor_move(0, 0);//cursor off
	}
}

void menu_56_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"59   NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"59   MODE :  INV    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 56;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_56_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"VOLTAGE : %5.1f [V]%c\0",(float)(OVR.voltage_set_temp*0.1),ENTER);
	sprintf(str[1],"OPLEVEL : %5.2f     \0",(float)(OVR.time_lever_temp*0.01));

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 56;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
	}
}

void menu_56_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OVR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OVR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 56;
			Screen_Position.x = 11;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_56_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OVR.mode = OVR.mode_temp;
			OVR.voltage_set = OVR.voltage_set_temp;
			OVR.time_lever = OVR.time_lever_temp;
			OVR.do_relay = OVR.do_relay_temp;
			if(setting_save(&OVR.use, OVR_USE, 6))
			{
				setting_load(&OVR.use, 6, OVR_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 56;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 56;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_56_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  59  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 44;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_56_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  59  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 44;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_57_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		if((OVGR.mode != INSTANT) && (OVGR.mode != INVERSE))
		{
			OVGR.mode = INVERSE;
		}
		sprintf(str[0],"  64    MODE : %s \0", OVGR.mode == INSTANT? "INST" : "INV ");  
		sprintf(str[1],"  INST?     INV?    \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 6);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			OVGR.mode_temp = INSTANT;
			Screen_Position.y = 57;
			Screen_Position.x = 5;
			Screen_Position.select = 2;
		} else if(Screen_Position.select == 1) {
			OVGR.mode_temp = INVERSE;
			Screen_Position.y = 58;
			Screen_Position.x = 5;
			Screen_Position.select = 2;
		}
		OVGR.voltage_set_temp = OVGR.voltage_set;
		OVGR.voltage_set_temp2 = OVGR.voltage_set; //전압의 경우 PT 2차 설정이 바뀌면 저장된 MIN, MAX 값을 조정하여 보여주기 위해 // 전류의 졍우는 2차를 설정하면 모두 DEFAULT 값으로 초기화 하므로 필요 없음
		if(GPT.pt_tertiary == 110)			{if(OVGR.voltage_set_temp < OVGR_V_MIN[0] || OVGR_V_MAX[0] < OVGR.voltage_set_temp) {OVGR.voltage_set_temp = OVGR_V_MAX[0]; OVGR.voltage_set_temp2 = OVGR_V_MAX[0];}}
		else if(GPT.pt_tertiary == 120) {if(OVGR.voltage_set_temp < OVGR_V_MIN[1] || OVGR_V_MAX[1] < OVGR.voltage_set_temp) {OVGR.voltage_set_temp = OVGR_V_MAX[1]; OVGR.voltage_set_temp2 = OVGR_V_MAX[1];}}
		else if(GPT.pt_tertiary == 190) {if(OVGR.voltage_set_temp < OVGR_V_MIN[2] || OVGR_V_MAX[2] < OVGR.voltage_set_temp) {OVGR.voltage_set_temp = OVGR_V_MAX[2]; OVGR.voltage_set_temp2 = OVGR_V_MAX[2];}}
	}
}

void menu_57_05(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  : %5.1f [V]\0",(float)(OVGR.voltage_set_temp2*0.1));
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(OVGR.voltage_set_temp*0.1));
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 15);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) {Screen_Position.select = 2;}
	} else if(value == UP_KEY) {
		OVGR.voltage_set_temp += number_digit[Screen_Position.select % 3];

		if(GPT.pt_tertiary == 110) 			{if(OVGR.voltage_set_temp >= OVGR_V_MAX[0]) OVGR.voltage_set_temp = OVGR_V_MAX[0];}
		else if(GPT.pt_tertiary == 120) {if(OVGR.voltage_set_temp >= OVGR_V_MAX[1]) OVGR.voltage_set_temp = OVGR_V_MAX[1];}
		else if(GPT.pt_tertiary == 190) {if(OVGR.voltage_set_temp >= OVGR_V_MAX[2]) OVGR.voltage_set_temp = OVGR_V_MAX[2];}

	} else if(value == DOWN_KEY) {
		OVGR.voltage_set_temp -= number_digit[Screen_Position.select % 3];

		if(GPT.pt_tertiary == 110) 			{if(OVGR.voltage_set_temp <= OVGR_V_MIN[0] || OVGR.voltage_set_temp >= 60000)	OVGR.voltage_set_temp = OVGR_V_MIN[0];}
		else if(GPT.pt_tertiary == 120) {if(OVGR.voltage_set_temp <= OVGR_V_MIN[1] || OVGR.voltage_set_temp >= 60000)	OVGR.voltage_set_temp = OVGR_V_MIN[1];}
		else if(GPT.pt_tertiary == 190) {if(OVGR.voltage_set_temp <= OVGR_V_MIN[2] || OVGR.voltage_set_temp >= 60000)	OVGR.voltage_set_temp = OVGR_V_MIN[2];}

	} else if(value == ENTER_KEY) {
			Screen_Position.y = 57;
			Screen_Position.x = 6;
			Screen_Position.select = 0;
			OVGR.do_relay_temp = OVGR.do_relay;
	}
}

void menu_57_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OVGR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OVGR.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OVGR.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 57;
		Screen_Position.x = 7;
		cursor_move(0, 0);//cursor off
	}
}

void menu_57_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"64   NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"64   MODE :  INST   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 57;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
	}
}

void menu_57_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"VOLTAGE : %5.1f [V]%c\0",(float)(OVGR.voltage_set_temp*0.1),ENTER);
	sprintf(str[1],"                    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 57;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_57_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OVGR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 57;
			Screen_Position.x = 10;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_57_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OVGR.mode = OVGR.mode_temp;
			OVGR.voltage_set = OVGR.voltage_set_temp;
			OVGR.do_relay = OVGR.do_relay_temp;
			if(setting_save(&OVGR.use, OVGR_USE, 5))
			{
				setting_load(&OVGR.use, 5, OVGR_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 57;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 57;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_57_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  64  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 45;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_57_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  64  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 45;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}


void menu_58_05(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  : %5.1f [V]\0",(float)(OVGR.voltage_set_temp2*0.1));
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(OVGR.voltage_set_temp*0.1));
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 15);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) {Screen_Position.select = 2;}
	} else if(value == UP_KEY) {
		OVGR.voltage_set_temp += number_digit[Screen_Position.select % 3];

		if(GPT.pt_tertiary == 110) 			{if(OVGR.voltage_set_temp >= OVGR_V_MAX[0]) OVGR.voltage_set_temp = OVGR_V_MAX[0];}
		else if(GPT.pt_tertiary == 120) {if(OVGR.voltage_set_temp >= OVGR_V_MAX[1]) OVGR.voltage_set_temp = OVGR_V_MAX[1];}
		else if(GPT.pt_tertiary == 190) {if(OVGR.voltage_set_temp >= OVGR_V_MAX[2]) OVGR.voltage_set_temp = OVGR_V_MAX[2];}

	} else if(value == DOWN_KEY) {
		OVGR.voltage_set_temp -= number_digit[Screen_Position.select % 3];

		if(GPT.pt_tertiary == 110) 			{if(OVGR.voltage_set_temp <= OVGR_V_MIN[0] || OVGR.voltage_set_temp >= 60000)	OVGR.voltage_set_temp = OVGR_V_MIN[0];}
		else if(GPT.pt_tertiary == 120) {if(OVGR.voltage_set_temp <= OVGR_V_MIN[1] || OVGR.voltage_set_temp >= 60000)	OVGR.voltage_set_temp = OVGR_V_MIN[1];}
		else if(GPT.pt_tertiary == 190) {if(OVGR.voltage_set_temp <= OVGR_V_MIN[2] || OVGR.voltage_set_temp >= 60000)	OVGR.voltage_set_temp = OVGR_V_MIN[2];}

	} else if(value == ENTER_KEY) {
			OVGR.time_lever_temp = OVGR.time_lever;
			if(OVGR.time_lever_temp < OVGR_TL_MIN || OVGR_TL_MAX < OVGR.time_lever_temp) OVGR.time_lever_temp = OVGR_TL_MAX;

			Screen_Position.y = 58;
			Screen_Position.x = 6;
			Screen_Position.select = 2;
	}
}

void menu_58_06(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.2f     \0",(float)(OVGR.time_lever*0.01));
	sprintf(str[1],"OPLEVEL  :%5.2f     \0",(float)(OVGR.time_lever_temp*0.01));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
	} else if(value == UP_KEY) {
		OVGR.time_lever_temp += number_digit[Screen_Position.select % 3];
		if(OVGR.time_lever_temp >= OVGR_TL_MAX) OVGR.time_lever_temp = OVGR_TL_MAX;
	} else if(value == DOWN_KEY) {
		OVGR.time_lever_temp -= number_digit[Screen_Position.select % 3];
		if(OVGR.time_lever_temp <= OVGR_TL_MIN || OVGR.time_lever_temp >= 60000)	OVGR.time_lever_temp = OVGR_TL_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 58;
		Screen_Position.x = 7;
		Screen_Position.select = 0;
		OVGR.do_relay_temp = OVGR.do_relay;
	}
}

void menu_58_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OVGR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		OVGR.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		OVGR.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 58;
		Screen_Position.x = 8;
		cursor_move(0, 0);//cursor off
	}
}

void menu_58_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"64   NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"64   MODE :  INV    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 58;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_58_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"VOLTAGE : %5.1f [V]%c\0",(float)(OVGR.voltage_set_temp*0.1),ENTER);
	sprintf(str[1],"OPLEVEL : %5.2f     \0",(float)(OVGR.time_lever_temp*0.01));

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 58;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
	}
}

void menu_58_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(OVGR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(OVGR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 58;
			Screen_Position.x = 11;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_58_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OVGR.mode = OVGR.mode_temp;
			OVGR.voltage_set = OVGR.voltage_set_temp;
			OVGR.time_lever = OVGR.time_lever_temp;
			OVGR.do_relay = OVGR.do_relay_temp;
			if(setting_save(&OVGR.use, OVGR_USE, 5))
			{
				setting_load(&OVGR.use, 5, OVGR_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 58;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 58;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_58_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  64  SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 45;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_58_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  64  SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 45;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_59_04(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"PRE-SET  : %5.1f [A]\0",(float)(DGR.current_set*0.1));
		sprintf(str[1],"CURRENT  : %5.1f [A]\0",(float)(DGR.current_set_temp*0.1));
		
		if(display) {
			screen_frame2(str);
			if (Screen_Position.select == 0) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 15);
			}
			return;
		}
		
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 2;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
		} else if(value == UP_KEY) {
			DGR.current_set_temp += number_digit[Screen_Position.select % 2];
			if(DGR.current_set_temp >= DGR_I_MAX[0]) DGR.current_set_temp = DGR_I_MAX[0];
		} else if(value == DOWN_KEY) {
			DGR.current_set_temp -= number_digit[Screen_Position.select % 2];
			if(DGR.current_set_temp <= DGR_I_MIN[0] || DGR.current_set_temp >= 60000)	DGR.current_set_temp = DGR_I_MIN[0];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 59;
				Screen_Position.x = 5;
				Screen_Position.select = 2;

				DGR.voltage_set_temp = DGR.voltage_set;
				DGR.voltage_set_temp2 = DGR.voltage_set; //전압의 경우 PT 2차 설정이 바뀌면 저장된 MIN, MAX 값을 조정하여 보여주기 위해 // 전류의 졍우는 2차를 설정하면 모두 DEFAULT 값으로 초기화 하므로 필요 없음
				if(GPT.pt_tertiary == 110)			{if(DGR.voltage_set_temp < DGR_V_MIN[0] || DGR_V_MAX[0] < DGR.voltage_set_temp) {DGR.voltage_set_temp = DGR_V_MAX[0]; DGR.voltage_set_temp2 = DGR_V_MAX[0];}}
				else if(GPT.pt_tertiary == 120) {if(DGR.voltage_set_temp < DGR_V_MIN[1] || DGR_V_MAX[1] < DGR.voltage_set_temp) {DGR.voltage_set_temp = DGR_V_MAX[1]; DGR.voltage_set_temp2 = DGR_V_MAX[1];}}
				else if(GPT.pt_tertiary == 190) {if(DGR.voltage_set_temp < DGR_V_MIN[2] || DGR_V_MAX[2] < DGR.voltage_set_temp) {DGR.voltage_set_temp = DGR_V_MAX[2]; DGR.voltage_set_temp2 = DGR_V_MAX[2];}}
		}
	}
	else {
		sprintf(str[0],"PRE-SET  : %5.2f [A]\0",(float)(DGR.current_set*0.01));
		sprintf(str[1],"CURRENT  : %5.2f [A]\0",(float)(DGR.current_set_temp*0.01));
		
		if(display) {
			screen_frame2(str);
			if (Screen_Position.select == 0) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 15);
			}
			return;
		}
		
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 2;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
		} else if(value == UP_KEY) {
			DGR.current_set_temp += number_digit[Screen_Position.select % 2];
			if(DGR.current_set_temp >= DGR_I_MAX[1]) DGR.current_set_temp = DGR_I_MAX[1];
		} else if(value == DOWN_KEY) {
			DGR.current_set_temp -= number_digit[Screen_Position.select % 2];
			if(DGR.current_set_temp <= DGR_I_MIN[1] || DGR.current_set_temp >= 60000)	DGR.current_set_temp = DGR_I_MIN[1];
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 59;
				Screen_Position.x = 5;
				Screen_Position.select = 2;

				DGR.voltage_set_temp = DGR.voltage_set;
				DGR.voltage_set_temp2 = DGR.voltage_set;	//전압의 경우 PT 2차 설정이 바뀌면 저장된 MIN, MAX 값을 조정하여 보여주기 위해 // 전류의 졍우는 2차를 설정하면 모두 DEFAULT 값으로 초기화 하므로 필요 없음
				if(GPT.pt_tertiary == 110)			{if(DGR.voltage_set_temp < DGR_V_MIN[0] || DGR_V_MAX[0] < DGR.voltage_set_temp) {DGR.voltage_set_temp = DGR_V_MAX[0]; DGR.voltage_set_temp2 = DGR_V_MAX[0];}}
				else if(GPT.pt_tertiary == 120) {if(DGR.voltage_set_temp < DGR_V_MIN[1] || DGR_V_MAX[1] < DGR.voltage_set_temp) {DGR.voltage_set_temp = DGR_V_MAX[1]; DGR.voltage_set_temp2 = DGR_V_MAX[1];}}
				else if(GPT.pt_tertiary == 190) {if(DGR.voltage_set_temp < DGR_V_MIN[2] || DGR_V_MAX[2] < DGR.voltage_set_temp) {DGR.voltage_set_temp = DGR_V_MAX[2]; DGR.voltage_set_temp2 = DGR_V_MAX[2];}}
		}
	}
}

void menu_59_05(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  : %5.1f [V]\0",(float)(DGR.voltage_set_temp2*0.1));
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(DGR.voltage_set_temp*0.1));
	
	if(display) {
		screen_frame2(str);

		if (Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 15);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) {Screen_Position.select = 2;}
	} else if(value == UP_KEY) {
		DGR.voltage_set_temp += number_digit[Screen_Position.select % 3];

		if(GPT.pt_tertiary == 110) 			{if(DGR.voltage_set_temp >= DGR_V_MAX[0]) DGR.voltage_set_temp = DGR_V_MAX[0];}
		else if(GPT.pt_tertiary == 120) {if(DGR.voltage_set_temp >= DGR_V_MAX[1]) DGR.voltage_set_temp = DGR_V_MAX[1];}
		else if(GPT.pt_tertiary == 190) {if(DGR.voltage_set_temp >= DGR_V_MAX[2]) DGR.voltage_set_temp = DGR_V_MAX[2];}

	} else if(value == DOWN_KEY) {
		DGR.voltage_set_temp -= number_digit[Screen_Position.select % 3];

		if(GPT.pt_tertiary == 110) 			{if(DGR.voltage_set_temp <= DGR_V_MIN[0] || DGR.voltage_set_temp >= 60000)	DGR.voltage_set_temp = DGR_V_MIN[0];}
		else if(GPT.pt_tertiary == 120) {if(DGR.voltage_set_temp <= DGR_V_MIN[1] || DGR.voltage_set_temp >= 60000)	DGR.voltage_set_temp = DGR_V_MIN[1];}
		else if(GPT.pt_tertiary == 190) {if(DGR.voltage_set_temp <= DGR_V_MIN[2] || DGR.voltage_set_temp >= 60000)	DGR.voltage_set_temp = DGR_V_MIN[2];}

	} else if(value == ENTER_KEY) {
			DGR.delay_time_temp = DGR.delay_time;
			if(DGR.delay_time_temp < DGR_T_MIN || DGR_T_MAX < DGR.delay_time_temp) DGR.delay_time_temp = DGR_T_MAX;

			Screen_Position.y = 59;
			Screen_Position.x = 6;
			Screen_Position.select = 1;
	}
}

void menu_59_06(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.1f[SEC]\0",(float)(DGR.delay_time*0.1));
	sprintf(str[1],"TIME     :%5.1f[SEC]\0",(float)(DGR.delay_time_temp*0.1));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		DGR.delay_time_temp += number_digit[Screen_Position.select % 2];
		if(DGR.delay_time_temp >= DGR_T_MAX) DGR.delay_time_temp = DGR_T_MAX;
	} else if(value == DOWN_KEY) {
		DGR.delay_time_temp -= number_digit[Screen_Position.select % 2];
		if(DGR.delay_time_temp <= DGR_T_MIN || DGR.delay_time_temp >= 60000)	DGR.delay_time_temp = DGR_T_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 59;
			Screen_Position.x = 7;
			Screen_Position.select = 1;
			DGR.angle_set_temp = DGR.angle_set;
	}
}

void menu_59_07(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%4d %c    \0",DGR.angle_set,DGREE);
	sprintf(str[1],"ANGLE    :%4d %c    \0",DGR.angle_set_temp,DGREE);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		DGR.angle_set_temp += number_digit[Screen_Position.select % 2];
		if(DGR.angle_set_temp >= DGR_A_MAX) DGR.angle_set_temp = DGR_A_MAX;
	} else if(value == DOWN_KEY) {
		DGR.angle_set_temp -= number_digit[Screen_Position.select % 2];
		if(DGR.angle_set_temp <= DGR_A_MIN || DGR.angle_set_temp >= 60000)	DGR.angle_set_temp = DGR_A_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 59;
		Screen_Position.x = 8;
		Screen_Position.select = 0;
		DGR.do_relay_temp = DGR.do_relay;
	}
}

void menu_59_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(DGR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		DGR.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		DGR.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 59;
		Screen_Position.x = 9;
		cursor_move(0, 0);//cursor off
	}
}

void menu_59_09(unsigned int value, int display)
{
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"67GD NEW SETTING : %c\0", ENTER);
		sprintf(str[1],"CURRENT  : %5.1f [A]\0",(float)(DGR.current_set_temp*0.1));
	} else { //1A
		sprintf(str[0],"67GD NEW SETTING : %c\0", ENTER);
		sprintf(str[1],"CURRENT  : %5.2f [A]\0",(float)(DGR.current_set_temp*0.01));
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 59;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
	}
}

void menu_59_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"VOLTAGE  :%5.1f [V]%c\0",(float)(DGR.voltage_set_temp*0.1),ENTER);
	sprintf(str[1],"TIME     :%5.1f[SEC]\0",(float)(DGR.delay_time_temp*0.1));

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 59;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
	}
}

void menu_59_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"ANGLE    :%4d %c   %c\0",DGR.angle_set_temp,DGREE,ENTER);
	sprintf(str[1],"                    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 59;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
	}
}

void menu_59_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(DGR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(DGR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 59;
			Screen_Position.x = 13;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_59_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			DGR.current_set = DGR.current_set_temp;
			DGR.voltage_set = DGR.voltage_set_temp;
			DGR.angle_set = DGR.angle_set_temp;
			DGR.delay_time = DGR.delay_time_temp;
			DGR.do_relay = DGR.do_relay_temp;
			if(setting_save(&DGR.use, DGR_USE, 6))
			{
				setting_load(&DGR.use, 6, DGR_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 59;
			Screen_Position.x = 14;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 59;
			Screen_Position.x = 15;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_59_14(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 67GD SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 45;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_59_15(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 67GD SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 45;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_60_04(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET : %5.1f [mA]\0",(float)(SGR.current_set*0.1));
	sprintf(str[1],"CURRENT : %5.1f [mA]\0",(float)(SGR.current_set_temp*0.1));
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		SGR.current_set_temp += number_digit[Screen_Position.select % 2];
		if(SGR.current_set_temp >= SGR_I_MAX) SGR.current_set_temp = SGR_I_MAX;
	} else if(value == DOWN_KEY) {
		SGR.current_set_temp -= number_digit[Screen_Position.select % 2];
		if(SGR.current_set_temp <= SGR_I_MIN || SGR.current_set_temp >= 60000)	SGR.current_set_temp = SGR_I_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 60;
			Screen_Position.x = 5;
			Screen_Position.select = 2;
	
			SGR.voltage_set_temp = SGR.voltage_set;
			SGR.voltage_set_temp2 = SGR.voltage_set;	//전압의 경우 PT 2차 설정이 바뀌면 저장된 MIN, MAX 값을 조정하여 보여주기 위해 // 전류의 졍우는 2차를 설정하면 모두 DEFAULT 값으로 초기화 하므로 필요 없음
			if(GPT.pt_tertiary == 110)			{if(SGR.voltage_set_temp < SGR_V_MIN[0] || SGR_V_MAX[0] < SGR.voltage_set_temp) {SGR.voltage_set_temp = SGR_V_MAX[0]; SGR.voltage_set_temp2 = SGR_V_MAX[0];}}
			else if(GPT.pt_tertiary == 120) {if(SGR.voltage_set_temp < SGR_V_MIN[1] || SGR_V_MAX[1] < SGR.voltage_set_temp) {SGR.voltage_set_temp = SGR_V_MAX[1]; SGR.voltage_set_temp2 = SGR_V_MAX[1];}}
			else if(GPT.pt_tertiary == 190) {if(SGR.voltage_set_temp < SGR_V_MIN[2] || SGR_V_MAX[2] < SGR.voltage_set_temp) {SGR.voltage_set_temp = SGR_V_MAX[2]; SGR.voltage_set_temp2 = SGR_V_MAX[2];}}
	}
}

void menu_60_05(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  : %5.1f [V]\0",(float)(SGR.voltage_set_temp2*0.1));
	sprintf(str[1],"VOLTAGE  : %5.1f [V]\0",(float)(SGR.voltage_set_temp*0.1));
	
	if(display) {
		screen_frame2(str);

		if (Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 15);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) {Screen_Position.select = 2;}
	} else if(value == UP_KEY) {
		SGR.voltage_set_temp += number_digit[Screen_Position.select % 3];

		if(GPT.pt_tertiary == 110) 			{if(SGR.voltage_set_temp >= SGR_V_MAX[0]) SGR.voltage_set_temp = SGR_V_MAX[0];}
		else if(GPT.pt_tertiary == 120) {if(SGR.voltage_set_temp >= SGR_V_MAX[1]) SGR.voltage_set_temp = SGR_V_MAX[1];}
		else if(GPT.pt_tertiary == 190) {if(SGR.voltage_set_temp >= SGR_V_MAX[2]) SGR.voltage_set_temp = SGR_V_MAX[2];}

	} else if(value == DOWN_KEY) {
		SGR.voltage_set_temp -= number_digit[Screen_Position.select % 3];

		if(GPT.pt_tertiary == 110) 			{if(SGR.voltage_set_temp <= SGR_V_MIN[0] || SGR.voltage_set_temp >= 60000)	SGR.voltage_set_temp = SGR_V_MIN[0];}
		else if(GPT.pt_tertiary == 120) {if(SGR.voltage_set_temp <= SGR_V_MIN[1] || SGR.voltage_set_temp >= 60000)	SGR.voltage_set_temp = SGR_V_MIN[1];}
		else if(GPT.pt_tertiary == 190) {if(SGR.voltage_set_temp <= SGR_V_MIN[2] || SGR.voltage_set_temp >= 60000)	SGR.voltage_set_temp = SGR_V_MIN[2];}

	} else if(value == ENTER_KEY) {
			SGR.delay_time_temp = SGR.delay_time;
			if(SGR.delay_time_temp < SGR_T_MIN || SGR_T_MAX < SGR.delay_time_temp) SGR.delay_time_temp = SGR_T_MAX;

			Screen_Position.y = 60;
			Screen_Position.x = 6;
			Screen_Position.select = 1;
	}
}

void menu_60_06(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%5.1f[SEC]\0",(float)(SGR.delay_time*0.1));
	sprintf(str[1],"TIME     :%5.1f[SEC]\0",(float)(SGR.delay_time_temp*0.1));

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		SGR.delay_time_temp += number_digit[Screen_Position.select % 2];
		if(SGR.delay_time_temp >= SGR_T_MAX) SGR.delay_time_temp = SGR_T_MAX;
	} else if(value == DOWN_KEY) {
		SGR.delay_time_temp -= number_digit[Screen_Position.select % 2];
		if(SGR.delay_time_temp <= SGR_T_MIN || SGR.delay_time_temp >= 60000)	SGR.delay_time_temp = SGR_T_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 60;
			Screen_Position.x = 7;
			Screen_Position.select = 1;
			SGR.angle_set_temp = SGR.angle_set;
	}
}

void menu_60_07(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET  :%4d %c    \0",SGR.angle_set,DGREE);
	sprintf(str[1],"ANGLE    :%4d %c    \0",SGR.angle_set_temp,DGREE);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		SGR.angle_set_temp += number_digit[Screen_Position.select % 2];
		if(SGR.angle_set_temp >= SGR_A_MAX) SGR.angle_set_temp = SGR_A_MAX;
	} else if(value == DOWN_KEY) {
		SGR.angle_set_temp -= number_digit[Screen_Position.select % 2];
		if(SGR.angle_set_temp <= SGR_A_MIN || SGR.angle_set_temp >= 60000)	SGR.angle_set_temp = SGR_A_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 60;
		Screen_Position.x = 8;
		Screen_Position.select = 0;
		SGR.do_relay_temp = SGR.do_relay;
	}
}

void menu_60_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(SGR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		SGR.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		SGR.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 60;
		Screen_Position.x = 9;
		cursor_move(0, 0);//cursor off
	}
}

void menu_60_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"67GS NEW SETTING : %c\0", ENTER);
	sprintf(str[1],"CURRENT : %5.1f [mA]\0",(float)(SGR.current_set_temp*0.1));

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 60;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
	}
}

void menu_60_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"VOLTAGE  :%5.1f [V]%c\0",(float)(SGR.voltage_set_temp*0.1),ENTER);
	sprintf(str[1],"TIME     :%5.1f[SEC]\0",(float)(SGR.delay_time_temp*0.1));

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 60;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
	}
}

void menu_60_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"ANGLE    :%4d %c   %c\0",SGR.angle_set_temp,DGREE,ENTER);
	sprintf(str[1],"                    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 60;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
	}
}

void menu_60_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(SGR.do_relay_temp & 0x01)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x02)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x04)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x08)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x10)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x20)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x40)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x80)? FULLDOT: ' ',
			(SGR.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 60;
			Screen_Position.x = 13;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_60_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			SGR.current_set = SGR.current_set_temp;
			SGR.voltage_set = SGR.voltage_set_temp;
			SGR.angle_set = SGR.angle_set_temp;
			SGR.delay_time = SGR.delay_time_temp;
			SGR.do_relay = SGR.do_relay_temp;
			if(setting_save(&SGR.use, SGR_USE, 6))
			{
				setting_load(&SGR.use, 6, SGR_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 60;
			Screen_Position.x = 14;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 60;
			Screen_Position.x = 15;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_60_14(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 67GS SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 45;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_60_15(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " 67GS SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 45;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}


void menu_80_03(unsigned int value, int display)  //2015.02.17
{
	const char *str[2] = {
			"[SELECT RLYS ] ?   \1\0",
			"[CT,PT RATIO ] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 81;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == FACTORY_KEY) {
		Screen_Position.y = 150;
		Screen_Position.x = 1;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
		 //설정값 저장
			OCR50_1.use_temp = OCR50_1.use;
			OCR50_2.use_temp = OCR50_2.use;
			OCR51_1.use_temp = OCR51_1.use;
			OCR51_2.use_temp = OCR51_2.use;
			OCGR50.use_temp = OCGR50.use;
			OCGR51.use_temp = OCGR51.use;
			UVR_1.use_temp = UVR_1.use;
			UVR_2.use_temp = UVR_2.use;
			UVR_3.use_temp = UVR_3.use;
			P47.use_temp = P47.use;
			N47.use_temp = N47.use;
			OVR.use_temp = OVR.use;
			OVGR.use_temp = OVGR.use;
			DGR.use_temp = DGR.use;
			SGR.use_temp = SGR.use;

			Screen_Position.y = 80;
			Screen_Position.x = 4;
			Screen_Position.select = 0;
		} else if(Screen_Position.select == 1) {
			//설정값 저장
			CPT.ct_primary_temp = CPT.ct_primary;
			CPT.nct_primary_temp = CPT.nct_primary;
			CPT.pt_primary_temp = CPT.pt_primary;
			CPT.rated_current_temp = CPT.rated_current;

			Screen_Position.y = 81;
			Screen_Position.x = 4;
			Screen_Position.select = 3;
		}
	}
}

void menu_80_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"[50   : %s] ? \1\0", OCR50_1.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		sprintf(str[1],"[51   : %s] ? \2\0", OCR51_1.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 11;
		Screen_Position.select = 1;
		cursor_move(1, 18);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 5;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
		//50 ENABLE TOGGLE
			if((OCR50_1.use_temp == ENABLE)||(OCR50_2.use_temp == ENABLE))	{OCR50_1.use_temp = DISABLE; OCR50_2.use_temp = DISABLE;}
			else																														{OCR50_1.use_temp = ENABLE; OCR50_2.use_temp = ENABLE;}
		} else if(Screen_Position.select == 1) {
		//51 ENABLE TOGGLE
			if((OCR51_1.use_temp == ENABLE)||(OCR51_2.use_temp == ENABLE))	{OCR51_1.use_temp = DISABLE; OCR51_2.use_temp = DISABLE;}
			else																														{OCR51_1.use_temp = ENABLE; OCR51_2.use_temp = ENABLE;}
		}
	}
}

void menu_80_05(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"[50G  : %s] ? \1\0", OCGR50.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		sprintf(str[1],"[51G  : %s] ? \2\0", OCGR51.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 11;
		Screen_Position.select = 1;
		cursor_move(1, 18);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 6;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
		//50G ENABLE TOGGLE
			if((OCGR50.use_temp == ENABLE))	{OCGR50.use_temp = DISABLE;}
			else														{OCGR50.use_temp = ENABLE;}
		} else if(Screen_Position.select == 1) {
		//51G ENABLE TOGGLE
			if((OCGR51.use_temp == ENABLE))	{OCGR51.use_temp = DISABLE;}
			else														{OCGR51.use_temp = ENABLE;}
		}
	}
}

void menu_80_06(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"[27R  : %s] ? \1\0", UVR_1.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		sprintf(str[1],"[27M  : %s] ? \2\0", UVR_2.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 11;
		Screen_Position.select = 1;
		cursor_move(1, 18);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 7;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
		//27R ENABLE TOGGLE
			if((UVR_1.use_temp == ENABLE))	{UVR_1.use_temp = DISABLE;}
			else														{UVR_1.use_temp = ENABLE;}
		} else if(Screen_Position.select == 1) {
		//27M ENABLE TOGGLE
			if((UVR_2.use_temp == ENABLE))	{UVR_2.use_temp = DISABLE;}
			else														{UVR_2.use_temp = ENABLE;}
		}
	}
}

void menu_80_07(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"[27S  : %s] ? \1\0", UVR_3.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		sprintf(str[1],"[47P  : %s] ? \2\0", P47.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 11;
		Screen_Position.select = 1;
		cursor_move(1, 18);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 8;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
		//27S ENABLE TOGGLE
			if((UVR_3.use_temp == ENABLE))	{UVR_3.use_temp = DISABLE;}
			else														{UVR_3.use_temp = ENABLE;}
		} else if(Screen_Position.select == 1) {
		//47P ENABLE TOGGLE
			if((P47.use_temp == ENABLE))	{P47.use_temp = DISABLE;}
			else													{P47.use_temp = ENABLE;}
		}
	}
}

void menu_80_08(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"[47N  : %s] ? \1\0", N47.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		sprintf(str[1],"[59   : %s] ? \2\0", OVR.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 11;
		Screen_Position.select = 1;
		cursor_move(1, 18);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 9;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
		//47N ENABLE TOGGLE
			if((N47.use_temp == ENABLE))	{N47.use_temp = DISABLE;}
			else													{N47.use_temp = ENABLE;}
		} else if(Screen_Position.select == 1) {
		//59 ENABLE TOGGLE
			if((OVR.use_temp == ENABLE))	{OVR.use_temp = DISABLE;}
			else													{OVR.use_temp = ENABLE;}
		}
	}
}

void menu_80_09(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"[64   : %s] ? \1\0", OVGR.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		sprintf(str[1],"[67GD : %s] ? \2\0", DGR.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 17);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 11;
		Screen_Position.select = 1;
		cursor_move(1, 18);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 10;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
		//64 ENABLE TOGGLE
			if((OVGR.use_temp == ENABLE))	{OVGR.use_temp = DISABLE;}
			else													{OVGR.use_temp = ENABLE;}
		} else if(Screen_Position.select == 1) {
		//67GD ENABLE TOGGLE
			if((DGR.use_temp == ENABLE))	{DGR.use_temp = DISABLE;}
			else													{DGR.use_temp = ENABLE;}
		}
	}
}

void menu_80_10(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"[67GS : %s] ? \1\0", SGR.use_temp == ENABLE ? "ENABLE " : "DISABLE");  
		sprintf(str[1],"                   \2\0");  
		screen_frame2(str);
		cursor_move(0, 17);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 11;
		Screen_Position.select = 1;
		cursor_move(1, 18);
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 4;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		//67GS ENABLE TOGGLE
		if((SGR.use_temp == ENABLE))	{SGR.use_temp = DISABLE;}
		else													{SGR.use_temp = ENABLE;}
	}
}

void menu_80_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
		 //설정값 저장
			if(OCR50_1.use != OCR50_1.use_temp) 
			{
				OCR50_1.use = OCR50_1.use_temp;	
				if(setting_save(&OCR50_1.use, OCR50_1_USE, 5))
				{
					setting_load(&OCR50_1.use, 5, OCR50_1_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(OCR50_2.use != OCR50_2.use_temp)	
			{
				OCR50_2.use = OCR50_2.use_temp;	
				if(setting_save(&OCR50_2.use, OCR50_2_USE, 5))
				{
					setting_load(&OCR50_2.use, 5, OCR50_2_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(OCR51_1.use != OCR51_1.use_temp)	
			{
				OCR51_1.use = OCR51_1.use_temp;	
				if(setting_save(&OCR51_1.use, OCR51_1_USE, 5))
				{
					setting_load(&OCR51_1.use, 5, OCR51_1_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(OCR51_2.use != OCR51_2.use_temp)	
			{
				OCR51_2.use = OCR51_2.use_temp;	
				if(setting_save(&OCR51_2.use, OCR51_2_USE, 5))
				{
					setting_load(&OCR51_2.use, 5, OCR51_2_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(OCGR50.use != OCGR50.use_temp)
			{
				OCGR50.use = OCGR50.use_temp;
				if(setting_save(&OCGR50.use, OCGR50_USE, 5))
				{
					setting_load(&OCGR50.use, 5, OCGR50_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(OCGR51.use != OCGR51.use_temp)
			{
				OCGR51.use = OCGR51.use_temp;
				if(setting_save(&OCGR51.use, OCGR51_USE, 5))
				{
					setting_load(&OCGR51.use, 5, OCGR51_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(UVR_1.use != UVR_1.use_temp)
			{
				UVR_1.use = UVR_1.use_temp;
				if(setting_save(&UVR_1.use, UVR_1_USE, 4))
				{
					setting_load(&UVR_1.use, 4, UVR_1_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(UVR_2.use != UVR_2.use_temp)
			{
				UVR_2.use = UVR_2.use_temp;
				if(setting_save(&UVR_2.use, UVR_2_USE, 4))
				{
					setting_load(&UVR_2.use, 4, UVR_2_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(UVR_3.use != UVR_3.use_temp)
			{
				UVR_3.use = UVR_3.use_temp;
				if(setting_save(&UVR_3.use, UVR_3_USE, 4))
				{
					setting_load(&UVR_3.use, 4, UVR_3_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(P47.use != P47.use_temp)
			{
				P47.use = P47.use_temp;
				if(setting_save(&P47.use, P47_USE, 4))
				{
					setting_load(&P47.use, 4, P47_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(N47.use != N47.use_temp)
			{
				N47.use = N47.use_temp;
				if(setting_save(&N47.use, N47_USE, 4))
				{
					setting_load(&N47.use, 4, N47_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(OVR.use != OVR.use_temp)
			{
				OVR.use = OVR.use_temp;
				if(setting_save(&OVR.use, OVR_USE, 6))
				{
					setting_load(&OVR.use, 6, OVR_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(OVGR.use != OVGR.use_temp)
			{
				OVGR.use = OVGR.use_temp;
				if(setting_save(&OVGR.use, OVGR_USE, 5))
				{
					setting_load(&OVGR.use, 5, OVGR_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(DGR.use != DGR.use_temp)
			{
				DGR.use = DGR.use_temp;
				if(setting_save(&DGR.use, DGR_USE, 6))
				{
					setting_load(&DGR.use, 6, DGR_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}
			if(SGR.use != SGR.use_temp)
			{
				SGR.use = SGR.use_temp;
				if(setting_save(&SGR.use, SGR_USE, 6))
				{
					setting_load(&SGR.use, 6, SGR_USE);
				}
				else
				{
					//FLASH WRITE ERROR pop up 화면
				}
			}

			Screen_Position.y = 80;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 80;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_80_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "RELAY SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 80;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_80_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "RELAY SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 80;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_81_03(unsigned int value, int display)
{
	const char *str[2] = {
			"[TIME SET    ] ?   \1\0",
			"[AUX RLY TEST] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 82;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			TIME.year_temp = TIME.year;
			TIME.month_temp = TIME.month;
			TIME.day_temp = TIME.day;
			TIME.hour_temp = TIME.hour;
			TIME.minute_temp = TIME.minute;
			TIME.second_temp = TIME.second;

			TIME.year_temp2 = TIME.year;
			TIME.month_temp2 = TIME.month;
			TIME.day_temp2 = TIME.day;
			TIME.hour_temp2 = TIME.hour;
			TIME.minute_temp2 = TIME.minute;
			TIME.second_temp2 = TIME.second;

			Screen_Position.y = 82;
			Screen_Position.x = 4;
			Screen_Position.select = 1;
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 83;
			Screen_Position.x = 4;
			Screen_Position.select = 0;
		}
	}
}

void menu_81_04(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"CT_Ph  :  %4d/5[A] \0",CPT.ct_primary);
		sprintf(str[1],"NEW    :  %4d/5[A] \0",CPT.ct_primary_temp);
	} else {
		sprintf(str[0],"CT_Ph  :  %4d/1[A] \0",CPT.ct_primary);
		sprintf(str[1],"NEW    :  %4d/1[A] \0",CPT.ct_primary_temp);
	}
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 13);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		CPT.ct_primary_temp += number_digit[Screen_Position.select % 4];
		if(CPT.ct_primary_temp >= CT_MAX) CPT.ct_primary_temp = CT_MAX;
	} else if(value == DOWN_KEY) {
		CPT.ct_primary_temp -= number_digit[Screen_Position.select % 4];
		if(CPT.ct_primary_temp <= CT_MIN || CPT.ct_primary_temp >= 60000)	CPT.ct_primary_temp = CT_MIN;
	} else if(value == ENTER_KEY) {
		if(SGR.use==ENABLE) {
			Screen_Position.y = 81;
			Screen_Position.x = 6;
			Screen_Position.select = 5;
		}
		else {
			Screen_Position.y = 81;
			Screen_Position.x = 5;
			Screen_Position.select = 3;
		}
	}
}

void menu_81_05(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	if(CORE.rated_ct == CT_5A) {
		sprintf(str[0],"CT_Io  :  %4d/5[A] \0",CPT.nct_primary);
		sprintf(str[1],"NEW    :  %4d/5[A] \0",CPT.nct_primary_temp);
	} else {
		sprintf(str[0],"CT_Io  :  %4d/1[A] \0",CPT.nct_primary);
		sprintf(str[1],"NEW    :  %4d/1[A] \0",CPT.nct_primary_temp);
	}
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 13);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		CPT.ct_primary_temp += number_digit[Screen_Position.select % 4];
		if(CPT.nct_primary_temp >= CT_MAX) CPT.nct_primary_temp = CT_MAX;
	} else if(value == DOWN_KEY) {
		CPT.nct_primary_temp -= number_digit[Screen_Position.select % 4];
		if(CPT.nct_primary_temp <= CT_MIN || CPT.nct_primary_temp >= 60000)	CPT.nct_primary_temp = CT_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 81;
			Screen_Position.x = 6;
			Screen_Position.select = 5;
	}
}

void menu_81_06(unsigned int value, int display)
{
	const unsigned long number_digit[6] = {100000, 10000, 1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PT_Ph :%6ld/%3d[V]\0",CPT.pt_primary,GPT.pt_secondary);
	sprintf(str[1],"NEW   :%6ld/%3d[V]\0",CPT.pt_primary_temp,GPT.pt_secondary);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 7);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 8);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 9);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 4) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 5) {
			cursor_move(1, 12);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 6;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 5) Screen_Position.select = 5;
	} else if(value == UP_KEY) {
		CPT.pt_primary_temp += number_digit[Screen_Position.select % 6];
		if(CPT.pt_primary_temp >= PT_MAX) CPT.pt_primary_temp = PT_MAX;
	} else if(value == DOWN_KEY) {
		CPT.pt_primary_temp -= number_digit[Screen_Position.select % 6];
		if(CPT.pt_primary_temp <= PT_MIN || CPT.pt_primary_temp >= 600000l)	CPT.pt_primary_temp = PT_MIN;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 81;
			Screen_Position.x = 7;
			Screen_Position.select = 3;
	}
}

void menu_81_07(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {10000, 1000, 100, 10};
	const unsigned int number_digit2[3] = {100, 10, 1};
	char str[2][22];
	unsigned int max_digit;

	if(CPT.rated_current >= 1000) {
		sprintf(str[0],"RATED CURR : %4d[A]\0",CPT.rated_current/10);
	} else{
		sprintf(str[0],"RATED CURR : %4.1f[A]\0",((float)CPT.rated_current*0.1));
	}

	if(CPT.rated_current_temp >= 1000) {
		sprintf(str[1],"NEW        : %4d[A]\0",CPT.rated_current_temp/10);
		
		if(display) {
			screen_frame2(str);
			if (Screen_Position.select == 0) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 15);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 16);
			}
			return;
		}
		
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 4;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
		} else if(value == UP_KEY) {
			CPT.rated_current_temp += number_digit[Screen_Position.select % 4];
			if(CPT.rated_current_temp >= 50000) CPT.rated_current_temp = 50000;
		} else if(value == DOWN_KEY) {
			if(CPT.rated_current_temp / 10000)			max_digit = 0;
			else if(CPT.rated_current_temp / 1000)  max_digit = 1;
			else if(CPT.rated_current_temp / 100)   max_digit = 2;
			else if(CPT.rated_current_temp / 10)    max_digit = 3;
			if(Screen_Position.select < max_digit) Screen_Position.select = max_digit;

			if(CPT.rated_current_temp < 1000 || CPT.rated_current_temp > 50000 ) {
				CPT.rated_current_temp = 999;
				cursor_move(1, 13);
			} else {
				CPT.rated_current_temp -= number_digit[Screen_Position.select % 4];
			}
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 81;
				Screen_Position.x = 8;
				cursor_move(0, 0);
		}
	} 
	else 
	{
		sprintf(str[1],"NEW        : %4.1f[A]\0",((float)CPT.rated_current_temp*0.1));

		if(display) {
			screen_frame2(str);
			if (Screen_Position.select == 0) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 16);
			}
			return;
		}
		
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 3;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
		} else if(value == UP_KEY) {
			CPT.rated_current_temp += number_digit2[Screen_Position.select % 3];
		} else if(value == DOWN_KEY) {
			CPT.rated_current_temp -= number_digit2[Screen_Position.select % 3];
			if(CPT.rated_current_temp <= 0 || CPT.rated_current_temp > 50000)	CPT.rated_current_temp = 0;
		} else if(value == ENTER_KEY) {
				Screen_Position.y = 81;
				Screen_Position.x = 8;
				cursor_move(0, 0);
		}
	}
}

void menu_81_08(unsigned int value, int display)
{
	char str[2][22];

	if(SGR.use==ENABLE) {
		if(CORE.rated_ct == CT_5A) {
			sprintf(str[0],"CT_Ph  : %4d/5 [A]%c\0",CPT.ct_primary_temp,ENTER);
			sprintf(str[1],"PT_Ph :%6ld/%3d[V]\0",CPT.pt_primary_temp,GPT.pt_secondary);
		} else {
			sprintf(str[0],"CT_Ph  : %4d/1 [A]%c\0",CPT.ct_primary_temp,ENTER);
			sprintf(str[1],"PT_Ph :%6ld/%3d[V]\0",CPT.pt_primary_temp,GPT.pt_secondary);
		}
	} else {
		if(CORE.rated_ct == CT_5A) {
			sprintf(str[0],"CT_Ph  : %4d/5 [A]%c\0",CPT.ct_primary_temp,ENTER);
			sprintf(str[1],"CT_Io  : %4d/5 [A] \0",CPT.nct_primary_temp);
		} else {
			sprintf(str[0],"CT_Ph  : %4d/1 [A]%c\0",CPT.ct_primary_temp,ENTER);
			sprintf(str[1],"CT_Io  : %4d/1 [A] \0",CPT.nct_primary_temp);
		}
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 81;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
	}
}

void menu_81_09(unsigned int value, int display)
{
	char str[2][22];

	if(SGR.use==ENABLE) {
		if(CPT.rated_current_temp >= 1000)	{sprintf(str[0],"RATED CURR : %4d[A]\0",CPT.rated_current_temp/10);}
		else																{sprintf(str[0],"RATED CURR : %4.1f[A]\0",((float)CPT.rated_current_temp*0.1));}
		sprintf(str[1],"                    \0");
	} else {
		sprintf(str[0],"PT_Ph :%6ld/%3d[V]\0",CPT.pt_primary_temp,GPT.pt_secondary);
		if(CPT.rated_current_temp >= 1000)	{sprintf(str[1],"RATED CURR : %4d[A]\0",CPT.rated_current_temp/10);}
		else																{sprintf(str[1],"RATED CURR : %4.1f[A]\0",((float)CPT.rated_current_temp*0.1));}
	}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 81;
			Screen_Position.x = 10;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_81_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			//설정값 저장
			CPT.ct_primary = CPT.ct_primary_temp;
			CPT.nct_primary = CPT.nct_primary_temp;
			CPT.pt_primary = CPT.pt_primary_temp;
			CPT.pt_primary_low = CPT.pt_primary_temp & 0xffff;
			CPT.pt_primary_temp >>=16;
			CPT.pt_primary_high = CPT.pt_primary_temp & 0xffff;
			CPT.rated_current = CPT.rated_current_temp;
			if(setting_save(&CPT.ct_primary, CT_PRIMARY, 5))
			{
				setting_load(&CPT.ct_primary, 5, CT_PRIMARY);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 81;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 81;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_81_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "RATIO SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 80;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_81_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "RATIO SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 80;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_82_03(unsigned int value, int display)
{
	const char *str[2] = {
			"[POWER RESET ] ?   \1\0",
			"[EVENT RESET ] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 83;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 85;
			Screen_Position.x = 4;
			Screen_Position.select = 1;
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 86;
			Screen_Position.x = 4;
			Screen_Position.select = 1;
		}
	}
}

void menu_82_04(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {10, 1};
	char str[2][22];

	sprintf(str[0],"YEAR   :  %02d        \0",TIME.year_temp2);
	sprintf(str[1],"NEW    :  %2d        \0",TIME.year_temp);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		TIME.year_temp += number_digit[Screen_Position.select % 2];
		if(TIME.year_temp >= 99) TIME.year_temp = 99;
	} else if(value == DOWN_KEY) {
		TIME.year_temp -= number_digit[Screen_Position.select % 2];
		if(TIME.year_temp <= 0 || TIME.year_temp >= 60000)	TIME.year_temp = 0;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 82;
		Screen_Position.x = 5;
		Screen_Position.select = 1;
	}
}

void menu_82_05(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {10, 1};
	char str[2][22];

	sprintf(str[0],"MONTH  :  %02d        \0",TIME.month_temp2);
	sprintf(str[1],"NEW    :  %2d        \0",TIME.month_temp);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		TIME.month_temp += number_digit[Screen_Position.select % 2];
		if(TIME.month_temp >= 12) TIME.month_temp = 12;
	} else if(value == DOWN_KEY) {
		TIME.month_temp -= number_digit[Screen_Position.select % 2];
		if(TIME.month_temp <= 1 || TIME.month_temp >= 60000)	TIME.month_temp = 1;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 82;
		Screen_Position.x = 6;
		Screen_Position.select = 1;
	}
}

void menu_82_06(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {10, 1};
	char str[2][22];

	sprintf(str[0],"DAYS   :  %02d        \0",TIME.day_temp2);
	sprintf(str[1],"NEW    :  %2d        \0",TIME.day_temp);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		TIME.day_temp += number_digit[Screen_Position.select % 2];
		if(TIME.day_temp >= 31) TIME.day_temp = 31;
	} else if(value == DOWN_KEY) {
		TIME.day_temp -= number_digit[Screen_Position.select % 2];
		if(TIME.day_temp <= 1 || TIME.day_temp >= 60000)	TIME.day_temp = 1;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 82;
		Screen_Position.x = 7;
		Screen_Position.select = 1;
	}
}

void menu_82_07(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {10, 1};
	char str[2][22];

	sprintf(str[0],"HOUR   :  %02d        \0",TIME.hour_temp2);
	sprintf(str[1],"NEW    :  %2d        \0",TIME.hour_temp);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		TIME.hour_temp += number_digit[Screen_Position.select % 2];
		if(TIME.hour_temp >= 23) TIME.hour_temp = 23;
	} else if(value == DOWN_KEY) {
		TIME.hour_temp -= number_digit[Screen_Position.select % 2];
		if(TIME.hour_temp <= 0 || TIME.hour_temp >= 60000)	TIME.hour_temp = 0;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 82;
		Screen_Position.x = 8;
		Screen_Position.select = 1;
	}
}

void menu_82_08(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {10, 1};
	char str[2][22];

	sprintf(str[0],"MIN    :  %02d        \0",TIME.minute_temp2);
	sprintf(str[1],"NEW    :  %2d        \0",TIME.minute_temp);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		TIME.minute_temp += number_digit[Screen_Position.select % 2];
		if(TIME.minute_temp >= 59) TIME.minute_temp = 59;
	} else if(value == DOWN_KEY) {
		TIME.minute_temp -= number_digit[Screen_Position.select % 2];
		if(TIME.minute_temp <= 0 || TIME.minute_temp >= 60000)	TIME.minute_temp = 0;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 82;
		Screen_Position.x = 9;
		Screen_Position.select = 1;
	}
}

void menu_82_09(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {10, 1};
	char str[2][22];

	sprintf(str[0],"SECOND :  %02d        \0",TIME.second_temp2);
	sprintf(str[1],"NEW    :  %2d        \0",TIME.second_temp);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		TIME.second_temp += number_digit[Screen_Position.select % 2];
		if(TIME.second_temp >= 59) TIME.second_temp = 59;
	} else if(value == DOWN_KEY) {
		TIME.second_temp -= number_digit[Screen_Position.select % 2];
		if(TIME.second_temp <= 0 || TIME.second_temp >= 60000)	TIME.second_temp = 0;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 82;
		Screen_Position.x = 10;
		cursor_move(0, 0);
	}
}

void menu_82_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"  DATE:%2d.%2d.%2d    %c\0",TIME.year_temp,TIME.month_temp,TIME.day_temp,ENTER);
	sprintf(str[1],"  TIME:%2d:%2d:%2d     \0",TIME.hour_temp,TIME.minute_temp,TIME.second_temp);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 82;
			Screen_Position.x = 11;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_82_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			//설정값 저장

			Screen_Position.y = 82;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 82;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_82_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " TIME SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 81;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_82_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " TIME SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 81;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_83_03(unsigned int value, int display)
{
	const char *str[2] = {
			"[Vo_max RESET] ?   \1\0",
			"[ADDRESS SET ] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 84;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 87;
			Screen_Position.x = 4;
			Screen_Position.select = 1;
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 88;
			Screen_Position.x = 4;
			Screen_Position.select = 2;
			ADDRESS.address_temp = ADDRESS.address;
		}
	}
}

void menu_83_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"  < AUX RLY TEST > \1\0");  
		sprintf(str[1],"  AUTO?    MANUAL?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 6);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == UP_KEY) {
		Screen_Position.y = 81;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 83;
			Screen_Position.x = 5;
			Screen_Position.select = 1;
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 84;
			Screen_Position.x = 5;
			Screen_Position.select = 0;
		}
	}
}

void menu_83_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO TEST ? [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 83;
			Screen_Position.x = 6;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 83;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_83_07(unsigned int, int);
void menu_83_06(unsigned int value, int display)
{
	unsigned int i;
	char str[2][22];
	static unsigned int auto_relay_test= 1;
	static unsigned int do_on_bit[10] =
		{0x2000, 0x4000, 0x0800, 0x1000, 0x0200, 0x0400, 0x0080, 0x0020, 0x0008, 0x0080};

	sprintf(str[0]," 1 2 3 4 5 6 7 8 9 0\0");

	if(display) {
		for(i = 0; i < 10; i++) {
			sprintf(str[1],"R%c %c %c %c %c %c %c %c %c %c\0",
				(auto_relay_test & 0x01)? FULLDOT: ' ',
				(auto_relay_test & 0x02)? FULLDOT: ' ',
				(auto_relay_test & 0x04)? FULLDOT: ' ',
				(auto_relay_test & 0x08)? FULLDOT: ' ',
				(auto_relay_test & 0x10)? FULLDOT: ' ',
				(auto_relay_test & 0x20)? FULLDOT: ' ',
				(auto_relay_test & 0x40)? FULLDOT: ' ',
				(auto_relay_test & 0x80)? FULLDOT: ' ',
				(auto_relay_test & 0x100)? FULLDOT:' ',
				(auto_relay_test & 0x200)? FULLDOT:' '
			);
			screen_frame2(str);
			auto_relay_test <<= 1;

			DO_Output(do_on_bit[i]);
			delay_us(500000);
			DO_Output((i==8)? 0x40: 0);
			delay_us(500000);
		}
		
		auto_relay_test = 1;
		menu_83_07(0xff, 1);
	}
	
	if(value) {
		Screen_Position.y = 83;
		Screen_Position.x = 7;
		Screen_Position.select = 0;		
	}
}

void menu_83_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   TEST COMPLETE !  \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
		Screen_Position.y = 83;
		Screen_Position.x = 4;
		Screen_Position.select = 0;
	}
}

void menu_83_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   TEST CANCELED !  \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 83;
			Screen_Position.x = 4;
			Screen_Position.select = 0;
	}
}

void menu_84_03(unsigned int value, int display)
{
	const char *str[2] = {
			"[AUTO DISPLAY] ?   \1\0",
			"[CAPTURE WAVE] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 85;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			AUTO_DISPLAY.mode_temp = AUTO_DISPLAY.mode;

			Screen_Position.y = 89;
			Screen_Position.x = 4;
			cursor_move(0, 0);
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 90;
			Screen_Position.x = 4;
			Screen_Position.select = 1;
		}
	}
}

void menu_84_05(unsigned int value, int display)
{
	char str[2][22];
	unsigned int manual_relay_test = 0;

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(manual_relay_test & 0x01)? FULLDOT: ' ',
			(manual_relay_test & 0x02)? FULLDOT: ' ',
			(manual_relay_test & 0x04)? FULLDOT: ' ',
			(manual_relay_test & 0x08)? FULLDOT: ' ',
			(manual_relay_test & 0x10)? FULLDOT: ' ',
			(manual_relay_test & 0x20)? FULLDOT: ' ',
			(manual_relay_test & 0x40)? FULLDOT: ' ',
			(manual_relay_test & 0x80)? FULLDOT: ' ',
			(manual_relay_test & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		manual_relay_test |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		manual_relay_test &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 84;
		Screen_Position.x = 6;
		Screen_Position.select = 1;
		cursor_move(1, 18);
	}
}

void menu_84_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO TEST ? [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 84;
			Screen_Position.x = 7;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 84;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_84_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  PRESS ENTER KEY   \0");
	sprintf(str[1], "     TO RESET !     \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 84;
			Screen_Position.x = 8;
			Screen_Position.select = 0;
	}
}

void menu_84_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   TEST COMPLETE !  \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 83;
			Screen_Position.x = 4;
			Screen_Position.select = 0;
	}
}

void menu_84_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   TEST CANCELED !  \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 83;
			Screen_Position.x = 4;
			Screen_Position.select = 0;
	}
}

void menu_85_03(unsigned int value, int display)
{
	const char *str[2] = {
			"[PASSWORD SET] ?   \1\0",
			"[(G)PT RATING] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 86;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			PASSWORD.real_temp = PASSWORD.real;

			Screen_Position.y = 91;
			Screen_Position.x = 4;
			Screen_Position.select = 3;
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 92;
			Screen_Position.x = 4;
			(GPT.pt_secondary == 100) ? (Screen_Position.select = 0) : (GPT.pt_secondary == 110) ? (Screen_Position.select = 1) : (GPT.pt_secondary == 120) ? (Screen_Position.select = 2) : (Screen_Position.select = 3);
		}
	}
}

void menu_85_04(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"DATA WILL BE LOST ! \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 85;
			Screen_Position.x = 5;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 85;
			Screen_Position.x = 6;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_85_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  RESET COMPLETE !  \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 82;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_85_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  RESET CANCELED !  \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 82;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_86_03(unsigned int value, int display)
{
	const char *str[2] = {
			"[SUPERVISION ] ?   \1\0",
			"[LOCAL CTRL  ] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 87;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
//			Screen_Position.y = 93;
//			Screen_Position.x = 4;
//			Screen_Position.select = 1;
		} else if(Screen_Position.select == 1) {
			LOCAL_CTRL.use_temp = LOCAL_CTRL.use;
			LOCAL_CTRL.close_time_temp = LOCAL_CTRL.close_time;
			LOCAL_CTRL.open_time_temp = LOCAL_CTRL.open_time;
			
			Screen_Position.y = 95;
			Screen_Position.x = 4;
			(LOCAL_CTRL.use == ENABLE) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
		}
	}
}

void menu_86_04(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"DATA WILL BE LOST ! \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 86;
			Screen_Position.x = 5;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 86;
			Screen_Position.x = 6;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_86_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  RESET COMPLETE !  \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 82;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_86_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  RESET CANCELED !  \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 82;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_87_03(unsigned int value, int display)
{
	const char *str[2] = {
			"[D/SGR ANGLE ] ?   \1\0",
			"[UVR MODE SET] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 88;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 97;
			Screen_Position.x = 4;
			Screen_Position.select = 1;
		} else if(Screen_Position.select == 1) {
			UVR_MODE.setting1_temp = UVR_MODE.setting1;
			UVR_MODE.setting2_temp = UVR_MODE.setting2;
			UVR_MODE.setting3_temp = UVR_MODE.setting3;

			Screen_Position.y = 98;
			Screen_Position.x = 4;
			(UVR_MODE.setting1 == AUTO) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
		}
	}
}

void menu_87_04(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"DATA WILL BE LOST ! \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 87;
			Screen_Position.x = 5;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 87;
			Screen_Position.x = 6;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_87_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  RESET COMPLETE !  \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 83;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_87_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  RESET CANCELED !  \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 83;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_88_03(unsigned int value, int display)
{
	const char *str[2] = {
			"[R-HOUR RESET] ?   \1\0",
			"[3PHASE DISP.] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 89;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 99;
			Screen_Position.x = 4;
			Screen_Position.select = 1;
		} else if(Screen_Position.select == 1) {
			DISPLAY_3PHASE.use_temp = DISPLAY_3PHASE.use;

			Screen_Position.y = 100;
			Screen_Position.x = 4;
			(DISPLAY_3PHASE.use == ENABLE) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
		}
	}
}

void menu_88_04(unsigned int value, int display)
{
	const unsigned int number_digit[3] = {100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE ADDRESS : %4d  \0",ADDRESS.address);
	sprintf(str[1],"NEW ADDRESS : %4d  \0",ADDRESS.address_temp);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 16);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 2;
	} else if(value == UP_KEY) {
		ADDRESS.address_temp += number_digit[Screen_Position.select % 3];
		if(ADDRESS.address_temp >= 254) ADDRESS.address_temp = 254;
	} else if(value == DOWN_KEY) {
		ADDRESS.address_temp -= number_digit[Screen_Position.select % 3];
		if(ADDRESS.address_temp <= 1 || ADDRESS.address_temp >= 60000)	ADDRESS.address_temp = 1;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 88;
		Screen_Position.x = 5;
		cursor_move(0, 0);
	}
}

void menu_88_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"ADDRESS :     %4d %c\0",ADDRESS.address_temp,ENTER);
	sprintf(str[1],"                    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 88;
			Screen_Position.x = 6;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_88_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			ADDRESS.address = ADDRESS.address_temp;
			if(setting_save(&ADDRESS.address, MOD_ADDR, 1))
			{
				setting_load(&ADDRESS.address, 1, MOD_ADDR);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 88;
			Screen_Position.x = 7;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 88;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_88_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " ADDR SET COMPLETE !\0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 83;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_88_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " ADDR SET CANCELED !\0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 83;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_89_03(unsigned int value, int display)
{
	const char *str[2] = {
			"[MODBUS COMM.] ?   \1\0",
			"[OCR MODE SET] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 90;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			MODBUS.baudrate_temp = MODBUS.baudrate;
			MODBUS.delay_temp = MODBUS.delay/5;
	
			Screen_Position.y = 101;
			Screen_Position.x = 4;
			Screen_Position.select = 0;
		} else if(Screen_Position.select == 1) {
			OCR_MODE_SET.ocr_mode_temp = OCR_MODE_SET.ocr_mode;
			OCR_MODE_SET.di_number_temp = OCR_MODE_SET.di_number;

			Screen_Position.y = 103;
			Screen_Position.x = 4;
			(OCR_MODE_SET.ocr_mode == NORMAL) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
		}
	}
}

void menu_89_04(unsigned int value, int display)
{
	char *disp_mode[6]={"   OFF   ","    I    ","    V    ","   PWR   ","   I,V   "," I,V,PWR "};
	char str[2][22];

	if(display) {
		sprintf(str[0],"PRE-SET :%s \1\0", disp_mode[AUTO_DISPLAY.mode]);  
		sprintf(str[1],"DISPLAY :%s \2\0", disp_mode[AUTO_DISPLAY.mode_temp]);  
		screen_frame2(str);
		return;
	}

	if(value == UP_KEY) {
		AUTO_DISPLAY.mode_temp += 1;
		if(AUTO_DISPLAY.mode_temp > 5)	{AUTO_DISPLAY.mode_temp = 0;}
	} else if(value == DOWN_KEY) {
		AUTO_DISPLAY.mode_temp -= 1;
		if(AUTO_DISPLAY.mode_temp > 60000)	{AUTO_DISPLAY.mode_temp = 5;}
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 89;
			Screen_Position.x = 5;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_89_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			AUTO_DISPLAY.mode = AUTO_DISPLAY.mode_temp;
			if(setting_save(&AUTO_DISPLAY.mode, AUTO_DISP_MODE, 1))
			{
				setting_load(&AUTO_DISPLAY.mode, 1, AUTO_DISP_MODE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 89;
			Screen_Position.x = 6;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 89;
			Screen_Position.x = 7;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_89_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  SET COMPLETE !    \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 84;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_89_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  SET CANCELED !    \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 84;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_90_03(unsigned int value, int display)
{
	const char *str[2] = {
			"[SYSTEM ALARM] ?   \1\0",
			"                   \2\0"
	};

	if(display) {
		screen_frame3(str);
		cursor_move(0, 15);
		return;
	}

	if(value == UP_KEY) {
		Screen_Position.y = 41;
		Screen_Position.x = 2;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		SYSTEM_ALARM.use_temp = SYSTEM_ALARM.use;
		SYSTEM_ALARM.do_relay_temp = SYSTEM_ALARM.do_relay;
		
		Screen_Position.y = 104;
		Screen_Position.x = 4;
		(SYSTEM_ALARM.use == ENABLE) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
	}
}

void menu_90_04(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"DATA WILL BE CHANGED\0");
	sprintf(str[1],"WANT TO CAPTURE? Y/N\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 17);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 19);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 90;
			Screen_Position.x = 5;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 90;
			Screen_Position.x = 6;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_90_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " CAPTURE COMPLETE ! \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 84;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_90_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " CAPTURE CANCELED ! \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 84;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_91_04(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0]," PRE PASS :  #%4d  \0",PASSWORD.real);
	sprintf(str[1]," NEW PASS :  #%4d  \0",PASSWORD.real_temp);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 14);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 16);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		PASSWORD.real_temp += number_digit[Screen_Position.select % 4];
		if(PASSWORD.real_temp >= 9999) PASSWORD.real_temp = 9999;
	} else if(value == DOWN_KEY) {
		PASSWORD.real_temp -= number_digit[Screen_Position.select % 4];
		if(PASSWORD.real_temp <= 1111 || PASSWORD.real_temp >= 60000)	PASSWORD.real_temp = 1111;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 91;
		Screen_Position.x = 5;
		cursor_move(0, 0);
	}
}

void menu_91_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0]," PASSWORD : #%4d  %c\0",PASSWORD.real_temp,ENTER);
	sprintf(str[1],"                    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 91;
			Screen_Position.x = 6;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_91_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			PASSWORD.real = PASSWORD.real_temp;
			if(setting_save(&PASSWORD.real, PASSWORD1, 1))
			{
				setting_load(&PASSWORD.real, 1, PASSWORD1);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 91;
			Screen_Position.x = 7;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 91;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_91_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET COMPLETE !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 85;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_91_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET CANCELED !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 85;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_92_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		GPT.pt_secondary_temp = GPT.pt_secondary;
		if((GPT.pt_secondary_temp != 100) && (GPT.pt_secondary_temp != 110) && (GPT.pt_secondary_temp != 120) && (GPT.pt_secondary_temp != 190))
		{
			GPT.pt_secondary_temp = 110;
		}
		sprintf(str[0],"PRE 2ND VOLTAGE:%s\0",(GPT.pt_secondary_temp == 100) ? "100V" : (GPT.pt_secondary_temp == 110) ? "110V": (GPT.pt_secondary_temp == 120) ? "120V" : "190V");
		sprintf(str[1],"100V?110V?120V?190V?\0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 4);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 9);
		} else if(Screen_Position.select == 2) {
			cursor_move(1, 14);
		} else if(Screen_Position.select == 3) {
			cursor_move(1, 19);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			GPT.pt_secondary_temp = 100;
		} else if(Screen_Position.select == 1) {
			GPT.pt_secondary_temp = 110;
		} else if(Screen_Position.select == 2) {
			GPT.pt_secondary_temp = 120;
		} else if(Screen_Position.select == 3) {
			GPT.pt_secondary_temp = 190;
		}
		Screen_Position.y = 92;
		Screen_Position.x = 5;
		(GPT.pt_tertiary == 110) ? (Screen_Position.select = 0) : (GPT.pt_tertiary == 120) ? (Screen_Position.select = 1) : (Screen_Position.select = 2) ;
	}
}

void menu_92_05(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		GPT.pt_tertiary_temp = GPT.pt_tertiary;
		if((GPT.pt_tertiary_temp != 110) && (GPT.pt_tertiary_temp != 120) && (GPT.pt_tertiary_temp != 190))
		{
			GPT.pt_tertiary_temp = 190;
		}
		sprintf(str[0],"PRE 3RD VOLTAGE:%s\0",(GPT.pt_tertiary_temp == 110) ? "110V" : (GPT.pt_tertiary_temp == 120) ? "120V": "190V");
		sprintf(str[1]," 110V? 120V? 190V?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 5);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 11);
		} else if(Screen_Position.select == 2) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			GPT.pt_tertiary_temp = 110;
		} else if(Screen_Position.select == 1) {
			GPT.pt_tertiary_temp = 120;
		} else if(Screen_Position.select == 2) {
			GPT.pt_tertiary_temp = 190;
		}
		Screen_Position.y = 92;
		Screen_Position.x = 6;
		cursor_move(0, 0);//cursor off
	}
}

void menu_92_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"2ND VOLTAGE : %s %c\0",(GPT.pt_secondary_temp == 100) ? "100V" : (GPT.pt_secondary_temp == 110) ? "110V": (GPT.pt_secondary_temp == 120) ? "120V" : "190V", ENTER);
	sprintf(str[1],"3RD VOLTAGE : %s  \0",(GPT.pt_tertiary_temp == 110) ? "110V" : (GPT.pt_tertiary_temp == 120) ? "120V": "190V");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 92;
			Screen_Position.x = 7;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_92_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			GPT.pt_secondary = GPT.pt_secondary_temp;
			GPT.pt_tertiary = GPT.pt_tertiary_temp;
			if(setting_save(&GPT.pt_secondary, PT_SECONDARY, 2))
			{
				setting_load(&GPT.pt_secondary, 2, PT_SECONDARY);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 92;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 92;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_92_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET COMPLETE !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 85;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_92_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET CANCELED !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 85;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_95_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"PRE MODE  : %s \0", LOCAL_CTRL.use_temp == ENABLE? "ENABLE " : "DISABLE");  
		sprintf(str[1]," ENABLE?   DISABLE? \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 7);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			LOCAL_CTRL.use_temp = ENABLE;
			Screen_Position.y = 95;
			Screen_Position.x = 5;
			Screen_Position.select = 0;
		} else if(Screen_Position.select == 1) {
			LOCAL_CTRL.use_temp = DISABLE;
			Screen_Position.y = 96;
			Screen_Position.x = 5;
			cursor_move(0, 0);
		}
	}
}

void menu_95_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"PRE-SET    : %d [SEC]\0",LOCAL_CTRL.close_time);
	sprintf(str[1],"CLOSE TIME : %d [SEC]\0",LOCAL_CTRL.close_time_temp);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 13);
		}
		return;
	}

	if(value == UP_KEY) {
		LOCAL_CTRL.close_time_temp += 1;
		if(LOCAL_CTRL.close_time_temp >= 5) LOCAL_CTRL.close_time_temp = 5;
	} else if(value == DOWN_KEY) {
		LOCAL_CTRL.close_time_temp -= 1;
		if(LOCAL_CTRL.close_time_temp >= 60000)	LOCAL_CTRL.close_time_temp = 0;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 95;
			Screen_Position.x = 6;
			Screen_Position.select = 0;
	}
}

void menu_95_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"PRE-SET    : %d [SEC]\0",LOCAL_CTRL.open_time);
	sprintf(str[1],"OPEN TIME  : %d [SEC]\0",LOCAL_CTRL.open_time_temp);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 13);
		}
		return;
	}

	if(value == UP_KEY) {
		LOCAL_CTRL.open_time_temp += 1;
		if(LOCAL_CTRL.open_time_temp >= 5) LOCAL_CTRL.open_time_temp = 5;
	} else if(value == DOWN_KEY) {
		LOCAL_CTRL.open_time_temp -= 1;
		if(LOCAL_CTRL.open_time_temp >= 60000)	LOCAL_CTRL.open_time_temp = 0;
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 95;
			Screen_Position.x = 7;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_95_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			LOCAL_CTRL.use = LOCAL_CTRL.use_temp;
			LOCAL_CTRL.close_time = LOCAL_CTRL.close_time_temp;
			LOCAL_CTRL.open_time = LOCAL_CTRL.open_time_temp;
			if(setting_save(&LOCAL_CTRL.use, LOCAL_CTRL_USE, 3))
			{
				setting_load(&LOCAL_CTRL.use, 3, LOCAL_CTRL_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 95;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 95;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_95_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET COMPLETE !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 86;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_95_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET CANCELED !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 86;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_96_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"   LOCAL CONTROL   %c\0",ENTER);
	sprintf(str[1],"      DISABLE !!    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 95;
			Screen_Position.x = 7;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_97_04(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"DGR PRE-SET: %4d%c  \0",DSGR_ANGLE.dgr_angle,DGREE);
	sprintf(str[1],"DGR NEW SET: %4d%c  \0",DSGR_ANGLE.dgr_angle_temp,DGREE);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 16);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		DSGR_ANGLE.dgr_angle_temp += number_digit[Screen_Position.select % 2];
		if(DSGR_ANGLE.dgr_angle_temp >= 100) DSGR_ANGLE.dgr_angle_temp = 100;
	} else if(value == DOWN_KEY) {
		DSGR_ANGLE.dgr_angle_temp -= number_digit[Screen_Position.select % 2];
		if(DSGR_ANGLE.dgr_angle_temp <= -100)	DSGR_ANGLE.dgr_angle_temp = -100;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 97;
		Screen_Position.x = 5;
		Screen_Position.select = 1;
	}
}

void menu_97_05(unsigned int value, int display)
{
	const unsigned int number_digit[2] = {10, 1};
	char str[2][22];

	sprintf(str[0],"SGR PRE-SET: %4d%c  \0",DSGR_ANGLE.sgr_angle,DGREE);
	sprintf(str[1],"SGR NEW SET: %4d%c  \0",DSGR_ANGLE.sgr_angle_temp,DGREE);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 16);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 1) Screen_Position.select = 1;
	} else if(value == UP_KEY) {
		DSGR_ANGLE.sgr_angle_temp += number_digit[Screen_Position.select % 2];
		if(DSGR_ANGLE.sgr_angle_temp >= 100) DSGR_ANGLE.sgr_angle_temp = 100;
	} else if(value == DOWN_KEY) {
		DSGR_ANGLE.sgr_angle_temp -= number_digit[Screen_Position.select % 2];
		if(DSGR_ANGLE.sgr_angle_temp <= -100)	DSGR_ANGLE.sgr_angle_temp = -100;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 97;
		Screen_Position.x = 6;
		cursor_move(0, 0);
	}
}

void menu_97_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"DGR SET   :  %4d%c %c\0",DSGR_ANGLE.dgr_angle_temp,DGREE,ENTER);
	sprintf(str[1],"SGR SET   :  %4d%c  \0",DSGR_ANGLE.sgr_angle_temp,DGREE);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 97;
			Screen_Position.x = 7;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_97_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			DSGR_ANGLE.dgr_angle = DSGR_ANGLE.dgr_angle_temp;
			DSGR_ANGLE.sgr_angle = DSGR_ANGLE.sgr_angle_temp;
//		setting_save(&SGR_ANGLE.dgr_angle, DGR_ANGLE_COMP, 2);

			Screen_Position.y = 97;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 97;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_97_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "ANGLE SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 87;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_97_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "ANGLE SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 87;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_98_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0]," 27R  MODE : %s  \0", UVR_MODE.setting1_temp == AUTO? "AUTO  " : "MANUAL");  
		sprintf(str[1],"  AUTO?    MANUAL?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 6);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			UVR_MODE.setting1_temp = AUTO;
		} else if(Screen_Position.select == 1) {
			UVR_MODE.setting1_temp = MANUAL;
		}
		Screen_Position.y = 98;
		Screen_Position.x = 5;
		(UVR_MODE.setting2 == AUTO) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
	}
}

void menu_98_05(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0]," 27M  MODE : %s  \0", UVR_MODE.setting2_temp == AUTO? "AUTO  " : "MANUAL");  
		sprintf(str[1],"  AUTO?    MANUAL?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 6);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			UVR_MODE.setting2_temp = AUTO;
		} else if(Screen_Position.select == 1) {
			UVR_MODE.setting2_temp = MANUAL;
		}
		Screen_Position.y = 98;
		Screen_Position.x = 6;
		(UVR_MODE.setting3 == AUTO) ? (Screen_Position.select = 0) : (Screen_Position.select = 1);
	}
}

void menu_98_06(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0]," 27S  MODE : %s  \0", UVR_MODE.setting3_temp == AUTO? "AUTO  " : "MANUAL");  
		sprintf(str[1],"  AUTO?    MANUAL?  \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 6);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 17);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			UVR_MODE.setting3_temp = AUTO;
		} else if(Screen_Position.select == 1) {
			UVR_MODE.setting3_temp = MANUAL;
		}
		Screen_Position.y = 98;
		Screen_Position.x = 7;
		cursor_move(0, 0);//cursor off
	}
}

void menu_98_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"NEW SETTING :      %c\0", ENTER);
	sprintf(str[1],"27R MODE : %s   \0", UVR_MODE.setting1_temp == AUTO? "AUTO  " : "MANUAL");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 98;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
	}
}

void menu_98_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"27M MODE : %s  %c\0", UVR_MODE.setting2_temp == AUTO? "AUTO  " : "MANUAL", ENTER);
	sprintf(str[1],"27S MODE : %s   \0", UVR_MODE.setting3_temp == AUTO? "AUTO  " : "MANUAL");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 98;
			Screen_Position.x = 9;
			Screen_Position.select = 1;
	}
}

void menu_98_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			UVR_MODE.setting1 = UVR_MODE.setting1_temp;
			UVR_MODE.setting2 = UVR_MODE.setting2_temp;
			UVR_MODE.setting3 = UVR_MODE.setting3_temp;

			if(setting_save(&UVR_MODE.setting1, UVR27R_MODE, 3))
			{
				setting_load(&UVR_MODE.setting1, 3, UVR27R_MODE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 98;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 98;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_98_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " UVR SET COMPLETE ! \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 87;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_98_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], " UVR SET CANCELED ! \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 87;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_99_04(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"DATA WILL BE LOST ! \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 99;
			Screen_Position.x = 5;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 99;
			Screen_Position.x = 6;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_99_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  RESET COMPLETE !  \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 88;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_99_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  RESET CANCELED !  \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 88;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_100_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"PRE MODE  : %s \0", DISPLAY_3PHASE.use_temp == ENABLE? "ENABLE " : "DISABLE");  
		sprintf(str[1]," ENABLE?   DISABLE? \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 7);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			DISPLAY_3PHASE.use_temp = ENABLE;
			Screen_Position.y = 100;
			Screen_Position.x = 5;
			cursor_move(0, 0);
		} else if(Screen_Position.select == 1) {
			DISPLAY_3PHASE.use_temp = DISABLE;
			Screen_Position.y = 100;
			Screen_Position.x = 5;
			cursor_move(0, 0);
		}
	}
}

void menu_100_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"NEW SETTING :      %c\0", ENTER);
	sprintf(str[1],"NEW MODE : %s  \0", DISPLAY_3PHASE.use_temp == ENABLE? "ENABLE " : "DISABLE");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 100;
			Screen_Position.x = 6;
			Screen_Position.select = 1;
	}
}

void menu_100_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			DISPLAY_3PHASE.use = DISPLAY_3PHASE.use_temp;
			if(setting_save(&DISPLAY_3PHASE.use, DISP_3PHASE_USE, 1))
			{
				setting_load(&DISPLAY_3PHASE.use, 1, DISP_3PHASE_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 100;
			Screen_Position.x = 7;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 100;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_100_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET COMPLETE !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 88;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_100_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET CANCELED !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 88;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_101_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"[BAUD RATE   ] ?    \0");  
		sprintf(str[1],"[RESPONSE DLY] ?    \0");  
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 101;
			Screen_Position.x = 5;
			cursor_move(0, 0);
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 102;
			Screen_Position.x = 5;
			cursor_move(0, 0);
		}
	}
}

void menu_101_05(unsigned int value, int display)
{
	char *Baud_Rate[5] = {"  9600", " 19200", " 38400", " 57600", "115200"};
	char str[2][22];

	if(display) {
		sprintf(str[0],"PRE-SET  : %s  \1\0",Baud_Rate[MODBUS.baudrate]);  
		sprintf(str[1],"Baudrate : %s  \2\0",Baud_Rate[MODBUS.baudrate_temp]);  
		screen_frame2(str);
		return;
	}

	if(value == UP_KEY) {
		MODBUS.baudrate_temp += 1;
		if(MODBUS.baudrate_temp > 4)	{MODBUS.baudrate_temp = 0;}
	} else if(value == DOWN_KEY) {
		MODBUS.baudrate_temp -= 1;
		if(MODBUS.baudrate_temp > 60000)	{MODBUS.baudrate_temp = 4;}
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 101;
			Screen_Position.x = 6;
			cursor_move(0, 0);
	}
}

void menu_101_06(unsigned int value, int display)
{
	char *Baud_Rate[5] = {"  9600", " 19200", " 38400", " 57600", "115200"};
	char str[2][22];

	sprintf(str[0],"NEW SETTING :      %c\0", ENTER);
	sprintf(str[1],"Baudrate : %s   \0", Baud_Rate[MODBUS.baudrate_temp]);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 101;
			Screen_Position.x = 7;
			Screen_Position.select = 1;
	}
}

void menu_101_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			MODBUS.baudrate = MODBUS.baudrate_temp;
			MODBUS.delay = MODBUS.delay_temp*5;
			if(setting_save(&MODBUS.baudrate, MODBUS_BAUDRATE, 2))
			{
				setting_load(&MODBUS.baudrate, 2, MODBUS_BAUDRATE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 101;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 101;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_101_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET COMPLETE !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 89;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_101_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET CANCELED !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 89;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_102_05(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"PRE-SET    : %2d[ms]\1\0",MODBUS.delay);  
		sprintf(str[1],"Resp Delay : %2d[ms]\2\0",MODBUS.delay_temp*5);  
		screen_frame2(str);
		return;
	}

	if(value == UP_KEY) {
		MODBUS.delay_temp += 1;
		if(MODBUS.delay_temp > 10)	{MODBUS.delay_temp = 0;}
	} else if(value == DOWN_KEY) {
		MODBUS.delay_temp -= 1;
		if(MODBUS.delay_temp > 60000)	{MODBUS.delay_temp = 10;}
	} else if(value == ENTER_KEY) {
			Screen_Position.y = 102;
			Screen_Position.x = 6;
			cursor_move(0, 0);
	}
}

void menu_102_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"NEW SETTING :      %c\0", ENTER);
	sprintf(str[1],"Resp Delay: %2d[ms]  \0", MODBUS.delay_temp*5);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 101;
			Screen_Position.x = 7;
			Screen_Position.select = 1;
	}
}

void menu_103_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"PRE MODE  : %s \0", OCR_MODE_SET.ocr_mode == NORMAL? "NORMAL " : "SELECT ");  
		sprintf(str[1]," NORMAL?    SELECT? \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 7);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			OCR_MODE_SET.ocr_mode_temp = NORMAL;
			Screen_Position.y = 103;
			Screen_Position.x = 6;
			cursor_move(0, 0);
		} else if(Screen_Position.select == 1) {
			OCR_MODE_SET.ocr_mode_temp = SELECT;
			Screen_Position.y = 103;
			Screen_Position.x = 5;
			Screen_Position.select = 0;
		}
	}
}

void menu_103_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"PRE-SET   : DI %d   \0",OCR_MODE_SET.di_number);
	sprintf(str[1],"NEW DI    : DI %d   \0",OCR_MODE_SET.di_number_temp);

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == UP_KEY) {
		OCR_MODE_SET.di_number_temp += 1;
		if(OCR_MODE_SET.di_number_temp >= 8) OCR_MODE_SET.di_number_temp = 8;
	} else if(value == DOWN_KEY) {
		OCR_MODE_SET.di_number_temp -= 1;
		if(OCR_MODE_SET.di_number_temp <= 1)	OCR_MODE_SET.di_number_temp = 1;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 103;
		Screen_Position.x = 6;
		cursor_move(0, 0);
	}
}

void menu_103_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"NEW SETTING :      %c\0", ENTER);
	if(OCR_MODE_SET.ocr_mode_temp == NORMAL)	{sprintf(str[1],"       %s       \0", "NORMAL");}
	else	{sprintf(str[1],"   %s / DI %d    \0", "SELECT", OCR_MODE_SET.di_number_temp);}

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 103;
			Screen_Position.x = 7;
			Screen_Position.select = 1;
	}
}

void menu_103_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			OCR_MODE_SET.ocr_mode = OCR_MODE_SET.ocr_mode_temp;
			OCR_MODE_SET.di_number = OCR_MODE_SET.di_number_temp;
			if(setting_save(&OCR_MODE_SET.ocr_mode, OCR_MODE, 2))
			{
				setting_load(&OCR_MODE_SET.ocr_mode, 2, OCR_MODE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 103;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 103;
			Screen_Position.x = 9;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_103_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  DI SET COMPLETE ! \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 89;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_103_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  DI SET CANCELED ! \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 89;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_104_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"PRE MODE  : %s \0", SYSTEM_ALARM.use_temp == ENABLE? "ENABLE " : "DISABLE");  
		sprintf(str[1]," ENABLE?   DISABLE? \0");
		screen_frame2(str);

		if(Screen_Position.select == 0) {
			cursor_move(1, 7);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			SYSTEM_ALARM.use_temp = ENABLE;
			Screen_Position.y = 104;
			Screen_Position.x = 5;
			Screen_Position.select = 0;
		} else if(Screen_Position.select == 1) {
			SYSTEM_ALARM.use_temp = DISABLE;
			Screen_Position.y = 104;
			Screen_Position.x = 6;
			cursor_move(0, 0);
		}
	}
}

void menu_104_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(SYSTEM_ALARM.do_relay_temp & 0x01)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x02)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x04)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x08)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x10)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x20)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x40)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x80)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 8;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 8) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		SYSTEM_ALARM.do_relay_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		SYSTEM_ALARM.do_relay_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 104;
		Screen_Position.x = 7;
		cursor_move(0, 0);//cursor off
	}
}

void menu_104_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"    SYSTEM ALARM   %c\0",ENTER);
	sprintf(str[1],"      DISABLE !!    \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 104;
			Screen_Position.x = 9;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_104_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"NEW SETTING :      %c\0", ENTER);
	sprintf(str[1],"NEW MODE : %s  \0", SYSTEM_ALARM.use_temp == ENABLE? "ENABLE " : "DISABLE");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 104;
			Screen_Position.x = 8;
			cursor_move(0, 0);//cursor off
	}
}

void menu_104_08(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8 9\0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c %c\0",
			(SYSTEM_ALARM.do_relay_temp & 0x01)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x02)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x04)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x08)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x10)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x20)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x40)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x80)? FULLDOT: ' ',
			(SYSTEM_ALARM.do_relay_temp & 0x100)? FULLDOT:' '
	);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 104;
			Screen_Position.x = 9;
			Screen_Position.select = 1;
			cursor_move(1, 18);
	}
}

void menu_104_09(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			SYSTEM_ALARM.use = SYSTEM_ALARM.use_temp;
			SYSTEM_ALARM.do_relay = SYSTEM_ALARM.do_relay_temp;
		
			if(setting_save(&SYSTEM_ALARM.use, SYSTEM_ALARM_USE, 2))
			{
				setting_load(&SYSTEM_ALARM.use, 2, SYSTEM_ALARM_USE);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 104;
			Screen_Position.x = 10;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 104;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_104_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET COMPLETE !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 90;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_104_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "   SET CANCELED !   \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 90;
			Screen_Position.x = 3;
			Screen_Position.select = 0;
	}
}

void menu_150_01(unsigned int value, int display)  //2015.02.17
{
	const char *str[2] = {
			"[VERSION     ] ?   \1\0",
			"[RATED VALUE ] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 151;
		Screen_Position.x = 1;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 150;
			Screen_Position.x = 2;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 151;
			Screen_Position.x = 2;
		}
		Screen_Position.select = 0;
	}
}

void menu_150_02(unsigned int value, int display)  //2015.02.17
{
	char str[2][22];

	if(display) {
		sprintf(str[0],"   %s,%s,%s   \1\0",(CORE.Hz == Hz60)? "60Hz" : (CORE.Hz == Hz50) ? "50Hz": " ERR",
																	  	 (CORE.rated_ct == CT_5A)? " 5A" : (CORE.rated_ct == CT_1A) ? " 1A": "ERR",
																			 (CORE.gr_select == NCT_SELECT)? " NCT" : (CORE.gr_select == ZCT_SELECT) ? " ZCT": " ERR");	//2015.02.25
		sprintf(str[1], "      Ver 7.00      \0");
		screen_frame2(str);
	}
	
	if(value == UP_KEY) {
		Screen_Position.y = 150;
		Screen_Position.x = 1;
		cursor_move(0, 7);
	}
}

void menu_151_01(unsigned int value, int display)
{
	const char *str[2] = {
			"[AUTO CALI.  ] ?   \1\0",
			"[MANUAL CALI.] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 152;
		Screen_Position.x = 1;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 152;
			Screen_Position.x = 2;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {

			CALIBRATION.slope_temp[0] = (long)(CALIBRATION.slope[0] * 100000000.0F);
			CALIBRATION.slope_temp[1] = (long)(CALIBRATION.slope[1] * 100000000.0F);
			CALIBRATION.slope_temp[2] = (long)(CALIBRATION.slope[2] * 100000000.0F);
			CALIBRATION.slope_temp[3] = (long)(CALIBRATION.slope[3] * 100000000.0F);
			CALIBRATION.slope_temp[4] = (long)(CALIBRATION.slope[4] * 100000000.0F);
			CALIBRATION.slope_temp[5] = (long)(CALIBRATION.slope[5] * 100000000.0F);
			CALIBRATION.slope_temp[6] = (long)(CALIBRATION.slope[6] * 100000000.0F);
			CALIBRATION.slope_temp[7] = (long)(CALIBRATION.slope[7] * 100000000.0F);
			CALIBRATION.slope_temp[8] = (long)(CALIBRATION.slope[8] * 100000000.0F);
			CALIBRATION.slope_temp[9] = (long)(CALIBRATION.slope[9] * 100000000.0F);

			Screen_Position.y = 153;
			Screen_Position.x = 2;
			Screen_Position.select = 5;
		}
	}
}

void menu_151_02(unsigned int value, int display)  //2015.02.17
{
	char str[2][22];

	if(display) {
		sprintf(str[0]," RATED FREQ : %s  \0",(CORE.Hz == Hz60)? "60Hz" : (CORE.Hz == Hz50) ? "50Hz": "ERR ");	//2015.02.25
		sprintf(str[1]," [60Hz] ?  [50Hz] ? \0");
		
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 8);
		} else {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			CORE.Hz = Hz60; //2015.02.25
			Screen_Position.y = 151;
			Screen_Position.x = 3;
		} else {
			CORE.Hz = Hz50; //2015.02.25
			Screen_Position.y = 151;
			Screen_Position.x = 3;
		}
		Screen_Position.select = 0;
	}
}

void menu_151_03(unsigned int value, int display)  //2015.02.25
{
	char str[2][22];

	if(display) {
		sprintf(str[0]," RATED CT : %s    \0",(CORE.rated_ct == CT_5A)? "5A  " : (CORE.rated_ct == CT_1A) ? "1A  ": "ERR ");	//2015.02.25
		sprintf(str[1]," [ 5A ] ?  [ 1A ] ? \0");
		
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 8);
		} else {
			cursor_move(1, 18);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			CORE.rated_ct = CT_5A;
			Screen_Position.y = 151;
			Screen_Position.x = 4;
		} else {
			CORE.rated_ct = CT_1A;
			Screen_Position.y = 151;
			Screen_Position.x = 4;
		}
		Screen_Position.select = 0;
	}
}

void menu_151_04(unsigned int value, int display)
{
	char str[2][22];

	if(display) {
		sprintf(str[0]," Io CT TYPE : %s  \0",(CORE.gr_select == NCT_SELECT)? "NCT " : (CORE.gr_select == ZCT_SELECT) ? "ZCT ": "ERR ");
		sprintf(str[1]," [NCT ] ?  [ZCT ] ? \0");
		
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 8);
		} else {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			CORE.gr_select = NCT_SELECT;
			Screen_Position.y = 151;
			Screen_Position.x = 5;
		} else {
			CORE.gr_select = ZCT_SELECT;
			Screen_Position.y = 151;
			Screen_Position.x = 5;
		}
		Screen_Position.select = 1;
	}
}

void menu_151_05(unsigned int value, int display)  //2015.02.17
{
	char str[2][22];
	unsigned int crc; //2015.02.25

	if(display) {
		sprintf(str[0],"   %s,%s,%s    \0",(CORE.Hz == Hz60)? "60Hz" : (CORE.Hz == Hz50) ? "50Hz": " ERR",
																	  	 (CORE.rated_ct == CT_5A)? " 5A" : (CORE.rated_ct == CT_1A) ? " 1A": "ERR",
																			 (CORE.gr_select == NCT_SELECT)? " NCT" : (CORE.gr_select == ZCT_SELECT) ? " ZCT": " ERR");	//2015.02.25
		sprintf(str[1],"WANT TO SET ?  [Y/N]\0");
		
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			//CORE 정보 저장
			crc = Setting_CRC(&CORE.Hz, 3);
			flash_sector_erase(RATED_Hz);
//		flash_word_write(RATED_Hz, MODBUS.baudrate); //코어 셋팅 깨짐 test 용
			flash_word_write(RATED_Hz, CORE.Hz); 
			flash_word_write(RATED_CT, CORE.rated_ct);
			flash_word_write(GR_SELECT, CORE.gr_select);
			flash_word_write(CORE_CRC, crc);
			ClearFLASH();	//Flash Reset 함수 실행
			//주파수 변경시 CALIBRATION 다시 추가(??)
			
			Screen_Position.y = 151;
			Screen_Position.x = 6;
		} else {
			Screen_Position.y = 151;
			Screen_Position.x = 7;
		}
		cursor_move(0, 0);
		Screen_Position.select = 0;
	}
}

void menu_151_06(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "RATED SET COMPLETE !\0");
	sprintf(str[1], "   POWER TURN OFF!  \0");

	if(display) {
		screen_frame2(str);
		return;
	}
}

void menu_151_07(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "RATED SET CANCELED !\0");
	sprintf(str[1], "   POWER TURN OFF!  \0");

	if(display) {
		screen_frame2(str);
		return;
	}
}

void menu_152_01(unsigned int value, int display)
{
	const char *str[2] = {
			"[FLASH RESET ] ?   \1\0",
			"[TCS STATUS  ] ?   \2\0"
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 153;
		Screen_Position.x = 1;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			Screen_Position.y = 154;
			Screen_Position.x = 2;
			Screen_Position.select = 1;
		} else if(Screen_Position.select == 1) {
//			Screen_Position.y = 153;
//			Screen_Position.x = 2;
//			Screen_Position.select = 0;
//			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_152_03(unsigned int, int);
void menu_152_02(unsigned int value, int display)
{
	char str[2][22];
	int err;

	sprintf(str[0]," OFFSET CALIBRATION \0");
	sprintf(str[1],"  DON'T APPLY V,I   \0");

	if(display) {
		screen_frame2(str);

		err = ADC_Offset_Calibration();
		if(err == 1) {
			menu_151_01(0xff, 1);
			Screen_Position.y = 151;
			Screen_Position.x = 1;
			cursor_move(0, 15);
			Screen_Position.select = 0;
			return;
		} else {
			menu_152_03(0xff, 1);
			return;
		}

	}

}

void menu_152_04(unsigned int, int);
void menu_152_03(unsigned int value, int display)
{
	char str[2][22];
	int err;
	Uint16 temp = 0;

	if(CORE.rated_ct == CT_5A)
	{
		if(CORE.gr_select == NCT_SELECT)
		{
			sprintf(str[0],"  HIGH LEVEL INPUT  \0");
			sprintf(str[1]," APPLY 110V, 6A, 6A \0");
		}
		else
		{
			sprintf(str[0],"  HIGH LEVEL INPUT  \0");
			sprintf(str[1]," APPLY 110V,6A,10mA \0");
		}
	}
	else
	{
		if(CORE.gr_select == NCT_SELECT)
		{
			sprintf(str[0],"  HIGH LEVEL INPUT  \0");
			sprintf(str[1]," APPLY 110V, 2A, 2A \0");
		}
		else
		{
			sprintf(str[0],"  HIGH LEVEL INPUT  \0");
			sprintf(str[1]," APPLY 110V,2A,10mA \0");
		}
	}

	if(display) {
		screen_frame2(str);

		err = ADC_High_Calibration();
		if(err == 1) {
			for(temp = 0; temp < 10; temp++) //원래 Calibration factor 복원
			{
				CALIBRATION.offset[temp] = CALIBRATION.offset_origin[temp];
				CALIBRATION.slope[temp] = CALIBRATION.slope_origin[temp];
				CALIBRATION.angle[temp] = CALIBRATION.angle_origin[temp];	
			}

			menu_151_01(0xff, 1);
			Screen_Position.y = 151;
			Screen_Position.x = 1;
			cursor_move(0, 15);
			Screen_Position.select = 0;
			delay_us(500000);
			return;
		} else {
			menu_152_04(0xff, 1);
		}
	}
}

void menu_152_05(unsigned int, int);
void menu_152_04(unsigned int value, int display)
{
	char str[2][22];
	int err;
	Uint16 temp = 0;

	if(CORE.rated_ct == CT_5A)
	{
		if(CORE.gr_select == NCT_SELECT)
		{
			sprintf(str[0],"  LOW LEVEL INPUT   \0");
			sprintf(str[1]," APPLY  10V, 1A, 1A \0");
		}
		else
		{
			sprintf(str[0],"  LOW LEVEL INPUT   \0");
			sprintf(str[1]," APPLY  10V, 1A,5mA \0");
		}
	}
	else
	{
		if(CORE.gr_select == NCT_SELECT)
		{
			sprintf(str[0],"  LOW LEVEL INPUT   \0");
			sprintf(str[1]," APPLY 10V,0.2A,0.2A\0");
		}
		else
		{
			sprintf(str[0],"  LOW LEVEL INPUT   \0");
			sprintf(str[1]," APPLY 10V,0.2A,5mA \0");
		}
	}

	if(display) {
		screen_frame2(str);

		err = ADC_low_Calibration();
		if(err == 1) {
			for(temp = 0; temp < 10; temp++) //원래 Calibration factor 복원
			{
				CALIBRATION.offset[temp] = CALIBRATION.offset_origin[temp];
				CALIBRATION.slope[temp] = CALIBRATION.slope_origin[temp];
				CALIBRATION.angle[temp] = CALIBRATION.angle_origin[temp];	
			}

			menu_151_01(0xff, 1);
			Screen_Position.y = 151;
			Screen_Position.x = 1;
			cursor_move(0, 15);
			Screen_Position.select = 0;
			delay_us(500000);
			return;
		} else {
			menu_152_05(0, 1);

			Screen_Position.y = 152;
			Screen_Position.x = 5;
			cursor_move(0, 0);
			Screen_Position.select = 0;

		}
	}
}

void menu_152_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"CALIBRATION COMPLETE\0");
	sprintf(str[1],"   PRESS ANY KEY !  \0");
	
	if(display) {
		screen_frame2(str);
		cursor_move(0, 0);
		delay_us(500000);
	}

	if(value) {
			Screen_Position.y = 151;
			Screen_Position.x = 1;
			Screen_Position.select = 0;
	}
}

void menu_153_01(unsigned int value, int display)
{
	const char *str[2] = {
			"[DI DEBOUNCE ] ?   \1\0",
			"[DO PROPERTY ] ?   \2\0" 
	};

	if(display) {
		screen_frame3(str);
		if(Screen_Position.select == 0) {
			cursor_move(0, 15);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 15);
		}
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 2;
		Screen_Position.select %= 3;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 2) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		Screen_Position.y = 80;
		Screen_Position.x = 3;
		Screen_Position.select = 0;
	} else if(value == DOWN_KEY) {
		Screen_Position.y = 150;
		Screen_Position.x = 1;
		Screen_Position.select = 0;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			DIGITAL_INPUT.debounce_temp[0] = DIGITAL_INPUT.debounce[0];
			DIGITAL_INPUT.debounce_temp[1] = DIGITAL_INPUT.debounce[1];
			DIGITAL_INPUT.debounce_temp[2] = DIGITAL_INPUT.debounce[2];
			DIGITAL_INPUT.debounce_temp[3] = DIGITAL_INPUT.debounce[3];
			DIGITAL_INPUT.debounce_temp[4] = DIGITAL_INPUT.debounce[4];
			DIGITAL_INPUT.debounce_temp[5] = DIGITAL_INPUT.debounce[5];
			DIGITAL_INPUT.debounce_temp[6] = DIGITAL_INPUT.debounce[6];
			DIGITAL_INPUT.debounce_temp[7] = DIGITAL_INPUT.debounce[7];

			Screen_Position.y = 156;
			Screen_Position.x = 2;
			Screen_Position.select = 3;
		} else if(Screen_Position.select == 1) {

			DIGITAL_OUTPUT.property_temp = DIGITAL_OUTPUT.property;

			Screen_Position.y = 157;
			Screen_Position.x = 2;
			Screen_Position.select = 0;
		}
	}
}

void menu_153_02(unsigned int value, int display)
{
	const unsigned long number_digit[6] = {100000, 10000, 1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET   : %6ld  \0",(long)(CALIBRATION.slope[0] * 100000000.0F));
	sprintf(str[1],"Ia FACTOR : %6ld  \0",CALIBRATION.slope_temp[0]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 15);
		} else if (Screen_Position.select == 4) {
			cursor_move(1, 16);
		}	else if (Screen_Position.select == 5) {
			cursor_move(1, 17);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 6;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 5) Screen_Position.select = 5;
	} else if(value == UP_KEY) {
		CALIBRATION.slope_temp[0] += number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[0] >= CALIB_I_MAX)	CALIBRATION.slope_temp[0] = CALIB_I_MAX;
	} else if(value == DOWN_KEY) {
		CALIBRATION.slope_temp[0] -= number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[0] <= CALIB_I_MIN)	CALIBRATION.slope_temp[0] = CALIB_I_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 153;
		Screen_Position.x = 3;
		Screen_Position.select = 5;
	}
}

void menu_153_03(unsigned int value, int display)
{
	const unsigned long number_digit[6] = {100000, 10000, 1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET   : %6ld  \0",(long)(CALIBRATION.slope[1] * 100000000.0F));
	sprintf(str[1],"Ib FACTOR : %6ld  \0",CALIBRATION.slope_temp[1]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 15);
		} else if (Screen_Position.select == 4) {
			cursor_move(1, 16);
		}	else if (Screen_Position.select == 5) {
			cursor_move(1, 17);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 6;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 5) Screen_Position.select = 5;
	} else if(value == UP_KEY) {
		CALIBRATION.slope_temp[1] += number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[1] >= CALIB_I_MAX)	CALIBRATION.slope_temp[1] = CALIB_I_MAX;
	} else if(value == DOWN_KEY) {
		CALIBRATION.slope_temp[1] -= number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[1] <= CALIB_I_MIN)	CALIBRATION.slope_temp[1] = CALIB_I_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 153;
		Screen_Position.x = 4;
		Screen_Position.select = 5;
	}
}

void menu_153_04(unsigned int value, int display)
{
	const unsigned long number_digit[6] = {100000, 10000, 1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET   : %6ld  \0",(long)(CALIBRATION.slope[2] * 100000000.0F));
	sprintf(str[1],"Ic FACTOR : %6ld  \0",CALIBRATION.slope_temp[2]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 15);
		} else if (Screen_Position.select == 4) {
			cursor_move(1, 16);
		}	else if (Screen_Position.select == 5) {
			cursor_move(1, 17);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 6;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 5) Screen_Position.select = 5;
	} else if(value == UP_KEY) {
		CALIBRATION.slope_temp[2] += number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[2] >= CALIB_I_MAX)	CALIBRATION.slope_temp[2] = CALIB_I_MAX;
	} else if(value == DOWN_KEY) {
		CALIBRATION.slope_temp[2] -= number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[2] <= CALIB_I_MIN)	CALIBRATION.slope_temp[2] = CALIB_I_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 153;
		Screen_Position.x = 5;
		Screen_Position.select = 5;
	}
}

void menu_153_05(unsigned int value, int display)
{
	const unsigned long number_digit[6] = {100000, 10000, 1000, 100, 10, 1};
	char str[2][22];

	if(CORE.gr_select == NCT_SELECT) 
	{
		sprintf(str[0],"PRE-SET   : %6ld  \0",(long)(CALIBRATION.slope[3] * 100000000.0F));
		sprintf(str[1],"Io FACTOR : %6ld  \0",CALIBRATION.slope_temp[3]);
		
		if(display) {
			screen_frame2(str);
			if (Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			} else if (Screen_Position.select == 4) {
				cursor_move(1, 16);
			}	else if (Screen_Position.select == 5) {
				cursor_move(1, 17);
			}
			return;
		}
		
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 6;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 5) Screen_Position.select = 5;
		} else if(value == UP_KEY) {
			CALIBRATION.slope_temp[3] += number_digit[Screen_Position.select % 6];
			if(CALIBRATION.slope_temp[3] >= CALIB_I_MAX)	CALIBRATION.slope_temp[3] = CALIB_I_MAX;
		} else if(value == DOWN_KEY) {
			CALIBRATION.slope_temp[3] -= number_digit[Screen_Position.select % 6];
			if(CALIBRATION.slope_temp[3] <= CALIB_I_MIN)	CALIBRATION.slope_temp[3] = CALIB_I_MIN;
		} else if(value == ENTER_KEY) {
			Screen_Position.y = 153;
			Screen_Position.x = 6;
			Screen_Position.select = 5;
		}
	}
	else
	{
		sprintf(str[0],"PRE-SET   : %6ld  \0",(long)(CALIBRATION.slope[5] * 100000000.0F));
		sprintf(str[1],"Io FACTOR : %6ld  \0",CALIBRATION.slope_temp[5]);
		
		if(display) {
			screen_frame2(str);
			if (Screen_Position.select == 0) {
				cursor_move(1, 12);
			} else if (Screen_Position.select == 1) {
				cursor_move(1, 13);
			} else if (Screen_Position.select == 2) {
				cursor_move(1, 14);
			} else if (Screen_Position.select == 3) {
				cursor_move(1, 15);
			} else if (Screen_Position.select == 4) {
				cursor_move(1, 16);
			}	else if (Screen_Position.select == 5) {
				cursor_move(1, 17);
			}
			return;
		}
		
		if(value == LEFT_KEY) {
			if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
			Screen_Position.select %= 6;
		} else if(value == RIGHT_KEY) {
			if(Screen_Position.select++ >= 5) Screen_Position.select = 5;
		} else if(value == UP_KEY) {
			CALIBRATION.slope_temp[5] += number_digit[Screen_Position.select % 6];
			if(CALIBRATION.slope_temp[5] >= CALIB_I_MAX)	CALIBRATION.slope_temp[5] = CALIB_I_MAX;
		} else if(value == DOWN_KEY) {
			CALIBRATION.slope_temp[5] -= number_digit[Screen_Position.select % 6];
			if(CALIBRATION.slope_temp[5] <= CALIB_I_MIN)	CALIBRATION.slope_temp[5] = CALIB_I_MIN;
		} else if(value == ENTER_KEY) {
			Screen_Position.y = 153;
			Screen_Position.x = 6;
			Screen_Position.select = 5;
		}
	}
}

void menu_153_06(unsigned int value, int display)
{
	const unsigned long number_digit[6] = {100000, 10000, 1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET   : %6ld  \0",(long)(CALIBRATION.slope[6] * 100000000.0F));
	sprintf(str[1],"Vab FACTOR: %6ld  \0",CALIBRATION.slope_temp[6]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 15);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 16);
		} else if (Screen_Position.select == 4) {
			cursor_move(1, 17);
		}	else if (Screen_Position.select == 5) {
			cursor_move(1, 18);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 6;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 5) Screen_Position.select = 5;
	} else if(value == UP_KEY) {
		CALIBRATION.slope_temp[6] += number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[6] >= CALIB_V_MAX)	CALIBRATION.slope_temp[6] = CALIB_V_MAX;
	} else if(value == DOWN_KEY) {
		CALIBRATION.slope_temp[6] -= number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[6] <= CALIB_V_MIN)	CALIBRATION.slope_temp[6] = CALIB_V_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 153;
		Screen_Position.x = 7;
		Screen_Position.select = 5;
	}
}

void menu_153_07(unsigned int value, int display)
{
	const unsigned long number_digit[6] = {100000, 10000, 1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET   : %6ld  \0",(long)(CALIBRATION.slope[7] * 100000000.0F));
	sprintf(str[1],"Vbc FACTOR: %6ld  \0",CALIBRATION.slope_temp[7]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 15);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 16);
		} else if (Screen_Position.select == 4) {
			cursor_move(1, 17);
		}	else if (Screen_Position.select == 5) {
			cursor_move(1, 18);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 6;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 5) Screen_Position.select = 5;
	} else if(value == UP_KEY) {
		CALIBRATION.slope_temp[7] += number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[7] >= CALIB_V_MAX)	CALIBRATION.slope_temp[7] = CALIB_V_MAX;
	} else if(value == DOWN_KEY) {
		CALIBRATION.slope_temp[7] -= number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[7] <= CALIB_V_MIN)	CALIBRATION.slope_temp[7] = CALIB_V_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 153;
		Screen_Position.x = 8;
		Screen_Position.select = 5;
	}
}

void menu_153_08(unsigned int value, int display)
{
	const unsigned long number_digit[6] = {100000, 10000, 1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET   : %6ld  \0",(long)(CALIBRATION.slope[8] * 100000000.0F));
	sprintf(str[1],"Vca FACTOR: %6ld  \0",CALIBRATION.slope_temp[8]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 14);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 15);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 16);
		} else if (Screen_Position.select == 4) {
			cursor_move(1, 17);
		}	else if (Screen_Position.select == 5) {
			cursor_move(1, 18);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 6;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 5) Screen_Position.select = 5;
	} else if(value == UP_KEY) {
		CALIBRATION.slope_temp[8] += number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[8] >= CALIB_V_MAX)	CALIBRATION.slope_temp[8] = CALIB_V_MAX;
	} else if(value == DOWN_KEY) {
		CALIBRATION.slope_temp[8] -= number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[8] <= CALIB_V_MIN)	CALIBRATION.slope_temp[8] = CALIB_V_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 153;
		Screen_Position.x = 9;
		Screen_Position.select = 5;
	}
}

void menu_153_09(unsigned int value, int display)
{
	const unsigned long number_digit[6] = {100000, 10000, 1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"PRE-SET   : %6ld  \0",(long)(CALIBRATION.slope[9] * 100000000.0F));
	sprintf(str[1],"Vo FACTOR : %6ld  \0",CALIBRATION.slope_temp[9]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 13);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 14);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 15);
		} else if (Screen_Position.select == 4) {
			cursor_move(1, 16);
		}	else if (Screen_Position.select == 5) {
			cursor_move(1, 17);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 6;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 5) Screen_Position.select = 5;
	} else if(value == UP_KEY) {
		CALIBRATION.slope_temp[9] += number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[9] >= CALIB_Vo_MAX) CALIBRATION.slope_temp[9] = CALIB_Vo_MAX;
	} else if(value == DOWN_KEY) {
		CALIBRATION.slope_temp[9] -= number_digit[Screen_Position.select % 6];
		if(CALIBRATION.slope_temp[9] <= CALIB_Vo_MIN)	CALIBRATION.slope_temp[9] = CALIB_Vo_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 153;
		Screen_Position.x = 10;
		Screen_Position.select = 1;
		cursor_move(1, 18);
	}
}

void menu_153_10(unsigned int value, int display)
{
	char str[2][22];
	unsigned int i;
	unsigned int temp1[100];
	void *void_p;
	unsigned int *temp16_p;

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			if(CORE.rated_ct == CT_5A)
			{
				CALIBRATION.slope[0] = CALIBRATION.slope_temp[0] * 0.00000001F;
				CALIBRATION.slope[1] = CALIBRATION.slope_temp[1] * 0.00000001F;
				CALIBRATION.slope[2] = CALIBRATION.slope_temp[2] * 0.00000001F;
				if(CORE.gr_select == NCT_SELECT)	{CALIBRATION.slope[3] = CALIBRATION.slope_temp[3] * 0.00000001F;}
				else															{CALIBRATION.slope[5] = CALIBRATION.slope_temp[5] * 0.00000001F;}
				CALIBRATION.slope[6] = CALIBRATION.slope_temp[6] * 0.00000001F;
				CALIBRATION.slope[7] = CALIBRATION.slope_temp[7] * 0.00000001F;
				CALIBRATION.slope[8] = CALIBRATION.slope_temp[8] * 0.00000001F;
				CALIBRATION.slope[9] = CALIBRATION.slope_temp[9] * 0.00000001F;
			}
			else
			{
	
			}

			//-------- EEROM 저장
			eerom_control(4, 0x80);
			eerom_control(4, 0xc0);
			
			for(i = 0; i < 10; i++)  //offset 저장
			{
				eerom_write(i, &CALIBRATION.offset[i]);
				temp1[i] = CALIBRATION.offset[i]; //[0]~[9]
			}
			for(i = 0; i < 10; i++) //slope 저장
			{
				void_p = &CALIBRATION.slope[i];
				temp16_p = (unsigned int *)void_p;
				eerom_write(0x10 + (i << 1), temp16_p);
				eerom_write(0x11 + (i << 1), temp16_p + 1);
				temp1[10 + (i << 1)] = *temp16_p;  //[10]~[29]
				temp1[11 + (i << 1)] = *(temp16_p + 1);
			}
			for(i = 0; i < 10; i++) //angle 저장
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
			//-------- EEROM 저장 END

			Screen_Position.y = 153;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 153;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_153_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "CALI. SET COMPLETE !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 151;
			Screen_Position.x = 1;
			Screen_Position.select = 0;
	}
}

void menu_153_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "CALI. SET CANCELED !\0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 151;
			Screen_Position.x = 1;
			Screen_Position.select = 0;
	}
}

void menu_154_02(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"DATA WILL BE LOST ! \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {
			ClearFLASH();

			Screen_Position.y = 154;
			Screen_Position.x = 3;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 154;
			Screen_Position.x = 4;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_154_03(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  RESET COMPLETE !  \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 152;
			Screen_Position.x = 1;
			Screen_Position.select = 0;
	}
}

void menu_154_04(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  RESET CANCELED !  \0");
	sprintf(str[1], "  PRESS ANY KEY !   \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 152;
			Screen_Position.x = 1;
			Screen_Position.select = 0;
	}
}

void menu_156_02(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"DI#1 PRE] %4d[ms]  \0",DIGITAL_INPUT.debounce[0]);
	sprintf(str[1],"     NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[0]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 13);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		DIGITAL_INPUT.debounce_temp[0] += number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[0] >= DEBOUNCE_MAX) DIGITAL_INPUT.debounce_temp[0] = DEBOUNCE_MAX;
	} else if(value == DOWN_KEY) {
		DIGITAL_INPUT.debounce_temp[0] -= number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[0] <= DEBOUNCE_MIN || DIGITAL_INPUT.debounce_temp[0] >= 60000)	DIGITAL_INPUT.debounce_temp[0] = DEBOUNCE_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 156;
		Screen_Position.x = 3;
		Screen_Position.select = 3;
	}
}

void menu_156_03(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"DI#2 PRE] %4d[ms]  \0",DIGITAL_INPUT.debounce[1]);
	sprintf(str[1],"     NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[1]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 13);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		DIGITAL_INPUT.debounce_temp[1] += number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[1] >= DEBOUNCE_MAX) DIGITAL_INPUT.debounce_temp[1] = DEBOUNCE_MAX;
	} else if(value == DOWN_KEY) {
		DIGITAL_INPUT.debounce_temp[1] -= number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[1] <= DEBOUNCE_MIN || DIGITAL_INPUT.debounce_temp[1] >= 60000)	DIGITAL_INPUT.debounce_temp[1] = DEBOUNCE_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 156;
		Screen_Position.x = 4;
		Screen_Position.select = 3;
	}
}

void menu_156_04(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"DI#3 PRE] %4d[ms]  \0",DIGITAL_INPUT.debounce[2]);
	sprintf(str[1],"     NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[2]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 13);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		DIGITAL_INPUT.debounce_temp[2] += number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[2] >= DEBOUNCE_MAX) DIGITAL_INPUT.debounce_temp[2] = DEBOUNCE_MAX;
	} else if(value == DOWN_KEY) {
		DIGITAL_INPUT.debounce_temp[2] -= number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[2] <= DEBOUNCE_MIN || DIGITAL_INPUT.debounce_temp[2] >= 60000)	DIGITAL_INPUT.debounce_temp[2] = DEBOUNCE_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 156;
		Screen_Position.x = 5;
		Screen_Position.select = 3;
	}
}

void menu_156_05(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"DI#4 PRE] %4d[ms]  \0",DIGITAL_INPUT.debounce[3]);
	sprintf(str[1],"     NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[3]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 13);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		DIGITAL_INPUT.debounce_temp[3] += number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[3] >= DEBOUNCE_MAX) DIGITAL_INPUT.debounce_temp[3] = DEBOUNCE_MAX;
	} else if(value == DOWN_KEY) {
		DIGITAL_INPUT.debounce_temp[3] -= number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[3] <= DEBOUNCE_MIN || DIGITAL_INPUT.debounce_temp[3] >= 60000)	DIGITAL_INPUT.debounce_temp[3] = DEBOUNCE_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 156;
		Screen_Position.x = 6;
		Screen_Position.select = 3;
	}
}

void menu_156_06(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"DI#5 PRE] %4d[ms]  \0",DIGITAL_INPUT.debounce[4]);
	sprintf(str[1],"     NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[4]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 13);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		DIGITAL_INPUT.debounce_temp[4] += number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[4] >= DEBOUNCE_MAX) DIGITAL_INPUT.debounce_temp[4] = DEBOUNCE_MAX;
	} else if(value == DOWN_KEY) {
		DIGITAL_INPUT.debounce_temp[4] -= number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[4] <= DEBOUNCE_MIN || DIGITAL_INPUT.debounce_temp[4] >= 60000)	DIGITAL_INPUT.debounce_temp[4] = DEBOUNCE_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 156;
		Screen_Position.x = 7;
		Screen_Position.select = 3;
	}
}

void menu_156_07(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"DI#6 PRE] %4d[ms]  \0",DIGITAL_INPUT.debounce[5]);
	sprintf(str[1],"     NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[5]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 13);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		DIGITAL_INPUT.debounce_temp[5] += number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[5] >= DEBOUNCE_MAX) DIGITAL_INPUT.debounce_temp[5] = DEBOUNCE_MAX;
	} else if(value == DOWN_KEY) {
		DIGITAL_INPUT.debounce_temp[5] -= number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[5] <= DEBOUNCE_MIN || DIGITAL_INPUT.debounce_temp[5] >= 60000)	DIGITAL_INPUT.debounce_temp[5] = DEBOUNCE_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 156;
		Screen_Position.x = 8;
		Screen_Position.select = 3;
	}
}

void menu_156_08(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"DI#7 PRE] %4d[ms]  \0",DIGITAL_INPUT.debounce[6]);
	sprintf(str[1],"     NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[6]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 13);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		DIGITAL_INPUT.debounce_temp[6] += number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[6] >= DEBOUNCE_MAX) DIGITAL_INPUT.debounce_temp[6] = DEBOUNCE_MAX;
	} else if(value == DOWN_KEY) {
		DIGITAL_INPUT.debounce_temp[6] -= number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[6] <= DEBOUNCE_MIN || DIGITAL_INPUT.debounce_temp[6] >= 60000)	DIGITAL_INPUT.debounce_temp[6] = DEBOUNCE_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 156;
		Screen_Position.x = 9;
		Screen_Position.select = 3;
	}
}

void menu_156_09(unsigned int value, int display)
{
	const unsigned int number_digit[4] = {1000, 100, 10, 1};
	char str[2][22];

	sprintf(str[0],"DI#8 PRE] %4d[ms]  \0",DIGITAL_INPUT.debounce[7]);
	sprintf(str[1],"     NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[7]);
	
	if(display) {
		screen_frame2(str);
		if (Screen_Position.select == 0) {
			cursor_move(1, 10);
		} else if (Screen_Position.select == 1) {
			cursor_move(1, 11);
		} else if (Screen_Position.select == 2) {
			cursor_move(1, 12);
		} else if (Screen_Position.select == 3) {
			cursor_move(1, 13);
		}
		return;
	}
	
	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 0;
		Screen_Position.select %= 4;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 3) Screen_Position.select = 3;
	} else if(value == UP_KEY) {
		DIGITAL_INPUT.debounce_temp[7] += number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[7] >= DEBOUNCE_MAX) DIGITAL_INPUT.debounce_temp[7] = DEBOUNCE_MAX;
	} else if(value == DOWN_KEY) {
		DIGITAL_INPUT.debounce_temp[7] -= number_digit[Screen_Position.select % 4];
		if(DIGITAL_INPUT.debounce_temp[7] <= DEBOUNCE_MIN || DIGITAL_INPUT.debounce_temp[7] >= 60000)	DIGITAL_INPUT.debounce_temp[7] = DEBOUNCE_MIN;
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 156;
		Screen_Position.x = 10;
		cursor_move(0, 0);//cursor off
	}
}

void menu_156_10(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"DI#1 NEW] %4d[ms] %c\0",DIGITAL_INPUT.debounce_temp[0],ENTER);
	sprintf(str[1],"DI#2 NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[1]);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 156;
			Screen_Position.x = 11;
			cursor_move(0, 0);//cursor off
	}
}

void menu_156_11(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"DI#3 NEW] %4d[ms] %c\0",DIGITAL_INPUT.debounce_temp[2],ENTER);
	sprintf(str[1],"DI#4 NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[3]);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 156;
			Screen_Position.x = 12;
			cursor_move(0, 0);//cursor off
	}
}

void menu_156_12(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"DI#5 NEW] %4d[ms] %c\0",DIGITAL_INPUT.debounce_temp[4],ENTER);
	sprintf(str[1],"DI#6 NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[5]);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 156;
			Screen_Position.x = 13;
			cursor_move(0, 0);//cursor off
	}
}

void menu_156_13(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"DI#7 NEW] %4d[ms] %c\0",DIGITAL_INPUT.debounce_temp[6],ENTER);
	sprintf(str[1],"DI#8 NEW] %4d[ms]  \0",DIGITAL_INPUT.debounce_temp[7]);

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value == ENTER_KEY) {
			Screen_Position.y = 156;
			Screen_Position.x = 14;
			Screen_Position.select = 1;
	}
}

void menu_156_14(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			DIGITAL_INPUT.debounce[0] = DIGITAL_INPUT.debounce_temp[0];
			DIGITAL_INPUT.debounce[1] = DIGITAL_INPUT.debounce_temp[1];
			DIGITAL_INPUT.debounce[2] = DIGITAL_INPUT.debounce_temp[2];
			DIGITAL_INPUT.debounce[3] = DIGITAL_INPUT.debounce_temp[3];
			DIGITAL_INPUT.debounce[4] = DIGITAL_INPUT.debounce_temp[4];
			DIGITAL_INPUT.debounce[5] = DIGITAL_INPUT.debounce_temp[5];
			DIGITAL_INPUT.debounce[6] = DIGITAL_INPUT.debounce_temp[6];
			DIGITAL_INPUT.debounce[7] = DIGITAL_INPUT.debounce_temp[7];

			if(setting_save(DIGITAL_INPUT.debounce, DI_DEBOUNCE1, 8))
			{
				setting_load(DIGITAL_INPUT.debounce, 8, DI_DEBOUNCE1);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 156;
			Screen_Position.x = 15;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 156;
			Screen_Position.x = 16;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_156_15(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  DI SET COMPLETE ! \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 153;
			Screen_Position.x = 1;
			Screen_Position.select = 0;
	}
}

void menu_156_16(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  DI SET CANCELED ! \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 153;
			Screen_Position.x = 1;
			Screen_Position.select = 0;
	}
}

void menu_157_02(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"AX.1 2 3 4 5 6 7 8  \0");
	sprintf(str[1],"RY.%c %c %c %c %c %c %c %c  \0",
			(DIGITAL_OUTPUT.property_temp & 0x01) ? 'L': 'U',
			(DIGITAL_OUTPUT.property_temp & 0x02) ? 'L': 'U',
			(DIGITAL_OUTPUT.property_temp & 0x04) ? 'L': 'U',
			(DIGITAL_OUTPUT.property_temp & 0x08) ? 'L': 'U',
			(DIGITAL_OUTPUT.property_temp & 0x10) ? 'L': 'U',
			(DIGITAL_OUTPUT.property_temp & 0x20) ? 'L': 'U',
			(DIGITAL_OUTPUT.property_temp & 0x40) ? 'L': 'U',
			(DIGITAL_OUTPUT.property_temp & 0x80) ? 'L': 'U'
	);

	if(display) {
		screen_frame2(str);
		cursor_move(1, Screen_Position.select*2 + 3);
		return;
	}

	if(value == LEFT_KEY) {
		if(Screen_Position.select-- <= 0) Screen_Position.select = 7;
	} else if(value == RIGHT_KEY) {
		if(Screen_Position.select++ >= 7) Screen_Position.select = 0;
	} else if(value == UP_KEY) {
		DIGITAL_OUTPUT.property_temp |= (1 << Screen_Position.select);
	} else if(value == DOWN_KEY) {
		DIGITAL_OUTPUT.property_temp &= ~(1 << Screen_Position.select);
	} else if(value == ENTER_KEY) {
		Screen_Position.y = 157;
		Screen_Position.x = 3;
		Screen_Position.select = 1;
		cursor_move(1, 18);
	}
}

void menu_157_03(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0],"                    \0");
	sprintf(str[1],"WANT TO SET ?  [Y/N]\0");

	if(display) {
		screen_frame2(str);
		if(Screen_Position.select == 0) {
			cursor_move(1, 16);
		} else if(Screen_Position.select == 1) {
			cursor_move(1, 18);
		}
		return;
	}

	if(value == LEFT_KEY) {
		Screen_Position.select -= 1;
		Screen_Position.select %= 2;
	} else if(value == RIGHT_KEY) {
		Screen_Position.select += 1;
		Screen_Position.select %= 2;
	} else if(value == ENTER_KEY) {
		if(Screen_Position.select == 0) {

			DIGITAL_OUTPUT.property = DIGITAL_OUTPUT.property_temp;
			if(setting_save(&DIGITAL_OUTPUT.property, DO_PROPERTY, 1))
			{
				setting_load(&DIGITAL_OUTPUT.property, 1, DO_PROPERTY);
			}
			else
			{
				//FLASH WRITE ERROR pop up 화면
			}

			Screen_Position.y = 157;
			Screen_Position.x = 4;
			cursor_move(0, 0);//cursor off
		} else if(Screen_Position.select == 1) {
			Screen_Position.y = 157;
			Screen_Position.x = 5;
			cursor_move(0, 0);//cursor off
		}
	}
}

void menu_157_04(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  DO SET COMPLETE ! \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 153;
			Screen_Position.x = 1;
			Screen_Position.select = 0;
	}
}

void menu_157_05(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  DO SET CANCELED ! \0");
	sprintf(str[1], "   PRESS ANY KEY !  \0");

	if(display) {
		screen_frame2(str);
		return;
	}

	if(value) {
			Screen_Position.y = 153;
			Screen_Position.x = 1;
			Screen_Position.select = 0;
	}
}

void menu_dummy(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "                     \0");
	sprintf(str[1], "menu dummy : %02d, %02d\0", Screen_Position.y, Screen_Position.x);

	if(display) {
		screen_frame2(str);
	}

}

void menu_popup(unsigned int value, int display)
{
	char str[2][22];

//if(OpRly==1)			Sprintf(VFDFBuffer.up," 50-1 SHORT CIRCUIT ");
//else if(OpRly==2)	Sprintf(VFDFBuffer.up," 50-2 SHORT CIRCUIT ");
//else if(OpRly==3)	Sprintf(VFDFBuffer.up," 51-1 OVER CURRENT  ");
//else if(OpRly==4)	Sprintf(VFDFBuffer.up," 51-2 OVER CURRENT  ");
//else if(OpRly==5)	Sprintf(VFDFBuffer.up," 50G GROUND FAULT   ");
//else if(OpRly==6)	Sprintf(VFDFBuffer.up," 51G GROUND FAULT   ");
//else if(OpRly==7)	Sprintf(VFDFBuffer.up," 27R UNDER VOLTAGE1 ");
//else if(OpRly==8)	Sprintf(VFDFBuffer.up," 27M UNDER VOLTAGE2 ");
//else if(OpRly==9)	Sprintf(VFDFBuffer.up," 27S UNDER VOLTAGE3 ");
//else if((OpRly==10)||(OpRly==11))	Sprintf(VFDFBuffer.up,"47 UNBALANCE VOLTAGE");
//else if(OpRly==12)	Sprintf(VFDFBuffer.up,"  59 OVER VOLTAGE   ");
//else if(OpRly==13)	Sprintf(VFDFBuffer.up,"  64 GROUND FAULT   ");
//else if(OpRly==14)	Sprintf(VFDFBuffer.up," 67GD GROUND FAULT  ");
//else if(OpRly==15)	Sprintf(VFDFBuffer.up," 67GS GROUND FAULT  ");

	sprintf(str[0], "%s\0", Trip_Message[0][0]);
	sprintf(str[1], "%s\0", Trip_Message[1][1]);

	if(display) {
		screen_frame2(str);
		cursor_move(0, 0);
		return;
	}

	if(value == ACK_KEY) {
		if(0) { //Reset Case
			Screen_Position.y = Save_Current_Screen.y;
			Screen_Position.x = Save_Current_Screen.x;
			Screen_Position.select = Save_Current_Screen.select;
			Screen_Position.data_change = Save_Current_Screen.data_change;
		} else {//Not Reset Case
			Screen_Position.y = 98;
			Screen_Position.x = 1;
			Screen_Position.select = 0;
			Screen_Position.data_change = NORMAL_MENU;
		}
	}
}

void menu_reset(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "  [[[ Warning ! ]]]  \0");//900 is sample.
	sprintf(str[1], " Fault is not Reset. \0", 98, 0);
		
		
	if(display) {
		screen_frame2(str);
		cursor_move(0, 0);
		delay_us(1000000);
		
//		Screen_Position.y = 98;
//		Screen_Position.x = 0;
//		Screen_Position.select = 0;
//		Screen_Position.data_change = NORMAL_MENU;
		
		menu_popup(0, 1);
				
		return;
	}

//	if(value == ACK_KEY) {
//		Screen_Position.y = 98;
//		Screen_Position.x = 0;
//		Screen_Position.select = 0;
//		Screen_Position.data_change = NORMAL_MENU;
//	}
}

void menu_99_00(unsigned int value, int display)
{
	char str[2][22];

	sprintf(str[0], "                     \0");//900 is sample.
	sprintf(str[1], "menu dummy : %02d, %02d\0", 99, 0);

	if(display) {
		screen_frame2(str);
	}

}

void Event_Item_Display(void)		//khs, 2015-03-31 오후 7:36:32
{
	char temp_char;
	unsigned int temp16;
	int temp_int;
	char str[22];
	

	EVENT.temp = *(EVENT_INDEX1 + (EVENT.view_point * 18));
	EVENT.temp &= 0x00ff;
	
	// pickup/op
	if((EVENT.temp == 0x00) || (EVENT.temp == 0x01))
	{
		// pickup
		if(EVENT.temp == 0x00) {
			//screen_frame3(event1);
			screen_frame3(event1);
		}
		// op
		else {
			//screen_frame3(event2);
			screen_frame3(event2);
		}
		
		// relay 종류
		temp16 = *(EVENT_INDEX2 + (EVENT.view_point * 18));
		temp16 &= 0x00ff;
//		LCD.line_2nd_adder = event_relay[temp16];
//		LCD.line_2nd_addressing = LCD_L2_00;
//		LCD.line_2nd_status = 1;
		
		// relay curve
		temp16 = *(EVENT_CONTENT1 + (EVENT.view_point * 18));
		temp16 &= 0x00ff;
//		LCD.line_3rd_adder = relay_curve[temp16];
//		LCD.line_3rd_addressing = LCD_L2_08;
//		LCD.line_3rd_status = 1;
		
		//상 정보
		temp16 = *(EVENT_CONTENT2 + (EVENT.view_point * 18));
		temp16 &= 0x00ff;				
//		LCD.line_4th_adder = event_phase[temp16];
//		LCD.line_4th_addressing = LCD_L2_16;
//		LCD.line_4th_status = 1;
	}
	
	// relay set
	else if(EVENT.temp == 0x02)
	{
		screen_frame3(event3);
		
		temp16 = *(EVENT_INDEX2 + (EVENT.view_point * 18));
		temp16 &= 0x00ff;
		
//		LCD.line_2nd_adder = event_relay[temp16];
//		LCD.line_2nd_addressing = LCD_L2_05;
//		LCD.line_2nd_status = 1;
	}
					
	// system set
	else if(EVENT.temp == 0x03)
	{	
		temp16 = *(EVENT_INDEX2 + (EVENT.view_point * 18));
		temp16 &= 0x00ff;
			
		if(temp16 & 0x0080)	{screen_frame3(event10);}
		else
		{
			screen_frame3(event4);
			
//			LCD.line_2nd_adder = event_set[temp16];
//			LCD.line_2nd_addressing = LCD_L2_05;
//			LCD.line_2nd_status = 1;
		}
	}
	
	// data reset
	else if(EVENT.temp == 0x04)
	{
		screen_frame3(event5);
		
		temp16 = *(EVENT_INDEX2 + (EVENT.view_point * 18));
		temp16 &= 0x00ff;
		
//		LCD.line_2nd_adder = event_reset[temp16];
//		LCD.line_2nd_addressing = LCD_L2_05;
//		LCD.line_2nd_status = 1;
	}
	
	// di off/on
	else if((EVENT.temp == 0x05) || (EVENT.temp == 0x06))
	{
		//khs, 2015-04-03 오후 6:59:32
		// open
		//if(EVENT.temp == 0x05)	{screen_frame3(event6);}
		// close
		//else										{screen_frame3(event7);}
		screen_frame3(event6);
		
		temp16 = *(EVENT_CONTENT1 + (EVENT.view_point * 18));
		temp16 &= 0x00ff;
		temp16 <<= 8;
		temp16 |= (*(EVENT_CONTENT2 + (EVENT.view_point * 18)) & 0x00ff);

		sprintf(str, "%d CHANNEL %s\0", temp16, (EVENT.temp == 0x05)? "ON": "OFF");
		VFD_Single_Line_dump(LCD_L2_05, str);

		
		for(temp_char = 0; temp_char < 8; temp_char++)
		{
			if(temp16 & (0x0001 << temp_char))	{LCD.line_data2[temp_char << 1] = (temp_char + 1) + 0x30;}
			else																{LCD.line_data2[temp_char << 1] = ' ';}
			LCD.line_data2[(temp_char << 1) + 1] = ' ';
		}
		
		LCD.line_data2[16] = 0;
		
		LCD.line_2nd_adder = LCD.line_data2;
		LCD.line_2nd_addressing = LCD_L2_00;
		LCD.line_2nd_status = 1;
	}
	
	// do off/on
	else if((EVENT.temp == 0x07) || (EVENT.temp == 0x08))
	{
		// open
		if(EVENT.temp == 0x07)	{screen_frame3(event8);}
		// close
		else										{screen_frame3(event9);}
		
		temp16 = *(EVENT_CONTENT1 + (EVENT.view_point * 18));
		temp16 &= 0x00ff;
		temp16 <<= 8;
		temp16 |= (*(EVENT_CONTENT2 + (EVENT.view_point * 18)) & 0x00ff);
		
		for(temp_char = 0; temp_char < 9; temp_char++)
		{
			if(temp16 & (0x0001 << temp_char))	{LCD.line_data2[temp_char << 1] = (temp_char + 1) + 0x30;}
			else																{LCD.line_data2[temp_char << 1] = ' ';}
			
			LCD.line_data2[(temp_char << 1) + 1] = ' ';
		}
		
		LCD.line_data2[18] = 0;
		
		LCD.line_2nd_adder = LCD.line_data2;
		LCD.line_2nd_addressing = LCD_L2_00;
		LCD.line_2nd_status = 1;
	}
	
	// mode change
	else if(EVENT.temp == 0x09)
	{
		screen_frame3(event11);
		
		temp16 = *(EVENT_INDEX2 + (EVENT.view_point * 18));
		temp16 &= 0x00ff;
		
//		LCD.line_2nd_adder = event_mode_change[temp16];
//		LCD.line_2nd_addressing = LCD_L2_05;
//		LCD.line_2nd_status = 1;
	}
	
	// control
	else if(EVENT.temp == 0x0a)
	{
		screen_frame3(event11);
		
		temp16 = *(EVENT_INDEX2 + (EVENT.view_point * 18));
		temp16 &= 0x00ff;
		
//		LCD.line_2nd_adder = event_control[temp16];
//		LCD.line_2nd_addressing = LCD_L2_05;
//		LCD.line_2nd_status = 1;
	}
	
	temp_int = EVENT.view_start - EVENT.view_point;
	
	if(temp_int < 0)	{temp_int += 99;}
	
	++temp_int;
	
	LCD.line_data1[0] = (temp_int / 100) + 0x30;
	
	temp16 = temp_int % 100;
	LCD.line_data1[1] = (temp16 / 10) + 0x30;
	
	temp16 = temp_int % 10;
	LCD.line_data1[2] = temp16 + 0x30;
	
	LCD.line_data1[3] = 0;
	
	LCD.line_1st_adder = LCD.line_data1;
	LCD.line_1st_addressing = LCD_L1_00;
	LCD.line_1st_status = 1;			
}

void Event_Time_Display(void)		//khs, 2015-04-03 오후 7:16:58
{
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned int msecond;

	char str[22];

		//순서
		
		year = *(EVENT_YEAR + (EVENT.view_point * 18));
		year &= 0x00ff;
		
		month = *(EVENT_MONTH + (EVENT.view_point * 18));
		month &= 0x00ff;
		
		day = *(EVENT_DAY + (EVENT.view_point * 18));
		day &= 0x00ff;
		
		hour = *(EVENT_HOUR + (EVENT.view_point * 18));
		hour &= 0x00ff;
		
		minute = *(EVENT_MINUTE + (EVENT.view_point * 18));
		minute &= 0x00ff;
		
		second = *(EVENT_SECOND + (EVENT.view_point * 18));
		second &= 0x00ff;
		
		
		msecond = *(EVENT_MS1 + (EVENT.view_point * 18));
		msecond &= 0x00ff;
		msecond <<= 8;
		msecond |= (*(EVENT_MS2 + (EVENT.view_point * 18)) & 0x00ff);

		delay_us(1000);
		sprintf(str, " DAY : %2d.%2d.%2d     %c\0", year, month, day, ENTER);
		VFD_Single_Line_dump(LCD_L1_00, str);
		sprintf(str, " TIME: %02d:%02d:%02d:%03d \0", hour, minute, second, msecond);
		VFD_Single_Line_dump(LCD_L2_00, str);
	
}

const Screen_Function_Pointer menu_tables[200][18] = { //2015.02.17
		{menu_00_00, menu_00_01, menu_00_02, menu_00_03, menu_00_04, menu_00_05, menu_00_06, menu_00_07, menu_00_08,},	// 00
		{menu_dummy, menu_01_01, menu_dummy, menu_01_03, menu_01_04, menu_01_05, menu_01_06, menu_01_07, menu_01_08, menu_01_09, menu_01_10,  menu_01_11,  menu_01_12, menu_01_13,  menu_01_14,},    // 01
		{menu_dummy, menu_02_01, menu_dummy, menu_02_03, menu_02_04, menu_02_05, menu_02_06, },	// 02
		{menu_dummy, menu_dummy, menu_dummy, menu_03_03, },																			// 03
		{menu_dummy, menu_dummy, menu_dummy, },    // 04
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 05
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 06
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 07
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 08
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 09
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 10
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 11
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 12
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 13
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 14
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 15
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 16
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 17
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 18
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 19
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 20
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 21
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 22
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 23
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 24
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 25
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 26
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 27
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 28
		{menu_dummy, menu_dummy, menu_29_02, menu_29_03, },                                                                                        // 29
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 30
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 31
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 32
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 33
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 34
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 35
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 36
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 37
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 38
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 39
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_dummy, },							   // 40
		{menu_dummy, menu_dummy, menu_41_02, menu_41_03, menu_41_04, menu_41_05, menu_41_06, menu_41_07, menu_41_08, menu_41_09, menu_41_10, menu_41_11, menu_41_12,}, // 41
		{menu_dummy, menu_42_01, menu_42_02, menu_42_03, menu_dummy, menu_42_05, menu_42_06, menu_42_07, menu_42_08, menu_42_09, menu_42_10, menu_42_11, menu_42_12, menu_42_13,}, // 42
		{menu_dummy, menu_dummy, menu_dummy, menu_43_03, menu_43_04, menu_43_05, menu_43_06, menu_43_07, menu_43_08, menu_43_09, menu_43_10, menu_43_11, menu_43_12,}, // 43
		{menu_dummy, menu_dummy, menu_dummy, menu_44_03, menu_dummy, menu_44_05, menu_44_06, menu_44_07, menu_44_08, menu_44_09, menu_44_10, menu_44_11, menu_44_12, menu_44_13,}, // 44
		{menu_dummy, menu_dummy, menu_dummy, menu_45_03, menu_45_04, menu_45_05, menu_45_06, menu_45_07, menu_45_08, menu_45_09, menu_45_10, menu_45_11, menu_45_12, menu_45_13,}, // 45
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_46_04, menu_46_05, menu_46_06, menu_46_07, menu_46_08, menu_46_09, menu_46_10, menu_46_11, menu_46_12, menu_46_13,}, // 46
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_47_04, menu_47_05, menu_47_06, menu_47_07, menu_47_08, menu_47_09, menu_47_10, menu_47_11, menu_47_12,}, // 47
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_48_05, menu_48_06, menu_48_07, menu_48_08, menu_48_09, menu_48_10, menu_48_11, menu_48_12, menu_48_13,}, // 48
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_49_04, menu_49_05, menu_49_06, menu_49_07, menu_49_08, menu_49_09, menu_49_10, menu_49_11, menu_49_12, menu_49_13,}, // 49
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_50_04, menu_50_05, menu_50_06, menu_50_07, menu_50_08, menu_50_09, menu_50_10, menu_50_11, menu_50_12,}, // 50
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_51_04, menu_51_05, menu_51_06, menu_51_07, menu_51_08, menu_51_09, menu_51_10, menu_51_11, menu_51_12,}, // 51
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_52_04, menu_52_05, menu_52_06, menu_52_07, menu_52_08, menu_52_09, menu_52_10, menu_52_11, menu_52_12,}, // 52
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_53_04, menu_53_05, menu_53_06, menu_53_07, menu_53_08, menu_53_09, menu_53_10, menu_53_11, menu_53_12,}, // 53
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_54_04, menu_54_05, menu_54_06, menu_54_07, menu_54_08, menu_54_09, menu_54_10, menu_54_11, menu_54_12,}, // 54
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_55_04, menu_55_05, menu_55_06, menu_55_07, menu_55_08, menu_55_09, menu_55_10, menu_55_11, menu_55_12, menu_55_13,}, // 55
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_56_05, menu_56_06, menu_56_07, menu_56_08, menu_56_09, menu_56_10, menu_56_11, menu_56_12, menu_56_13,}, // 56
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_57_04, menu_57_05, menu_57_06, menu_57_07, menu_57_08, menu_57_09, menu_57_10, menu_57_11, menu_57_12,}, // 57
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_58_05, menu_58_06, menu_58_07, menu_58_08, menu_58_09, menu_58_10, menu_58_11, menu_58_12, menu_58_13,}, // 58
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_59_04, menu_59_05, menu_59_06, menu_59_07, menu_59_08, menu_59_09, menu_59_10, menu_59_11, menu_59_12, menu_59_13, menu_59_14, menu_59_15,}, // 59
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_60_04, menu_60_05, menu_60_06, menu_60_07, menu_60_08, menu_60_09, menu_60_10, menu_60_11, menu_60_12, menu_60_13, menu_60_14, menu_60_15,}, // 60
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 61
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 62
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 63
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 64
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 65
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 66
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 67
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 68
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 69
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 70
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 71
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 72
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 73
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 74
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 75
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 76
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 77
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 78
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 79
		{menu_dummy, menu_dummy, menu_dummy, menu_80_03, menu_80_04, menu_80_05, menu_80_06, menu_80_07, menu_80_08, menu_80_09, menu_80_10, menu_80_11, menu_80_12, menu_80_13,}, // 80
		{menu_dummy, menu_dummy, menu_dummy, menu_81_03, menu_81_04, menu_81_05, menu_81_06, menu_81_07, menu_81_08, menu_81_09, menu_81_10, menu_81_11, menu_81_12,}, // 81
		{menu_dummy, menu_dummy, menu_dummy, menu_82_03, menu_82_04, menu_82_05, menu_82_06, menu_82_07, menu_82_08, menu_82_09, menu_82_10, menu_82_11, menu_82_12, menu_82_13,}, // 82
		{menu_dummy, menu_dummy, menu_dummy, menu_83_03, menu_83_04, menu_83_05, menu_83_06, menu_83_07, menu_83_08,},                             // 83
		{menu_dummy, menu_dummy, menu_dummy, menu_84_03, menu_dummy, menu_84_05, menu_84_06, menu_84_07, menu_84_08, menu_84_09,},                 // 84
		{menu_dummy, menu_dummy, menu_dummy, menu_85_03, menu_85_04, menu_85_05, menu_85_06,},                                                     // 85
		{menu_dummy, menu_dummy, menu_dummy, menu_86_03, menu_86_04, menu_86_05, menu_86_06,},                                                     // 86
		{menu_dummy, menu_dummy, menu_dummy, menu_87_03, menu_87_04, menu_87_05, menu_87_06,},                                                     // 87
		{menu_dummy, menu_dummy, menu_dummy, menu_88_03, menu_88_04, menu_88_05, menu_88_06, menu_88_07, menu_88_08,},                             // 88
		{menu_dummy, menu_dummy, menu_dummy, menu_89_03, menu_89_04, menu_89_05, menu_89_06, menu_89_07,},    						                         // 89
		{menu_dummy, menu_dummy, menu_dummy, menu_90_03, menu_90_04, menu_90_05, menu_90_06,},                                                     // 90
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_91_04, menu_91_05, menu_91_06, menu_91_07, menu_91_08,},                             // 91
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_92_04, menu_92_05, menu_92_06, menu_92_07, menu_92_08, menu_92_09,},                 // 92
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 93
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 94
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_95_04, menu_95_05, menu_95_06, menu_95_07, menu_95_08, menu_95_09,},                 // 95
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_96_05,},                                                                 // 96
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_97_04, menu_97_05, menu_97_06, menu_97_07, menu_97_08, menu_97_09,},                 // 97
		{menu_popup, menu_reset, menu_dummy, menu_dummy, menu_98_04, menu_98_05, menu_98_06, menu_98_07, menu_98_08, menu_98_09, menu_98_10, menu_98_11,}, // 98
		{menu_99_00, menu_dummy, menu_dummy, menu_dummy, menu_99_04, menu_99_05, menu_99_06,},                                                     // 99
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_100_04, menu_100_05, menu_100_06, menu_100_07, menu_100_08,},                        // 100
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_101_04, menu_101_05, menu_101_06, menu_101_07, menu_101_08, menu_101_09,},           // 101
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_102_05, menu_102_06,},																									 // 102
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_103_04, menu_103_05, menu_103_06, menu_103_07, menu_103_08, menu_103_09,},           // 103
		{menu_dummy, menu_dummy, menu_dummy, menu_dummy, menu_104_04, menu_104_05, menu_104_06, menu_104_07, menu_104_08, menu_104_09, menu_104_10, menu_104_11,}, // 104
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 105
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 106
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 107
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 108
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 109
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 110
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 111
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 112
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 113
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 114
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 115
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 116
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 117
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 118
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 119
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 120
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 121
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 122
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 123
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 124
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 125
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 126
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 127
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 128
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 129
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 130
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 131
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 132
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 133
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 134
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 135
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 136
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 137
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 138
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 139		
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 140
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 141
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 142
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 143
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 144
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 145
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 146
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 147
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 148
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 149
		{menu_dummy, menu_150_01, menu_150_02, },                                                                                                  // 150
		{menu_dummy, menu_151_01, menu_151_02, menu_151_03, menu_151_04, menu_151_05, menu_151_06, menu_151_07, },																 // 151
		{menu_dummy, menu_152_01, menu_152_02, menu_152_03, menu_152_04, menu_152_05, },                                                           // 152
		{menu_dummy, menu_153_01, menu_153_02, menu_153_03, menu_153_04, menu_153_05, menu_153_06, menu_153_07, menu_153_08, menu_153_09, menu_153_10, menu_153_11, menu_153_12,},	// 153
		{menu_dummy, menu_dummy, menu_154_02, menu_154_03, menu_154_04,},                                                                         // 154
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 155
		{menu_dummy, menu_dummy, menu_156_02, menu_156_03, menu_156_04, menu_156_05, menu_156_06, menu_156_07, menu_156_08, menu_156_09, menu_156_10, menu_156_11, menu_156_12, menu_156_13, menu_156_14, menu_156_15, menu_156_16,}, // 156
		{menu_dummy, menu_dummy, menu_157_02, menu_157_03, menu_157_04, menu_157_05,},                                                             // 157
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 158
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 159
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 160
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 161
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 162
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 163
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 164
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 165
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 166
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 167
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 168
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 169
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 170
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 171
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 172
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 173
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 174
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 175
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 176
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 177
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 178
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 179
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 180
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 181
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 182
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 183
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 184
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 185
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 186
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 187
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 188
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 189
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 190
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 191
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 192
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 193
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 194
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 195
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 196
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 197
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 198
		{menu_dummy, menu_dummy, menu_dummy, },                                                                                                    // 199
};

void menu_init(void)
{								   //01234567890123456789
	VFD_Single_Line_dump(LCD_L1_00, "    [ HIMAP-FI ]    \0");
	VFD_Single_Line_dump(LCD_L2_00, "                    \0");

	Screen_Position.x = 0; //init. screen info.
	Screen_Position.y = 0; //init. screen info.
	Screen_Position.select = 0; //init. screen info.
	menu_tables[Screen_Position.y][Screen_Position.x](NULL, 1);
}

void Save_Screen_Info(void)//2015-2-16, khs
{
	memcpy(&Save_Current_Screen, &Screen_Position, sizeof(Screen_Position));
	Screen_Position.data_change = POPUP_MENU;
}

void Restore_Screen_Info(void)//2015-2-16, khs
{
	memcpy(&Screen_Position, &Save_Current_Screen, sizeof(Screen_Position));
}

//int Test_Popup = 0;//khs, 2015-04-06 오후 5:16:08
void menu_drive(void)
{
	static unsigned int key_old = 0;

	TIMER.lcd = 0;
//if(Test_Popup == 1) {	//khs, 2015-04-06 오후 5:15:53
//	Save_Screen_Info();
//	Screen_Position.data_change = POPUP_MENU;
//	Test_Popup = 0;
//}	
	if(Screen_Position.data_change == REALTIME_MENU)
	{
		if(Data_Change_Count++ > 3)
		{
			menu_tables[Screen_Position.y][Screen_Position.x](KHS_Key_Press, 2);
			key_old = KHS_Key_Press;
			Data_Change_Count = 0;
			return;
		}
	} 
	else if(Screen_Position.data_change == POPUP_MENU)
	{
		//Save_Screen_Info();//테스트용으로 이곳에 위치시켰으며, 이 함수는 사고발생이 알려지는 시점에서 사용할 것.
		Screen_Position.y = 98;
		Screen_Position.x = 0;
		Screen_Position.data_change = NORMAL_MENU;

		menu_tables[Screen_Position.y][Screen_Position.x](KHS_Key_Press, 0);
		menu_tables[Screen_Position.y][Screen_Position.x](KHS_Key_Press, 1);
		return;
	}

	if((KHS_Key_Press == key_old) || (KHS_Key_Press == 0)) 
	{
		key_old = KHS_Key_Press;
		return;
	}
	key_old = KHS_Key_Press;

	menu_tables[Screen_Position.y][Screen_Position.x](KHS_Key_Press, 0); //2015.02.17
	menu_tables[Screen_Position.y][Screen_Position.x](KHS_Key_Press, 1); //2015.02.17
}
