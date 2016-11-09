/**
   The MySensors Arduino library handles the wireless radio link and protocol
   between your home built sensors/actuators and HA controller of choice.
   The sensors forms a self healing radio network with optional repeaters. Each
   repeater and gateway builds a routing tables in EEPROM which keeps track of the
   network topology allowing messages to be routed to nodes.

   Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
   Copyright (C) 2013-2015 Sensnology AB
   Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors

   Documentation: http://www.mysensors.org
   Support Forum: http://forum.mysensors.org

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.

 *******************************

   DESCRIPTION

   Simple binary 2 switch node
   Connect button or door/window reed switch between
   digitial I/O pin 2 or 3 and GND.

   This sketch is for the very narrow node.

   Based on the example found here:
   http://www.mysensors.org/build/binary
   
*/

#define SKETCH_NAME "2 Switches Sensor"
#define SKETCH_MAJOR_VER "1"
#define SKETCH_MINOR_VER "0"

//#define MY_NODE_ID 13 (uncomment this if you want to control the NODE_ID yourself
//#define MY_PARENT_ID 0

#define MY_BAUD_RATE 57600

#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

// Enable serial gateway
//#define MY_GATEWAY_SERIAL

// Enabled repeater feature for this node
//#define MY_REPEATER_FEATURE

// Flash leds on rx/tx/err
//#define MY_LEDS_BLINKING_FEATURE
// Set blinking period
//#define MY_DEFAULT_LED_BLINK_PERIOD 300

// Enable inclusion mode
//#define MY_INCLUSION_MODE_FEATURE
// Enable Inclusion mode button on gateway
//#define MY_INCLUSION_BUTTON_FEATURE
// Set inclusion mode duration (in seconds)
//#define MY_INCLUSION_MODE_DURATION 60
// Digital pin used for inclusion mode button
//#define MY_INCLUSION_MODE_BUTTON_PIN  3

//#define MY_DEFAULT_ERR_LED_PIN 5  // Error led pin
//#define MY_DEFAULT_RX_LED_PIN  6  // Receive led pin
//#define MY_DEFAULT_TX_LED_PIN  7  // the PCB, on board LED

// possible values: RF24_PA_LOW (is default on gateway), RF24_PA_MED, RF24_PA_HIGH, RF24_PA_MAX (is default on nodes)
#define MY_RF24_PA_LEVEL RF24_PA_HIGH

// RF channel for the sensor net, 0-127
#define MY_RF24_CHANNEL     76

//RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
#define MY_RF24_DATARATE      RF24_250KBPS


#include <SPI.h>
#include <MySensor.h>
#include <Vcc.h>

// Child declarations
#define BATTERYVOLTAGE  1
#define DOORSWITCH1     2
#define DOORSWITCH2     3

// pin definitions
#define DOOR_PIN1  2  // Arduino Digital I/O pin for button/reed switch
#define DOOR_PIN2  3  // Arduino Digital I/O pin for button/reed switch
#define LEDPIN     8  // on the very narrow node, this is my only available led.

// constant definitions
#define MESSAGEWAIT  500
#define NORMALONTIME 5
#define BLIPINTERVAL 75
#define REPORTINTERVAL 82800000 // every 23 hours
//#define REPORTINTERVAL 43200000 // every 12 hours
//#define REPORTINTERVAL 3600000 // every 1 hour
//#define REPORTINTERVAL 1800000 // every 30 minutes
//#define REPORTINTERVAL 15000 // every 15 seconds

int oldValue1 = -1;
int oldValue2 = -1;
int SleepResult = -1;

const float VccMin   = 1.9;           // Minimum expected Vcc level, in Volts.
const float VccMax   = 3.3;           // Maximum expected Vcc level, in Volts.
const float VccCorrection = 3.0 / 3.0 ; // Measured Vcc by multimeter divided by reported Vcc, use this to calibrate
float BatteryVoltage = 0.0;

Vcc vcc(VccCorrection);

MyMessage door1msg(DOORSWITCH1, V_TRIPPED);
MyMessage door2msg(DOORSWITCH2, V_TRIPPED);
MyMessage batterymsg(BATTERYVOLTAGE, V_VOLTAGE);

void setup()
{
  // Setup the led
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  // Setup the buttons
  pinMode(DOOR_PIN1, INPUT);
  pinMode(DOOR_PIN2, INPUT);

}


void presentation() {

  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);
  sleep(MESSAGEWAIT); // only needed when using Domoticz and direct connected NRF24 - needs some time between messages

  present(BATTERYVOLTAGE, S_MULTIMETER);
  sleep(MESSAGEWAIT); // only needed when using Domoticz and direct connected NRF24 - needs some time between messages

  present(DOORSWITCH1, S_DOOR);
  sleep(MESSAGEWAIT); // only needed when using Domoticz and direct connected NRF24 - needs some time between messages

  present(DOORSWITCH2, S_DOOR);
  sleep(MESSAGEWAIT); // only needed when using Domoticz and direct connected NRF24 - needs some time between messages

// the next calls to BlipLed are optional (I like blinky lights)
  BlipLed();
  sleep(BLIPINTERVAL);
  BlipLed();
  sleep(BLIPINTERVAL);
}



void loop()
{
// optional BlipLed :-)
  BlipLed();

  float BatteryVoltage = vcc.Read_Volts();
  int value = digitalRead(DOOR_PIN1);

  if (SleepResult == -1)
  {
    // we get here if the processor wakes up after the reporting interval (so not when a switch is open/closed).
    send(batterymsg.set(BatteryVoltage, 3));
  }
  else
  {
    // we get here if one of the swicthes was opened or closed.
    
    send(batterymsg.set(BatteryVoltage, 3));
    sleep(MESSAGEWAIT); // only needed when using Domoticz and direct connected NRF24 - needs some time between messages

// read the switch, it should be settled by now after sending the battery voltage

    value = digitalRead(DOOR_PIN1);

    if (value != oldValue1)
    {
      // Send in the new value
      send(door1msg.set(value != HIGH ? 0 : 1));
      oldValue1 = value;
      BlipLed();
      sleep(MESSAGEWAIT); // only needed when using Domoticz and direct connected NRF24 - needs some time between messages
    }

// do it again: read the switch

    value = digitalRead(DOOR_PIN2);

    if (value != oldValue2)
    {
      // Send in the new value
      send(door2msg.set(value != HIGH ? 0 : 1));
      oldValue2 = value;
      BlipLed();
      sleep(MESSAGEWAIT); // only needed when using Domoticz and direct connected NRF24 - needs some time between messages
    }

  }

  // Sleep until something happens with the switches, or wakeup after interval
  SleepResult = sleep(DOOR_PIN1 - 2, CHANGE, DOOR_PIN2 - 2, CHANGE, REPORTINTERVAL);
}



////////////////////////////////////////////////////////
void BlipLed()
{
  digitalWrite(LEDPIN, HIGH);
  wait(NORMALONTIME);
  digitalWrite(LEDPIN, LOW);
}

