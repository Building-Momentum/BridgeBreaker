/*
  Code used to measure force to break bridges during innovation bootcamp

  Author: A.Vega
  Date: 10 JUN 2019
  Version: 03

  Changes from previous version:

  This 3rd version is to be used in a hydralic press where the load cell undergoes compression not elongation.

  This example demonstrates basic scale output. See the calibration sketch to get the calibration_factor for your
  specific load cell setup.

  Arduino pin 2 -> HX711 CLK
  3 -> DAT
  5V -> VCC
  GND -> GND

  The HX711 board can be powered from 2.7V to 5V, so the Arduino 5V power should be fine.

*/

#include "HX711.h"

#include <SPI.h>
#include <Wire.h>


#include "Nextion.h"

int TareButton = 10;
int ResetButton = 11;

int led1 = 5;
int led2 = 6;

int TareValue = 1;
int ResetValue = 1;

static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000
};

#define calibration_factor 13023.181640 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define DOUT 3  // Pin for digital info from the HX711 to the Arduino
#define CLK  2  // Pin for clocking data from the HX711 to the Arduino
#define COMMAND_WAIT_TIME 2000  // Time for the display to hold after issuing a command

NexButton b0 = NexButton(1, 2, "b0");
NexButton b1 = NexButton(1, 3, "b1");
NexButton t0 = NexButton(1, 4, "t0");
NexButton b2 = NexButton(0, 2, "b0");
NexButton b3 = NexButton(1, 8, "b3");
NexGauge x0 = NexGauge(1, 7, "x0");
NexGauge x1 = NexGauge(1, 6, "x1");
NexPage page0 = NexPage(0, 0, "page0");
NexPage page1 = NexPage(1, 0, "page1");

char buffer[100] = {0};

NexTouch *nex_listen_list[] =
{
  &b0,
  &b1,
  &b2,
  &b3,
  &page0,
  &page1,
  NULL
};



HX711 scale;
unsigned long display_delay = 800; // blah
unsigned long next_display_time = 0; // blah
float max_weight = 0;
float current_weight = 0;
char command_input;


void setup(void) {
  Serial.begin(9600);

  pinMode(ResetButton, INPUT_PULLUP);
  pinMode(TareButton, INPUT_PULLUP);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  
  nexInit();
  b0.attachPop(b0PopCallback, &b0);
  b1.attachPop(b1PopCallback, &b1);
  b2.attachPop(b2PopCallback, &b2);
  b3.attachPop(b3PopCallback, &b3);
  page0.attachPop(page0PopCallback);
  page1.attachPop(page1PopCallback);
  dbSerialPrintln("setup done");




  scale.begin(DOUT, CLK);
  delay(1000);
  scale.set_offset(4294967295);
  scale.set_scale(calibration_factor);  // This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();                         // Reset the scale to zero
}

void loop(void) {
  // Obtain a scale reading

  nexLoop(nex_listen_list);

  current_weight = scale.get_units();   // Returns a float

  // Store current reading as a max if greater than value stored in the current max
  if (current_weight > max_weight) {
    max_weight = current_weight;
  }

  // Display a reading at a slower rate to not flood serial monitor
  if (millis() > next_display_time) {
    //testdrawstyles("Current Weight:", 2, current_weight);
    //Serial.println(current_weight);
    //float new_current_weight = current_weight * 100;
    x0.setValue(current_weight * 625);

    //testdrawstyles("Max Weight:", 2, max_weight);
    //Serial.println(max_weight);
    //float new_max_weight = max_weight * 100;
    x1.setValue(max_weight * 625);
    //x1.setValue(0);

    //Serial.print("Timestamp:  ");
    //Serial.println(millis());
    next_display_time += display_delay; // Sets next display time
  }

  // Checks for commands from the computer
  //if (Serial.available()){
  //TareValue = digitalRead(TareButton);
  //ResetValue = digitalRead(ResetButton);
/*
  if (TareValue == 0) {       // If the command is to tare:
    scale.tare();                   // Reset the scale to zero
    max_weight = 0;                 // Set max to zero
  } else if (ResetValue == 0) { // If the command is to reset the max weight:
    max_weight = 0;                 // Set max to zero
    //testdrawstyles("Max Weight Reset", 2);
    next_display_time += COMMAND_WAIT_TIME;     // Hold display for a bit
  } else {
    //      Serial.println("Command Unknown!!");        // If command is unknown:
    //      Serial.println("Enter the letter 't' at any time to perform a manual tare.");
    //      Serial.println("Enter the letter 'r' at any time to reset the max weight.");
    next_display_time += COMMAND_WAIT_TIME;     // Hold display for a bit
  }
*/
  //Serial.println("end of loop");
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  //page1.show();
}

void b0PopCallback(void *ptr)
{
  //Serial.println("im here");
  x0.setValue(0);
  digitalWrite(led1, HIGH);
  scale.tare();                   // Reset the scale to zero
  max_weight = 0;
  delay(200);
}

void b1PopCallback(void *ptr)
{
  //Serial.println("im there");
  //x0.setValue(0.);
  x1.setValue(0);
  digitalWrite(led2, HIGH);
  max_weight = 0;
  next_display_time += COMMAND_WAIT_TIME;
  delay(200);
}

void b2PopCallback(void *ptr)
{
  //Serial.println("im there");
  //x0.setValue(0.);
  digitalWrite(led2,HIGH);
  page0.show();
}
void b3PopCallback(void *ptr)
{
  //Serial.println("im there");
  //x0.setValue(0.);
  digitalWrite(led1,HIGH);
  page1.show();
}

void page0PopCallback(void *ptr)
{
    dbSerialPrintln("page0PopCallback");
    page1.show();
}

void page1PopCallback(void *ptr)
{
    dbSerialPrintln("page1PopCallback");
    page0.show();
}



 

 
