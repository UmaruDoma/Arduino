// cndrbrbr 2024
void setup() {
  
pinMode(LED_BUILTIN, OUTPUT); // D13 (LED_BUILTIN) wird als Output festgelegt.
}

void Blink (int duration,int times){
  for (int i = 0;i<times;i++){
      digitalWrite(LED_BUILTIN, HIGH); // LED-Spannung wird auf High gesetzt, die LED leuchtet.
      delay(duration); // Wartezeit in Millisekunden - die LED leuchtet weiterhin.
      digitalWrite(LED_BUILTIN, LOW); // LED-Spannung wird auf LOW gesetzt, die LED erlischt.
      delay(duration); // Wartezeit in Millisekunden - die LED ist weiterhin aus.
  }
}

void loop() {
  Blink (200,3);
  delay(1000); 
  Blink (1000,3);
  delay(1000); 
  Blink (200,3);
  delay(1000); 
}
