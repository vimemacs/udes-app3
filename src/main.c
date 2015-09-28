#include <time.h>
#include <C6713Helper_UdeS.h>

Uint32 fs = DSK6713_AIC23_FREQ_8KHZ;
#define DSK6713_AIC23_INPUT_MIC 0x0015
#define DSK6713_AIC23_INPUT_LINE 0x0011
Uint16 inputsource=DSK6713_AIC23_INPUT_MIC;

#define LOOPLENGTH 8
#define BUFFERLENGTH 256

#define CTL1 0x01980000					// Adresse du registre de contrôle du timer1
#define PRD1 0x01980004					// Adresse du registre de configuration du temps du timer1
#define CNT1 0x01980008					// Valeur de comptage du timer 1
#define USER_REG 0x90080000				// Registre de contrôle des DIPs switch et des LEDs
#define GPIO_EN 0x01B00000				// Registre de contrôle des GPIO (activation)
#define GPIO_DIR 0x01B00004				// Registre de contrôle des GPIO (direction)
#define ADDR_ADC 0xB03C0000				// Adresse de base de l ADC
#define CECTL3 0x01800014				// EMIF CE3 space control
#define GPPOL 0x01B00024				// Registre de configuration pour les interruptions
#define MASK_GET_DIP_BIT 0x000000F0;	// Masque pour contrôler les DIPS sitch

//
// Configuration de la memoire
//
#define BUFFER_SIZE 80000
unsigned short Data[BUFFER_SIZE];
#pragma DATA_SECTION(Data,".EXT_RAM")

extern void enableInterrupts();
void interrupt c_int4();
void interrupt c_int11();
void interrupt c_int15();

unsigned short status = 0;				// variable de verification des etats des DIPS
unsigned int counter = 0;				// Compteur de temps de 0,1s pour LED0 (enregistrement)
unsigned int maxtime = 0;				// Compteur de temps de 10s pour LED0 (enregistrement)
unsigned int maxtime2 = 0;				// Compteur de temps de 10s pour LED1 (jouer un enregistrement)
unsigned short flash_led0 = 0;			// flag d'interruption pour DIP0
unsigned short flash_led1 = 0;			// flag d'interruption pour DIP1
unsigned int i = 0;						// Compteur de tableau

short gain = 10;

//
// Routine d'interruption 4 (EXT_INT4)
//
void interrupt c_int4() {
	if (i < 80000) {
		Data[i] = *(unsigned int*) ADDR_ADC;
		i++;
	} else Data[0] = *(unsigned int*) ADDR_ADC;
}

interrupt void c_int11() {
	int out_sample;

	if (maxtime2 == 80000) {					//Si condition vrai alors fin de l ecoute
			flash_led1 = 0;						//remise a 0 du flag d interruption
			maxtime2 = 0;						//Reinitialisation du compteur temps
			*(unsigned int*) USER_REG = 0x00; 	//On eteint la LED
	}

	if (flash_led1 == 1) {						//ON est en train d ecouter l enregistrement
		*(unsigned int*) USER_REG = 0x02;

		out_sample = Data[i++]*gain;
		output_left_sample(out_sample);
		maxtime2++;								//Incrementation du compteur temps
	} else output_sample(0);

	return;
}

void interrupt c_int15() {		                //Routine d'interruption 15 (Timer 1)
	if (maxtime == 80000) {		                //Fin d enregistrement si condition vrai au bout de 10s
		flash_led0 = 0;			                //Flag a 0
		maxtime = 0;			                //Reinitialisation du compteur temps
	}

	if (flash_led0 == 1) {				        //Enregistrement sonore actif
		*(unsigned int*) ADDR_ADC = 1;          //Lancement d une conversion
		counter++;						        //Incrementation des compteurs temps
		maxtime++;

		if (counter == 800) {	                //Si condition vrai alors on change l etat de la LED0 au bout de 0,1s
			*(unsigned int*) USER_REG ^= 0x01;
			counter = 0;
		}
	}
}

void configAndStartTimer1() {
	*(unsigned int*) CTL1 &= 0xFFFFFF7F; 	    //HLD a 0 (step1)
	*(unsigned int*) PRD1 = 3515;			    //Reglage de la frequence du timer a 8 kHz (step2)
	*(unsigned int*) CTL1 |= 0x00000315;	    //Configuration du timer (step3)
	*(unsigned int*) CTL1 |= 0xC0;			    //HLD et GO a 1 (step4)
}

unsigned short testDIP0() {
	status = *(unsigned int*) USER_REG;	        //On regarde l etat des DIPs switch
	status = status & MASK_GET_DIP_BIT;
	if (status == 0xE0)					        //DIP0 actif
		return 1;
	else
		return 0;						        //DIP0 inactif
}

unsigned short testDIP1() {
	status = *(unsigned int*)USER_REG;	        //On regarde l etat des DIPs switch
	status = status & MASK_GET_DIP_BIT;
	if (status == 0xD0)			    	        //DIP1 actif
		return 1;
	else
		return 0;					            //DIP1 inactif
}

void main() {
	unsigned short sw0_old = 0;				    // variables utilisees pour detecter les fronts montants
	unsigned short sw0_new = 0;
	unsigned short sw1_old = 0;
	unsigned short sw1_new = 0;

	*(unsigned int*) CECTL3 = 0x01400513;	    // Configuration EMIF3
	*(unsigned int*) GPIO_EN |= 0x10;		    // Configuration de l'entree EXT_INT4
	*(unsigned int*) GPIO_DIR &= 0xFFEF;
	*(unsigned int*) GPPOL |= 0x10;
	*(unsigned int*) USER_REG = 0x00;		    //Initialement les LED sont eteintes

	configAndStartTimer1();
	enableInterrupts();
	comm_intr();

	while (1) {
		sw0_new = testDIP0();				    //Nouvel etat pour DIP0

		if (sw0_old == 0 && sw0_new == 1) {	    //Enregistrement sonore
			flash_led0 = 1;
			i = 0;
		}

		sw0_old = testDIP0();				    //Ancien etat pour DIP0
		sw1_new = testDIP1();				    //Nouvel etat pour DIP1

		if (sw1_old == 0 && sw1_new == 1) {	    //Emission sonore
			flash_led1 = 1;
			i = 0;
		}

		sw1_old = testDIP1();				    //Ancien etat pour DIP1
	}
}
