#include "config.h"
#include "functions.h"
#include "SPI.h"
#include "NRFLite.h"
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <Arduino_LSM6DS3.h>
#include "gyro.h"

int status = WL_IDLE_STATUS;
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;

unsigned int localPort = 2390;

char packetBuffer[256];
//char  ReplyBuffer[] = "acknowledged";

WiFiUDP Udp;


const static uint8_t RADIO_ID = 1;
const static uint8_t DESTINATION_RADIO_ID = 0;
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

struct RadioPacket
{
  uint8_t FromRadioId;
  // uint32_t FailedTxCount;
  double roll;
  double pitch;
  double throttle;
  double yaw;
  // double main_switch;
  double left_button;
  double right_button;
  double bottom_button;
  double gyro_roll;
  double gyro_pitch;
  double gyro_yaw;
};

NRFLite _radio;
RadioPacket _radioData;

Joy_Tune_Params params;

void setup() {
  pinMode(roll_pin, INPUT);
  pinMode(pitch_pin, INPUT);
  pinMode(throttle_pin, INPUT);
  pinMode(yaw_pin, INPUT);
  pinMode(main_switch_pin, INPUT);
  pinMode(left_button_pin, INPUT_PULLUP);
  pinMode(right_button_pin, INPUT_PULLUP);
  pinMode(bottom_button_pin, INPUT_PULLUP);

  Serial.begin(115200);
  while (!Serial);
  Serial.println("BEGIN");

  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
  {
    Serial.println("Cannot communicate with radio");
    while (1);
  }

  _radioData.FromRadioId = RADIO_ID;

  //Controller_Joy snapshot = get_joystick();
  //params = get_params(snapshot);

  Serial.println(digitalRead(main_switch_pin));
  if (digitalRead(main_switch_pin) == 0) {
    if (WiFi.status() == WL_NO_MODULE) {
      Serial.println("Communication with WiFi module failed!");
      while (true);
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
      Serial.println("Please upgrade the firmware");
    }

    if (status != WL_CONNECTED) {
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
      status = WiFi.begin(ssid, pass);
      delay(10000);
    }
    Serial.println("Connected to WiFi");
    printWifiStatus();

    Serial.println("\nStarting connection to server...");

    Udp.begin(localPort);

    //    int packetSize = Udp.parsePacket();
    //    if (packetSize) {
    //      Serial.print("Received packet of size ");
    //      Serial.println(packetSize);
    //      Serial.print("From ");
    //      remoteIp = Udp.remoteIP();
    //      Serial.print(remoteIp);
    //      Serial.print(", port ");
    //      Serial.println(Udp.remotePort());
    //      port = Udp.remotePort();
    //    }
  }
  else {
    Serial.println("PASS");
  }

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  calibrateIMU(250, 250);
  lastTime = micros();
  Serial.println("Passed calib");
}

void loop() {
  Controller_Joy data = get_full_data();

  if (readIMU()) {
    long currentTime = micros();
    lastInterval = currentTime - lastTime; // expecting this to be ~104Hz +- 4%
    lastTime = currentTime;

    doCalculations();
    printCalculations();

  }

  if (data.main_switch == 1) {
    _radioData.roll = data.roll;
    _radioData.pitch = data.pitch;
    _radioData.throttle = data.throttle;
    _radioData.yaw = data.yaw;
    // _radioData.main_switch = data.main_switch;
    _radioData.left_button = data.left_button;
    _radioData.right_button = data.right_button;
    _radioData.bottom_button = data.bottom_button;
    _radioData.gyro_roll = gyroCorrectedRoll;
    _radioData.gyro_pitch = gyroCorrectedPitch;
    _radioData.gyro_yaw = gyroCorrectedYaw;

    Serial.print(data.roll);
    Serial.print(", ");
    Serial.print(data.pitch);
    Serial.print(", ");
    Serial.print(data.throttle);
    Serial.print(", ");
    Serial.print(data.yaw);
    Serial.print(", ");
    // Serial.print(data.main_switch);
    // Serial.print(", ");
    Serial.print(data.left_button);
    Serial.print(", ");
    Serial.print(data.right_button);
    Serial.print(", ");
    Serial.print(data.bottom_button);
    Serial.print(", ");
    Serial.print(gyroCorrectedRoll);
    Serial.print(", ");
    Serial.print(gyroCorrectedPitch);
    Serial.print(", ");
    Serial.print(gyroCorrectedYaw);
    Serial.println(", ");
    

    if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData)))
    {
      Serial.println("...Success");
    }
    else
    {
      Serial.println("...Failed");
      // _radioData.FailedTxCount++;
    }

    delay(100);
  }

  if (data.main_switch == 0) {
    String message2 = String(data.roll) + ", " + String(data.pitch) + ", " + String(data.throttle) + ", " + String(data.yaw) + ", " + String(data.main_switch) + ", " + String(data.left_button) + ", " + String(data.right_button) + ", " + String(data.bottom_button) + ", " + String(complementaryRoll) + "," + String(complementaryPitch) + "," + String(complementaryYaw);
    int str_len = message2.length() + 1;

    char ReplyBuffer[str_len];

    message2.toCharArray(ReplyBuffer, str_len);

    int packetSize = Udp.parsePacket();
    if (packetSize) {
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      IPAddress remoteIp = Udp.remoteIP();
      Serial.print(remoteIp);
      Serial.print(", port ");
      Serial.println(Udp.remotePort());

      int len = Udp.read(packetBuffer, 255);
      if (len > 0) {
        packetBuffer[len] = 0;
      }
      Serial.println("Contents:");
      Serial.println(packetBuffer);

      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(ReplyBuffer);
      Udp.endPacket();
    }
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
