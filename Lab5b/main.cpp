#include "mbed.h"
DigitalOut myCLR(p28);
//!CLR is pin 28 LPC

DigitalOut myCLK(p27);
//CLK is pin 27 LPC

DigitalOut myData(p30);
//DATA(A/B) is pin 30 LPC

// 7-segment order: --> 0, 1, 2, 3, 4, 5, 6, 7

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
	switch(val) {
		case '0':
			// '0' ... 1 1 0 0 0 0 0 0
			cycle(1, 2);
			cycle(0, 6);
		case '1':
			// '1' ... 1 1 1 1 1 0 0 1
			cycle(1, 5);
			cycle(0, 2);
			cycle(1, 1);
		case '2':
			// '2' ... 1 0 1 0 0 1 0 0
			cycle(1, 1);
			cycle(0, 1);
			cycle(1, 1);
			cycle(0, 2);
			cycle(1, 1);
			cycle(0, 2);
		case '3':
			// '3' ... 1 0 1 1 0 0 0 0
			cycle(1, 1);
			cycle(0, 1);
			cycle(1, 2);
			cycle(0, 4);
		case '4':
			// '4' ... 1 0 0 1 1 0 0 1
			cycle(1, 1);
			cycle(0, 2);
			cycle(1, 2);
			cycle(0, 2);
			cycle(1, 1);
		case '5':
			// '5' ... 1 0 0 1 0 0 1 0
			cycle(1, 1);
			cycle(0, 2);
			cycle(1, 1);
			cycle(0, 2);
			cycle(1, 1);
			cycle(0, 1);
		case '6':
			// '6' ... 1 0 0 0 0 0 1 0
			cycle(1, 1);
			cycle(0, 5);
			cycle(1, 1);
			cycle(0, 1);
		case '7':
			// '7' ... 1 1 1 1 1 0 0 0
			cycle(1, 5);
			cycle(0, 3);
		case '8':
			// '8' ... 1 0 0 0 0 0 0 0
			cycle(1, 1);
			cycle(0, 7);
		case '9':
			// '9' ... 1 0 0 1 1 0 0 0
			cycle(1, 1);
			cycle(0, 2);
			cycle(1, 2);
			cycle(0, 3);
		case 'A':
			// 'A' ... 1 0 0 0 1 0 0 0
			cycle(1, 1);
			cycle(0, 3);
			cycle(1, 1);
			cycle(0, 3);
		case 'b':
			// 'b' ... 1 0 0 0 0 0 1 1
			cycle(1, 1);
			cycle(0, 5);
			cycle(1, 2);
		case 'C':
			// 'C' ... 1 1 0 0 0 1 1 0
			cycle(1, 2);
			cycle(0, 3);
			cycle(1, 2);
			cycle(0, 1);
		case 'd':
			// 'd' ... 1 0 1 0 0 0 0 1
			cycle(1, 1);
			cycle(0, 1);
			cycle(1, 1);
			cycle(0, 4);
			cycle(1, 1);
		case 'E':
			// 'E' ... 1 0 0 0 0 1 1 0
			cycle(1, 1);
			cycle(0, 4);
			cycle(1, 2);
			cycle(0, 1);
		case 'F':
			// 'F' ... 1 0 0 0 1 1 1 0
			cycle(1, 1);
			cycle(0, 3);
			cycle(1, 3);
			cycle(0, 1);
		case '.':
			// '.' ... 0 1 1 1 1 1 1 1
			cycle(0, 1);
			cycle(1, 7);
	}
}

// showing values from 000 to FFF on LED
void count() {
	char hex[3];
	// counting from 0x000 to 0xFFF
	for (int i = 0x000; i <= 0xFFF; i++) {
		// store 3 byte hex value in char array hex[]
		sprintf(hex, "%.3x", i);
		// iterate through each byte of hex value
		for (int j = 0; j < 3; j++) {
			// send each byte to LED
			sendChar( hex[j] );
		}
		// wait 1/8th second between each value displayed
		wait(.125);
	}
}

void blinkDots() {
	for (int i = 0; i <= 2; i++) {
		sendChar('.');
	}
	wait(.125);
	clear();
}

int main() {
  // call clear() function
	clear();
		
	// ensure clock begins at 0
	myCLK = 0;
	
	// cycle( data (0 / 1), # of cycles )
	// data = 0 ... ON
	// data = 1 ... OFF
	while(true) {
		//call count() function
		count();
		blinkDots();
	}
}
