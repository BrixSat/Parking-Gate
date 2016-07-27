# Parking-Gate
Arduino parking gate controller


This project is intended to be a Camping site gate controller.
The description is the following. 

![alt tag](https://github.com/BrixSat/Parking-Gate/blob/master/desenho.png?raw=true)

Legend:
A - Exit card rfid rc522 card reader
B - Entrance card rfid rc522 card reader
C - Photocell module one
D - Photocell module two
E - 24 volt motor
F - Blinking light

We have a photocell to control the existence of cars and persons on the gate. 
Two RFID rc522 card readers one for the entrance and another for the other side the exit.
The arduino has a network shield, 4 relay module and a screen (2 rows). 
Two sensors at the motor to know if gate is full opened or full closed.

Use case:
A user passes a card on the card reader, arduino reads the id. 
Ethernet shield sends to a webserver the id of the card readed and if entering or exiting. The response will be true or false and if possible a message to the screen like "Already inside" when trying to enter with out exiting or already outside" when exiting without enter.
If OK is received the arduino checks for the photocell and trigger  relay 1 (open the gate) and relay 3 (blinking light) and it keeps triggering until the Max motor sensor is pressed. 
After X seconds or after the photocell is triggered (from on to off with a few seconds delay) the arduino checks for the photocell presence of obstacles and triggers relay 2 (close gate) until photocell is triggered or min motor sensor is pressed. 

Then wait again for a card. 


The webserver part is the most simple (at least for me). If you guys can help and give some help I would appreciate. 
