#include "mbed.h"
#include "i2c.h"

// LCD Commands
// -------------------------------------------------------------------------
#define LCD_CLEARDISPLAY		0x01
#define LCD_RETURNHOME			0x02
#define LCD_ENTRYMODESET		0x04
#define LCD_DISPLAYCONTROL		0x08
#define LCD_CURSORSHIFT			0x10
#define LCD_FUNCTIONSET			0x20
#define LCD_SETCGRAMADDR		0x40
#define LCD_SETDDRAMADDR		0x80

// flags for display entry mode
// -------------------------------------------------------------------------
#define LCD_ENTRYRIGHT			0x00
#define LCD_ENTRYLEFT			0x02
#define LCD_ENTRYSHIFTINCREMENT		0x01
#define LCD_ENTRYSHIFTDECREMENT		0x00

// flags for display on/off and cursor control
// -------------------------------------------------------------------------
#define LCD_DISPLAYON			0x04
#define LCD_DISPLAYOFF			0x00
#define LCD_CURSORON			0x02
#define LCD_CURSOROFF			0x00
#define LCD_BLINKON			0x01
#define LCD_BLINKOFF			0x00

// flags for display/cursor shift
// -------------------------------------------------------------------------
#define LCD_DISPLAYMOVE			0x08
#define LCD_CURSORMOVE			0x00
#define LCD_MOVERIGHT			0x04
#define LCD_MOVELEFT			0x00

// flags for function set
// -------------------------------------------------------------------------
#define LCD_8BITMODE			0x10
#define LCD_4BITMODE			0x00
#define LCD_2LINE			0x08
#define LCD_1LINE			0x00
#define LCD_5x10DOTS			0x04
#define LCD_5x8DOTS			0x00

///*********************************************
// Project: I2C to LCD Interface-Routine
// Port PCF8574: 7  6  5  4  3  2  1  0
//		D7 D6 D5 D4 BL EN RW RS
///*********************************************

#define PCF8574T			0x27

I2C i2c(p9, p10);

static unsigned char wr_lcd_mode(char c, char mode) {
	char ret = 1;
	char seq[5];
	static char backlight = 8;
	
	if (mode == 8) {
		backlight = (c != 0) ? 8 : 0;
		return 0;
	}
	mode |= backlight;
	
	seq[0] = mode;			// EN = 0, RW = 0, RS = mode
	seq[1] = (c & 0xF0) | mode | 4;	// EN = 1, RW = 0, RS = 1
	seq[2] = seq[1] & ~4;		// EN = 0, RW = 0, RS = 1
	seq[3] = (c << 4) | mode | 4;	// EN = 1, RW = 0, RS = 1
	seq[4] = seq[3] & ~4;		// EN = 0, RW = 0, RS = 1
	
	i2c.start();
	i2c.write(PCF8574T << 1);
	uint8_t i;
	
	for (i = 0; i < 5; i++) {
		i2c.write(seq[i]);
		wait(0.002);
	}
	ret = 0;
	
	i2c.stop();
	if (!(mode & 1) && c <= 2) {
		wait(0.2);	// CLS and HOME
	}
	return ret;
}

unsigned char lcd_command(char c) {
	wr_lcd_mode(c, 0);
}

unsigned char lcd_data(char c) {
	wr_lcd_mode(c, 1);
}

void lcd_init(void) {
	char i;
	
	// High-Nibble von Byte 8 = Display Control:
	// 1DCB**** D: Disp on/off; C: Cursor on/off; B: blink on/off
	char init_sequenz[] = { 0x33, 0x32, 0x28, 0x0C, 0x06, 0x01 };
	wait(1);
	
	for (i = 0; i < sizeof(init_sequenz); i++) {
		lcd_command(init_sequenz[i]);
	}
}

void lcd_backlight(char on) {
	wr_lcd_mode(on, 8);
}

int main() {
	lcd_init();
}

// OBJECTIVE OF B:
// 	a: display all alphabets (A-> Z) simultaneously on LCD
//	b: display all alphabets (A-> z) simultaneously on LCD
//	c: display all numbers simultaneously in order, including
//		decimal point on LCD ("0 -> 9 .", next row: ". 9 -> 0")
