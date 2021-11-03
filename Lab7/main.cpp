#include "mbed.h"

#include "I2C.h"

#include <stdint.h>

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

int tempHex;
int16_t tempBinVal;
int celsius;
int fahrenheit;
char tempArray[4];
char tempForCut[16];
char trimmedTemp[12];

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
void sendChar(char val) {
        switch (val) {
        case '0':
                // '0' ... 1 1 0 0 0 0 0 0
                cycle(1, 2);
                cycle(0, 6);
                break;
        case '1':
                // '1' ... 1 1 1 1 1 0 0 1
                cycle(1, 5);
                cycle(0, 2);
                cycle(1, 1);
                break;
        case '2':
                // '2' ... 1 0 1 0 0 1 0 0
                cycle(1, 1);
                cycle(0, 1);
                cycle(1, 1);
                cycle(0, 2);
                cycle(1, 1);
                cycle(0, 2);
                break;
        case '3':
                // '3' ... 1 0 1 1 0 0 0 0
                cycle(1, 1);
                cycle(0, 1);
                cycle(1, 2);
                cycle(0, 4);
                break;
        case '4':
                // '4' ... 1 0 0 1 1 0 0 1
                cycle(1, 1);
                cycle(0, 2);
                cycle(1, 2);
                cycle(0, 2);
                cycle(1, 1);
                break;
        case '5':
                // '5' ... 1 0 0 1 0 0 1 0
                cycle(1, 1);
                cycle(0, 2);
                cycle(1, 1);
                cycle(0, 2);
                cycle(1, 1);
                cycle(0, 1);
                break;
        case '6':
                // '6' ... 1 0 0 0 0 0 1 0
                cycle(1, 1);
                cycle(0, 5);
                cycle(1, 1);
                cycle(0, 1);
                break;
        case '7':
                // '7' ... 1 1 1 1 1 0 0 0
                cycle(1, 5);
                cycle(0, 3);
                break;
        case '8':
                // '8' ... 1 0 0 0 0 0 0 0
                cycle(1, 1);
                cycle(0, 7);
                break;
        case '9':
                // '9' ... 1 0 0 1 1 0 0 0
                cycle(1, 1);
                cycle(0, 2);
                cycle(1, 2);
                cycle(0, 3);
                break;
        case 'A':
                // 'A' ... 1 0 0 0 1 0 0 0
                cycle(1, 1);
                cycle(0, 3);
                cycle(1, 1);
                cycle(0, 3);
                break;
        case 'b':
                // 'b' ... 1 0 0 0 0 0 1 1
                cycle(1, 1);
                cycle(0, 5);
                cycle(1, 2);
                break;
        case 'C':
                // 'C' ... 1 1 0 0 0 1 1 0
                cycle(1, 2);
                cycle(0, 3);
                cycle(1, 2);
                cycle(0, 1);
                break;
        case 'd':
                // 'd' ... 1 0 1 0 0 0 0 1
                cycle(1, 1);
                cycle(0, 1);
                cycle(1, 1);
                cycle(0, 4);
                cycle(1, 1);
                break;
        case 'E':
                // 'E' ... 1 0 0 0 0 1 1 0
                cycle(1, 1);
                cycle(0, 4);
                cycle(1, 2);
                cycle(0, 1);
                break;
        case 'F':
                // 'F' ... 1 0 0 0 1 1 1 0
                cycle(1, 1);
                cycle(0, 3);
                cycle(1, 3);
                cycle(0, 1);
                break;
        case '.':
                // '.' ... 0 1 1 1 1 1 1 1
                cycle(0, 1);
                cycle(1, 7);
                break;
        }
}

void clearArray() {
        for (int i = 0; i < 4; i++) {
                tempArray[i] = '0';
				sendChar('0');
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

// sensor address = 1001 000; 0x90
int main() {
        // call clear() function
        clear();

        // ensure clock begins at 0
        myCLK = 0;

        while (true) {
                // p9 - sda, p10 - scl
                I2C i2c(p9, p10);
                i2c.start();
                i2c.write(0xac);
                i2c.write(2);
                i2c.stop();

                // start collecting data
                i2c.start();
                i2c.write(0x90); // 0x90 -> write, 0x91 -> read
                i2c.write(0x51);
                i2c.stop();

                // read temp data
                i2c.start();
                i2c.write(0x90);
                i2c.write(0xaa);

                i2c.start();
                i2c.write(0x91);
                // reads temp from DS1631 as 16 bit int?
                tempBinVal = i2c.read(0);
                i2c.stop();

                // need to cut last 4 bits off of tempBinVal
                //sprintf(tempForCut, "%u", (unsigned int) celsius);
                //for (int i = 0; i < 13; i++) {
                //        trimmedTemp[i] = tempForCut[i];
                //}
                //sscanf(trimmedTemp, "%u", (unsigned int) tempBinVal);

                // get temperature in celsius
                celsius = tempBinVal / 16;
                clearArray();
                // send celsius temp to char array
                sprintf(tempArray, "%2.0d", celsius);
                sendChar('C'); // send C for celsius
                //tempArray is size 4 {0, 1, 2, 3}
                // send first two chars of tempArray to 7Seg
                for (int i = sizeof(tempArray) - 3; i >= 0; i--) {
                        sendChar(tempArray[i]);
                } // 145
				// if temp is single digit fill left led w/ 0
				if (tempArray[2] == 0) {
					sendChar('0');
				}
                wait(1);

                // convert temp to fahrenheit
                fahrenheit = (celsius * (9 / 5)) + 32;
                clearArray();
                sprintf(tempArray, "%d", fahrenheit);
                sendChar('F'); // send F for fahrenheit
                // send first two chars of tempArray to 7Seg
                for (int i = sizeof(tempArray) - 3; i >= 0; i--) {
                        sendChar(tempArray[i]);
                }
                wait(1);
        }
}
