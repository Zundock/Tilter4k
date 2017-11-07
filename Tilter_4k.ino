#include <avr/interrupt.h>
#include "tinysnore.h"
#define greenLED 0
#define redLED 1
#define deploy 3
#define tilt 2

int count(0);
double batteryVoltage(0);

void setup() {
  pinMode(greenLED, OUTPUT);
  pinMode(deploy, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(tilt, INPUT_PULLUP);
  digitalWrite(redLED, LOW);    // make sure red led is low      
  GIMSK = 0b01000000;           // turns on pin change interrupts
  sei();                        // enables interrupts
  MCUCR = 0b00000000;           // enable falling edge interupt on pin 7
  initialBatteryCheck();
}

void loop() {
  
  batteryCheck();               //Check if battery has enough power to deploy.
  powerIndicator();             //Indicate if device has power.
  
}

ISR(INT0_vect)

{
  
  deployCheck();

}

void deployCheck(){

  volatile unsigned long startCount = 0;
  bool tiltCheck = digitalRead(tilt);
  
  while(tiltCheck == LOW) {
    
    if (startCount > 170000){
      digitalWrite(deploy, HIGH);
    }
    delayMicroseconds(250);
    tiltCheck = digitalRead(tilt);
    startCount++;
  }
}
  
void powerIndicator() {
  
  digitalWrite(greenLED, HIGH);       // Turn green LED on
  snore(20);
  digitalWrite(greenLED, LOW);        // Turn green LED OFF
  snore(10000);
  
}

void batteryCheck() {
  
  count++;
  
  if (count > 8640) {
    
    batteryVoltage = getVcc();
    if (batteryVoltage < 3900){
      digitalWrite(redLED, HIGH); // turns red LED on
    } 
  }

  
}

void initialBatteryCheck() {
  
  count++;
    batteryVoltage = getVcc();
    if (batteryVoltage < 3300){
      digitalWrite(redLED, HIGH); // turns red LED on
    } 
  }

int getVcc() {
  //reads internal 1.1V reference against VCC

  ADMUX = _BV(MUX3) | _BV(MUX2); // For ATtiny85/45
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  uint8_t low = ADCL;
  unsigned int val = (ADCH << 8) | low;
  //discard previous result
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  low = ADCL;
  val = (ADCH << 8) | low;
  
  return (((long)1024 * 1000) / val - 100 );  
}


