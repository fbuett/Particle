
#include "mqtt.h"
#include "ibm_iot.h"

/*
TO DO:
 - Org and device_typefrom config file
 - self-register device - store token in EEPROM
 - Proper JSON generation
 - an application
*/

#define TEMPERATUR_INTERVAL 10*1000       // every 10 sec

#define MQTT_SERVER "us.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_PUBLISH_TOPIC "iot-2/evt/event/fmt/json"
#define MQTT_SUBSCRIBE_TOPIC "iot-2/cmd/+/fmt/json"

#define RED 101
#define GREEN 102
#define BLUE 103

char payload[256];     // holds the MQTT message payload

String clientID="";    // global clientID

void callback(char* topic, byte* payload, unsigned int length);

// set MQTT client adresse
MQTT client(MQTT_SERVER, MQTT_PORT, callback);

/*
  Set ClientID according to IBM IoT spec
*/
void setClientID(){

  clientID="d:l58oai:Photon:";

  clientID.concat(System.deviceID());

  Serial.println(clientID);
}

/*
  Get current temperatur from sensor
*/
int getTemperatur() {
  // fake implementation
  // return random value between 15-25
  return random (15,25);
}

/*
  Simple LED blink
*/
void blinkLED(const unsigned int color){
  for (int i=0; i<3; i++) {
    RGB.color(0,0,0);
    delay(200);
    switch (color) {
      case RED :
        RGB.color(255,0,0);
        break;
      case GREEN :
        RGB.color(0,255,0);
        break;
      case BLUE :
        RGB.color(0,0,255);
        break;
      default :
        RGB.color(255,0,0);
        break;
    }
    delay(200);
  }
  RGB.color(0,0,0);
}

/*
  Callback function for MQTT subscriptions
*/
void callback(char* topic, byte* payload, unsigned int length) {
    // MQTT messages received here in topic and payload
    blinkLED(BLUE);
    Serial.println("Subscription event received!");
}

/*
  Callback function for regular timer triggered messages
  i.e. keep alive, temperatur, ambient light, etc
*/
void sendTemperaturMessage()
{
  if (client.isConnected()) {

    // compile message payload in JSON format
    sprintf(payload, "{\"d\":{\"temperatur\":%d}}", getTemperatur());

    // publish MQTT message
    if (client.publish(MQTT_PUBLISH_TOPIC, payload)) {
      blinkLED(GREEN);

      Serial.print("Publish: "); Serial.println(payload);
    }
  }
}

/*
  create a software timer for regular messages
*/
Timer temperaturTimer(TEMPERATUR_INTERVAL, sendTemperaturMessage);

/*
  Called at Photon startup
*/
void setup()
{
  bool init = false;

  // take control of the Photon LED
  RGB.control(true);
  RGB.color(255,0,0);
  delay(1000);

  while (!init) {
    // Wait it out until we have a network to talk to
    if (WiFi.ready()) {
      // set clientID for MQTT connection
      setClientID();

      // connect to IBM IoT as clientID with username and token
      client.connect(clientID, AUTHORIZATION, TOKEN);

      if (client.isConnected()) {
        // set LED to GREEN
        RGB.color(0,255,0);
        init = true;
      }
      else {
        // wait 1 sec before next connection attempt
        delay(1000);
      }
  	}
  }

  // start temperatur timer
  temperaturTimer.start();

  // subscribe to MQTT commands
  client.subscribe(MQTT_SUBSCRIBE_TOPIC);
}

/*
  Main loop
*/
void loop() {

  // Listen for MQTT subscription events
  if (client.isConnected()) {
    // set LED to GREEN
    // RGB.color(0,255,0);

    // listen for MQTT subscription events
    client.loop();
  }
  else {
    // Set LED to RED
    RGB.color(255,0,0);

    // reconnect
    client.connect(clientID, AUTHORIZATION, TOKEN);
    delay(1000);

    Serial.println("Reconnect");
  }
}
