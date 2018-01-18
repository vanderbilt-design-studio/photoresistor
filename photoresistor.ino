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
// Need two because Arduino doesn't have enough power to drive the relay with 1 pin.
#define RELAY_PIN1 5
#define RELAY_PIN2 6
#define RELAY_PIN3 7
void setup()
{
  // Serial @ 9600 BAUD. If you change this, make sure you change it in the studio status program.
  Serial.begin(9600);
  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);
  pinMode(RELAY_PIN3, OUTPUT);
}

class photo {
#define PHOTO_READ_PERIOD 10
  protected:
    unsigned long lastRead = 0;
    const int port;
    photo(int port) : port(port) {}
    int photoRead() {
      auto diff = millis() - lastRead;
      if (diff < PHOTO_READ_PERIOD) {
        delay(PHOTO_READ_PERIOD - diff);
      }
      lastRead = millis();
      return analogRead(port);
    }
  public:
    virtual void observe() = 0;
};

class doorSensor : photo {
  private:
    int lastVal;
  public:
    doorSensor(int port) : photo(port) {}
    void observe() {
      lastVal = photoRead();
    }
    bool ajar() {
      return lastVal > 100;
    }
};

class motionSensor : photo {
#define MOTION_ALPHA 0.7 // How much previous measurements should be taken into account
#define MOTION_DETECT 0.3 // How much disturbance should be considered movement
#define MOTION_RECENT_TIMEFRAME 300000 // 5 mins
  private:
    float runningVal;
    unsigned long lastDetected;
    bool recentlyDetected = false;
  public:
    motionSensor(int port) : photo(port) {}
    void observe() {
      int nextVal = photoRead();
      runningVal = MOTION_ALPHA * nextVal + (1 - MOTION_ALPHA) * runningVal;

      // We only want to track positive values of this, because this means
      // there was a sudden decrease in light. Sudden increase might mean
      // that the lights were turned on or someone was putting their hand
      // in front of the sensor for an extended period of time.
      bool detected = ((runningVal - nextVal) / runningVal) > MOTION_DETECT;
      if (detected) {
        if (!recentlyDetected) {
          recentlyDetected = true;
        }
        lastDetected = millis();
      }
    }
    bool detected() {
      if (millis() - lastDetected > MOTION_RECENT_TIMEFRAME) {
        recentlyDetected = false;
      }
      return recentlyDetected;
    }
};

#define DOOR_PHOTO 0
#define MOTION_PHOTO 1
doorSensor door(DOOR_PHOTO);
motionSensor motion(MOTION_PHOTO);

void loop()
{
  // Any available data indicates that the studio status program sent a request.
  if (Serial.available() > 0) {
    // The value of the data does not matter. so we just empty the input without actually reading it.
    while (Serial.available() > 0) {
      handleRequest(Serial.read());
      // Force the data to be sent right away.
      Serial.flush();
    }
  }
  door.observe();
  motion.observe();
}


#define ARDUINO_UID 16

int handleRequest(byte req) {
#define DOOR_SENSOR_REQ 4
#define MOTION_SENSOR_REQ 8
#define RELAY_CHANGE 16
#define IDENT_REQ 32
  switch (req) {
    case IDENT_REQ:
      Serial.write(ARDUINO_UID);
      break;
    case DOOR_SENSOR_REQ:
      // Send the value as unsigned byte. Studio status program reassembles them on the other end.
      // (I tried other ways of sending the data but it gets very garbled very quickly. Simply using
      // two bytes makes the handling fast and clear.)
      door.ajar() ? Serial.write(2) : Serial.write(0);
      break;
    case MOTION_SENSOR_REQ:
      motion.detected() ? Serial.write(2) : Serial.write(0);
      break;
    case RELAY_CHANGE:
      if (Serial.read() == 2) {
        digitalWrite(RELAY_PIN1, HIGH); digitalWrite(RELAY_PIN2, HIGH); digitalWrite(RELAY_PIN3, HIGH);
      } else {
        digitalWrite(RELAY_PIN1, LOW); digitalWrite(RELAY_PIN2, LOW); digitalWrite(RELAY_PIN3, LOW);
      }
      break;
    default:
      // Do nothing. I would like to have better error handling, but can't think of anything at the moment.
      break;
  }
}

