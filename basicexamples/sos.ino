// (c) umarudoma 2026
const int ledPin = LED_BUILTIN;

void setup() {
  // put your setup code here, to run once:
 pinMode(ledPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // S = kurz kurz kurz
  blinkKurz();
  blinkKurz();
  blinkKurz();

  pauseBuchstabe();

  // O = lang lang lang
  blinkLang();
  blinkLang();
  blinkLang();

  pauseBuchstabe();

  // S = kurz kurz kurz
  blinkKurz();
  blinkKurz();
  blinkKurz();

  pauseWort();
}

void blinkKurz() {
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
}

void blinkLang() {
  digitalWrite(ledPin, HIGH);
  delay(600);
  digitalWrite(ledPin, LOW);
  delay(200);
}

void pauseBuchstabe() {
  delay(400);
}

void pauseWort() {
  delay(1200);
}
