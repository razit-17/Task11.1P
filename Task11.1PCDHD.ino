#include "thingProperties.h"
#include <Servo.h>

Servo foodServo;

const int buttonPin = 3; 
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

bool feedingInProgress = false;
unsigned long feedStartTime = 0;
const unsigned long feedDuration = 8000; // 8 seconds

void setup() {
  Serial.begin(9600);
  delay(1500); 

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode(buttonPin, INPUT_PULLUP); // Button wired to GND and D3
  foodServo.attach(6);              // Servo connected to pin 6

  statusMessage = "Ready";
  ArduinoCloud.update();
}

void loop() {
  ArduinoCloud.update();

  int reading = digitalRead(buttonPin);
  

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && !feedingInProgress) {
      startFeeding();
    }
  }
  lastButtonState = reading;

  


  // -------- FEEDING CONTROL --------
  if (feedingInProgress) {
    unsigned long elapsed = millis() - feedStartTime;

    if (!feedNow && elapsed < feedDuration) {
      // Feeding cancelled early from cloud
      foodServo.write(0);
      statusMessage = "Feeding stopped early";
      feedingInProgress = false;
      ArduinoCloud.update();

      delay(2000);
      statusMessage = "Ready";
      ArduinoCloud.update();
    }
    else if (elapsed >= feedDuration) {
      // Feeding completed
      foodServo.write(0);
      feedingInProgress = false;
      statusMessage = "Done feeding";
      feedNow = false;
      ArduinoCloud.update();

      delay(2000);
      statusMessage = "Ready";
      ArduinoCloud.update();
    }
  }
}

// -------- CLOUD TRIGGER --------
void onFeedNowChange() {
  if (feedNow && !feedingInProgress) {
    startFeeding(); // Start feeding from cloud
  }
}

// -------- START FEEDING FUNCTION --------
void startFeeding() {
  feedingInProgress = true;
  feedStartTime = millis();
  feedNow = true;

  foodServo.write(90); // Start servo
  statusMessage = "Dispensing food...";
  ArduinoCloud.update();
}

void onButtonPressedChange() {}
