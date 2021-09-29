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

void cycle(int num) {
	for (int i = 1; i <= num; i++) {
		myCLK = 1;
		myCLK = 0;
	}
}

int main() {
	myCLR = 1;
	wait(.1);
	myCLR = 0;
	wait(.1);
	myCLR = 1;
		
	myCLK = 0;
	
	while(true) {
		// 0
		myData = 1;
		cycle(2);
		
		myData = 0;
		cycle(6);
		
		wait(.5);
		
		// 1
		myData = 1;
		cycle(5);
		
		myData = 0;
		cycle(2);
		
		myData = 1;
		cycle(1);
		
		wait(.5);
		
		// 2
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(1);
		
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(2);
		
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(2);

		wait(.5);
		
		// 3
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(1);
		
		myData = 1;
		cycle(2);
		
		myData = 0;
		cycle(4);

		wait(.5);
		
		// 4
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(2);
		
		myData = 1;
		cycle(2);
		
		myData = 0;
		cycle(2);
		
		myData = 1;
		cycle(1);

		wait(.5);
		
		// 5
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(2);
		
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(2);
		
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(1);

		wait(.5);
		
		// 6
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(5);
		
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(1);

		wait(.5);
		
		// 7
		myData = 1;
		cycle(1);
		
		myData = 1;
		cycle(4);
		
		myData = 0;
		cycle(3);

		wait(.5);
		
		// 8
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(7);

		wait(.5);
		
		// 9
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(2);
		
		myData = 1;
		cycle(2);
		
		myData = 0;
		cycle(3);

		wait(.5);
		
		
		
		
		// A
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(3);
		
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(3);

		wait(.5);
		
		// b
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(5);
		
		myData = 1;
		cycle(2);

		wait(.5);
		
		// C
		myData = 1;
		cycle(1);
		
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(3);
		
		myData = 1;
		cycle(2);
		
		myData = 0;
		cycle(1);

		wait(.5);
		
		// d
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(1);
		
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(4);
		
		myData = 1;
		cycle(1);

		wait(.5);
		
		// E
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(4);
		
		myData = 1;
		cycle(2);
		
		myData = 0;
		cycle(1);

		wait(.5);
		
		// F
		myData = 1;
		cycle(1);
		
		myData = 0;
		cycle(3);
		
		myData = 1;
		cycle(3);
		
		myData = 0;
		cycle(1);

		wait(.5);
		
		// .
		myData = 0;
		cycle(1);
		
		myData = 1;
		cycle(7);

		wait(.5);
		
		for (int i = 0; i < 8; i++) {
			myData = i;
			cycle(1);
			wait(.5);
		}
	}
}
