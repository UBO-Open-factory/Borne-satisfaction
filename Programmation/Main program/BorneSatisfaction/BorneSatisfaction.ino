/**
 * @file BorneSatisfaction.ino
 * @brief Main programm for ESP32 micro-controller
 * @details Ce programme permet de compter le nombre d'appui sur un bouton de vote.
   - La sauvegarde de chaque vote est faite dans un fichier CSV.
   - La sauvegarde du cumul de chaque vote est faite dans un fichier TXT.
 * @date 2022-05-10
 * @author Alexandre PERETJATKO (peretjatko@univ-brest.fr)
 * 
 * @version 1.0.6
 */ // ___________________________________________________________________________________

/**
 * @def ModeDeug
 * @brief Mode prod => commenter la ligne suivante
              pour faire en sorte que le port série ne soit pas initialisé, ce qui
              permet de gagner de la vitesse d'execution au boot.
 */
//#define ModeDebug


#include "initialisation.h"
#include "RTC.h"
#include "batterie.h"
#include "carteSD.h"
#include "deepsleep.h"

// Gestion des erreurs
boolean BOO_ProblemeBatterie;
boolean BOO_ProblemeCarteSD;
boolean BOO_FichierParamsManquant;
boolean BOO_Clignote;

unsigned long LNG_Timer;


void setup() {
  // -------------------------------------------------------------------------------------------------------------
  // Mise à jour de la frequence du CPU pour une meilleur longévité de la batterie
  setCpuFrequencyMhz(80);


  // -------------------------------------------------------------------------------------------------------------
  // initialisation des clignotements des LEDs pour la gestion d'erreurs.
  //
  BOO_ProblemeBatterie      = false;
  BOO_ProblemeCarteSD       = false;
  BOO_FichierParamsManquant = false;
  BOO_Clignote = false;

  int batterieLevel =  0;

  // -------------------------------------------------------------------------------------------------------------
  // initialisation de la liaison série.
  //
#ifdef ModeDebug
  Serial.begin(115200);
  delay(1000);  // On attend que le port serie soit initialisé
  DEBUG("OK, let's go _____________________________________________________________________________");
#endif

  // -------------------------------------------------------------------------------------------------------------
  // Initialisation des PIN
  //
  pinMode(LED_ROUGE,  OUTPUT);  // Pin du bouton rouge
  pinMode(LED_VERT,   OUTPUT);  // Pin du bouton vert
  pinMode(LED_JAUNE,  OUTPUT);  // Pin du bouton jaune
  pinMode(BTN_ROUGE,  INPUT_PULLUP);  // Pin du bouton rouge
  pinMode(BTN_VERT,   INPUT_PULLUP);  // Pin du bouton vert
  pinMode(BTN_JAUNE,  INPUT_PULLUP);  // Pin du bouton jaune
  pinMode(PIN_PWR_EN, OUTPUT);  // Pin de l'autorisation de l'alimentation des périphériques


  // -------------------------------------------------------------------------------------------------------------
  // Récupération de la cause du wake up
  //
  esp_sleep_wakeup_cause_t wakeupCause = esp_sleep_get_wakeup_cause();
  DEBUG("Wake up reason : " + String(wakeupCause) );



  // -------------------------------------------------------------------------------------------------------------
  // Alimentation des périphériques
  //
  digitalWrite(PIN_PWR_EN, HIGH);
  delay(20); // pour laisser à l'alimentation le temps de s'établir 20 ms mini
  SPI.begin();


  /** -------------------------------------------------------------------------------------------------------------
      A-t-on une mise sous tension ?
     (on fait les tests de bon fonctionnements)
  */
  if (wakeupCause != ESP_SLEEP_WAKEUP_EXT0
      and wakeupCause != ESP_SLEEP_WAKEUP_EXT1
      and wakeupCause != ESP_SLEEP_WAKEUP_TIMER
      and wakeupCause != ESP_SLEEP_WAKEUP_TOUCHPAD
      and wakeupCause != ESP_SLEEP_WAKEUP_ULP) {
    
    DEBUG("mise sous tension");
    
    // .......................................................................
    // Carte SD manquante
    //
    if (!SD.begin()) {
      DEBUG("Carte SD manquante");
      BOO_ProblemeCarteSD = true;

    } else {
      // .......................................................................
      // Fichier params manquant
      // (on test l'existance du fichier de configuration).
      //
      if (! CARTESD_existeFile( fileName_Config )) {
        DEBUG("Fichier params manquant");
        BOO_ProblemeCarteSD = true;
        BOO_Clignote = true;

        CARTESD_writeConfigFile( fileName_Config );

        CARTESD_EraseROMSynthese("question");

      } else {
        String siteID, question = "";
        if ( ! CARTESD_readConfigFile( fileName_Config, siteID, question ) ) {
          
          // Il y a eu un problème de lecture de fichier de config sur la carte SD
          DEBUG("Il y a eu un problème de lecture de variables dans le fichier de config sur la carte SD");

          CARTESD_writeConfigFile( fileName_Config );
          CARTESD_EraseROMSynthese("question");

          BOO_ProblemeCarteSD = true;
          BOO_Clignote = true;
        } else {

          // .......................................................................
          // on réinitialise les synthèses si la question a changée
          String question = "";
          if ( CARTESD_questionChange(question) ) {
            // Efface littelFS
            // et
            // Reinitialise les fichiers de synthese et de la question dans le LittelFS
            CARTESD_EraseROMSynthese(question);
            DEBUG("Reinitialisation des synthèses");

          } else {
            DEBUG("pas de changement dans la question");
          }

          // Lecture de la date et l'heure
          DEBUG("Lecture de la date et l'heure");
          bool erreurFormat = false;
          if ( CARTESD_updateDate(fileName_Config, erreurFormat)) {
            // initialisation de la RTC à partir du fichier de paramétrage
            DEBUG("initialisation de la RTC à partir du fichier de paramétrage");
            for ( int i = 0; i < 3; i++) {
              digitalWrite(LED_VERT, LOW);
              delay(DelayExtinctionLEDs/3);
              digitalWrite(LED_VERT, HIGH);
              delay(DelayExtinctionLEDs/3);
            }
          } else {
            if( erreurFormat ){
              DEBUG("Erreur de format de date ou d'heure dans '"+ String(fileName_Config) +"' à l'initialisation de la RTC");
              for ( int i = 0; i < 3; i++) {
                digitalWrite(LED_JAUNE, HIGH);
                delay(DelayExtinctionLEDs);
                digitalWrite(LED_JAUNE, LOW);
                delay(DelayExtinctionLEDs);
              }
            }

          }

          // Extinction LEDs
          digitalWrite(LED_VERT, LOW);
          digitalWrite(LED_JAUNE, LOW);




          // .......................................................................
          // Batterie faible
          //
          batterieLevel = BATTERIE_getBatterieLevel();
          if (batterieLevel < 20) {
            DEBUG("Batterie faible");
            BOO_ProblemeBatterie = true;

            if (batterieLevel < 5) {
              DEBUG("Batterie Tres faible");
              BOO_Clignote = true;
            }


          } else {

            // .......................................................................
            // Tout est ok
            //
            DEBUG("Tout est ok");
            // Allumage LED verte
            digitalWrite(LED_VERT, HIGH);
            delay( DelayExtinctionLEDs);


            // Deep sleep
            DEEPSLEEP_start();
          }
        }
      }
    }
  } else {
    /** -------------------------------------------------------------------------------------------------------------
       On vient de réveiller la borne en appuyant sur un des boutons
    */
    // .......................................................................
    //  Allumage de la LEDS correspondante au bouton appuyé
    int gpio = DEEPSLEEP_getGPIOWakeUp();
    int reponseVert  = 0;
    int reponseRouge = 0;
    int reponseJaune = 0;

    switch ( gpio ) {
      case BTN_VERT:
        DEBUG("Bouton vert");
        digitalWrite(LED_VERT,  HIGH);
        reponseVert = 1;
        break;

      case BTN_ROUGE:
        DEBUG("Bouton rouge");
        digitalWrite(LED_ROUGE,  HIGH);
        reponseRouge = 1;
        break;

      case BTN_JAUNE:
        DEBUG("Bouton jaune");
        digitalWrite(LED_JAUNE,  HIGH);
        reponseJaune = 1;
        break;
      default:
        DEBUG("Le GPIO " + String(gpio) + " a réveillé l'ESP32 : cas non traité (GPIO inconnu)");
    }


    // .......................................................................
    //  Lecture de la date et l'heure
    RTC_setTime();
    String date  = RTC_getDate();   // "1971/01/27";
    String heure = RTC_getTime();  // "14:06:24";



    // .......................................................................
    //  Lecture du niveau de la batterie
    batterieLevel = BATTERIE_getBatterieLevel();


    // .......................................................................
    //  Lecture du fichier de configuration
    String siteID, question = "";
    if ( CARTESD_readConfigFile( fileName_Config, siteID, question ) ) { // Initialise siteID et question
      DEBUG(siteID);
      DEBUG(question);


      // .......................................................................
      //  Ecriture dans le fichier des mesures
      String ligneFichierMesure = siteID + ";" + date + ";" + heure + ";" + question + ";"
                                  + String(reponseVert) + ";" + String(reponseRouge) + ";" + String(reponseJaune) + ";"
                                  + String(batterieLevel);
      CARTESD_appendFileMesure(date, postfixeFileMesures, ligneFichierMesure );


      // .......................................................................
      //  Mise à jour de la synthèse des votes
      CARTESD_miseAJourSynthese(postfixeFileSynthese, reponseRouge, reponseVert, reponseJaune, batterieLevel, date, question, siteID);
      delay( DelayExtinctionLEDs );


      // .......................................................................
      //  Extinction des LEDs
      digitalWrite(LED_ROUGE, LOW);
      digitalWrite(LED_VERT,  LOW);
      digitalWrite(LED_JAUNE, LOW);


      // .......................................................................
      // Deep sleep
      DEEPSLEEP_start();


    } else {
      // Lecture impossible du fichier de paramétrage
      DEBUG("Lecture impossible du fichier de paramétrage : Carte SD arrachée ?");
      BOO_ProblemeCarteSD = true;
    }
  }

  // Timer pour le clignottement des leds
  LNG_Timer = millis();
}



/**
 * ****************************************************************************************************************
 * ****************************************************************************************************************
   Boucle principale.
   On n'entre à l'interieur que pour la gestion des erreurs au démarrage du boitier de vote.
*/
void loop() {

  // SI LE TIMER EST DÉPASSÉ .............................................................
  if( LNG_Timer + DelayExtinctionLEDs < millis() ){
    // Allumage/Extinction led ROUGE
    if ( BOO_ProblemeBatterie ) {
      DEBUG("ProblemeBatterie -> JAUNE");
      if( BOO_Clignote ) {
        digitalWrite(LED_JAUNE, ! digitalRead(LED_JAUNE) );
        DEBUG("Clignotement JAUNE");
      } else {
        digitalWrite(LED_JAUNE, HIGH );
      }
    }

    // Allumage/Extinction  led JAUNE
    if ( BOO_ProblemeCarteSD ) {
      DEBUG("ProblemeCarteSD -> ROUGE");
      if( BOO_Clignote ) {
        digitalWrite(LED_ROUGE, ! digitalRead(LED_ROUGE) );
        DEBUG("Clignottement ROUGE");
      } else {
        digitalWrite(LED_ROUGE, HIGH );

      }
    }
    // Réinitilaisation du timer
    LNG_Timer = millis();
  }


  // SI ON APPUI SUR UN BOUTON ...........................................................
  if( digitalRead(BTN_JAUNE) == HIGH or digitalRead(BTN_ROUGE) == HIGH or digitalRead( BTN_VERT) == HIGH ){
    DEBUG("Alarme acquitée. On reboot");

    delay(DelayExtinctionLEDs);

    // Deep sleep
    DEEPSLEEP_start();
  }
}
