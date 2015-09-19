#define CTL1 0x01980000
#define PRD1 0x01980004
#define CNT1 0x01980008
#define TIMER_PRD 2813

void configAndStartTimer1() {

    *(unsigned int*) CTL1 &= 0xFFFFFF7F;	// step 1
    *(unsigned int*) PRD1 = TIMER_PRD;		// step 2
    *(unsigned int*) CTL1 |= 0x00008385;	// step 3
    *(unsigned int*) CTL1 |= 0x000000C0;	// step 4
}

void main() {
	configAndStartTimer1();

	while (1);
}
