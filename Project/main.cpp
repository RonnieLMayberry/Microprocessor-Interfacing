// DESCRIPTION OF WHAT IS IN THE CODE, PURPOSE OF
// OBJECTS (functions, definitions, etc) AND AUTHOR(S)

///*********************************************
// Project: I2C to LCD Interface-Routine
// Port PCF8574: 7  6  5  4  3  2  1  0
//		D7 D6 D5 D4 BL EN RW RS
///*********************************************

// Header File Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "mbed.h"
#include "i2c.h"

// DEFINES

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

// PORT
#define PCF8574T			0x27

#define no 0
#define yes 1
		
#define ADDRTC 0xD0
#define ACK 1
#define NACK 0

#define decode_bcd(x)	( ( x >> 4 ) * 10 + ( x & 0x0F ) )
#define encode_bcd(x)	( ( ( (x / 10) & 0x0F ) << 4 ) + (x % 10) )

#define line1 1
#define line2 2
#define line3 3
#define line4 4

I2C i2c(p9, p10);

// Keys
/** 	1 - c1r1, 2 - c2r1, 3 - c3r1, A - c4r1
	4 - c1r2, 5 - c2r2, 6 - c3r2, B - c4r2
	7 - c1r3, 8 - c2r3, 9 - c3r3, C - c4r3
	0 - c1r4, F - c2r4, E - c3r4, D - c4r4
**/ 

// Pmod KYPD out (rows)
// pulled high
DigitalIn r4(p11);
DigitalIn r3(p12);
DigitalIn r2(p13);
DigitalIn r1(p14);

// Pmod KYPD in (cols)
// low
DigitalOut c1(p5);
DigitalOut c2(p6);
DigitalOut c3(p7);
DigitalOut c4(p8);

// sun mon tue wed thu fri sat, 1 -> 7
static int twelveHr = 0, sec = 0, min = 0, hour = 0, day = 0, date = 0, month = 0, year = 0;
static bool alarmTriggered = 0;

// current line's characters (lines are 20 char long)
static char currentLine[20];
static int arrIndex = 0;	///////////////// RENAME
static int prevIndex = 0;	///////////////// RENAME


// FUNCTION PROTOTYPES
// initialize
static unsigned char wr_lcd_mode(char c, char mode);
unsigned char lcd_command(char c);
unsigned char lcd_data(char c);
void lcd_init(void);
void lcd_backlight(char on);
// defined
void sendLine(char line[], int lineNum);
void lcdPrev(char c);
void fillArr(char word[], char ch, int size, int choice);
void clearArr(void);
void lcdIdle(void);
void setClock(int mode);
int normalMode(void);
int calcMode(void);
int getSize(int result);
int getFromPad(int len);
int getOneChar(void);
int getNum(int rowNum, int colNum);
int getOpNum(void);
int rowScan(int colNum);
int rowPressed(void);
void partB(void);

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

void sendLine(char line[], int lineNum) {
	int lineSize = (int)strlen(line) - 1, lineOffset; 	//** possible error strlen
	switch (lineNum) {
		// line 1
		case 1:
			lineOffset = 0;
			break;
		// line 2
		case 2:
			lineOffset = 20;
			break;
		// line 3
		case 3:
			lineOffset = 40;
			break;
		// line 4
		case 4:
			lineOffset = 60;
			break;
	}
	
	for (int i = 0; i < lineOffset; i++) {
		lcd_command(0x14);
	}
	
	for (int i = 0; i < lineSize; i++) {
		lineOffset++;
		lcd_data(line[i]);
	}
	
	prevIndex = lineOffset; 				////// PREV CURSOR THING
	
	while (lineOffset != 0) {
		lineOffset--;
		lcd_command(LCD_CURSORSHIFT);
	}
}

void lcdPrev(char c) { 						//////////////////////////
	int lineOffset = prevIndex;
	for (int i = 0; i < lineOffset; i++) {
		lcd_command(0x14);
	}
	
	lineOffset++;
	lcd_data(c);
	
	prevIndex = lineOffset;
	
	while (lineOffset != 0) {
		lineOffset--;
		lcd_command(LCD_CURSORSHIFT);
	}
}

void fillArr(char word[], char ch, int size, int choice) {	/////////////////////////
	if (choice == 0) {
		for (int i = 0; i < size - 1; i++) {
			currentLine[arrIndex] = word[i];
			arrIndex++;
		}
	} else {
		currentLine[arrIndex] = ch;
		arrIndex++;
	}
}

void clearArr() {						///////////////////////
	sprintf(currentLine, "%d", 0);
	arrIndex = 0;
}

void lcdIdle() {
	int mode = 0;
	
	i2c.start();
	i2c.write(ADDRTC);
	i2c.write(0x00);
	i2c.write(0x00);
	i2c.stop();
	
	// initializing temp sensor
	i2c.start();
	i2c.write(0x90);
	i2c.write(0xAC);
	i2c.write(2);
	i2c.stop();
	setClock(0);
	setClock(1);
	
	while (true) {
		if (mode == 0) {
			lcd_command(LCD_CLEARDISPLAY);
			prevIndex = 0;
			mode = normalMode();
		} else {
			lcd_command(LCD_CLEARDISPLAY);
			prevIndex = 0;
			mode = calcMode();
		}
	}
}

void setClock(int mode) {
	if (mode == 0) {
		sendLine("SET CLOCK", line1);
		wait(2);
		lcd_command(LCD_CLEARDISPLAY);
		do {
			sendLine("Hour? (HH)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			hour = getFromPad(2);
			lcd_command(LCD_CLEARDISPLAY);
		} while (hour > 12 || hour < 1);
		
		do {
			sendLine("Minute? (MM)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			min = getFromPad(2);
			lcd_command(LCD_CLEARDISPLAY);
		} while (min > 59);
		
		do {
			sendLine("AM or PM? (1 or 2)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			twelveHr = getOneChar();
			lcd_command(LCD_CLEARDISPLAY);
		} while (twelveHr != 1 && twelveHr != 2);
		
		do {
			sendLine("Month? (MM)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			month = getFromPad(2);
			lcd_command(LCD_CLEARDISPLAY);
		} while (month > 12 || month < 1);
		
		do {
			sendLine("Day? (DD)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			date = getFromPad(2);
			lcd_command(LCD_CLEARDISPLAY);
		} while (date > 31 || date < 1);
		
		do {
			sendLine("Weekday? (1 -> 7)", line1);
			sendLine("(Sun->Sat)", line2);
			sendLine("Enter:", line3);
			sendLine(" ", line4);
			day = getFromPad(1);
			lcd_command(LCD_CLEARDISPLAY);
		} while (day > 7 || day < 1);
		
		do {
			sendLine("Year? (YYYY)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			year = getFromPad(4);
			lcd_command(LCD_CLEARDISPLAY);
		} while (year > 2099 || year < 2000);
		
		if (twelveHr == 1) {
			twelveHr = 0x40;
		} else if (twelveHr == 2) {
			twelveHr = 0x60;
		}
		
		// initialize clock with entered values
		i2c.start();
                i2c.write(ADDRTC);
                i2c.write(0x00); // clock
                i2c.write(encode_bcd(0)); // seconds
                i2c.write(encode_bcd(min));
                i2c.write(encode_bcd(hour) | twelveHr);
                i2c.write(encode_bcd(day));
                i2c.write(encode_bcd(date));
                i2c.write(encode_bcd(month));
                i2c.write(encode_bcd(year % 2000));
                i2c.start();
                i2c.write(ADDRTC);
                i2c.write(0x0e);
                i2c.write(0x20);
                i2c.write(0);
                i2c.stop();
	} else {
		sendLine("SET ALARM", line1);
		wait(2);
		lcd_command(LCD_CLEARDISPLAY);
		do {
			sendLine("Hour? (HH)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			hour = getFromPad(2);
			lcd_command(LCD_CLEARDISPLAY);
		} while (hour > 12 || hour < 1);
		
		do {
			sendLine("Minute? (MM)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			min = getFromPad(2);
			lcd_command(LCD_CLEARDISPLAY);
		} while (min > 59);
		
		do {
			sendLine("AM or PM? (1 or 2)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			twelveHr = getOneChar();
			lcd_command(LCD_CLEARDISPLAY);
		} while (twelveHr != 1 && twelveHr != 2);
		
		do {
			sendLine("Month? (MM)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			month = getFromPad(2);
			lcd_command(LCD_CLEARDISPLAY);
		} while (month > 12 || month < 1);
		
		do {
			sendLine("Day? (DD)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			date = getFromPad(2);
			lcd_command(LCD_CLEARDISPLAY);
		} while (date > 31 || date < 1);
		
		do {
			sendLine("Weekday? (1 -> 7)", line1);
			sendLine("(Sun->Sat)", line2);
			sendLine("Enter:", line3);
			sendLine(" ", line4);
			day = getFromPad(1);
			lcd_command(LCD_CLEARDISPLAY);
		} while (day > 7 || day < 1);
		
		do {
			sendLine("Year? (YYYY)", line1);
			sendLine("Enter:", line2);
			sendLine(" ", line4);
			year = getFromPad(4);
			lcd_command(LCD_CLEARDISPLAY);
		} while (year > 2099 || year < 2000);
		
		if (twelveHr == 1) {
			twelveHr = 0x40;
		} else if (twelveHr == 2) {
			twelveHr = 0x60;
		}
		
		// initialize alarm with entered values
		i2c.start();
                i2c.write(ADDRTC);
                i2c.write(0x07); // alarm
                i2c.write(encode_bcd(0)); // seconds
                i2c.write(encode_bcd(min));
                i2c.write(encode_bcd(hour) | twelveHr);
		i2c.write(encode_bcd(day));
                i2c.write(encode_bcd(date));
                i2c.write(encode_bcd(month));
                i2c.write(encode_bcd(year % 2000));
                i2c.start();
                i2c.write(ADDRTC);
                i2c.write(0x0e);
                i2c.write(0x20);
                i2c.write(0);
                i2c.stop();
	}
}

int normalMode() {
	int alarmTwelveHr, alarmMin, alarmHour, alarmDate, alarmFlag = 0;
	
	i2c.start();
	i2c.write(ADDRTC);
	i2c.write(0x00);
	i2c.start();
	i2c.write(ADDRTC | 1);
	sec = i2c.read(ACK);
	min = i2c.read(ACK);
	hour = i2c.read(ACK);
	day = i2c.read(ACK);
	date = i2c.read(ACK);
	month = i2c.read(ACK);
	year = i2c.read(NACK);
	i2c.stop();
	
	i2c.start();
	i2c.write(ADDRTC);
	i2c.write(0x08);
	i2c.start();
	i2c.write(ADDRTC | 1);
	alarmMin = i2c.read(ACK);
	alarmHour = i2c.read(ACK);
	alarmDate = i2c.read(NACK);
	i2c.stop();
	
	i2c.start();
	i2c.write(ADDRTC);
	i2c.write(0x0F);
	i2c.start();
	i2c.write(ADDRTC | 1);
	alarmFlag = i2c.read(NACK);
	i2c.stop();
	
	alarmFlag &= 1;
	if (!alarmTriggered) {
		alarmTriggered = alarmFlag;
	}
	
	twelveHr = (hour & 0x20) >> 5;
	sec = decode_bcd(sec);
	min = decode_bcd(min);
	hour = decode_bcd(hour);
	day = decode_bcd(day);
	date = decode_bcd(date);
	month = decode_bcd(month);
	year = decode_bcd(year);
	
	alarmTwelveHr = (alarmHour & 0x20) >> 5;
	alarmMin = decode_bcd(alarmMin);
	alarmHour = decode_bcd(alarmHour & 0x1F);
	alarmDate = decode_bcd(alarmDate);
	
	i2c.start();
	i2c.write(0x90);
	i2c.write(0x51);
	i2c.stop();
	
	i2c.start();
	i2c.write(0x90);
	i2c.write(0xaa);
	
	i2c.start();
	i2c.write(0x91);
	int topHW = i2c.read(ACK);
	int botHW = i2c.read(NACK);
	i2c.stop();
	
	int rawTemp = (topHW << 8) + botHW;
	float celsius = rawTemp >> 4;
	if (celsius > 0x7FF) {
		celsius = (((int)(2 << 12) - celsius) * 0.0625f);
	} else {
		celsius *= 0.0625f;
	}
	float fahren = (celsius * 1.8f) + 32;
	
	if (alarmFlag == 1) {
		sendLine("Alarm has expired", line1);
	}
	
	char forPrint[3];
	int size = sizeof(forPrint);
	
	// HOUR
	sprintf(forPrint, "%d", hour);
	if (hour < 10) {
		fillArr(forPrint, '0', size, 1);
		fillArr(forPrint, ' ', size - 1, 0);
	} else {
		fillArr(forPrint, ' ', size, 0);
	}
	// END HOUR
	
	fillArr(" ", ':', 1, 1);
	
	// MIN
	sprintf(forPrint, "%d", min);
	if (min < 10) {
		fillArr(forPrint, '0', size, 1);
		fillArr(forPrint, ' ', size - 1, 0);
	} else {
		fillArr(forPrint, ' ', size, 0);
	}
	// END MIN
	
	// AM/PM
	if (twelveHr == 1) {
		fillArr("PM ", ' ', 3, 0);
	} else {
		fillArr("AM ", ' ', 3, 0);
	}
	// END AM/PM
	
			////////////////////////////////////////// CAN BE FUNCTION
	// MONTH
	switch (month) {
		case 1: 
			fillArr("Jan ", ' ', 4, 0);
			break;
		case 2: 
			fillArr("Feb ", ' ', 4, 0);
			break;
		case 3: 
			fillArr("Mar ", ' ', 4, 0);
			break;
		case 4: 
			fillArr("Apr ", ' ', 4, 0);
			break;
		case 5: 
			fillArr("May ", ' ', 4, 0);
			break;
		case 6: 
			fillArr("Jun ", ' ', 4, 0);
			break;
		case 7: 
			fillArr("Jul ", ' ', 4, 0);
			break;
		case 8: 
			fillArr("Aug ", ' ', 4, 0);
			break;
		case 9: 
			fillArr("Sep ", ' ', 4, 0);
			break;
		case 10: 
			fillArr("Oct ", ' ', 4, 0);
			break;
		case 11: 
			fillArr("Nov ", ' ', 4, 0);
			break;
		case 12: 
			fillArr("Dec ", ' ', 4, 0);
			break;
	}
	// END MONTH
	
	// DATE
	sprintf(forPrint, "%d", date);
	if (date < 10) {
		fillArr(forPrint, '0', size, 1);
		fillArr(forPrint, ' ', size - 1, 0);
	} else {
		fillArr(forPrint, ' ', size, 0);
	}
	// END DATE
	
	fillArr(", ", ' ', 2, 0);
	
	// YEAR
	char yearArr[5];
	sprintf(yearArr, "%d", 2000 + year);
	fillArr(yearArr, ' ', sizeof(yearArr), 0);
	// END YEAR
	
	sendLine(currentLine, line2);
	
	clearArr();
	char celArr[5];
	sprintf(celArr, "%.2f", (double)celsius);
	
	char fahArr[5];
	sprintf(fahArr, "%.2f", (double)fahren);
	
	// TEMP: C (F)
	fillArr("Temp: ", ' ', 6, 0);
	fillArr(celArr, ' ', size, 0);
	fillArr(" ", 0xDF, 1, 1); // deg
	fillArr(" ", 'C', 1, 1);
	fillArr(" ", '(', 1, 1);
	fillArr(fahArr, ' ', size, 0);
	fillArr(" ", 0xDF, 1, 1);
	fillArr(" ", 'F', 1, 1);
	fillArr(" ", ')', 1, 1);
	sendLine(currentLine, line2);
	clearArr();
	
	int chr;
	int colNum = 1;
	float counter = 0;
	while (counter < 1.25f) {
		chr = rowScan(colNum);
		if (chr != 0) {
			chr = getNum(chr, colNum);
			if (chr == 15) {
				if (alarmFlag == 1) {
					i2c.start();
					i2c.write(ADDRTC);
					i2c.write(0x0F);
					i2c.write(0x00);
					i2c.stop();
					return 0;
				}
				return 1;
			} else if (chr == 14 && !alarmTriggered) {
				lcd_command(LCD_CLEARDISPLAY);
				sendLine("Alarm Setting", line1);
				
				// ALARM HOUR
				sprintf(forPrint, "%d", alarmHour);
				if (alarmHour < 10) {
					fillArr(forPrint, '0', size, 1);
					fillArr(forPrint, ' ', size - 1, 0);
				} else {
					fillArr(forPrint, ' ', size, 0);
				}
				// END ALARM HOUR
				
				fillArr(" ", ':', 1, 1);
				
				// ALARM MIN
				sprintf(forPrint, "%d", alarmMin);
				if (alarmMin < 10) {
					fillArr(forPrint, '0', size, 1);
					fillArr(forPrint, ' ', size - 1, 0);
				} else {
					fillArr(forPrint, ' ', size, 0);
				}
				// END ALARM MIN
				
				// ALARM AM/PM
				if (alarmTwelveHr == 1) {
					fillArr("PM ", ' ', 3, 0);
				} else {
					fillArr("AM ", ' ', 3, 0);
				}
				// END ALARM AM/PM
				
						////////////////////////////////////////// CAN BE FUNCTION
				// MONTH
				switch (month) {
					case 1: 
						fillArr("Jan ", ' ', 4, 0);
						break;
					case 2: 
						fillArr("Feb ", ' ', 4, 0);
						break;
					case 3: 
						fillArr("Mar ", ' ', 4, 0);
						break;
					case 4: 
						fillArr("Apr ", ' ', 4, 0);
						break;
					case 5: 
						fillArr("May ", ' ', 4, 0);
						break;
					case 6: 
						fillArr("Jun ", ' ', 4, 0);
						break;
					case 7: 
						fillArr("Jul ", ' ', 4, 0);
						break;
					case 8: 
						fillArr("Aug ", ' ', 4, 0);
						break;
					case 9: 
						fillArr("Sep ", ' ', 4, 0);
						break;
					case 10: 
						fillArr("Oct ", ' ', 4, 0);
						break;
					case 11: 
						fillArr("Nov ", ' ', 4, 0);
						break;
					case 12: 
						fillArr("Dec ", ' ', 4, 0);
						break;

				}
				// END MONTH
				
				// ALARM DATE
				sprintf(forPrint, "%d", alarmDate);
				if (alarmDate < 10) {
					fillArr(forPrint, '0', size, 1);
					fillArr(forPrint, ':', size - 1, 0);
				} else {
					fillArr(forPrint, ' ', size, 0);
				}
				// END ALARM DATE
				
				fillArr(", ", ' ', 2, 0);
				
				// YEAR
				sprintf(yearArr, "%d", 2000 + year);
				sendLine(currentLine, line2);
				clearArr();
				
				do {
					chr = rowScan(colNum);
					if (chr != 0) {
						chr = getNum(chr, colNum);
					}
					if (colNum == 4) {
						colNum = 1;
					} else {
						colNum++;
					}
					wait(0.03);
				} while (chr != 14 && chr != 15);
				
				switch (chr) {
					case 14:
						return 0;
					case 15:
						return 1;
				}
			}
		}
		if (colNum == 4) {
			colNum = 1; // wrap around
		} else {
			colNum++; // increment through columns
		}
		wait(0.03);
		counter += 0.03;
	}
	return 0; // reaches here when alarm flag is set
}

int calcMode() {
	int op1, op2, opNum, result, colNum = 1;
	
	op1 = getFromPad(3);
	result = op1;
	
	opNum = getOpNum();
	
	while (opNum != 14 && opNum != 15) {
		switch (opNum) {
			case 10:
				lcdPrev('+');
				break;
			case 11:
				lcdPrev('-');
				break;
			case 12:
				lcdPrev('*');
				break;
			case 13:
				lcdPrev('/');
				break;
		}
		
		op2 = getFromPad(3);	// possibly combine switches
		
		switch (opNum) {
			case 10:
				result += op2;
				break;
			case 11:
				result -= op2;
				break;
			case 12:
				result *= op2;
				break;
			case 13:
				result /= op2;
				break;
		}
		
		opNum = getOpNum();
	}
	
	// -> normal mode
	if (opNum == 15) {
		lcd_command(LCD_CLEARDISPLAY);
		return 0;
	} else if (opNum == 14) {
		char resultArr[7];
		sprintf(resultArr, "=%d", result);
		sendLine(resultArr, line3);
		wait(0.1);
		while (rowScan(colNum) == 0) {
			if (colNum == 4) {
				colNum  = 1;
			} else {
				colNum++;
			}
			wait(0.05);
		}
		lcd_command(LCD_CLEARDISPLAY);
		return 1;
	}
}

int getSize(int result) {
	int counter = 6, outcome = 0, startingDiv = 100000;
	
	if (result == 0) {
		return 3;
	}
	
	while (outcome == 0) {
		outcome = result / startingDiv;
		
		if (outcome != 0) {
			if (result < 0) {
				return counter + 3;
			} else {
				return counter + 2;
			}
		} else {
			counter--;
			startingDiv /= 10;
		}
	}
}

int getFromPad(int length) {
	int count = 0, first = 0, second = 0, third = 0, fourth = 0, num = 0, colNum = 1;
	
	// waits until value of length "length" is entered
	// count keeps track of how many keys have been pressed
	while (count < length) {
		if (count == 0) { 
			first = rowScan(colNum);
			if (first != 0) {
				first = getNum(first, colNum);
				if (first < 10) {
					count++;
					char c = first + '0';
					lcdPrev(c);
				}
			}
		} else if (count == 1) {
			second = rowScan(colNum);
			if (second != 0) {
				second = getNum(second, colNum);
				if (second < 10) {
					count++;
					char c = second + '0';
					lcdPrev(c);
				}
			}
		} else if (count == 2) {
			third = rowScan(colNum);
			if (third != 0) {
				third = getNum(third, colNum);
				if (third < 10) {
					count++;
					char c = third + '0';
					lcdPrev(c);
				}
			}
		} else if (count == 3) {
			fourth = rowScan(colNum);
			if (fourth != 0) {
				fourth = getNum(fourth, colNum);
				if (third < 10) {
					count++;
					char c = fourth + '0';
					lcdPrev(c);
				}
			}
		}
		
		if (colNum == 4) {
			colNum = 1;
		} else {
			colNum++;
		}
		wait(0.03);
	}
	
	switch (length) {
		case 1:
			num = first;
			break;
		case 2:
			num = first * 10 + second;
			break;
		case 3:
			num = first * 100 + second * 10 + third;
			break;
		case 4:
			num = first * 1000 + second * 100 + third * 10 + fourth;
			break;
	}
	return num;
}

int getOneChar() {
	int count = 0, first = 0, colNum = 1;
	char keys[16] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'A', 'B', 
		'C', 'D', 'E', 'F'
	};
	
	while (count < 1) {
		first = rowScan(colNum);
		if (first != 0) {
			first = getNum(first, colNum);
			count++;
			lcdPrev(keys[first]);
		}
		
		if (colNum == 4) {
			colNum = 1;
		} else {
			colNum++;
		}
		wait(0.03);
	}
	
	return first;
}

int getNum(int rowNum, int colNum) {
	int keyPad[4][4] = {
		{ 1, 2,  3,  10 },
		{ 4, 5,  6,  11 },
		{ 7, 8,  9,  12 },
		{ 0, 15, 14, 13 }
	};
	return keyPad[rowNum - 1][colNum - 1];
}

int getOpNum() {
	int op = 0, colNum = 1;
	
	while (op < 10) {
		op = rowScan(colNum);
		if (op != 0) {
			op = getNum(op, colNum);
		}
		
		if (colNum == 4) {
			colNum = 1;
		} else {
			colNum++;
		}
		wait(0.03);
	}
	return op;
}

int rowScan(int colNum) {
	int rowNum = 0;
	if (colNum == 1) {
		c1 = 0;
	} else if (colNum == 2) {
		c2 = 0;
	} else if (colNum == 3) {
		c3 = 0;
	} else {
		c4 = 0;
	}
	
	rowNum = rowPressed();
	
	c1 = 1;
	c2 = 1;
	c3 = 1;
	c4 = 1;
	return rowNum;
}

int rowPressed() {
	int num = 0;
	if (r1 == 0) {
		num = 1;
	} else if (r2 == 0) {
		num = 2;
	} else if (r3 == 0) {
		num = 3;
	} else if (r4 == 0) {
		num = 4;
	}
	return num;
}

void partB() {
	// part a - display uppercase alphabet
	lcd_command(LCD_CLEARDISPLAY);
	sendLine("ABCDEFGHIJKLM", line1);
	sendLine("NOPQRSTUVWXYZ", line2);
	wait(1);
	
	// part b - display upper and lowercase alphabet
	lcd_command(LCD_CLEARDISPLAY);
	sendLine("ABCDEFGHIJKLM", line1);
	sendLine("NOPQRSTUVWXYZ", line2);
	sendLine("abcdefghijklm", line3);
	sendLine("nopqrstuvwxyz", line4);
	wait(1);
	
	// part c - display upper and lowercase alphabet, and numbers
	lcd_command(LCD_CLEARDISPLAY);
	sendLine("0123456789.", line1);
	sendLine(".9876543210", line2);
	wait(1);
	
	lcd_command(LCD_CLEARDISPLAY);
	prevIndex = 0; 	////// PREV CURSOR THING
}

int main() {
	lcd_init();
	partB();
	lcdIdle();
}
