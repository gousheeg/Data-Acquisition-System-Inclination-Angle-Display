//*********************************************************************                  
//*                          Final Project                             
//*                       McMaster University                         
//*                      2DP4 Microcontrollers                        
//*                          Lab Section L08                           
//*            Gousheeg Kasivenkatasubbu, kasiveng, 400061872     
//*********************************************************************
//*********************************************************************
//* As per project specifications:
//* The onboard ADC is set to 12-bit operation mode and is used to                           
//* sample values from analog port 6.                                                                                                                                                                           
//* Clock Speed is 8MHz.  
//***********************************************************************
// Simple ADC example for the Technological Arts EsduinoXtreme board
// by Carl Barnes, 12/03/2014
//***********************************************************************

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"  /* derivative information */
#include "SCI.h"

/////////// GLOBAL VARIABLES ////////////
char string[20];
unsigned short val; //To store values from analog channel 6
unsigned short center;
int mode; //changes mode
int startstop; //starts and stops the code
double test;
int theta;
  
///////// FUNCTION PROTOTYPES ///////////  
void setClk(void);
void Delay1ms(unsigned int time);
void delay1ms(unsigned int multiple);
//math functions
double factorial(int num);
double exponent(double base, int power);
int arcsin(double x);

//---------------------OutCRLF---------------------
// Output a CR,LF to SCI to move cursor to a new line
// Input: none
// Output: none
// Toggle LED each time through the loop

void OutCRLF(void){
  SCI_OutChar(CR);
  SCI_OutChar(LF);
  PTJ ^= 0x20;          // toggle LED D2
}

void main(void) {

setClk();  //setting bus clock to 8MHz

//////// INITIALIZING GLOBAL VARIABLES ///////////////
startstop=0;
mode=0;
test=0;
theta=0;
val=0;
center=2055;

////////// CONFIGURING ADC /////////////////
// Setup and enable ADC channel 6 and resolution to 12 bits      
    ATDCTL1 = 0x4F;     // set for 12-bit resolution
    ATDCTL3 = 0x88;     // right justified, one sample per sequence
    ATDCTL4 = 0x03;     // prescaler = 3; ATD clock = 8MHz / (2 * (2 + 1)) == 1MHz
    ATDCTL5 = 0x26;     // continuous conversion on channel 6
   
//////// CONFIGURING I/O ///////////
  DDRJ = 0xFF; 
  DDR0AD=0x0F;      
  DDR1AD=0xBF;      
  DDRP=0x02;
  PER1AD=0x00;
//turning on LEDS connected to see if program started
  PT1AD=0x0F;
  PT0AD=0x0F;
  PTP=0x02; 
  
////////////////// CONFIGURING TIME INPUT CAPTURE ////////////////////////////////                                                   
            
  TSCR1 = 0x90;    //Timer System Control Register 1
                    // TSCR1[7] = TEN:  Timer Enable (0-disable, 1-enable)
                    // TSCR1[6] = TSWAI:  Timer runs during WAI (0-enable, 1-disable)
                    // TSCR1[5] = TSFRZ:  Timer runs during WAI (0-enable, 1-disable)
                    // TSCR1[4] = TFFCA:  Timer Fast Flag Clear All (0-normal 1-read/write clears interrupt flags)
                    // TSCR1[3] = PRT:  Precision Timer (0-legacy, 1-precision)
                    // TSCR1[2:0] not used

  TSCR2 = 0x04;    //Timer System Control Register 2
                    // TSCR2[7] = TOI: Timer Overflow Interrupt Enable (0-inhibited, 1-hardware irq when TOF=1)
                    // TSCR2[6:3] not used
                    // TSCR2[2:0] = Timer Prescaler Select: See Table22-12 of MC9S12G Family Reference Manual r1.25 (set for bus/1)
  
                    
  TIOS = 0xFC;     //Timer Input Capture or Output capture
                    //set TIC[0] and input (similar to DDR)
  PERT = 0x03;     //Enable Pull-Up resistor on TIC[0] and TIC[1]

  TCTL3 = 0x00;    //TCTL3 & TCTL4 configure which edge(s) to capture
  TCTL4 = 0x0A;    //Configured for falling edge on TIC[0] (channel 0) and TIC[1] (channel 1)       
  TIE = 0x03;      //Timer Interrupt Enable
 
  SCI_Init(38400);  //Setting highest possible baud rate for selected bus clock

  EnableInterrupts;

  
for(;;) {
	while(startstop==1){
		val=ATDDR0; //assigning values from analog channel 6
		
		
		//Scaling ADC reading between -1 to 1
		if (val>center)
		{
			test=(val-center)/420.0;  //2475 was ADC output at +90 degrees, 420 is the distance from 2055 to 2475
			SCI_OutString("p"); //For serial communication to interpret as positive angle
		} 
		else
		{
			test=(center-val)/410.0; //1645 was ADC output at -90 degrees, 410 is the distance from 2055 to 1645
			SCI_OutString("n"); //For serial communication to interpret as negative angle
		}
		
		theta=arcsin(test); //arcsin function returns degree value of the angle
			
////////////////// MODE 0 ///////////////////
		if(mode==0){   
		PT1AD=0x00;  // for lower 4 bits
		PT0AD=0x00;  // for upper 4 bits
		PTP=0x00;
		   
		PT1AD=theta%10;  //lower 4 bits (PAD0-3)
		PT0AD=theta/10;	 //higher 4 bits (PAD8-11)	
		}
		
/////////////////  MODE 1 //////////////////	  
	 else{    
		PT1AD=0x00; 
		PT0AD=0x00;
		PTP=0x00;
	  
	    if(theta==0){
	        PTP=0x00;
		  	PT1AD=0x00;
		    PT0AD=0x00;
	    } 
		else if(theta>0 && theta<=10){
			PTP=0x02;
			PT0AD=0x00;
			PT1AD=0x00;
		}
		else if (theta<=20){
			PTP=0x02;
			PT0AD=0x08;
			PT1AD=0x00;

		}
		else if(theta<=30){
	   	    PTP=0x02;
			PT0AD=0x0C;
			PT1AD=0x00;
		}
		else if(theta<=40){
			PTP=0x02;
			PT0AD=0x0E;
			PT1AD=0x00;
		}
		else if(theta<=50){
			PTP=0x02;
			PT0AD=0x0F;
			PT1AD=0x00;
		}
		else if(theta<=60){
	     PTP=0x02;
			PT0AD=0x0F;
			PT1AD=0x08;
		}
		else if(theta<=70){
			 PTP=0x02;
			PT0AD=0x0F;
			PT1AD=0x0C;
		}
		else if(theta<=80){
		   PTP=0x02;
			PT0AD=0x0F;
			PT1AD=0x0E;
			 
		}
	    else{
			PTP=0x02;
			PT0AD=0x0F;
			PT1AD=0x0F;
		  }
	  } 
////////////// SERIAL OUTPUT ///////////////////////
    //Uncomment line below to view ADC output
		//SCI_OutUDec(val); //to view ADC ouput
		
    SCI_OutUDec(theta);
    OutCRLF();
    PTJ^=0x01;
    Delay1ms(100);    // 100 msec delay
    
	}//end of while loop 
 }//end of for loop
  
/*  
///////////// TO VERIFY BUS CLOCK //////////////////  
//to verify bus clock  
  for(;;){
    
    //Delay1ms(1000);
    PTJ^=0x01;
    delay1ms(250);    //using timing count register
    Delay1ms(250);    
	}
*/  
  

}//end of main


//////////////////// INTERRUPTS //////////////////////
interrupt  VectorNumber_Vtimch0 void ISR_Vtimch0(void)
{
  unsigned int temp;
  startstop^=1;     //pressing button 1 increments this variable

  temp = TC0;       //Refer back to TFFCA, we enabled FastFlagClear, thus by reading the Timer Capture input we automatically clear the flag, allowing another TIC interrupt
 }
 
        
interrupt  VectorNumber_Vtimch1 void ISR_Vtimch1(void)
{
  unsigned int temp;
  mode=mode^=1;     //pressing button 2 increments this variable
 
  temp = TC1;       //Refer back to TFFCA, we enabled FastFlagClear, thus by reading the Timer Capture input we automatically clear the flag, allowing another TIC interrupt
}
//////////////////////////////////////////////////////


//////////////////// MATH FUNCTIONS //////////////////
double exponent(double base, int power){
	double exp=1.0;
	int i;
	for(i=0;i<power;i++){
		exp=exp*base;
	}
	return exp;
}


int arcsin(double x){ 
  double sum=0;
  //first 8 terms of the taylor series expansion
  sum=sum+ (x+ (exponent(x,3)/6.0));       //n=0 and n=1
  sum=sum+(exponent(x,5)*(3.0/40.0));      //n=2
  sum=sum+(exponent(x,7)*(5.0/112.0));     //n=3
  sum=sum+(exponent(x,9)*(35.0/1152.0));   //n=4
  sum=sum+(exponent(x,11)*(63.0/2816.0));  //n=5
  sum=sum+(exponent(x,13)*(231.0/13312.0));//n=6
  sum=sum+(exponent(x,15)*(143.0/10240.0));//n=7
  sum=sum*(180/3.14); //converting to degree
  sum=sum*1.12;
  return sum;
}
////////////////////////////////////////////////////


////////// CLOCK and DELAY FUNCTIONS //////////////

//New Delay working with 8MHz Bus Clock
void Delay1ms(unsigned int time)
{
    unsigned int j,k;
    for(j=0;j<time;j++)
        for(k=0;k<1323;k++);
}


/////////////////////////////////////////////////////
// NAME: 1ms Delay using timing count register
// DESCRIPTION: 1ms delay function when bus clock is 8MHz
/////////////////////////////////////////////////////
void delay1ms(unsigned int multiple){ 
  unsigned int i; //loop control variable 
  TSCR1 = 0x90;   //enable timer and fast timer flag clear 
  TSCR2 = 0x00;   //Disable timer interrupt, set prescaler=1
  //TC0 = TCNT + 8000;
   
  for(i=0;i<multiple;i++) {
    TFLG2 = 0x80; //clear the TOF flag
    while (!(TFLG1_C0F));
    TC0 += 8000;
  }
  
  //TIOS &= -0x01; //Disable OC0 (not necessary) 
}

/////////////////////////////////////////////////////
// NAME: Set Clock
// DESCRIPTION: The following code is adapted from the ESDX User Guide and the Serial Monitor code (S12SerMon2r7) to set the clock speed to 8MHz
/////////////////////////////////////////////////////

//Using Oscillator Frequency (8MHz)  
#define VCOFRQ 0x00        //VCOFRQ[1:0]  32MHz <= VCOCLK <= 48MHz
#define SYNDIV 0x03        //SYNDIV[5:0]  Syn divide is 3
#define REFFRQ 0x40        //REFFRQ[1:0]  2MHz < fREF <= 6MHz
#define REFDIV 0x01        //REFDIV[3:0]  Ref Divide is 1


void setClk(void){
  CPMUPROT = 0x26;               //Protection of clock configuration is disabled
  CPMUCLKS = 0x80;               //PLLSEL=1. Select Bus Clock Source:  PLL clock or Oscillator.
  CPMUOSC = 0x80;                //OSCE=1. Select Clock Reference for PLLclk as:fOSC (8 MHz).
  
  CPMUREFDIV = REFFRQ+REFDIV;    //Set fREF divider and selects reference clock frequency Range. fREF= 4 MHz.  
  
  CPMUSYNR=VCOFRQ + SYNDIV;      //Set Syn divide and selects VCO frequency range. fVCO = 32 MHz.
  
  CPMUPOSTDIV=0x01;              //Set Post Divider (0x00= 0000 0000). fPLL= 16 MHz.
                                 //Overall clock speed is fBUS=fPLL/2=8 MHz  
  
  while (CPMUFLG_LOCK == 0) {}  //Wait for PLL to achieve desired tolerance of target frequency. NOTE: For use when the source clock is PLL. comment out when using external oscillator as source clock 
 
 CPMUPROT = 1;                  //Protection for clock configuration is reenabled 
}


// Using Internal Reference Frequency (1MHz)
// #define VCOFRQ 0x00        //VCOFRQ[1:0]  32MHz <= VCOCLK <= 48MHz
// #define SYNDIV 0x0F        //SYNDIV[5:0]  Syn divide is 15
// #define REFFRQ 0x40        //REFFRQ[1:0]  2MHz < fREF <= 6MHz
// #define REFDIV 0x01        //REFDIV[3:0]  Ref Divide is 1
// }

// void setClk8(void){
  // CPMUPROT = 0x26;               //Protection of clock configuration is disabled
  // CPMUCLKS = 0x80;                //PLLSEL=1. Select Bus Clock Source:  PLL clock or Oscillator.
  // CPMUOSC = 0x00;                //OSCE=0. Select Clock Reference for PLLclk as:f1RC1M (1 MHz).
  
  // CPMUREFDIV = REFFRQ+REFDIV;    //Set fREF divider and selects reference clock frequency Range. fREF= 4 MHz.  
  
  // CPMUSYNR=VCOFRQ + SYNDIV;      //Set Syn divide and selects VCO frequency range. fVCO = 32 MHz.
  
  // CPMUPOSTDIV=0x01;              //Set Post Divider (0x00= 0000 0000). fPLL= 16 MHz.
                                 //Overall clock speed is fBUS=fPLL/2=8 MHz  
  
  // while (CPMUFLG_LOCK == 0) {}  //Wait for PLL to achieve desired tolerance of target frequency. NOTE: For use when the source clock is PLL. comment out when using external oscillator as source clock 
 
 // CPMUPROT = 1;                  //Protection for clock configuration is reenabled 
// }
