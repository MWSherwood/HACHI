/*
Im murging code for LED with debounce with code for debouncing and reading multiple buttons
Addafruit blog: https://blog.adafruit.com/2009/10/20/example-code-for-multi-button-checker-with-debouncing/

Hachi v1, an open hardware pressurized perfusion system.

This Adruino file is designed to be uploaded on an Arduino Mega for the OpenPerfusion project.
Source : https//github.com/MWSherwood/OpenPerfusion
This project is Open Source licensed, released under CERNOHL v1.2

by M.W. Sherwood, 2022.06.24
Taruno Lab, KPUM, Kyoto, Japan

*/

///////////////////////////////////////////////////////////////////////////
//          KEY PARAMETERS (USER DEFINED)                                        
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
//          GLOBAL VARIABLES                                        
///////////////////////////////////////////////////////////////////////////

// Set up the pins



//  Declare gloabl variables

int Pulse_Duration = 20;  // Amount of time the pulse is on


///////////////////////////////////////////////////////////////////////////
//          Define I/O pins                                        
///////////////////////////////////////////////////////////////////////////


#define DEBOUNCE 10  // button debouncer, how many ms to debounce, 5+ ms is usually plenty

// here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
// just add/delete pins to increase/decrease the number of buttons
byte buttons[] = {22, 24, 26, 28, 30, 32, 34, 36}; 

// This handy macro lets us determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)

// we will track if a button is just pressed, just released, or 'currently pressed' 
//volatile byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];

// define the LED pins
// just add/delete pins to increase/decrease the number of buttons
byte LEDS[] = {2, 3, 4, 5, 6, 7, 8, 9};

// This handy macro lets us determine how big the LEDS array is
#define NUMLEDS sizeof(LEDS)

///////////////////////////////////////////////////////////////////////////
//          SETUP (runs once when the Arduino is initialised)                                        
///////////////////////////////////////////////////////////////////////////

void setup() {
  byte i;

/* I don't want serial interface for now
  // set up serial port
  Serial.begin(9600);
  Serial.print("Button checker with ");
  Serial.print(NUMBUTTONS, DEC);
  Serial.println(" buttons");
*/ 
// Make input & enable pull-down resistors on switch pins
  for (i=0; i< NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT);
    digitalWrite(buttons[i], LOW);
  }
  // Initialize the LED pins as outputs:
  for (i=0; i< NUMLEDS; i++) {
    pinMode(LEDS[i], OUTPUT);
  }


  // Run timer2 interrupt every 15 ms 
  TCCR2A = 0;
  TCCR2B = 1<<CS22 | 1<<CS21 | 1<<CS20;

  //Timer2 Overflow Interrupt Enable
  TIMSK2 |= 1<<TOIE2;

}

SIGNAL(TIMER2_OVF_vect) {
  check_switches();
}

///////////////////////////////////////////////////////////////////////////
//                   MODULES                                        
///////////////////////////////////////////////////////////////////////////

void Pulse(){
  //  Open the valve, Blink the LED for the define Pulse Duration time
  digitalWrite(SolenoidPin,  HIGH);
  digitalWrite(LEDpin, HIGH); // light the LED
  delay(Pulse_Duration); // wait for the pulse duration.
  digitalWrite(SolenoidPin,  LOW); // close the solenoid and switch off the LED.
  digitalWrite(LEDPin, LOW);
}


void check_switches() {

  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];

  static byte LEDState[NUMLEDS];
  
  static long lasttime;
  byte index;

  if (millis() < lasttime) {
     // we wrapped around, lets just try again
     lasttime = millis();
  }
  
  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return; 
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();
  
  for (index = 0; index < NUMBUTTONS; index++) {
     
    currentstate[index] = digitalRead(buttons[index]);   // read the button
    
    // MWS Turn LED on if button HIGH
     if (currentstate[index] == HIGH && previousstate[index] == LOW) {
       if (LEDState[index] == 0) {
        digitalWrite(LEDS[index], HIGH);
        LEDState[index] = 1;
       } else {
        digitalWrite(LEDS[index], LOW);
        LEDState[index] = 0;
       }
     }
    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }
}


void loop() {
    }
