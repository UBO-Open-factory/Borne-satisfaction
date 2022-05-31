/**
 * @file deepsleep.h
 * @brief contains all function for deepsleep purpose.
 * @date 2022-05-10
   @author Alexandre PERETJATKO
 * 
 * @version 1.0.2
 */

/**
  ----------------------------------------------------------------------------------
  @brief Défintion du deep sleep pour la mise ne place du wake up lorsqu'on appuie sur un bouton.

  @see https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
  */
void DEEPSLEEP_start() {
  #ifdef ModeDebug
    // Calcul du mask de bits pour les GPIO utilisés par les boutons
    int somme = pow(2, BTN_ROUGE) + pow(2, BTN_VERT) + pow(2, BTN_JAUNE); // 33554432 + 67108864 + 268435456 = 234881024
    String chaine = "0x" + String( somme, HEX);
    DEBUG("Masque de réveille pour le deep sleep :" + chaine);
  #endif

  // On authorise l'appuie sur les boutons à réveiller l'ESP32
  esp_sleep_enable_ext1_wakeup(0xe000000, ESP_EXT1_WAKEUP_ANY_HIGH);

  // On coupe l'alimentation des périphériques
  digitalWrite(PIN_PWR_EN, LOW);
  delay(20);

  // Mise en deep sleep
  DEBUG("En deep sleep");
  esp_deep_sleep_start();
}



/**
  ----------------------------------------------------------------------------------
  @brief Renvoie le numéro du GPIO qui a sortie l'ESP du Deep sleep.
  @see https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
   
  @return GPIO number as int
  */
int DEEPSLEEP_getGPIOWakeUp() {
  uint64_t GPIO_reason = esp_sleep_get_ext1_wakeup_status();

  return (log(GPIO_reason)) / log(2);
}
