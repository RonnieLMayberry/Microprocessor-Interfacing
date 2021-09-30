#include "mbed.h"
DigitalOut myCLR(p28);
//!CLR is pin 28 LPC

DigitalOut myCLK(p27);
//CLK is pin 27 LPC

DigitalOut myData(p30);
//DATA(A/B) is pin 30 LPC

// 7-segment order: --> 0, 1, 2, 3, 4, 5, 6, 7

// 0 - 0, 1, 2, 3, 4, 5
// 1 - 1, 2
// 2 - 0, 1, 3, 4, 6
// 3 - 0, 1, 2, 3, 6
// 4 - 1, 2, 5, 6
// 5 - 0, 2, 3, 5, 6
// 6 - 0, 2, 3, 4, 5, 6
// 7 - 0, 1, 2, 3
// 8 - 0, 1, 2, 3, 4, 5, 6
// 9 - 0, 1, 2, 5, 6

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

int main() {
	// reset !CLR by setting to 1, 0, then 1
	myCLR = 1;
	wait(.1);
	myCLR = 0;
	wait(.1);
	myCLR = 1;
		
	// ensure clock begins at 0
	myCLK = 0;
	
	// cycle( data (0 / 1), # of cycles )
	// data = 0 ... ON
	// data = 1 ... OFF
	while(true) {
		// '0' ... 1 1 0 0 0 0 0 0
		cycle(1, 2);
		cycle(0, 6);
		
		wait(.5);
		
		// '1' ... 1 1 1 1 1 0 0 1
		cycle(1, 5);
		cycle(0, 2);
		cycle(1, 1);
		
		wait(.5);
		
		// '2' ... 1 0 1 0 0 1 0 0
		cycle(1, 1);
		cycle(0, 1);
		cycle(1, 1);
		cycle(0, 2);
		cycle(1, 1);
		cycle(0, 2);

		wait(.5);
		
		// '3' ... 1 0 1 1 0 0 0 0
		cycle(1, 1);
		cycle(0, 1);
		cycle(1, 2);
		cycle(0, 4);

		wait(.5);
		
		// '4' ... 1 0 0 1 1 0 0 1
		cycle(1, 1);
		cycle(0, 2);
		cycle(1, 2);
		cycle(0, 2);
		cycle(1, 1);

		wait(.5);
		
		// '5' ... 1 0 0 1 0 0 1 0
		cycle(1, 1);
		cycle(0, 2);
		cycle(1, 1);
		cycle(0, 2);
		cycle(1, 1);
		cycle(0, 1);

		wait(.5);
		
		// '6' ... 1 0 0 0 0 0 1 0
		cycle(1, 1);
		cycle(0, 5);
		cycle(1, 1);
		cycle(0, 1);

		wait(.5);
		
		// '7' ... 1 1 1 1 1 0 0 0
		cycle(1, 5);
		cycle(0, 3);

		wait(.5);
		
		// '8' ... 1 0 0 0 0 0 0 0
		cycle(1, 1);
		cycle(0, 7);

		wait(.5);
		
		// '9' ... 1 0 0 1 1 0 0 0
		cycle(1, 1);
		cycle(0, 2);
		cycle(1, 2);
		cycle(0, 3);

		wait(.5);
		
		// 'A' ... 1 0 0 0 1 0 0 0
		cycle(1, 1);
		cycle(0, 3);
		cycle(1, 1);
		cycle(0, 3);

		wait(.5);
		
		// 'b' ... 1 0 0 0 0 0 1 1
		cycle(1, 1);
		cycle(0, 5);
		cycle(1, 2);

		wait(.5);
		
		// 'C' ... 1 1 0 0 0 1 1 0
		cycle(1, 2);
		cycle(0, 3);
		cycle(1, 2);
		cycle(0, 1);

		wait(.5);
		
		// 'd' ... 1 0 1 0 0 0 0 1
		cycle(1, 1);
		cycle(0, 1);
		cycle(1, 1);
		cycle(0, 4);
		cycle(1, 1);

		wait(.5);
		
		// 'E' ... 1 0 0 0 0 1 1 0
		cycle(1, 1);
		cycle(0, 4);
		cycle(1, 2);
		cycle(0, 1);

		wait(.5);
		
		// 'F' ... 1 0 0 0 1 1 1 0
		cycle(1, 1);
		cycle(0, 3);
		cycle(1, 3);
		cycle(0, 1);

		wait(.5);
		
		// '.' ... 0 1 1 1 1 1 1 1
		cycle(0, 1);
		cycle(1, 7);

		wait(.5);
		
		// iterate through each LED
		for (int i = 0; i < 8; i++) {
			cycle(i, 1);
			wait(.5);
		}
	}
}
