/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second. Increases durations by
 * 1 seconds repeatedly.
 */
#include "Arduino.h"
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include "ESPAsyncTCP.h"
#include <string>
#include "pigpio-remote/BasicIo.h"
#include "pigpio-remote/PiConnection.h"

pigpio_remote::PiConnection pi;
pigpio_remote::BasicIo io(pi);

static constexpr char* IP = "192.168.140.80";

void setup()
{
  Serial.begin(115200);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("Wifi connected...");

  auto connection_result = pi.Connect(IP);
  Serial.printf("Connection result = %d\n", connection_result);
  auto result = io.SetMode(17, pigpio_remote::GpioMode::PI_OUTPUT);
  Serial.printf("Set mode result = %d\n", static_cast<int>(result));
}

void loop()
{
  for (auto delay_duration = 1000;; delay_duration += 1000)
  {
    auto result = io.Write(17, pigpio_remote::GpioLevel::PI_ON);
    Serial.printf("gpio write result = %d\n", static_cast<int>(result));

    delay(delay_duration);

    auto result1 = io.Write(17, pigpio_remote::GpioLevel::PI_OFF);
    Serial.printf("gpio write result = %d\n", static_cast<int>(result1));

    delay(delay_duration);
    if (delay_duration >= 30000)
    {
      delay_duration = 1000;
    }
  }
}