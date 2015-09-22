#include <time.h>
//#include <C6713Helper_UdeS.h>

#define CTL1 0x01980000		//Adresse du registre de contrôle du timer1
#define PRD1 0x01980004		//Adresse du registre de configuration du temps du timer1
#define CNT1 0x01980008		//Valeur de comptage du timer 1
#define USER_REG 0x90080000	//
#define GPIO_EN 0x01B00000
#define GPIO_DIR 0x01B00004
#define ADDR_ADC 0xB03C0000
#define CECTL3 0x01800014	// EMIF CE3 space control
#define GPPOL 0x01B00024
#define MASK_GET_DIP_BIT 0x000000F0;

// Configuration de la memoire
#define BUFFER_SIZE 80000
unsigned short Data[BUFFER_SIZE];
#pragma DATA_SECTION(Data,".EXT_RAM")

/*Uint32 fs = DSK6713_AIC23_FREQ_8KHZ;
 #define DSK6713_AIC23_INPUT_MIC 0x0015
 #define DSK6713_AIC23_INPUT_LINE 0x0011
 Uint16 inputsource=DSK6713_AIC23_INPUT_MIC;*/

unsigned short status = 0;
unsigned int counter = 0;
unsigned int maxtime = 0;
unsigned int maxtime2 = 0;
unsigned short flash_led0 = 0;
unsigned short flash_led1 = 0;
unsigned int i = 0;

extern void enableInterrupts();

void interrupt c_int4();
void interrupt c_int0015();

void interrupt c_int4() {

	if (i < 80000) {
		Data[i] = *(unsigned int*) ADDR_ADC;
		i++;
	}
	//*(unsigned int*) USER_REG = 0x00;
	//*(unsigned int*) ADDR_ADC = 1;
}

void interrupt c_int0015() {

	if (maxtime == 80000) {
		flash_led0 = 0;
		maxtime = 0;
	}

	if (flash_led0 == 1) {			//Enregistrement sonore actif
		counter++;
		maxtime++;

		*(unsigned int*) ADDR_ADC = 1; //Lancement d une conversion

		if (counter == 800) {

			*(unsigned int*) USER_REG ^= 0x01;
			counter = 0;

		}

	}

	if (maxtime2 == 80000) {
		flash_led1 = 0;
		maxtime2 = 0;
		*(unsigned int*) USER_REG = 0x00;
	}

	if (flash_led1 == 1) {

		maxtime2++;
		*(unsigned int*) USER_REG = 0x02;

	}

	//*(unsigned int*) USER_REG = 0x00;
	//*(unsigned int*) USER_REG = 0x01;
	//*(unsigned int*) ADDR_ADC = 1;
}

void configAndStartTimer1() {
	*(unsigned int*) CTL1 &= 0xFFFFFF7F; 	//HLD a 0 (step1)
	*(unsigned int*) PRD1 = 3515;	//Reglage de la frequence du timer a 8 kHz
	*(unsigned int*) CTL1 |= 0x00000315;
	*(unsigned int*) CTL1 |= 0xC0;			//HLD et GO a 1 (step4)
}

unsigned short testDIP0() {

	status = *(unsigned int*) USER_REG;
	status = status & MASK_GET_DIP_BIT
	;
	//status = status >> 4;
	if (status == 0xE0)	//actif
		return 1;
	else
		return 0;		//inactif

}

unsigned short testDIP1() {

	status = *(unsigned int*) USER_REG;
	status = status & MASK_GET_DIP_BIT
	;
	//status = status >> 5;
	if (status == 0xD0)
		return 1;
	else
		return 0;
}

void main() {

	unsigned short sw0_old = 0;
	unsigned short sw0_new = 0;
	unsigned short sw1_old = 0;
	unsigned short sw1_new = 0;

	*(unsigned int*) CECTL3 = 0x01400513;	// Configuration EMIF3
	*(unsigned int*) GPIO_EN |= 0x10;	// Configuration de l'entree EXT_INT4
	*(unsigned int*) GPIO_DIR &= 0xFFEF;
	*(unsigned int*) GPPOL |= 0x10;
	*(unsigned int*) USER_REG = 0x00;		//Initialement les LED sont eteintes

	//*(unsigned int*)USER_REG = 0x01;
	//configAndStartTimer1();

	configAndStartTimer1();
	enableInterrupts();
	//comm_intr();

	while (1) {
		Data[0] = *(unsigned int*) ADDR_ADC;
		sw0_new = testDIP0();

		if (sw0_old == 0 && sw0_new == 1) { //Enregistrement sonore
			flash_led0 = 1;
			i = 0;
		}

		sw0_old = testDIP0();

		sw1_new = testDIP1();

		if (sw1_old == 0 && sw1_new == 1) { //Emission sonore
			flash_led1 = 1;
		}

		sw1_old = testDIP1();
	}
}
