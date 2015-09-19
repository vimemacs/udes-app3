#include <time.h>

#define CTL1 0x01980000
#define PRD1 0x01980004
#define CNT1 0x01980008
#define USER_REG 0x90080000
#define GPIO_EN 0x01B00000
#define GPIO_DIR 0x01B00004

extern void enableInterrupts();
void interrupt c_int4();

void interrupt c_int4() {
	*(unsigned int*) USER_REG ^= 0x01;
}

void configAndStartTimer1() {
    *(unsigned int*) CTL1 &= 0xFFFFFF7F;
    *(unsigned int*) PRD1 = 0x2AEA54;
    *(unsigned int*) CTL1 |= 0x00008385;
    *(unsigned int*) CTL1 |= 0xC0;
}

void main() {
	*(unsigned int*) GPIO_EN |= 0x10;
	*(unsigned int*) GPIO_DIR &= 0xFFEF;
	*(unsigned int*) USER_REG &= 0x0;
	configAndStartTimer1();
	enableInterrupts();

	while (1);
}
