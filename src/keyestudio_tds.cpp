#include <ArduinoJson.h>
#include <Arduino.h>
#include <iostream>
#include <string>
#include <datetime.cpp>

#define TdsSensorPin A1
#define VREF 5.0 // Analog reference voltage (Volt) of the ADC
#define SCOUNT 30 // Sum of sample point

class TdsSensor {
    public:
        int analogBuffer[SCOUNT];
        int analogBufferTemp[SCOUNT];
        int analogBufferIndex = 0, copyIndex = 0;
        float averageVoltage = 0, tdsValue = 0, temperature = 25;

    void setup() {
        Serial.begin(115200);
        pinMode(TdsSensorPin, INPUT);
    }

    float readTDSValue() {
        static unsigned long analogSampleTimepoint = millis();

        if (millis() - analogSampleTimepoint > 40U) {
            analogSampleTimepoint = millis();
            analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);
            analogBufferIndex++;

            if (analogBufferIndex == SCOUNT)
                analogBufferIndex = 0;
        }

        static unsigned long printTimepoint = millis();

        if (millis() - printTimepoint > 800U) {
            printTimepoint = millis();
            for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
                analogBufferTemp[copyIndex] = analogBuffer[copyIndex];

            averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0;

            float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
            float compensationVoltage = averageVoltage / compensationCoefficient;

            tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;

            return tdsValue;
        }

        return 0; // Return 0 if the TDS value is not ready yet
    }

    int getMedianNum(int bArray[], int iFilterLen) {
        int bTab[iFilterLen];

        for (byte i = 0; i < iFilterLen; i++)
            bTab[i] = bArray[i];

        int i, j, bTemp;

        for (j = 0; j < iFilterLen - 1; j++) {
            for (i = 0; i < iFilterLen - j - 1; i++) {
                if (bTab[i] > bTab[i + 1]) {
                    bTemp = bTab[i];
                    bTab[i] = bTab[i + 1];
                    bTab[i + 1] = bTemp;
                }
            }
        }

        if ((iFilterLen & 1) > 0)
            bTemp = bTab[(iFilterLen - 1) / 2];
        else
            bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;

        return bTemp;
    }


    String generateTDSJson(float tdsValue, String currentTime) {
        StaticJsonDocument<200> doc;

        doc["_id"] = "garden-id";
        doc["sensor_id"] = "tds-sensor-id";
        doc["value"] = tdsValue;
        doc["created_at"] = "idk what this is";
        doc["updated_at"] = currentTime;

        String jsonString;
        serializeJson(doc, jsonString);

        return jsonString;
    }
};

