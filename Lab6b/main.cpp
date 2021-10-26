#include "mbed.h"

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

bool recentlyPressed;
int tempNum[3];
int number, tempVal, op1, result, index;
char operation;

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

void pressTimer() {
        recentlyPressed = true;
        wait(1);
        recentlyPressed = false;
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

void clearNum() {
        for (int i = 0; i <= 2; i++) {
                tempNum[i] = 0;
                sendChar(tempNum[i] + '0');
        }

        number = 0;
        index = 2;
}

void storeNumber(int num) {
        if (index == 2) {
                tempNum[index] = num;
                tempVal = num;
        } else if (index == 1) {
                tempNum[2] = num;
                tempNum[1] = tempVal;
        }

        for (int i = 2; i >= 0; i--) {
                sendChar(tempNum[i] + '0');
        }

        // convert int array to int
        for (int i = 1; i <= 2; i++) {
                number = 10 * number + tempNum[i];
        }

        if (index > 1) {
                index--;
        } else {
                index = 2;
        }
}

void performOp() {
        char resultArray[3];
        switch (operation) {
        case 'A':
                result = op1 + number;
                sprintf(resultArray, "%.3x", result);
                break;
        case 'b':
                result = op1 - number;
                sprintf(resultArray, "%.3x", result);
                break;
        case 'C':
                result = op1 & number;
                sprintf(resultArray, "%.3x", result);
                break;
        case 'd':
                result = op1 | number;
                sprintf(resultArray, "%.3x", result);
                break;
        case 'E':
                for (int i = 2; i > 0; i--) {
                        sendChar(resultArray[i]);
                        printf("%d", resultArray[i]);
                }
                break;
        case 'F':
                clearNum();
                break;
        }
}

void handleOp(char op) {
        op1 = number;
        operation = op;
        clearNum();
}

void keyPress() {
        while (!recentlyPressed) {
                for (int c = 1; c <= 4; c++) {
                        if (c == 1) {
                                c1 = 0;
                                if (r1 == 0) {
                                        storeNumber(1);
                                } else if (r2 == 0) {
                                        storeNumber(4);
                                } else if (r3 == 0) {
                                        storeNumber(7);
                                } else if (r4 == 0) {
                                        storeNumber(0);
                                }
                                c1 = 1;
                                wait(.025);
                        } else if (c == 2) {
                                c2 = 0;
                                if (r1 == 0) {
                                        storeNumber(2);
                                } else if (r2 == 0) {
                                        storeNumber(5);
                                } else if (r3 == 0) {
                                        storeNumber(8);
                                } else if (r4 == 0) {
                                        handleOp('F');
                                }
                                c2 = 1;
                                wait(.025);
                        } else if (c == 3) {
                                c3 = 0;
                                if (r1 == 0) {
                                        storeNumber(3);
                                } else if (r2 == 0) {
                                        storeNumber(6);
                                } else if (r3 == 0) {
                                        storeNumber(9);
                                } else if (r4 == 0) {
                                        handleOp('E');
                                }
                                c3 = 1;
                                wait(.025);
                        } else if (c == 4) {
                                c4 = 0;
                                if (r1 == 0) {
                                        handleOp('A');
                                } else if (r2 == 0) {
                                        handleOp('b');
                                } else if (r3 == 0) {
                                        handleOp('C');
                                } else if (r4 == 0) {
                                        handleOp('d');
                                }
                                c4 = 1;
                                wait(.025);
                        }
                        wait(.01);
                }
        }
        if (operation != NULL) {
                performOp();
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
                        sendChar(hex[j]);
                }
                // wait 1/8th second between each value displayed
                wait(1);
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
        recentlyPressed = false;
        number = 0;

        // call clear() function
        clear();
        clearNum();

        // ensure clock begins at 0
        myCLK = 0;

        // cycle( data (0 / 1), # of cycles )
        // data = 0 ... ON
        // data = 1 ... OFF
        while (true) {
                keyPress();
        }
}
