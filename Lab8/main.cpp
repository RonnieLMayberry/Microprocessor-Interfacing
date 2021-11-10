#include "mbed.h"
#include "I2C.h"
#include <stdint.h>

#define no 0
#define yes 1
		
#define ADDRTC 0xD0
#define ACK 0
#define NACK 1
#define pm_offset 0x20
#define twelveHrTime 0x40

#define decode_bcd(x)	( ( x >> 4 ) * 10 + ( x & 0x0F ) )
#define encode_bcd(x)	( ( ( (x / 10) & 0x0F ) << 4 ) + (x % 10) )

DigitalOut myCLR(p28);
// !CLR is pin 28 LPC

DigitalOut myCLK(p27);
// CLK is pin 27 LPC

DigitalOut myData(p30);
// DATA(A/B) is pin 30 LPC

// Keys
// 1 - c1r1
// 2 - c2r1
// 3 - c3r1
// A - c4r1
// 4 - c1r2
// 5 - c2r2
// 6 - c3r2
// B - c4r2
// 7 - c1r3
// 8 - c2r3
// 9 - c3r3
// C - c4r3
// 0 - c1r4
// F - c2r4
// E - c3r4
// D - c4r4

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

// 7-segment order: --> 7, 6, 5, 4, 3, 2, 1

// 0 - 0, 1, 2, 3, 4, 5, x, x
// 1 - x, 1, 2, x, x, x, x, x
// 2 - 0, 1, x, 3, 4, x, 6, x
// 3 - 0, 1, 2, 3, x, x, 6, x
// 4 - x, 1, 2, x, x, 5, 6, x
// 5 - 0, x, 2, 3, x, 5, 6, x
// 6 - 0, x, 2, 3, 4, 5, 6, x
// 7 - 0, 1, 2, 3, x, x, x, x
// 8 - 0, 1, 2, 3, 4, 5, 6, x
// 9 - 0, 1, 2, x, x, 5, 6, x

int16_t i2cBinVal;
int shortBin;
int celsius;
int fahrenheit;
char i2cArray[4];
char i2cForCut[16];
char trimmedi2c[12];

I2C i2c(p9, p10);

int sec, min, hour, day, date, month, year;
// 24hr format hourW = 22; s m t w th f sa 1->7
// Feb 28, 2020 -> 11:57pm, friday (dayW = 0x6)
int secW = 0x00, minW = 0x57, hourW = 0x11 + pm_offset + twelveHrTime, // 0101 0001 0x51, hour 11(+0x60 for PM)
    dayW = 0x06, dateW = 0x28, monthW = 0x02,
    yearW = 0x20;
		
// take in the data value, and the number of cycles
void cycle(int data, int num) {
        // set data value
        myData = data;
        // go through 'num' cycles with set data value
        for (int i = 1; i <= num; i++) {
                myCLK = 1;
                myCLK = 0;
        }
}

void clear() {
        // reset !CLR by setting to 1, 0, then 1
        myCLR = 1;
        wait(.1);
        myCLR = 0;
        wait(.1);
        myCLR = 1;
}

// sends given value/character to the LED
void sendChar(char val, int endPeriod) {
	// endPeriod => 1 is on, 0 is off
	// tells the program whether the character should
	// have a period at the end
	endPeriod = !endPeriod;
        switch (val) {
        case '0':
                // '0' ... endPeriod 1 0 0 0 0 0 0
		cycle(endPeriod, 1);
                cycle(1, 1);
                cycle(0, 6);
                break;
        case '1':
                // '1' ... endPeriod 1 1 1 1 0 0 1
		cycle(endPeriod, 1);
                cycle(1, 4);
                cycle(0, 2);
                cycle(1, 1);
                break;
        case '2':
                // '2' ... endPeriod 0 1 0 0 1 0 0
                cycle(endPeriod, 1);
                cycle(0, 1);
                cycle(1, 1);
                cycle(0, 2);
                cycle(1, 1);
                cycle(0, 2);
                break;
        case '3':
                // '3' ... endPeriod 0 1 1 0 0 0 0
                cycle(endPeriod, 1);
                cycle(0, 1);
                cycle(1, 2);
                cycle(0, 4);
                break;
        case '4':
                // '4' ... endPeriod 0 0 1 1 0 0 1
                cycle(endPeriod, 1);
                cycle(0, 2);
                cycle(1, 2);
                cycle(0, 2);
                cycle(1, 1);
                break;
        case '5':
                // '5' ... endPeriod 0 0 1 0 0 1 0
                cycle(endPeriod, 1);
                cycle(0, 2);
                cycle(1, 1);
                cycle(0, 2);
                cycle(1, 1);
                cycle(0, 1);
                break;
        case '6':
                // '6' ... endPeriod 0 0 0 0 0 1 0
                cycle(endPeriod, 1);
                cycle(0, 5);
                cycle(1, 1);
                cycle(0, 1);
                break;
        case '7':
                // '7' ... endPeriod 1 1 1 1 0 0 0
		cycle(endPeriod, 1);
                cycle(1, 4);
                cycle(0, 3);
                break;
        case '8':
                // '8' ... endPeriod 0 0 0 0 0 0 0
                cycle(endPeriod, 1);
                cycle(0, 7);
                break;
        case '9':
                // '9' ... endPeriod 0 0 1 1 0 0 0
                cycle(endPeriod, 1);
                cycle(0, 2);
                cycle(1, 2);
                cycle(0, 3);
                break;
        case 'A':
                // 'A' ... endPeriod 0 0 0 1 0 0 0
                cycle(endPeriod, 1);
                cycle(0, 3);
                cycle(1, 1);
                cycle(0, 3);
                break;
        case 'b':
                // 'b' ... endPeriod 0 0 0 0 0 1 1
                cycle(endPeriod, 1);
                cycle(0, 5);
                cycle(1, 2);
                break;
        case 'C':
                // 'C' ... endPeriod 1 0 0 0 1 1 0
		cycle(endPeriod, 1);
                cycle(1, 1);
                cycle(0, 3);
                cycle(1, 2);
                cycle(0, 1);
                break;
        case 'd':
                // 'd' ... endPeriod 0 1 0 0 0 0 1
                cycle(endPeriod, 1);
                cycle(0, 1);
                cycle(1, 1);
                cycle(0, 4);
                cycle(1, 1);
                break;
        case 'E':
                // 'E' ... endPeriod 0 0 0 0 1 1 0
                cycle(endPeriod, 1);
                cycle(0, 4);
                cycle(1, 2);
                cycle(0, 1);
                break;
        case 'F':
                // 'F' ... endPeriod 0 0 0 1 1 1 0
                cycle(endPeriod, 1);
                cycle(0, 3);
                cycle(1, 3);
                cycle(0, 1);
                break;
	case 'H':
		// H
                cycle(endPeriod, 1);
                cycle(0, 3);
                cycle(1, 1);
                cycle(0, 2);
                cycle(1, 1);
		break;
	case 'I':
		// I
                cycle(endPeriod, 1);
                cycle(1, 4);
                cycle(0, 2);
                cycle(1, 1);
                break;
	case 'n':
		// n
                cycle(endPeriod, 1);
                cycle(0, 1);
                cycle(1, 1);
                cycle(0, 1);
                cycle(1, 1);
                cycle(0, 1);
                cycle(1, 2);
		break;
	case 'O':
		// O
                cycle(endPeriod, 1);
		cycle(1, 1);
                cycle(0, 6);
		break;
	case 'r':
		// r
                cycle(endPeriod, 1);
		cycle(1, 1);
                cycle(0, 2);
                cycle(1, 2);
                cycle(0, 2);
		break;
	case 'S':
		// S
                cycle(endPeriod, 1);
                cycle(0, 2);
                cycle(1, 1);
                cycle(0, 2);
                cycle(1, 1);
                cycle(0, 1);
		break;
	case 'T':
		// T
                cycle(endPeriod, 1);
                cycle(0, 4);
                cycle(1, 3);
		break;
	case 'U':
		// U
                cycle(endPeriod, 1);
		cycle(1, 1);
                cycle(0, 5);
                cycle(1, 1);
		break;
	case 'W':
		// W
                cycle(endPeriod, 1);
                cycle(0, 1);
                cycle(1, 2);
                cycle(0, 4);
                break;
        case '.':
                // '.' ... endPeriod 1 1 1 1 1 1 1
		endPeriod = 0; // no matter what this prints a period
                cycle(endPeriod, 1);
                cycle(1, 7);
                break;
	// represents a single quote
	case '#':
		// ''' ... endPeriod 1 1 1 1 1 0 1
		cycle(endPeriod, 1);
		cycle(1, 5);
		cycle(0, 1);
		cycle(1, 1);
		break;
	case '"':
		// '"' ... endPeriod 1 0 1 1 1 0 1
		cycle(endPeriod, 1);
		cycle(1, 1);
		cycle(0, 1);
		cycle(1, 3);
		cycle(0, 1);
		cycle(1, 1);
		break;
        }
}

void sendDate(int val) {
        switch (val) {
        // sunday
        case 0x01:
                // n
                sendChar('n', no);

                // U
                sendChar('U', no);

                // S
                sendChar('S', no);

                break;
        // monday
        case 0x02:
                // n
                sendChar('n', no);

                // O
                sendChar('O', no);

                // E
                sendChar('E', no);

                break;
        // tuesday
        case 0x03:
                // E
                sendChar('E', no);

                // U
                sendChar('U', no);

                // T
                sendChar('T', no);

                break;
        // wednesday
        case 0x04:
                // d
                sendChar('d', no);

                // E
                sendChar('E', no);

                // backwards E (W)
                sendChar('W', no);

                break;
        // thursday
        case 0x05:
                // U
                sendChar('U', no);

                // H
                sendChar('H', no);

                // T
                sendChar('T', no);

                break;
        // friday
        case 0x06:
                // I
                sendChar('I', no);

                // r
                sendChar('r', no);

                // F
                sendChar('F', no);

                break;
        // saturday
        case 0x07:
                // T
                sendChar('T', no);

                // A
                sendChar('A', no);

                // S
                sendChar('S', no);

                break;
	default:
		sendChar('.', yes);
		sendChar('.', yes);
		sendChar('.', yes);
	
		break;
        }
}



void clearArray() {
        for (int i = 0; i < 4; i++) {
                i2cArray[i] = '0';
                sendChar('0', no);
        }
}

int pow(int base, int power) {
        int result = 1;
        if (power > 0) {
                for (int i = 1; i <= power; i++) {
                        result *= base;
                }
                return result;
        } else {
                return 1;
        }
}

int binToDecimal(int binary) {
        int result = 0, remainder, power = 0;
        while (binary > 0) {
                remainder = binary % 10;
                result += (remainder * pow(2, power));
                binary /= 10;
                power++;
        }
        return result;
}

void displayCelsius() {
        // get i2cerature in celsius
        clearArray();
        // send celsius i2c to char array
        sprintf(i2cArray, "%2.0d", shortBin);
        sendChar('C', no); // send C for celsius
        //i2cArray is size 4 {0, 1, 2, 3}
        // send first two chars of i2cArray to 7Seg
        for (int i = sizeof(i2cArray) - 3; i >= 0; i--) {
                sendChar(i2cArray[i], no);
        }
        wait(1);
}

void displayFahrenheit() {
        // convert i2c to fahrenheit
        fahrenheit = (shortBin * 9) / 5 + 32;
        clearArray();
        sprintf(i2cArray, "%d", fahrenheit);
        sendChar('F', no); // send F for fahrenheit
        // send first two chars of i2cArray to 7Seg
        for (int i = sizeof(i2cArray) - 2; i >= 0; i--) {
                sendChar(i2cArray[i], no);
        }
        wait(1);
}

// i2c sensor address = 1001 000; 0x90
void tempSetup() {
        // p9 - sda, p10 - scl
        //I2C i2c(p9, p10);
        i2c.start();
        i2c.write(0xac);
        i2c.write(2);
        i2c.stop();

        // start collecting data
        i2c.start();
        i2c.write(0x90); // 0x90 -> write, 0x91 -> read
        i2c.write(0x51);
        i2c.stop();

        // read i2c data
        i2c.start();
        i2c.write(0x90);
        i2c.write(0xaa);

        i2c.start();
        i2c.write(0x91);
        // reads i2c from DS1631 as 16 bit int?
        i2cBinVal = i2c.read(0);
        i2c.stop();

        // cut 4 MSBs from i2cBinVal
        sprintf(i2cForCut, "%u", (unsigned int) i2cBinVal);
        for (int i = 0; i <= sizeof(trimmedi2c) - 1; i++) {
                trimmedi2c[i] = i2cForCut[i];
        }
        sscanf(trimmedi2c, "%u", & shortBin);
}

// #define ADDRTC 0xD0

// i2c sensor address = 1101 000; 0xD0
void clockWrite() {
        // p32 - sda, p31 - scl
        // clear register values
        i2c.start();
        i2c.write(ADDRTC | 0); // write slave addr + write
        i2c.write(0x00);       // write reg addr, 1st clk reg
        i2c.write(0x00);
        i2c.stop();

        i2c.start();
        i2c.write(ADDRTC | 0); // write slave addr + write
        i2c.write(0x00);       // write reg addr, 1st clk reg
        // write as 8bit BCD
        i2c.write(secW); 	
        i2c.write(minW);	// ex. 0x19 -> BCD 0001 1001 -> minute 19
        i2c.write(hourW);
        i2c.write(dayW);	// 0x06
        i2c.write(dateW);
        i2c.write(monthW);
        i2c.write(yearW);
        i2c.start();
        i2c.write(ADDRTC | 0);    // write slave addr + write
        i2c.write(0x0e);      // write reg addr, ctrl reg
        i2c.write(0x20);      // enable osc, bbsqi
        i2c.write(0);         // clear osf, alarm flags
        i2c.stop();
}

void clockRead() {
        i2c.start(); // start 
        i2c.write(ADDRTC | 0); // slave ID (R/W bit 0)
        i2c.write(0x00); // start reg address

        i2c.start(); // "restart"
        i2c.write(ADDRTC | 1); // slave ID (R/W bit 1)

        // read block of data into variables
	// begin at last addr in reg ptr
        sec = i2c.read(ACK); // 0
        min = i2c.read(ACK); // 255
        hour = i2c.read(ACK); // 255
        day = i2c.read(ACK); // 255
        date = i2c.read(ACK); // 
        month = i2c.read(ACK);
        year = i2c.read(NACK);

        i2c.stop(); // stop block read
}

void clockDisplay() {
        // start in BCD format
        // day of week increments at midnight
        // 1->7; sunday->saturday
        // hours register bit 6 is 12/24 mode select
        // 12hr mode -> bit 5 is AM/PM bit (1 is PM)
        // 24hr mode -> bit 5 is 2nd 10 hour bit (20-23 hrs)
        // century bit 7 acts as year register overflow (99->00; bit7: 0 -> 1)

	// starts Feb 28, 2020 -> 11:57pm, friday (dayW = 0x6)
	sendDate(day);
	wait(2);
	
	//for (int j = 1; j <= 7; j++) {
	//	sendDate(j);
	//	wait(1);
	//}
	
	char test[3];
	sprintf(test, "%d", day);
	for (int i = sizeof(test); i >= 0; i--) {
		sendChar(test[i], no);
	}
	wait(10);
}

int main() {
        // call clear() function
        clear();

        // ensure i2c begins at 0
        myCLK = 0;

        clockWrite(); // set time to 11:57pm, Feb 28, 2020
	
        while (true) {
                // i2c
                clockRead();
                clockDisplay();
		
		// i2c SENSOR
                tempSetup();
                displayCelsius();
                displayFahrenheit();
        }
}
