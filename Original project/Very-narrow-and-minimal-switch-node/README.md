**What izz dizz ???**

**_
### Version 1-0:
_**

This board is a 1 trick pony (it's not very smart). It is meant to allow connection of 2 switches on pins D2 and D3 of the processor. It uses the SMD version of the NRF24L01+ module, runs the atmega328p on internal oscillator and is meant to sleep most of the time.

There is space for a pull up resistor (any value between 1M Ohm and 10M Ohm is fine) for each of the two pins (D2 and/or D3).
```
R1 is for D2
R2 is for D3

```
A led is connected to pin D8 via a 330 Ohm resistor (R3). An electrolytical capacitor of 10uF is provided to decouple the Vcc and Gnd pins of the NRF24L01+ smd module. Two 100nF ceramic capacitors are decoupling the power line (Vcc/AVcc and ARef).

And that is it. No temp sensor, no voltage divider, no prototyping area, no FTDI connector, no ISP connector, no crystal.

Just a very narrow node based on @m26872's original idea.

**How can you use it ?**

Since there are two pins for digital signals and 2 pins for power, you still have a few options.

Lets look at the easy part first: power

This boards needs DC power between 1.9V DC (the minimum the NRF24L01+ module needs to function) and maximum 3.6V DC (again a limitation of the NRF24L01+). The processor can handle a wider range (1.8v - 5.1V), but as said, the radio sets the limits on the allowed voltages. There are two pins at the edge called "GND" and "3V3", function should be obvious. GND has a square pad and sits at the edge of the board.

For further orientation, hold the board so that the radio is on the right hand side, the "GND" terminal sits in the top left corner.

Now for the two pins called "D2" and "D3".

These two pins are connected directly to the pins of the processor called ... wait for it ... "D2" and "D3". I'm feeling very pleased with knowing this now :-), moving on.

If you do not mount the pull up resistors, you can use these pins as outputs. Seriously? Yes !!!! Outputs ! We can give "1" or "0" on both D2 and D3 so this could be used for control of something. Personally not my use case, but it is possible.

**1 switch input**

If you need only 1 pin to connect a switch (magnetic, mechanical, reed, optical,...), but you are missing a "GND" pin, then there is a solution.

You do NOT mount the pull up resistor for D3, and define pin D3 as an output LOW pin. This sets D3 equal to ground. You will need to define this in the setup procedure of the sketch.

Then you will need to have mounted the pull up resistor for D2 (on the board it is marked with "R1"), and the header pin called "D2" becomes the input.

You would get the following on the header:

```
square pad: GND    -----+ power terminal, houurrah !
round pad: VCC    ------+
round pad: D2 (input)     -------+
round pad: D3 (= GND)     -------+ switch terminal, yeah !
```

**powering something with low consumption**

You can set both pins D2 and D3 of the processor to OUTPUT, do not mount the pull up resistors, do not activate the internal PullUPs, and by setting one of the pins to HIGH and the other to LOW you can power something. You will need to take care that a maximum of 20mA is consumed, as this is the recommended maximum amperage the pins of the atmega328 can drive or source.

**Now for another option: two switches.**

This is easy to understand: when both pull up resistors are mounted and both the processor pins D2 and D3 are set to INPUT, then the header pins are ready to receive GND or 3V3. You will need to use the power "GND" pin as ground reference.

The nice thing about D2 and D3 as input pins, is that they can be set as hardware interrupt pins to wake up the processor from power down (MySensor "SLEEP(0,CHANGE,1,CHANGE,0)" function).

Power consumption is measured at 7.7 uA maximum during sleep (2 open switches) and 7.0 uA minimum (2 closed switches), if using the Watchdog timer to wake up every now and then to send battery level. Under full power down (as the function call above does) the power consumption will be lower but I did not measure it yet.

This is my preferred use case, hence the name of this node.

**about Passive Infra Red sensing**

If a PIR sensor is powered by 3V3, and it can produce a HIGH signal, you could connect a switch (on a door or window) to pin D2. And you still can connect the PIR to D3. Any change on D2 or D3 would wake up the processor.

Regarding PIR sensors to use: only use 3V3 versions, not the ones which need 5V, because they would drive the input pin too high. Not a disaster if you do this, but I'm not sure how long the atmega328 would accept it before failing.

**Other sensors ???**

Indeed, any other sensor which produces a logical HIGH or LOW can be used as input on D2 or D3. Just make sure that the sensor is powered by itself, or works within the limits of the battery voltage powering the node.

**Measuring and reporting battery voltage**

Battery voltage is measured by using the internal reference and the Vcc library (so no voltage divider needed). My DEV library based sketch is included and uses this library. As we are not allowed to power the board above 3.6V DC, we can use the "hidden" voltage measuring technique. Look at the provided sketch for this (look for "vcc.something").
This internal measuring technique has an error of around 10%, so do not expect accurate readings. Use it to decide when to replace the battery. I set an alarm in my controller, when a node reports a battery level below 2V. This gives me time to go buy batteries and replace them (usually more than a week time between first alarm and replacement, I'm slow ...).

**About the included sketch**

-- important: the sketch was written using the Development library, so not version 1.5.x but the newest of the newest of the best ;-). It may look like some things are missing in the code, that would be true if you have MySensors library 1.5.x

In my sketch, the battery voltage is sent every time the processor wakes up. If nothing changes at the input pins, then the node will wake up every 23 hours to report battery voltage (not battery percent, only battery voltage).
If a signal change is seen at the pins, the battery voltage is measured and sent first. This allows the signal at the input pins to settle to a stable state (this is software debounce), and if the stable state at the pin is different from previous reporting, it is sent to the gateway/controller.

**about the hardware**

You can get this board at OSH Park:

v1-0  https://oshpark.com/shared_projects/LAJtrt2k

v1-1  https://oshpark.com/shared_projects/GTBdUqTV

I'm using a Optiboot 6.2 version which expects a baudrate of 38400. Any boot loader is fine, as long as you set the fuses to 8MHz internal oscillator. You will need to flash the atmega328p before you plug it into this board, so make sure it works in another setup or on the breadboard before you push it hard into the socket.

There is no way to reprogram the board, not via FTDI, not over the air, not by jedi hand waving. Only when you pull out the processor and program it in another setup, can you change anything. Remember: this is a minimal board, it does 1 thing (or 2 if you use two pins), and it can notify your controller of whatever it does or sees. But it is certainly not a versatile node like it's godfather (the slim 2AA battery node by m26872). This is a minion.

**_
### Version 1-1:
_**


In a comment someone suggested to add I2C capability, and actually this is relatively easy. So I added solder jumpers to give the option.

There are two solder jumpers called SJ1 and SJ2. Together they define which signals are presented on PIN1 and PIN2 of the header at the edge of the board.

The silkscreen also shows this. The consequence of this change is that you can not add 2 pull up resistors, but just 1 (for D2). Here are the options.

```
SJ1:
+--SDA-A4
+--PIN1--+
   D2----+
```
With SJ1 you choose to connect either D2 to PIN1 of you choose A4 connected to PIN1. This is independent of the other jumper.
If you want to use I2C, you will need to connect SCL-A5 as well on SJ2.

```
SJ2:
+--SCL-A5
+--PIN2---+
   GND----+
```
With this solder jumper you can connect GND to PIN2, or you choose A5 connected to PIN2. If I2C needs to be used, then A4 needs to be connected, and the I2C functions need to be called.

Here are all possible combinations you can make on the header

```
GND
D2
GND
VCC
```

```
GND
D2
A5
VCC
```

```
GND
A4
GND
VCC
```

```
GND
A4
A5
VCC
```

In this last case you can use the pins as analog inputs, or as the I2C "ports" SDA/SCL:

```
GND
SDA
SCL
VCC
```

This is - by pure coincidence - also the pinout of the small SMD versions of a SI7021 board.

Second remark on I2C: this node does not provide pull up resistors for I2C, so if needed, the connecting device needs to provide the I2C pull up resistors. Most I2C sensor boards do this, but check anyway.

**Using D6**

Version 1-1 of the board is slightly different than v1-0 in another aspect: instead of using pin D8 to allow flashing a LED, I now use pin D6. I have added a bootloader (based on Optiboot 6.2) which will flash the LED on pin D6 three times when reset is given. You can find the hex file in the design files.


Why pin D6 ? Because it can be driven with Pulse Width Modulation via the function AnalogWrite(). Pin D8 is a pure on/off pin.

If you skip mounting R3 and put a wire instead, then you have D6 and GND at the pads where normally one would solder the LED. This means that if you can live without the LED, you actually have access to a third processor pin. In this case D6.

I thought D6 would be more versatile then D8. And for those that do use a LED, D6 allows dimming effects (whereas D8 was only on or off).

**RESET pullup**

Finally, another change between v1-0 and v1-1 is how I handle the reset pin. I left it floating in v1-0. So far that has not caused any problems, except in 1 board. So I decided to pull up RESET just to be sure. As I had to choose between giving GND or a second interrupt cable pin on the external connector, I decided in favour of GND. This node is simple and cheap enough to handle a single switch. Handling 2 switches is possible, but not really needed. And we can still use an analog pin as digital input. And if you skip mounting the LED to get a third pin (input or PWM capable output): D6.

The fact that only 1 pin can wake the node from deep sleep is a tradeoff I accept.


**Version history**

JAN 2016: Added Version 1 info.
This is revision 1 of this board. It is meant as a test board for mounting the SMD version of the NRF24L01+ (a quick hack I admit). A possible improvement is to add two more holes next to R1 and R2, so they could be used as pull down resistors.
The silkscreen (the text on the board) can be better, and marking the names of the header pins on he bottom side is something I should have done (using the boards tells me this now). The numbering of the capacitors does not start at C1, because originally I had more of them on the board. I did not renumber after cleaning out all unnecessary stuff. I may do that at a later stage if I revisit this design.
The designed is licensed under CERN OHL V1.2 and is fully Open Hardware.

13-FEB-2016: added v1-1 files (tested) and use cases.
Explanation of the solder jumpers and what you could do with the pins in various combinations.
15-FEB-2016: added bootloader
Added a recompiled Optiboot 6.2 with LED flash on pin D6, for 8MHz internal oscillator (or 8MHz crystal if you use this in another board) and baudrate of 38400 for uploads.