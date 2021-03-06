/**
   @example TCPClientMultiple.ino
   @brief The TCPClientMultiple demo of library WeeESP8266.
   @author Wu Pengfei<pengfei.wu@itead.cc>
   @date 2015.02

   @par Copyright:
   Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd. \n\n
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version. \n\n
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/
#include "ESP8266.h"

uint16_t baud_data[5] = {9600, 19200, 38400, 57600, 115200};

//CoreUSB UART Port: [Serial1] [D0,D1]
#if defined(__AVR_ATmega32U4__)
#define EspSerial Serial1
#define UARTSPEED  115200
#endif

//Core+ UART Port: [Serial1] [D2,D3]
#if defined(__AVR_ATmega1284P__) || defined (__AVR_ATmega644P__) || defined(__AVR_ATmega128RFA1__)
#define EspSerial Serial1
#define UARTSPEED  115200
#endif


//Core UART Port: [SoftSerial] [D2,D3]
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); /* RX:D3, TX:D2 */
#define EspSerial mySerial
#define UARTSPEED  19200
#endif


#define SSID        "Makermodule"
#define PASSWORD    "microduino"
#define HOST_NAME   "192.168.1.1"
#define HOST_PORT   (8090)

ESP8266 wifi(&EspSerial);

void setup(void)
{
  Serial.begin(115200);
  while (!Serial); // wait for Leonardo enumeration, others continue immediately
  Serial.print("setup begin\r\n");
  delay(100);

  for (int a = 0; a < 5; a++) {
    EspSerial.begin(baud_data[a]);
    delay(100);
    while (EspSerial.available() > 0) {
      EspSerial.read();
    }

    wifi.setUart(UARTSPEED, DEFAULT_PATTERN);
  }

  EspSerial.begin(UARTSPEED);
  delay(100);
  while (EspSerial.available() > 0) {
    EspSerial.read();
  }


  Serial.print("FW Version: ");
  Serial.println(wifi.getVersion().c_str());


  if (wifi.setOprToStationSoftAP()) {
    Serial.print("to station + softap ok\r\n");
  } else {
    Serial.print("to station + softap err\r\n");
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }

  if (wifi.enableMUX()) {
    Serial.print("multiple ok\r\n");
  } else {
    Serial.print("multiple err\r\n");
  }

  Serial.print("setup end\r\n");
}

void loop(void)
{
  uint8_t buffer[128] = {0};
  static uint8_t mux_id = 0;

  if (wifi.createTCP(mux_id, HOST_NAME, HOST_PORT)) {
    Serial.print("create tcp ");
    Serial.print(mux_id);
    Serial.println(" ok");
  } else {
    Serial.print("create tcp ");
    Serial.print(mux_id);
    Serial.println(" err");
  }


  char *hello = "Hello, this is client!";
  if (wifi.send(mux_id, (const uint8_t*)hello, strlen(hello))) {
    Serial.println("send ok");
  } else {
    Serial.println("send err");
  }

  uint32_t len = wifi.recv(mux_id, buffer, sizeof(buffer), 10000);
  if (len > 0) {
    Serial.print("Received:[");
    for (uint32_t i = 0; i < len; i++) {
      Serial.print((char)buffer[i]);
    }
    Serial.print("]\r\n");
  }

  if (wifi.releaseTCP(mux_id)) {
    Serial.print("release tcp ");
    Serial.print(mux_id);
    Serial.println(" ok");
  } else {
    Serial.print("release tcp ");
    Serial.print(mux_id);
    Serial.println(" err");
  }

  delay(3000);
  mux_id++;
  if (mux_id >= 5) {
    mux_id = 0;
  }
}

