#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoHttpClient.h>

// Declare external WiFiClient from your main sketch


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 36000, 60000); // AEST (UTC+10)
HttpClient client(wifi, "maker.ifttt.com", 80);

// IFTTT settings
const char* iftttKey = "bBUEgUsZfULUxOHUXbnoCd";
const char* iftttEvent = "pet_feeder";

// Notification flags
bool notified12 = false;
bool notified15 = false;

// Call this in setup() of main sketch
void initNotificationSystem() {
  timeClient.begin();
}

// Call this regularly in loop() of main sketch
void checkAndSendNotifications() {
  timeClient.update();

  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();

  // 12 PM notification
  if (hour == 12 && minute == 0 && !notified12) {
    sendIFTTTNotification("Time to feed your pet (12 PM)");
    notified12 = true;
  }

  // 3 PM notification
  if (hour == 15 && minute == 0 && !notified15) {
    sendIFTTTNotification("Time to feed your pet (3 PM)");
    notified15 = true;
  }

  // Reset at 12:01 AM
  if (hour == 0 && minute == 1) {
    notified12 = false;
    notified15 = false;
  }
}

// Helper function
void sendIFTTTNotification(String message) {
  String url = "/trigger/" + String(iftttEvent) + "/with/key/" + String(iftttKey);
  String payload = "{\"value1\":\"" + message + "\"}";

  client.beginRequest();
  client.post(url);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", payload.length());
  client.beginBody();
  client.print(payload);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("IFTTT Status: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
}
