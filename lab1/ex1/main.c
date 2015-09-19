#include <time.h>

#define USER_REG 0x90080000

int main() {
    unsigned int val1 = 6;						// = 0b110
    *(unsigned int*) USER_REG = val1;		    // turn-on LED2 and LED3

    unsigned int retTime = time(0) + 5;			// wait for 5 seconds
    while (time(0) < retTime);

    unsigned int mask = 3;						// = 0b011
    *(unsigned int*) USER_REG = mask & val1;	// turn-off LED3

    return 0;
}
