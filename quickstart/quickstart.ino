#include "rest_client.h"

/**
* Declaring the variables.
*/
unsigned int nextTime = 0;    // Next time to contact the server

String myID = "";
String response="";
int statusCode = 0;

// Sample message in JSON
String body="{\"d\":{\"cpuTemp\":20}}";

// The IBM IoT Quickstart URL
RestClient client = RestClient("quickstart.internetofthings.ibmcloud.com");

// The IBM IoT API handler
String api = "/api/v0002/device/types/particle/devices/DEVICE_ID/events/fire";

void setup() {
    Serial.begin(9600);

    // get Particle ID as unique device ID
    myID = System.deviceID();

    // Prints out the device ID over Serial
    Serial.println(myID);

    // replace DEVICE_ID with myid
    api.replace("DEVICE_ID", myID);

    // Prints out the API call
    Serial.println(api);
}

void loop() {
    if (nextTime > millis()) {
        return;
    }

    Serial.println("Application>\tStart of Loop.");

    // Set POST header
    client.setHeader("Content-Type: application/json");

    // send HTTP POST
    statusCode = client.post(api, body, &response);

    Serial.print("Status code from server: "); Serial.println(statusCode);

    // wait 10sec between messages
    nextTime = millis() + 10000;
}
