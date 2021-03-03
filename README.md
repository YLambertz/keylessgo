# keylessgo
In this project we built a simple keyless-go System for an old Motorcycle. 
A nfc sensor from dfrobot is used to read a ndef message pushed by a smartphone app via near field communication (NFC). 
The Microcontroller (ESP8266) then turns on the ignition and the speedometer, if the message contained the "key" to start.
If the smartphone is lost while driving, the ignition and the speedometer will stay on, as long as the killswitch button isn't pressed, due to safety reasons. 
In an additional subroutine an emergency-card is identified by its UID (unsafe), to be able to start the bike without the smartphone. 
To prevent the device from a "bruteforce-attack" (just trying out every possible key as fast as possible) the software will stop reading from the nfc-Sensor for 10 seconds after a wrong key was read.



![IMG_20190109_185457](https://user-images.githubusercontent.com/78151090/109825302-fd533180-7c39-11eb-97e6-aa2ce708f5b5.jpg)

Der NFC Sensor wurde in einer eigens konstruierten Smartphone-Halterung mit Epoxitharz wasserdicht vergossen.

![IMG_20190117_150513](https://user-images.githubusercontent.com/78151090/109825320-00e6b880-7c3a-11eb-853e-361507312301.jpg)

![mc zeug](https://user-images.githubusercontent.com/78151090/109825335-047a3f80-7c3a-11eb-95a3-7dd3843b2283.jpg)


The Housing for the microcontroller: https://www.thingiverse.com/thing:4781438 

![KeylessGo_Baugruppe_PLA](https://user-images.githubusercontent.com/78151090/109825616-499e7180-7c3a-11eb-8522-53df8d6d9e41.jpg)

