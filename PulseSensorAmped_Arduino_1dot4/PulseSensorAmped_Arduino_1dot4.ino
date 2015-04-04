
/*  Pulse Sensor Amped 1.4    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com

----------------------  Notes ----------------------  ---------------------- 
This code:
1) Blinks an LED to User's Live Heartbeat   PIN 13
2) Fades an LED to User's Live HeartBeat
3) Determines BPM
4) Prints All of the Above to Serial

Read Me:
https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino/blob/master/README.md   
 ----------------------       ----------------------  ----------------------
*/

#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

#define CE_PIN 9
#define CS_PIN 10

// for the RF component on pins 9 and 10
RF24 radio( CE_PIN, CS_PIN);;
const uint64_t pipe = 0xE8E8F0F0E1LL;
 
// a message to be sent
int msg;
//  Variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 6;                // pin to blink led at each beat
int fadePin = 7;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = false;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse 


void setup(){
  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
  Serial.begin(9600);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
   // UN-COMMENT THE NEXT LINE IF YOU ARE POWERING The Pulse Sensor AT LOW VOLTAGE, 
   // AND APPLY THAT VOLTAGE TO THE A-REF PIN
//   analogReference(EXTERNAL);   
 
  // initialize the RF component
  radio.begin();
  radio.openWritingPipe(pipe);
  
  // start message at 0
  msg = 0;
}


void sendMsg( int val )
{
  
  // want to send value 4
  msg = val;
  
  // send the message  
  radio.write(&msg, 1);
}

//  Where the Magic Happens
void loop(){
  
    serialOutput() ;       
    
  if (QS == true){     //  A Heartbeat Was Found
                       // BPM and IBI have been Determined
                       // Quantified Self "QS" true when arduino finds a heartbeat
        digitalWrite(blinkPin,HIGH);     // Blink LED, we got a beat. 
        fadeRate = 255;         // Makes the LED Fade Effect Happen
                                // Set 'fadeRate' Variable to 255 to fade LED with pulse
        serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.     
        QS = false;                      // reset the Quantified Self flag for next time    
       } 
      else { 

      digitalWrite(blinkPin,LOW);            // There is not beat, turn off pin 13 LED
      }
     
   ledFadeToBeat();                      // Makes the LED Fade Effect Happen 
  delay(20);   
  //  take a break
  
  // want to send value 4
  //msg = 4;
  
  // send the message  
  //radio.write(&msg, 1);
  sendMsg(msg);
  
/*  // restart messages after 255
  if ( msg == 255 )
    msg = 0;
  // otherwise, increment
  else
    msg++;
*/    
  // wait a little before sending another message
  delay( 100 );
}









