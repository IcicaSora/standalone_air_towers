/* by IcicaSora */
/* Charlieplex LED code taken from the OPENITHM project */


/* Define air tower type */
  //#define DIGITAL 1         //Uncomment if using arcade cab air towers
  //#define CHARLIEPLEXED 1   //Uncomment if using Openithm air towers (Designs up to and including v2.1)
  //#define STAGGERED 1       //Uncomment if using Openithm staggered air towers


/* Below doesn't apply if using charlieplexed air towers (CHARLIEPLEXED) */
  //#define SINGLE_SCAN 1     //Scan 1 LED in a single cycle    //Safest
  //#define DOUBLE_SCAN 1     //Scan 2 LEDs in a single cycle   //Safe
  //#define TRIPLE_SCAN 1     //Scan 3 LEDs in a single cycle   //Risks sensors crosstalking


/* Serial print debug */
  //#define SERIAL_PRINT_ANALOG 1   //Serial print IR analog values
  //#define SERIAL_PRINT_VALUE 1    //Serial print if the sensor is blocked or not


/* Below doesn't apply if using arcade cab air towers (DIGITAL) */
  #define DELAY_BEFORE_CALIBRATE 100
  #define AIR_INPUT_THRESHOLD 0.80

#ifdef CHARLIEPLEXED
int led_pins[3] = {0,0,0};        //Pins for IR LEDs (using Charlieplex)
                                  //In Openithm's words, first value is LED_0, second is LED_1, thrid is LED_2
#endif

#ifdef STAGGERED
int led_pins[6] = {0,0,0,0,0,0};  //Pins for IR LEDs (using Staggered)
                                  //Bottom most is the first value, top most is the last value
#endif

int receiver_pins[6] = {1,1,1,1,1,1};   //Needs to be analog capable pins, unless you're using arcade cab towers
                                        //Bottom most is the first value, top most is the last value


/* Keybind */
char keybind[6] = {'0','1','2','3','4','5'};


/* Don't touch anything below */
#include "HID-Project.h"

int receiver_values[6];
int receiver_baseline[6];
bool receiver_states[6];

int calibrate_timer = DELAY_BEFORE_CALIBRATE;

void setup() {
  
  for (int i=0; i<6; i++){
    pinMode(led_pins[i], OUTPUT);
  }

#if (defined(CHARLIEPLEXED) || defined(STAGGERED))
  for (int i=0; i<6; i++){
    pinMode(receiver_pins[i], INPUT);
  }
#endif

#ifdef DIGITAL
  for (int i=0; i<6; i++){
    pinMode(receiver_pins[i], INPUT_PULLUP);
  }
#endif

  NKROKeyboard.begin();

#if (defined(CHARLIEPLEXED) || defined(STAGGERED))
  for (int i=0; i<DELAY_BEFORE_CALIBRATE; i++){
    calibrate_timer--;
  }
  calibrate();
#endif
}


void loop() {

#ifdef STAGGERED
  staggered_scan();
#endif

#ifdef CHARLIEPLEXED
  charlieplex_scan();
#endif

#ifdef DIGITAL
  digital_scan();
#endif

  judge();
  send();

#ifdef SERIAL_PRINT_ANALOG
  printAnalogValue();
#endif

#ifdef SERIAL_PRINT_STATE
  printState();
#endif
}


#ifdef STAGGERED
void staggered_scan() {

#ifdef SINGLE_SCAN
  for (int i=0; i<6; i++){
    digitalWrite(led_pins[i], HIGH);
    receiver_values[i] = analogRead(receiver_pins[i]);
    digitalWrite(led_pins[i], LOW);
  }
#endif

#ifdef DOUBLE_SCAN
  for (int i=0; i<3; i++){
    digitalWrite(led_pins[i], HIGH);
    digitalWrite(led_pins[i+3], HIGH);
    receiver_values[i] = analogRead(receiver_pins[i]);
    receiver_values[i+3] = analogRead(receiver_pins[i+3]);
    digitalWrite(led_pins[i], LOW);
    digitalWrite(led_pins[i+3], LOW);
  }
#endif

#ifdef TRIPLE_SCAN
  for (int i=0; i<2; i++){
    digitalWrite(led_pins[i], HIGH);
    digitalWrite(led_pins[i+2], HIGH);
    digitalWrite(led_pins[i+4], HIGH);
    receiver_values[i] = analogRead(receiver_pins[i]);
    receiver_values[i+2] = analogRead(receiver_pins[i+2]);
    receiver_values[i+4] = analogRead(receiver_pins[i+4]);
    digitalWrite(led_pins[i], LOW);
    digitalWrite(led_pins[i+2], LOW);
    digitalWrite(led_pins[i+4], LOW);
  }
#endif

}
#endif


#ifdef CHARLIEPLEXED
void charlieplex_scan() {
  //bottom most
    pinMode(led_pins[0], OUTPUT);
    pinMode(led_pins[1], OUTPUT);
    pinMode(led_pins[2], INPUT);
    digitalWrite(led_pins[0], HIGH);
    digitalWrite(led_pins[1], LOW);
    digitalWrite(led_pins[2], LOW);
    receiver_values[0] = analogRead(receiver_pins[0]);
    
  //bottom +1
    pinMode(led_pins[0], OUTPUT);
    pinMode(led_pins[1], OUTPUT);
    pinMode(led_pins[2], INPUT);
    digitalWrite(led_pins[0], LOW);
    digitalWrite(led_pins[1], HIGH);
    digitalWrite(led_pins[2], LOW);
    receiver_values[1] = analogRead(receiver_pins[1]);
    
  //bottom +2
    pinMode(led_pins[0], INPUT);
    pinMode(led_pins[1], OUTPUT);
    pinMode(led_pins[2], OUTPUT);
    digitalWrite(led_pins[0], LOW);
    digitalWrite(led_pins[1], HIGH);
    digitalWrite(led_pins[2], LOW);
    receiver_values[2] = analogRead(receiver_pins[2]);
    
  //bottom +3
    pinMode(led_pins[0], INPUT);
    pinMode(led_pins[1], OUTPUT);
    pinMode(led_pins[2], OUTPUT);
    digitalWrite(led_pins[0], LOW);
    digitalWrite(led_pins[1], LOW);
    digitalWrite(led_pins[2], HIGH);
    receiver_values[3] = analogRead(receiver_pins[3]);
    
  //bottom +4
    pinMode(led_pins[0], OUTPUT);
    pinMode(led_pins[1], INPUT);
    pinMode(led_pins[2], OUTPUT);
    digitalWrite(led_pins[0], HIGH);
    digitalWrite(led_pins[1], LOW);
    digitalWrite(led_pins[2], LOW);
    receiver_values[4] = analogRead(receiver_pins[4]);
    
  //bottom +5
    pinMode(led_pins[0], OUTPUT);
    pinMode(led_pins[1], INPUT);
    pinMode(led_pins[2], OUTPUT);
    digitalWrite(led_pins[0], LOW);
    digitalWrite(led_pins[1], LOW);
    digitalWrite(led_pins[2], HIGH);
    receiver_values[5] = analogRead(receiver_pins[5]);
    
  //all off
    pinMode(led_pins[0], INPUT);
    pinMode(led_pins[1], INPUT);
    pinMode(led_pins[2], INPUT);
    
}
#endif


#ifdef DIGITAL
void digital_scan() {

#ifdef SINGLE_SCAN
  for (int i=0; i<6; i++){
    digitalWrite(led_pins[i], HIGH);
    receiver_values[i] = digitalRead(receiver_pins[i]);
    digitalWrite(led_pins[i], LOW);
  }
#endif

#ifdef DOUBLE_SCAN
  for (int i=0; i<3; i++){
    digitalWrite(led_pins[i], HIGH);
    digitalWrite(led_pins[i+3], HIGH);
    receiver_values[i] = digitalRead(receiver_pins[i]);
    receiver_values[i+3] = digitalRead(receiver_pins[i+3]);
    digitalWrite(led_pins[i], LOW);
    digitalWrite(led_pins[i+3], LOW);
  }
#endif

#ifdef TRIPLE_SCAN
  for (int i=0; i<2; i++){
    digitalWrite(led_pins[i], HIGH);
    digitalWrite(led_pins[i+2], HIGH);
    digitalWrite(led_pins[i+4], HIGH);
    receiver_values[i] = digitalRead(receiver_pins[i]);
    receiver_values[i+2] = digitalRead(receiver_pins[i+2]);
    receiver_values[i+4] = digitalRead(receiver_pins[i+4]);
    digitalWrite(led_pins[i], LOW);
    digitalWrite(led_pins[i+2], LOW);
    digitalWrite(led_pins[i+4], LOW);
  }
#endif
}

#endif


void calibrate(){

#ifdef CHARLIEPLEXED
  charlieplex_scan();
#endif 

#ifdef STAGGERED
  staggered_scan();
#endif 

  for (int i=0; i<6; i++){
    receiver_baseline[i] = receiver_values[i];
  }
  
}


void judge(){

#if (defined(CHARLIEPLEXED) || defined(STAGGERED))
  for (int i=0; i<6; i++){
    if (receiver_values[i] < receiver_baseline[i] * AIR_INPUT_THRESHOLD){
      receiver_states[i] = true;
    }
    else {
      receiver_states[i] = false;
    }
  }
#endif

#ifdef DIGITAL
  for (int i=0; i<6; i++){
    receiver_states[i] = receiver_values[i];
  }
#endif

}


void send(){
  for (int i=0; i<6; i++){
    if (receiver_states[i] == true){
      NKROKeyboard.write(keybind[i]);
    }
  }
  //NKROKeyboard.send();
}


void printAnalogValue(){
  for (int i=0; i<6; i++){
      Serial.print(receiver_values[i]);
      Serial.print(" ");
  }
  Serial.println("");
}

void printState(){
  for (int i=0; i<6; i++){
      Serial.print(receiver_states[i]);
      Serial.print(" ");
  }
  Serial.println("");
}


#if (defined(DIGITAL) + defined(CHARLIEPLEXED) + defined(STAGGERED)) != 1
#error "Must define only ONE air tower type."
#endif

#ifndef CHARLIEPLEXED
  #if (defined(SINGLE_SCAN) + defined(DOUBLE_SCAN) + defined(TRIPLE_SCAN)) != 1
  #error "Must define only ONE scanning mode."
  #endif
#endif

#if (defined(SERIAL_PRINT_ANALOG) + defined(SERIAL_PRINT_VALUE)) > 1
#error "Must define no more than ONE serial print output type."
#endif
