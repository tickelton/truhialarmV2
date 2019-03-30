/*
 * Copyright (c) 2019 tick <tickelton@gmail.com>
 *
 * SPDX-License-Identifier:	ISC
 */

#include <ESP8266WiFi.h>

// Types
enum lid_state {
  LID_OPEN = HIGH,
  LID_CLOSED = LOW,
};

enum pin_numbers {
  GPIO0 = 0,
  PIN_TX = 1,
  GPIO2 = 2,
  PIN_RX = 3,
};

// Constants
const uint8_t SENSOR_PIN = PIN_RX;  // Use RX pin as sensor input
const uint8_t MAIL_MAX_RETRIES = 5;
const char* const WIFI_SSID = "xxxx"; // Replace with correct SSID
const char* const WIFI_PASSWORD = "xxxx"; // Replace with WiFi password
const char* const SMTP_SERVER = "mail.example.com"; // Replace with SMTP server
const uint16_t SMTP_PORT = 25; // Replace with correct port
const uint16_t RECONNECT_INTERVAL = 10000;
const uint32_t NOTIFICATION_DELAY = 5 * 60 * 1000; // Wait 5 minutes before sending the notification
const uint16_t MAIN_LOOP_DELAY = 1000; // Run the main loop every second

// Global variables
bool sensor_irq = false;
uint8_t wifi_connection_state = 0;
WiFiClient espClient;

// Interrupt handler
void sensorEvent() {
  sensor_irq = true;
}

// Functions
uint8_t WiFiConnect()
{
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  for (uint8_t i = 0; i < 50; ++i) {
    if (WiFi.status() != WL_CONNECTED)
    {
      delay(200);
      Serial.print(".");
    }
  }

  Serial.println("");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection TIMEOUT");
    Serial.println("Check if access point available or SSID and Password");
    return false;
  }

  Serial.println("Connection: ESTABLISHED");
  Serial.print("Got IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

void Awaits()
{
  uint32_t ts = millis();
  while (!wifi_connection_state)
  {
    delay(50);
    if (millis() > (ts + RECONNECT_INTERVAL) && !wifi_connection_state) {
      wifi_connection_state = WiFiConnect();
      ts = millis();
    }
  }
}

byte sendEmail()
{
  if (espClient.connect(SMTP_SERVER, SMTP_PORT) == 1)
  {
    Serial.println("connected");
  }
  else
  {
    Serial.println("connection failed");
    return 0;
  }
  if (!emailResp())
    return 0;

  Serial.println("Sending EHLO");
  espClient.println("EHLO www.example.com");
  if (!emailResp())
    return 0;

  Serial.println("Sending auth login");
  espClient.println("AUTH LOGIN");
  if (!emailResp())
    return 0;

  Serial.println("Sending User");
  espClient.println("dXNlcg=="); //base64, ASCII encoded Username: 'user'
  if (!emailResp())
    return 0;

  Serial.println("Sending Password");
  espClient.println("cGFzc3dvcmQ=");  //base64, ASCII encoded Password: 'password'
  if (!emailResp())
    return 0;

  Serial.println("Sending From");
  espClient.println("MAIL From: sender@example.com"); // Sender email address
  if (!emailResp())
    return 0;

  Serial.println("Sending To");
  espClient.println("RCPT To: receiver1@example.com"); // recipient email address (envelope-to)
  espClient.println("RCPT To: receiver2@example.com"); // repeat for multiple recipients
  if (!emailResp())
    return 0;

  Serial.println("Sending DATA");
  espClient.println("DATA");
  if (!emailResp())
    return 0;

  Serial.println("Sending email");
  espClient.println("To:  <receiver1@example.com>, <receiver2@example.com>"); // sender and recipient addresses as they appear in the email
  espClient.println("From: sender@example.com");
  espClient.println("Subject: Subject text");
  espClient.println("");
  espClient.println("");
  espClient.println("Example");
  espClient.println("");
  espClient.println("Email");
  espClient.println("");
  espClient.println("Text");

  espClient.println(".");
  if (!emailResp())
    return 0;

  Serial.println("Sending QUIT");
  espClient.println("QUIT");
  if (!emailResp())
    return 0;

  espClient.stop();
  Serial.println("disconnected");
  return 1;
}

byte emailResp()
{
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  // Abort if no response was received within 20 seconds.
  while (!espClient.available())
  {
    delay(1);
    loopCount++;
    if (loopCount > 20000)
    {
      espClient.stop();
      Serial.println("Timeout");
      return 0;
    }
  }

  responseCode = espClient.peek();
  while (espClient.available())
  {
    readByte = espClient.read();
    Serial.write(readByte);
  }

  if (responseCode >= '4')
  {
    return 0;
  }
  return 1;
}

void send_notification() {
  uint8_t mail_retries = 0;

  Serial.println("Sending notification.");
  wifi_connection_state = WiFiConnect();
  if (!wifi_connection_state) { // if not connected to WIFI try to (re)connect
    Awaits();
  }

  while (mail_retries < MAIL_MAX_RETRIES) {
    Serial.println("Sending email");
    if (sendEmail()) {
      break;
    }
    mail_retries++;
  }

}

void setup() {
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

  Serial.println("Setup start");
  delay(100);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(
    digitalPinToInterrupt(SENSOR_PIN),
    sensorEvent,
    CHANGE
  );

  Serial.println("Setup done");
}

void loop() {
  if (sensor_irq) {
    delay(100); // 100ms debounce
    if (digitalRead(SENSOR_PIN) == LID_OPEN) {
      Serial.println("Lid was opened.");
    } else {
      Serial.println("Lid was closed.");
      delay(NOTIFICATION_DELAY);
      send_notification();
    }
    sensor_irq = false;
  }

  delay(MAIN_LOOP_DELAY);
}
