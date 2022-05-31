/*
 * BORNE DE SATISFACTION - TEST DE BON FONCTIONNEMENT
 * 
 * Auteur : L. MARCHAL
 * 
 * License : CC - BY - SA
 * 
 * Date   : 02/2022
 * 
 * 
 * Version 0.1 : test de la carte mère de la borne de satisfaction :
 *                    allumage des leds
 *                    vérification de la prise en compte des boutons
 *                    présence, alimentation et mise à l'heure de la RTC
 *                    présence alimentation et fonctionnement de la carte SD (lecture / écriture)
 *                    
 *                    Lancer la console (115200 bps) et laissez vous guider.
 * 
 * 
 */

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "RTClib.h"

#define TEST_LEDS true
#define TEST_BP   true
#define TEST_RTC  true
#define TEST_SD   true

#define VERTE  0
#define VERT 0
#define JAUNE 1
#define ROUGE 2

#define ON HIGH
#define OFF LOW

#define PIN_LED_VERTE 4
#define PIN_LED_JAUNE 12
#define PIN_LED_ROUGE 14

#define PIN_BP_VERT 27
#define PIN_BP_JAUNE 26
#define PIN_BP_ROUGE 25

#define PIN_PWR_EN 13

#define TIMEOUT_TEST_BP 10000 // en ms

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};
String reponse="";
unsigned long debut=0;
bool res=false;



void setup() {
  // initialisation de la liaison USB
  Serial.begin(115200);
  delay(100);

  // initialisation des E/S
  pinMode (PIN_LED_VERTE,OUTPUT);
  pinMode (PIN_LED_JAUNE,OUTPUT);
  pinMode (PIN_LED_ROUGE,OUTPUT);
  led(VERT,OFF);
  led(JAUNE,OFF);
  led(ROUGE,OFF);
  
  pinMode (PIN_BP_VERT,INPUT);
  pinMode (PIN_BP_JAUNE,INPUT);
  pinMode (PIN_BP_ROUGE,INPUT);

  pinMode(PIN_PWR_EN,OUTPUT);
  peripheriques(OFF);
  
  // début du test
  Serial.println ("");
  Serial.println ("");
  Serial.println ("");
  Serial.println ("***************************************************************");
  Serial.println ("*****                                                     *****");
  Serial.println ("*****             BORNE DE SATISFACTION                   *****");
  Serial.println ("*****                                                     *****");
  Serial.println ("*****      Vérification du bon fonctionnement général     *****");
  Serial.println ("*****                                                     *****");
  Serial.println ("***************************************************************");
  Serial.println ("");
  Serial.println ("Début du test de la carte mère :");
  Serial.println ("");


  if (TEST_LEDS) {
    Serial.println ("");
    Serial.println ("------------------------ Test des leds ------------------------");
    Serial.println ("");
    // test de la led verte
    led(VERT,ON);
    Serial.print ("     La led verte est-elle allumée ?");
    while (!Serial.available());
    reponse = Serial.readString().substring(0,1);
    if (reponse !="O" and reponse !="o") {
      led(VERT,OFF);
      erreur("vérifiez le cablage de la led verte");
    }
    led(VERT,OFF);
    Serial.println (" => OK");
    
    // test de la led jaune
    led(JAUNE,ON);
    Serial.print ("     La led jaune est-elle allumée ?");
    while (!Serial.available());
    reponse = Serial.readString().substring(0,1);
    if (reponse !="O" and reponse !="o") {
      led(JAUNE,OFF);
      erreur("vérifiez le cablage de la led jaune");
    }
    led(JAUNE,OFF);
    Serial.println (" => OK");
     
    // test de la led rouge
    led(ROUGE,ON);
    Serial.print ("     La led rouge est-elle allumée ?");
    while (!Serial.available());
    reponse = Serial.readString().substring(0,1);
    if (reponse !="O" and reponse !="o") {
      led(ROUGE,OFF);
      erreur("vérifiez le cablage de la led rouge");
    }
    led(ROUGE,OFF);
    Serial.println (" => OK");
  }

  if (TEST_BP) {
    Serial.println ("");
    Serial.println ("------------------ Test des boutons poussoirs------------------");
    Serial.println ("");
    // test du BP vert
    Serial.print ("     Appuyez sur le bouton VERT dans les 1es "+String(TIMEOUT_TEST_BP/1000)+" secondes. ");
    debut = millis();
    res=false;
    while (millis() - debut < TIMEOUT_TEST_BP) {
      if (digitalRead(PIN_BP_VERT) == HIGH) {
        res=true;
        break;
      }
    }
    if (res) {
      Serial.println (" => OK");
    } else {
      erreur("vérifiez le cablage du bouton.");
    }
    // test du BP jaune
    Serial.print ("     Appuyez sur le bouton JAUNE dans les 1es "+String(TIMEOUT_TEST_BP/1000)+" secondes. ");
    debut = millis();
    res=false;
    while (millis() - debut < TIMEOUT_TEST_BP) {
      if (digitalRead(PIN_BP_JAUNE) == HIGH) {
        res=true;
        break;
      }
    }
    if (res) {
      Serial.println (" => OK");
    } else {
      erreur("vérifiez le cablage du bouton.");
    }
    // test du BP rouge
    Serial.print ("     Appuyez sur le bouton ROUGE dans les 1es "+String(TIMEOUT_TEST_BP/1000)+" secondes. ");
    debut = millis();
    res=false;
    while (millis() - debut < TIMEOUT_TEST_BP) {
      if (digitalRead(PIN_BP_ROUGE) == HIGH) {
        res=true;
        break;
      }
    }
    if (res) {
      Serial.println (" => OK");
    } else {
      erreur("vérifiez le cablage du bouton.");
    }
    Serial.println ("");
  }

  if (TEST_RTC) {
    Serial.println ("");
    Serial.println ("---------------------- Test de la RTC --------------------------");
    Serial.println ("");
    Serial.print ("Test de l'alimentation commandée : ");
    peripheriques(OFF);
    if (rtc.begin()) {
      erreur("l'alimentation de la RTC semble ne pas etre coupée lorsque l'ESP en fairt la demande.");
    } else {
      Serial.println ("=> OK");
      Serial.println ("");
    }
    peripheriques(ON);
    Serial.print ("     RTC Présente et alimentée : ");
    if (! rtc.begin()) {
      erreur("RTC introuvable");
    } else {
      Serial.println ("=> OK");
      Serial.println ("");
    }
    Serial.print ("     Vérification de l'heure RTC : ");
    Serial.println ("     Programmation de l'heure...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.print ("     Vérification de l'heure RTC : ");
    // When the RTC was stopped and stays connected to the battery, it has
    // to be restarted by clearing the STOP bit. Let's do this to ensure
    // the RTC is running.
    rtc.start();
    Serial.print ("     Date et heure de la RTC : ");
    // lectrue de la date et de l'heure : 
    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    Serial.println ("     L'heure et la date sont-elles correste ?");
    while (!Serial.available());
    reponse = Serial.readString().substring(0,1);
    if (reponse !="O" and reponse !="o") {
      erreur("Problème sur la RTC.");
    }
  }

 
  if (TEST_SD) {
    Serial.println ("");
    Serial.println ("---------------------- Test de la carte SD --------------------------");
    Serial.println ("");
    Serial.print ("Test de l'alimentation commandée : ");
    peripheriques(OFF);

    if(SD.begin()){
      erreur("l'alimentation de la carte SD semble ne pas etre coupée lorsque l'ESP en fairt la demande.");
    } else {
      Serial.println ("=> OK");
      Serial.println ("");
    }
    peripheriques(ON);

    if(!SD.begin()){
      erreur("Problème sur le lecteur de carte SD : carte absente, lecteur defectueux ou problème de cablage.");
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    writeFile(SD, "/hello.txt", "Hello ");
    appendFile(SD, "/hello.txt", "World!\n");
    readFile(SD, "/hello.txt");
    deleteFile(SD, "/hello.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
  }
  // extinction des périphériques
    peripheriques(OFF);

  // fin du test

  Serial.println ("");
  Serial.println ("");
  Serial.println ("                 Le test s'est déroulé avec succès");
  Serial.println ("");
  Serial.println ("");

  fin();
}


void loop () {
}

void fin() {
  Serial.println ("");
  Serial.println ("");
  Serial.println ("***************************************************************");
  Serial.println ("*****                                                     *****");
  Serial.println ("*****                     FIN DU TEST                     *****");
  Serial.println ("*****                                                     *****");
  Serial.println ("***************************************************************");
 
  while (1) delay(10);
}

void erreur(String texte) {
  Serial.println ("");
  Serial.println ("");
  Serial.println ("     ERREUR : "+ texte);
  Serial.println ("");
  Serial.println ("");
  fin();
}

void led( byte couleur, bool etat) {
  if (couleur == VERTE) digitalWrite (PIN_LED_VERTE,etat);
  if (couleur == VERT)  digitalWrite (PIN_LED_VERTE,etat);
  if (couleur == JAUNE) digitalWrite (PIN_LED_JAUNE,etat);
  if (couleur == ROUGE) digitalWrite (PIN_LED_ROUGE,etat);
}

void peripheriques(bool etat) {
  // alimentation des périphériques
  digitalWrite(PIN_PWR_EN,etat);
  delay(20); // pour laisser à l'alimentation le temps de s'établir 20 ms mini
  if (etat) {
    Serial.println ("Alimentation des périphériques ON");
  } else {
    Serial.println ("Alimentation des périphériques OFF");
  }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}
