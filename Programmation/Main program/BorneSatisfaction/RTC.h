/**
 * @file RTC.h
 * @brief contains all function for RTC purpose.
 * @date 2022-05-10
   @author Alexandre PERETJATKO
 * 
 * @version 1.0.2
 */

/*
  capteur RTC pcf8523
  Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
*/
#include <RTClib.h>
RTC_PCF8523 rtc;
DateTime now;



/**
  ----------------------------------------------------------------------------------
  @brief Initialise "now" avec la RTC
  */
String RTC_setTime() {
  rtc.begin();
  rtc.start();

  now = rtc.now();
  /*
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    //Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    Serial.println();
  */
  return String(now.unixtime());
}


   


/**
  ----------------------------------------------------------------------------------
 * @brief Renvoie la date sous forme de chaine de caractères.
 * 
 * @return String 
  */
String RTC_getDate() {
  rtc.begin();
  rtc.start();

  DateTime now = rtc.now();
  char jour[3]; sprintf(jour, "%02i", now.day());
  char mois[3]; sprintf(mois, "%02i", now.month());

  return String(now.year() ) + "/" + String(mois) + "/" + String(jour);
}


/** --------------------------------------------------------------------------------------
 * @brief    Renvoie l'heure sous forme de chaine de caractères.
 * 
 * @return String 
 */
String RTC_getTime() {
  rtc.begin();
  rtc.start();

  DateTime now = rtc.now();
  char hour[3]; sprintf(hour, "%02i", now.hour());
  char minute[3]; sprintf(minute, "%02i", now.minute());
  char second[3]; sprintf(second, "%02i", now.second());
 
  return String(hour) + ":" + String(minute) + ":" + String(second);
}

/** --------------------------------------------------------------------------------------
 * @brief Renvoie l'année courant à partir de la RTC.
 * 
 * @return String 
 */
String RTC_getYear() {
  rtc.begin();
  rtc.start();

  DateTime now = rtc.now();
  return String(now.year());
}

/** --------------------------------------------------------------------------------------
 * @brief Renvoie le mois courant à partir de la RTC.
 * 
 * @return String 
 */
String RTC_getMonth() {
  rtc.begin();
  rtc.start();

  DateTime now = rtc.now();
  char mois[3]; sprintf(mois, "%02i", now.month());

  return String(mois);
}

/** --------------------------------------------------------------------------------------
 * @brief Renvoie le jour courant à partir de la RTC.
 * 
 * @return String 
 */
String RTC_getDay() {
  rtc.begin();
  rtc.start();

  DateTime now = rtc.now();
  char jour[3]; sprintf(jour, "%02i", now.day());

  return String(jour);
}
/** --------------------------------------------------------------------------------------
 * @brief Renvoie l'heure courante à partir de la RTC.
 * 
 * @return String 
 */
String RTC_getHour() {
  rtc.begin();
  rtc.start();

  DateTime now = rtc.now();
  char hour[3]; sprintf(hour, "%02i", now.hour());
 
  return String(hour);
}
/** --------------------------------------------------------------------------------------
 * @brief Renvoie l'heure courante à partir de la RTC.
 * 
 * @return String 
 */
String RTC_getMinute() {
  rtc.begin();
  rtc.start();

  DateTime now = rtc.now();
  char minute[3]; sprintf(minute, "%02i", now.minute());
 
  return String(minute);
}
