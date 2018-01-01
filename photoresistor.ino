/* Program to use a photoresistor to check if a door is open

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
    // Serial @ 9600 BAUD. If you change this, make sure you change it in the studio status program.
    Serial.begin(9600);
}

int lastRead; // Last value received from the photoresistor.
void loop()
{
  // Any available data indicates that the studio status program sent a request.
  if (Serial.available() > 0) {
    // The value of the data does not matter. so we just empty the input without actually reading it.
    while (Serial.available() > 0) Serial.read();
    // Send the value as two unsigned bytes. Studio status program reassembles them on the other end.
    // (I tried other ways of sending the data but it gets very garbled very quickly. Simply using
    // two bytes makes the handling fast and clear.)
    Serial.write(lastRead / 256);
    Serial.write(lastRead % 256);
    // Force the data to be sent right away.
    Serial.flush();
  }
   lastRead = analogRead(0);
        
   delay(10);
}
