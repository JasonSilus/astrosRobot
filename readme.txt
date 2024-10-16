// ASsistive Talking Robot Operating System - ASTROS A.1.0 (Alpha Build 1 - Patch 0)

/*
	Project Goal: Build a robotic arm that can respond to vocal user input in natural language.
	(i.e.: User: "Astros, please hold this for me." Astros will run a movement command intended to take an object from a user and hold it until it is requested to hand it back.)

	Within this project exist three departments of development:
	1. Robotics Dept.:
		Responsible for choosing which industrial robotic arm to use, programming the robotic arm, end of arm tool setup, and all physical wiring and manufacturing. 
	2. Natural Language Interface Technology Dept.:
		Responsible for developing the software where the user inputs requests in natural language, extracts keywords out of the user input, and matches the keyword with a hexadecimal byte that it sends to the robot. 
	3. LLM Technology Dept.:
		Responsible for integrating an LLM AI that will respond to user input in a natural way that aligns with what the robot intends on doing or not doing. 

	All programs are currently written in C++ as that is the only programming language I know how to use. 

	Instructions for Building:
	Requirements:
		Arduino Mega2560
		Arduino Braccio Sheild
		Arduino Braccio Robotic Arm
		Raspberry Pi 4 Model B
		TXS0108E Full Duplex Logic-Level Converter
	For this device to work, the Raspberry Pi must be able to send hexadecimal bytes to the Arduino. This is done by connecting the UART from each together with a Logic-Level Voltage Converter as the Raspberry Pi outputs 3.3V Logic and the Arduino outputs 5V Logic. You could configure the Ardunio to read from Serial0 and the Raspberry Pi to send Logic through the USB Port; however, I like being able to download sketches to the Arduino, without having to unplug devices. For this reason, I used the Raspberry Pi's UART GPIO pins and Arduino Mega's Serial1 pins (Pin 18 & 19) this way there is no accidental miscommunication by having two devices trying to talk to the Arduino when uploading sketches. 
	***Wiring Diagram is moot if using PCB instead. 
	The wiring diagram is available online here: https://lirp.cdn-website.com/65e30418/dms3rep/multi/opt/Wiring+Diagram+-+TXS0108E+Logic+Level+Shifter_bb-960w.jpg
	Raspberry Pi GPIO Pinout Diagram is available online here: https://pinout.xyz/pinout/uart
	
	Download the Linux.cpp file to the Raspberry Pi and the Arduino.ino file to the Arduino Mega.
	You can now send a command such as "Astros, hold this for me please." and the Braccio arm will now move into a pre-set position believe to be able to reach out towards the user and grab whatever it is they wish for it to hold. You can now respond to it by sending a command such as "Thank you Astros." and it will close its gripper around the object, wait about 2 seconds for you to let go of the object, then take the object back to its home position.


	At this moment in time, it is clear that the Arduino Braccio Robotic arm will not suffice; however, that does not take away its usefulness in the current state of the project and in the future. If another robot is eventually used, the Arduino will likely be needed to convert the hexadecimal codes sent from the Raspberry Pi into physically wired I/O control to each pin on the Arduino Mega's board; however, this would limit the amount of available movements to 68, as the board is limited to 68 different digital I/O pins; however, this could be expanded with multiple boards or even a PLC conversion. As it currently stands, since it is not limited to physically wired I/O but instead the possible hexadecimal combinations within a singular byte, the maximum possible movements is limited to 256.

*/