/**
 * @file initialisation.h
 * @brief Every globale variables are initiate here.
 * 
 * @author Alexandre PERETJATKO
 */

// Definition des noms des fichiers
// La synthese et les mesuress seront modifiés avec la date courante.

/** Config file name */
char *fileName_Config       = "/configuration.ini";

/** End of syntheses file name (Date will be postfix */
char *postfixeFileSynthese  = "_recap.txt";

/** End of detail file name (Date will be postfix */
char *postfixeFileMesures   = "_detail.csv";



// Brochage des PIN
#define LED_VERT  4
#define LED_JAUNE 12
#define LED_ROUGE 14

#define BTN_ROUGE 25
#define BTN_JAUNE 26
#define BTN_VERT  27

#define PIN_PWR_EN 13


/** Extinction delay for LEDs when the box is turn ON and there are some error messages */
const int DelayExtinctionLEDs = 1000;

/**
 * @brief Macro d'affichage de chaines (compilée seulement en mode debug)
 * Le mode débug est mis lorsque \b ModeDebug est défini dans BorneSatisfaction.ino
 * @see BorneSatisfaction.ino
 * @def DEBUG
 */
#ifdef ModeDebug
#define DEBUG(message) \
  Serial.print("[DEBUG:"); \
  Serial.print(__func__); \
  Serial.print("("); \
  Serial.print(__LINE__); \
  Serial.print(")]-> "); \
  Serial.println(message);
#else
#define DEBUG(message);
#endif