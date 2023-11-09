#include <keyestudio_tds.cpp>
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <ctime>
#include <iostream>
#include <HTTPClient.h>

const char* wifiSSID = "OLIN-DEVICES";
const char* wifiPassword = ""; // put ur own passcode
char jsonOutput[128];

TdsSensor myTdsSensor;
TimeHandler timeHandler(wifiSSID, wifiPassword);

void setup() {
    Serial.begin(115200);
    myTdsSensor.setup(); 
    timeHandler.connectToWiFi();
    timeHandler.initNTP();
}

void loop() {
    float currentTDS = myTdsSensor.readTDSValue();
    
    // Generate JSON
    snprintf(jsonOutput, sizeof(jsonOutput), "%s", myTdsSensor.generateTDSJson(currentTDS, timeHandler.currentDateTime()).c_str());

    // Create HTTP client instance
    HTTPClient http;

    // Your server address to handle the POST request
    const char* serverAddress = "https://jsonplaceholder.typicode.com/posts";

    // Send the POST request
    http.begin(serverAddress);
    http.addHeader("Content-Type", "application/json");

    // Post the JSON data
    int httpResponseCode = http.POST(jsonOutput);

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
    } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }

    http.end();

    delay(1000);
}
