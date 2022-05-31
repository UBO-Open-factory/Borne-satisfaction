/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-date-time-ntp-client-server-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>
#include "time.h"

const char* ssid     = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3;
const int   daylightOffset_sec = 3600;


#include "RTClib.h"
RTC_PCF8523 rtc;
#define PIN_PWR_EN 13
struct tm timeinfo;





void printLocalTime() {

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A,month=%m (%B)  day=%d  year=%Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.print("mois:");
  Serial.println(timeinfo.tm_mon + 1);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_PWR_EN, OUTPUT);  // Pin de l'autorisation de l'alimentation des périphériques
  digitalWrite(PIN_PWR_EN, HIGH);
  delay(20); // pour laisser à l'alimentation le temps de s'établir 20 ms mini


  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
  }


  /*
    // Connect to Wi-Fi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");

    // Init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();

    //disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);


    // Affichage de l'heure et la date du serveur NTP
    printLocalTime();
  */

  // Initilisaiton de la RTC avec l'heure récupéréé du serveur NTP
  // Voir référence de struct tm http://www.cplusplus.com/reference/ctime/tm/
  Serial.println("Mise à l'heure de la RTC avec le serveur NTP");

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (! rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.

    /*
        rtc.adjust(DateTime(timeinfo.tm_year,
                            timeinfo.tm_mon + 1,
                            timeinfo.tm_mday,
                            timeinfo.tm_hour + 1,
                            timeinfo.tm_min,
                            timeinfo.tm_sec));
    */
  }
  rtc.start();
}

void loop() {
  delay(1000);

  // Heure de la RTC
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print("    ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

}
