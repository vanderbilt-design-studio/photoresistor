/* Simple test of the functionality of the photo resistor

Connect the photoresistor one leg to pin 0, and pin to +5V
Connect a resistor (around 10k is a good value, higher
values gives higher readings) from pin 0 to GND. (see appendix of arduino notebook page 37 for schematics).

----------------------------------------------------

           PhotoR     10K
 +5    o---/\/\/--.--/\/\/---o GND
                  |
 Pin 0 o-----------

----------------------------------------------------
*/

void setup()
{
    Serial.begin(600);  //Begin serial communcation
}

bool isOpen = false;
void loop()
{
  if (Serial.available() > 0) {
    while (Serial.available() > 0) Serial.read();
    Serial.write(isOpen ? (byte) 1 : (byte) 0);
    Serial.flush();
  }
   if (analogRead(0) > 200 ) {
    isOpen = true;
   } else {
    isOpen = false;
   }
   delay(10); //short delay for faster response to light.
}
