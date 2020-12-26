/**
 * Blink
 *
 * Turns on and off all the outputs of an I2C port extender PCF8574.
 * State is changed every 2 seconds.
 */
#include "Arduino.h"
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include "ESPAsyncTCP.h"
#include <string>
#include "pigpio-remote/I2cDevice.h"
#include "pigpio-remote/PiConnection.h"

pigpio_remote::PiConnection pi;
pigpio_remote::I2cDevice device(pi);

static constexpr const char *const IP = "192.168.140.80";
static constexpr uint8_t I2C_ADDRESS = 0x21;

static uint8_t data = 0;

void setup()
{
  Serial.begin(115200);

  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("Wifi connected...");

  auto connection_result = pi.Connect(IP);

  if (connection_result == pigpio_remote::ConnectionError::SUCCESS)
  {
    Serial.println("Successfully connected.");
  }
  else
  {
    Serial.printf("Connection failed with error code %d.\n", static_cast<int>(connection_result));
    return;
  }

  auto open_result = device.Open(I2C_ADDRESS);
  if (open_result == pigpio_remote::PigpioError::PI_OK)
  {
    Serial.printf("Opened I2C connection to device on address %x.\n", static_cast<int>(device.GetAddress()));
  }
  else
  {
    Serial.printf("Could not open I2C connection. Error code: %d\n", static_cast<int>(open_result));
    pi.Stop();
    return;
  }
}

void loop()
{
  data ^= 0xFFU;
  auto write_result = device.WriteByte(data);
  if (write_result == pigpio_remote::PigpioError::PI_OK)
  {
    Serial.printf("Wrote %d successfully.\n", static_cast<int>(data));
  }
  else
  {
    Serial.printf("Write failed with error code %d.\n", static_cast<int>(write_result));
  }
  delay(2000);
}