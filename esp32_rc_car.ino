/*
  NeonRC ESP32 Vehicle Core
  -------------------------
  This code turns your ESP32 into a WiFi Access Point and runs a web server
  to handle HTTP commands from the NeonRC Cockpit web app.

  Hardware Connections:
  - Motor Driver (L298N / ESC):
    - ENA (Speed): GPIO 25 (PWM)
    - IN1 (Forward): GPIO 26
    - IN2 (Backward): GPIO 27
  - Steering Servo:
    - Signal: GPIO 14 (PWM)
  - Battery: 7.4V LiPo connected to VIN/GND (via voltage regulator if needed)
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// --- CONFIGURATION ---
const char* ssid = "NeonRC_Car";
const char* password = "password123"; // Optional

// Pin Definitions
const int MOTOR_ENA = 25;
const int MOTOR_IN1 = 26;
const int MOTOR_IN2 = 27;
const int SERVO_PIN = 14;

// PWM Settings
const int PWM_FREQ = 5000;
const int PWM_RES = 8; // 0-255
const int MOTOR_SPEED = 200; // Default speed (0-255)

// Servo Settings
Servo steeringServo;
const int SERVO_CENTER = 90;
const int SERVO_LEFT = 60;
const int SERVO_RIGHT = 120;

WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "NeonRC Core Active");
}

void handleForward() {
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  ledcWrite(0, MOTOR_SPEED);
  server.send(200, "text/plain", "Moving Forward");
}

void handleReverse() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  ledcWrite(0, MOTOR_SPEED);
  server.send(200, "text/plain", "Moving Backward");
}

void handleStop() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  ledcWrite(0, 0);
  server.send(200, "text/plain", "Stopped");
}

void handleLeft() {
  steeringServo.write(SERVO_LEFT);
  server.send(200, "text/plain", "Turning Left");
}

void handleRight() {
  steeringServo.write(SERVO_RIGHT);
  server.send(200, "text/plain", "Turning Right");
}

void handleCenter() {
  steeringServo.write(SERVO_CENTER);
  server.send(200, "text/plain", "Centering Steering");
}

void setup() {
  Serial.begin(115200);

  // Motor Pins
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);

  // Setup PWM for Motor Speed
  ledcSetup(0, PWM_FREQ, PWM_RES);
  ledcAttachPin(MOTOR_ENA, 0);

  // Servo Setup
  steeringServo.attach(SERVO_PIN);
  steeringServo.write(SERVO_CENTER);

  // WiFi Setup (Access Point Mode)
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Routes
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/reverse", handleReverse);
  server.on("/stop", handleStop);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/center", handleCenter);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
