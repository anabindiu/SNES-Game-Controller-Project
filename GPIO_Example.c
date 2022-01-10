//Name: Ana Bindiu
//UCID: 30062098

//referenced/used code from lecture slides 5a - SNES
#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <bcm_host.h>

#define CLK 11
#define LAT 9
#define DAT 10
#define GPSET0 7
#define GPCLR0 10
#define GPLEV0 13
#define HCYC 6
#define FCYC 12

#define BLOCK_SIZE 4096

//variables defined here
volatile unsigned int *gpio; // GPIO base
static unsigned int *sysclock; // clock base
static  long *clock_1; // CLO/CHI
static int fd;
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |= (1<<(((g)%10)*3))

int initSNES();
void timedWait( int delay );
int readSnesController();

//create a main that loops through the readSNESController infinite times
int main(){
	//call and initialize the pins
	initSNES();
	
	int loopEnd = 99999;
	for(int i = 0; i < loopEnd; i++) {
		readSnesController();
		sleep(1);
	}
	return 0;
}

//this function is taken from the lecture slides: 5a-SNES on page 23
int initSNES() {
	 //initialize GPIO_BASE and CLOCK_BASE here 
	 unsigned int GPIO_BASE;
	 unsigned int CLOCK_BASE;
	 void* GPIO_M; 
	 //get the peripheral address of the bcm host
	 unsigned peripheralBase = bcm_host_get_peripheral_address();
	 //set the GPIO_BASE to be the addition of the peripheral address and 0x2000000
	 GPIO_BASE = peripheralBase + 0x200000;
	 //set the CLOCK_BASE to be the addition of the peripheral address and 0x3000
	 CLOCK_BASE = peripheralBase + 0x3000;  // took this formula from slides: 2-timer-memory-mapped-io
	 
	 if ((fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) return -1;
	 GPIO_M = (unsigned int *)mmap( NULL, BLOCK_SIZE,
		PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_BASE );
	 if (GPIO_M == MAP_FAILED) return -1;
	 
	 sysclock = (unsigned int *)mmap( NULL, BLOCK_SIZE,
		PROT_READ|PROT_WRITE, MAP_SHARED, fd, CLOCK_BASE );
	 if (sysclock == MAP_FAILED) return -1;
	 
	 gpio = (volatile unsigned int*) GPIO_M;
	 clock_1 = (long *)(&sysclock[1]);
	 
	 INP_GPIO( CLK ); // CLK
	 OUT_GPIO( CLK );
	 INP_GPIO( LAT ); // LATCH
	 OUT_GPIO( LAT );
	 INP_GPIO( DAT ); // DATA
	 
	 return 0;
}

//this function is taken from the lectures slides: 5a-SNES on page: 25
void timedWait( int delay ) {
	 long endTime;

	 endTime = *clock_1 + delay;
	 while ( endTime > *clock_1 );
}

//this function is taken from the lecture slides: 5a-SNES on page: 26
int readSnesController() {
	 int conWord = 0;
	 int bitMask;
	 int bit;

	 gpio[GPSET0] = 1 << CLK; // CLK high
	 gpio[GPSET0] = 1 << LAT; // Latch high
	 timedWait( FCYC ); // wait for 12usec
	 gpio[GPCLR0] = 1 << LAT; // Latch low
	 
	 bitMask = 1;
	 while ( bitMask < 0x10000 ) {
		 timedWait( HCYC ); // wait for 6usec
		 gpio[GPCLR0] = 1 << CLK; // CLK low
		 timedWait( HCYC ); // wait for 6usec
		 bit = (gpio[GPLEV0] >> DAT) & 1; // read DAT
		 if ( bit ) conWord |= bitMask; // set bit in conWord
		 gpio[GPSET0] = 1 << CLK; // CLK high
		 bitMask <<= 1;
	 }
	 return conWord;
}

//create a peek function that takes the address of a register that is 4-byte aligned and return the 32-bit value in the register at that address
int peek(int peek_element){
	return gpio[peek_element];
}

//create a poke function that writes a 32-bit value to the register at the given address that is 4-byte aligned
void poke(int poke_element, int poke_result){
	gpio[poke_element] = poke_result;
}

