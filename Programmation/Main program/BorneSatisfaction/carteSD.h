/**
 * @file carteSD.h
 * @brief contains all function for SD card purpose.
 * @date 2022-05-10
   @author Alexandre PERETJATKO
 * 
 * @version 1.0.2
 */
#include <SPI.h>
#include <IniFile.h>

#include "FS.h"
#include <LITTLEFS.h>

//Nom du fichier de synthese sur littelFS
char l_CHAR_fileNameSynthese[15] = "/question.txt";






/**
 * @brief Diaply error message (only in debug mode) from SD card driver.
 * @return String : error message.
 */
#ifdef ModeDebug
String _printErrorMessage(uint8_t e, bool eol = true){
  switch (e) {
  case IniFile::errorNoError:
    return "no error";
    break;
  case IniFile::errorFileNotFound:
    return "file not found";
    break;
  case IniFile::errorFileNotOpen:
    return "file not open";
    break;
  case IniFile::errorBufferTooSmall:
    return "buffer too small";
    break;
  case IniFile::errorSeekError:
    return "seek error";
    break;
  case IniFile::errorSectionNotFound:
    return"section not found";
    break;
  case IniFile::errorKeyNotFound:
    return"key not found";
    break;
  case IniFile::errorEndOfFile:
    return"end of file";
    break;
  case IniFile::errorUnknownError:
    return"unknown error";
    break;
  default:
    return"unknown error value";
    break;
  }
}

/**
  ----------------------------------------------------------------------------------
 * @brief Fait clignoter une leds un certain nombre de fois (only for debug mode)
 * 
 * @param nb int le nombre de fois que l'on veut un allumage de la LED ledPIN
 */
void _clignoteLED(unsigned int nb, unsigned int ledPin){
  for( int i= 1; i <= nb; i++){
    digitalWrite(ledPin, HIGH );
    delay( 250 );
    digitalWrite(ledPin, LOW );
    delay( 250 );
  }
  digitalWrite(ledPin, LOW );
  delay( 1000 );
}
#else
void _clignoteLED(unsigned int nb, unsigned int ledPin){
}
#endif
   
/**
   ----------------------------------------------------------------------------------
   @brief Effacement des fichiers en ROM. (Formattage du file system)
   @params:
    STRING question : la question à écrire dans littelFS
   ----------------------------------------------------------------------------------*/
void CARTESD_EraseROMSynthese(String question) {
  if (LITTLEFS.begin(true)) {
    if ( LITTLEFS.format() )
      DEBUG("Formattage du file system en ROM");

    // Stock la nouvelle question sur le LittleFS
    File file = LITTLEFS.open(l_CHAR_fileNameSynthese, FILE_WRITE);
    if ( file ) {
      file.print(question);
      DEBUG("Ecriture de '" + question + "' dans le fichier <" + String(l_CHAR_fileNameSynthese) + "> sur littleFS");
    }
    file.close();

  } else {
    DEBUG("Impossible de formatter le file system en ROM");
  }
}


/** --------------------------------------------------------------------------------------
 * @brief    Lecture du fichier de configuration.
 * 
 * @param fileName le nom du fichier de conf
 * @param p_STR_siteID  l'ID sdu site
 * @param p_STR_question la question du sondage 
 * @return boolean false : initialisation impossible.
          String siteID:    le Site ID du boitier de vote
          String question:  la question écrite dans le fichier de configuration.
------------------------------------------------------------------------------------------ */
boolean CARTESD_readConfigFile(const char *fileName, String &p_STR_siteID, String &p_STR_question) {

  // test de la carte SD
  if (!SD.begin())
    DEBUG("SD.begin() failed");


  // Si on ne peut pas ouvrir le fichier de configuration
  IniFile ini(fileName);
  if (!ini.open()) {
    DEBUG("Failed to open <" + String(fileName) + "> for reading");

    // On ne fait rien d'autre
    return false;
  }


  // Check if the file is valid. This can be used to warn if any lines are longer than the buffer.
  const size_t bufferLen = 500;
  char buffer[bufferLen];
  if (!ini.validate(buffer, bufferLen)) {
    DEBUG("ini file " + String(ini.getFilename()) + " not valid");

    // On ne fait rien d'autre
    return false;
  }


  // Lecture du siteID
  if (ini.getValue("config", "siteID", buffer, bufferLen)) {
    p_STR_siteID = buffer;

  } else {
    DEBUG("Could not read 'siteID' from section 'config'");
    return false;
  }

  // Lecture de la question
  if (ini.getValue("config", "question", buffer, bufferLen)) {
    p_STR_question = buffer;

  } else {
    DEBUG("Could not read 'question' from section 'config'");
    return false;
  }
  return true;
}




/** --------------------------------------------------------------------------------------
   @brief test l'existance d'un fichier.
      Ex : CARTESD_existeFile("/hello.txt");

   @param fileName : Le nom du fichier à tester.
   @return boolean -> TRUE si le fichier existe.
  ---------------------------------------------------------------------------------------- */
boolean CARTESD_existeFile(const char *fileName) {
  File file = SD.open(fileName);
  if (!file) {
    DEBUG("File <" + String(fileName) + "> doesn't exist");
    return false;
  } else {
    return true;
  }
}

/**
   ----------------------------------------------------------------------------------
   Renvoie une chaine aléatoire avec nbCaract caractères.
  ---------------------------------------------------------------------------------- */
String _getRandomChar(int nbCaract) {
  char caracts[38] = "AZERTYUIOPQSDFGHJKLMWXCVBN 0123456789";
  char Chaine[nbCaract];
  for (int i = 0; i < nbCaract; i++) {
    Chaine[i] = caracts[random(0, 38)];
  }

  // Ajout du caractère de fin de chaine
  Chaine[nbCaract] = '\0';

  return String(Chaine);
}



/**
   ---------------------------------------------------------------------------------------
    @brief Ecriture d'un fichier de configuration.
  Ex : CARTESD_existeFile("/config.ini");

    @param fileName Le nom du fichier de parametrage
    @param siteID    l'ID du site (random par defaut)
    @param question  la question en toutes lettres
  ---------------------------------------------------------------------------------------- */
void CARTESD_writeConfigFile(const char *fileName, String siteID = "", String question = "") {

  DEBUG("Ecriture du fichier de configuration: " + String(fileName));

  // Ouverture du fichier en écriture (et réinitialisation)
  File myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {
    myFile.println("# Ceci est le fichier de configuration pour l'application 'Compteur de vote'.");
    myFile.println("# Ne modifiez pas le nom des variables (ce qui se trouve avant le signe égal sur une ligne).");
    myFile.println("# Vous pouvez modifier le texte se trouvant après le signe égal à votre guise.");
    myFile.println("#____________________________________________________________________________________________");
    myFile.println("[config]");
    myFile.println("");

    myFile.println("# Le siteID est votre identifiant comme il vous a été donné par l'enquêteur,");
    myFile.println("# ne le modifiez pas s'il ne vous le demande pas.");
    if ( siteID == "") {
      myFile.println("siteID=Cool Food " + _getRandomChar(10));

    } else {
      myFile.println("siteID=" + siteID );
    }
    myFile.println("");

    myFile.println("# Cette phrase apparaîtra dans le fichier résultat à votre questionnaire mais n'est pas visible sur la borne.");
    myFile.println("# Il est conseillé de définir ici une phrase courte.");
    if ( question == "") {
      myFile.println("question= Aimez-vous les brocolis ?");
    } else {
      myFile.println("question=" + question);
    }

    myFile.println("");

    myFile.println("# Pour faire la mise à l'heure, décommentez (supprimez le # en début de ligne) et ");
    myFile.println("# remplissez les champs suivant avec des valeurs numériques uniquement.");
    myFile.println("# Une fois la mise à l'heure faite (au démarrage de la borne), ces 5 lignes seront automatiquement commentées.");
    myFile.println("#year=2022");
    myFile.println("#month=05");
    myFile.println("#day=11");
    myFile.println("#hour=14");
    myFile.println("#minute=56");

    myFile.close();
  } else {
    DEBUG("Impossible d'ouvrir le fichier '" + String(fileName) + "' sur la carte SD pour écrire dedans");
  }
}


  /**
   ---------------------------------------------------------------------------------------
   * @brief    Ajout d'une ligne à la fin du fichier des mesures.
   * 
   * @param date la date courante sous la forme 1971/01/27
   * @param path le chemin où sauver le fichier
   * @param message la chaine de caractères à enregistrer dans le fichier.
  ----------------------------------------------------------------------------------------*/
void CARTESD_appendFileMesure(String date, const char *path, String message) {

  // Construction du nom du fichier avec la date en préfixe
  date.replace("/", "-");  // remplacment des / par des - dans la date
  String fileName = "/" + date + String(path);

  // Convertion du nom du fichier en Char
  char charFileName[25];
  fileName.toCharArray(charFileName, 25);

  // si le fichier n'existe pas
  if (!CARTESD_existeFile(charFileName)) {
    DEBUG("Le fichier " + fileName + " n'existe pas, on l'initialise");

    // Ajoute la ligne d'entête des colonnes
    message = "Identifiant du site;Date;Heure;Question;Vert;Rouge;Jaune;Niveau Batterie\n" + message;
    DEBUG(message);
  }


  // ajout de la ligne à la fin du fichier
  File file = SD.open(fileName, FILE_APPEND);
  if (!file) {
    DEBUG("Failed to open <" + String(path) + "> for writing");
    return;
  }
  if (file.println(message)) {
    DEBUG("Message appended :" + String(message));
  } else {
    DEBUG("Append failed");
  }
  file.close();
}




/**
   ----------------------------------------------------------------------------------
   Mise à jour du fichier de synthese des votes.
   ----------------------------------------------------------------------------------*/
void CARTESD_miseAJourSynthese(const char *path, int rouge, int vert, int jaune, int batterieLevel, String date, String question, String siteID) {

  // Remplacement des / par des - dans la date
  String l_STR_Date = date;
  l_STR_Date.replace("/", "-");
  char l_CHAR_Date[15];
  l_STR_Date.toCharArray(l_CHAR_Date, 15);

  // Construction du nom du fichier avec la date en préfixe
  char l_CHAR_fileName[25];
  strcpy(l_CHAR_fileName, "/");
  strcat(l_CHAR_fileName, l_CHAR_Date);
  strcat(l_CHAR_fileName, path);
  DEBUG("l_CHAR_fileName:" + String(l_CHAR_fileName) );

  // Mise en place de LittleFS pour aller lire le fichier de synthese
  if (!LITTLEFS.begin()) {
    DEBUG("Il n'y a pas de file system little FS installé, lecture impossible !");
  }

  // Lecture du fichier de syntheses de la ROM
  int cumulRouge  = 0;
  int cumulVert   = 0;
  int cumulJaune  = 0;
  String buff;
  if ( LITTLEFS.exists(l_CHAR_fileName)) {
    DEBUG("Lecture des cumuls à partir du fichier <" + String(l_CHAR_fileName) + "> sur LittelFS");
    File fileFs = LITTLEFS.open(l_CHAR_fileName, FILE_READ);
    cumulRouge  = fileFs.readStringUntil('\n').toInt();   // 1ere ligne
    cumulVert   = fileFs.readStringUntil('\n').toInt();   // 2eme ligne
    cumulJaune  = fileFs.readStringUntil('\n').toInt();   // 3eme ligne
    fileFs.close();

  } else {
    DEBUG("Impossible de lire le fichier <" + String(l_CHAR_fileName) + "> sur LittelFS ***********************************");
  }



  // Calcul des cumuls et des moyennes
  cumulRouge  = cumulRouge  + rouge;
  cumulVert   = cumulVert   + vert;
  cumulJaune  = cumulJaune  + jaune;
  int total   = cumulRouge + cumulVert + cumulJaune;
  DEBUG("cumulRouge:" + String(cumulRouge, DEC));
  DEBUG("cumulVert:" + String(cumulVert, DEC));
  DEBUG("cumulJaune:" + String(cumulJaune, DEC));
  DEBUG("total:" + String(total, DEC) );


  // Ecriture du fichier de synthese en ROM
  File file = LITTLEFS.open(l_CHAR_fileName, FILE_WRITE);
  file.println(cumulRouge);
  file.println(cumulVert);
  file.println(cumulJaune);
  file.close();


  // Ecriture du fichier de synthèse sur la carte SD
  file = SD.open(l_CHAR_fileName, FILE_WRITE);
  file.println("Etablissement = " + siteID);
  file.println("Date          = " + date);
  file.println("Question      = " + question);
  file.println("Batterie      = " + String(batterieLevel) + "%" );
  file.println("Nombre d'appui sur les boutons :");
  file.println("  Rouge = " + String(cumulRouge, DEC) );
  file.println("  Vert  = " + String(cumulVert,  DEC) );
  file.println("  Jaune = " + String(cumulJaune, DEC) );
  file.println("  TOTAL = " + String(total, DEC) );
  file.println("Pourcentage d'appui sur les boutons :");
  if( total != 0 ){
    file.println("  Rouge = " + String(int(round(cumulRouge * 100 / total)), DEC) + "%" );
    file.println("  Vert  = " + String(int(round(cumulVert * 100 / total)),  DEC) + "%" );
    file.println("  Jaune = " + String(int(round(cumulJaune * 100 / total)), DEC) + "%" );
    DEBUG("  Rouge = " + String(int(round(cumulRouge * 100 / total)), DEC) + "%" );
    DEBUG("  Vert  = " + String(int(round(cumulVert * 100 / total)),  DEC) + "%" );
    DEBUG("  Jaune = " + String(int(round(cumulJaune * 100 / total)), DEC) + "%" );
  }

  file.close();
}


/**
   ----------------------------------------------------------------------------------
   Vérifie si la question a changée.
   @return:
     True / false si la quesiton du fichier params.ini a changé par rapport à celle dans littleFS.
     questionDansFichierConfig = La question lue dans le fichier de paramétrage.

   ----------------------------------------------------------------------------------*/
boolean CARTESD_questionChange(String &questionDansFichierConfig) {
  String siteID, question = "";

  // Démarrage de littleFS
  if (!LITTLEFS.begin()) {
    DEBUG("Il n'y a pas de file system little FS installé, lecture impossible !");
  }
  delay(500); // Pour laisser le temps à littleFS de démarrer


  // Initialise siteID et question a partir de la carte SD
  CARTESD_readConfigFile( fileName_Config, siteID, questionDansFichierConfig );



  // Lecture de la question stockée sur littleFS
  if ( LITTLEFS.exists(l_CHAR_fileNameSynthese)) {
    File file = LITTLEFS.open(l_CHAR_fileNameSynthese, FILE_READ);
    question = file.readString();
    DEBUG("Lecture de '" + question + "' dans le fichier <" + String(l_CHAR_fileNameSynthese) + "> sur LittelFS");
    file.close();

  } else {
    DEBUG("Fichier <" + String(l_CHAR_fileNameSynthese) + "> n'existe pas dans littleFS");
  }
  DEBUG("question:" + question );
  DEBUG("questionDansFichierConfig:" + questionDansFichierConfig);



  // Renvoie le test d'égalité entre la question actuelle et celle stockée dans litlleFS
  return (question != questionDansFichierConfig);
}


/** --------------------------------------------------------------------------------------
   @brief Permet de lire le fichier de configuration et de mettre à jour la date et l'heure si besoin.

   @param fileName : le nom du fichier ini dans lequel lire la date et l'heure.
   @return true si la config a pu être lu à partir du fichier de config

   @version issue #23 : Protection contre la saisie d'une année < 2000.
  */
  //----------------------------------------------------------------------------------------- 
bool CARTESD_updateDate(const char *fileName, bool &erreurFormat) {
  bool initFromFile = false;
  const size_t bufferLen = 300;
  char buffer[bufferLen];
  String STR_annee, STR_mois, STR_jour, STR_heure, STR_minute;


  // Si on ne peut pas ouvrir le fichier de configuration
  IniFile ini(fileName);
  if (!ini.open()) {
    DEBUG("Failed to open <" + String(fileName) + "> for reading");

    // On ne fait rien d'autre
    return false;
  }

  DEBUG("RTC_getDate:" + RTC_getDate());
  DEBUG("RTC_getTime:" + RTC_getTime());




  // Lecture de l'année à partir du fichier
  if ( ini.getValue("config", "year", buffer, bufferLen)) {
    STR_annee = buffer;
    initFromFile = true;
  } else {
    DEBUG("year : " + _printErrorMessage(ini.getError()) );
    STR_annee = RTC_getYear();
  }

  // Lecture du mois à partir du fichier ou de la RTC
  if ( ini.getValue("config", "month", buffer, bufferLen)) {
    STR_mois = buffer;
    initFromFile = true;
  } else {
    DEBUG("month : " + _printErrorMessage(ini.getError()) );
    STR_mois = RTC_getMonth();
  }

  // Lecture du jour à partir du fichier ou de la RTC
  if ( ini.getValue("config", "day", buffer, bufferLen)) {
    STR_jour = buffer;
    initFromFile = true;
  } else {
    DEBUG("day : " +_printErrorMessage(ini.getError()) );
    STR_jour = RTC_getDay();
  }

  // Lecture de l'heure à partir du fichier ou de la RTC
  if ( ini.getValue("config", "hour", buffer, bufferLen)) {
    STR_heure = buffer;
    initFromFile = true;
  } else {
    DEBUG("hour : " + _printErrorMessage(ini.getError()) );
    STR_heure = RTC_getHour();
  }

  // Lecture des minutes à partir du fichier ou de la RTC
  if ( ini.getValue("config", "minute", buffer, bufferLen)) {
    STR_minute = buffer;
    initFromFile = true;
  } else {
    DEBUG("minute :" + _printErrorMessage(ini.getError()) );
    STR_minute = RTC_getMinute();
  }


  // SI on a besoin d'initialiser la RTC à partir du fichier
  if ( initFromFile ) {
    // Creation d'une date (année doit être > 2000 )
    _clignoteLED(1, LED_JAUNE);
    DateTime date =  DateTime(STR_annee.toInt() < 2000 ? 2000:STR_annee.toInt(),
                        STR_mois.toInt(),
                        STR_jour.toInt(),
                        STR_heure.toInt(),
                        STR_minute.toInt(), 
                        0);

    // Si on a une date valide
    if( date.isValid() ) {
      _clignoteLED(1, LED_VERT);
      erreurFormat = false;

      // Mise à jour de la RTC ...............................
      rtc.begin();
      rtc.start();
      rtc.adjust(date);
      _clignoteLED(2, LED_VERT);
      
      // Reecriture du fichier de config .....................
      // Lecture du fichier de configuration
      String siteID, question = "";
      CARTESD_readConfigFile( fileName, siteID, question );
      _clignoteLED(3, LED_VERT);

      // Ecriture du fichier avec la question et le siteID
      CARTESD_writeConfigFile( fileName, siteID, question);
      DEBUG("Réécriture du fichier de paramétrage");
      _clignoteLED(4, LED_VERT);

    } else {
      _clignoteLED(2, LED_ROUGE);
      DEBUG("Date non valide, impossible de mettre la RTC à jour");
      DEBUG("STR_annee:" + STR_annee);
      DEBUG("STR_mois:" + STR_mois);
      DEBUG("STR_jour:" + STR_jour);
      DEBUG("STR_heure:" + STR_heure);
      DEBUG("STR_minute:" + STR_minute);
      initFromFile = false;
      
      erreurFormat = true;
    }
  }


  DEBUG("RTC_getDate:" + RTC_getDate());
  DEBUG("RTC_getTime:" + RTC_getTime());

  _clignoteLED(5, LED_VERT);

  return initFromFile;
}