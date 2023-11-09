#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

class TimeHandler {
private:
    const char* ssid;
    const char* password;
    const char* ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = -5 * 3600;  // GMT offset in seconds
    const int daylightOffset_sec = 3600;  // Daylight offset in seconds
    WiFiUDP ntpUDP;
    NTPClient timeClient;

public:
    TimeHandler(const char* ssid, const char* password)
        : ssid(ssid), password(password), timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec) {}

    void connectToWiFi() {
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("Connecting to WiFi..");
        }
        Serial.println("Connected to WiFi");
    }

    void initNTP() {
        timeClient.begin();
        timeClient.update();
    }

    String currentDateTime() {
        timeClient.update();
        time_t currentTime = timeClient.getEpochTime();
        struct tm localTime;
        char buffer[80];
        localtime_r(&currentTime, &localTime);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
        return String(buffer);
    }
};