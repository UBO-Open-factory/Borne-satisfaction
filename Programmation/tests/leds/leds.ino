#define LED_VERT 4
#define LED_JAUNE 12
#define LED_ROUGE 14


void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_VERT, OUTPUT);
  pinMode(LED_JAUNE, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);

  Serial.println("LEDs pin inititilisés");
}

void loop() {
  // test de la led verte
  Serial.println("Allumage de la led verte");
  digitalWrite(LED_VERT, HIGH);
  delay(1000);
  digitalWrite(LED_VERT, LOW);

  // test de la led jaune
  Serial.println("Allumage de la led jaune");
  digitalWrite(LED_JAUNE, HIGH);
  delay(1000);
  digitalWrite(LED_JAUNE, LOW);

  // test de la led rouge
  Serial.println("Allumage de la led rouge");
  digitalWrite(LED_ROUGE, HIGH);
  delay(1000);
  digitalWrite(LED_ROUGE, LOW);
}
